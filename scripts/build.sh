#!/bin/bash
##### color #####
if [ -t 0 ]; then
RED='\033[0;31m'
GREEN='\033[0;32m'
ORANGE='\033[0;33m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
LIGHTBLUE='\033[1;34m'
NC='\033[0m' # No Color
fi

# Predefine and used for prelight build
CLANG="CLANG_BINDIR=../prebuilts/clang/host/linux-x86/clang-r383902/bin"
TOOLCHAIN="ARCH_arm_TOOLCHAIN_PREFIX=../prebuilts/gcc/linux-x86/arm/\
arm-linux-androideabi-4.9.1/bin/arm-linux-androideabi- ARCH_arm64_TOOLCHAIN_PREFIX=\
../prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-"

prj=$1
variant=

# Preflight can assign OUT_DIR and LOG_NAME
OUT_DIR=${OUT_DIR:=.}  #If $OUT_DIR and $out not set, use current directory as output dir
LOG_FILE=${LOG_NAME:=${OUT_DIR}/build_$prj.log}
LK_BIN=$OUT_DIR/build-$prj/lk.bin

if [ $# -lt 1 ]; then
    echo examples:
    echo build : $0 project_name
    exit -1
fi

check_file_exist () {
    local file=$1
    if [ -s "$file" ]
    then
        echo -e "$file is found." | echo_log true
        echo -e "${YELLOW}build $file pass! ${NC}\n" | echo_log true
    else
        echo "$file is not found." | echo_log true
        echo -e "${RED}build $file fail! ${NC}" | echo_log true
        echo "An error occurred." | echo_log true
        exit -1
     fi
}

clean_build () {
    rm -rf $OUT_DIR/build-*
    echo -e "${YELLOW} clean out ${NC}"
}

echo_log() {
	if [ "$1" = "true" ]; then
		cat - | tee -a $LOG_FILE
	else
		cat - >> ${LOG_FILE}
	fi
}

# Get all variants from project file
get_variant() {
    local file=$1
    local -n lk_as=$2
    local idx=0

    lk_as[$idx]="" # for normal lk
    while IFS= read -r line || [[ -n "$line" ]]; do
        if [[ $line =~ \$\(LK_AS\),[[:space:]]*([[:print:]]*)\) ]]; then
            lk_as[++idx]="LK_AS?=${BASH_REMATCH[1]}"
        fi
    done < $file
}

# Switch to lk root dir
SCRIPT_PATH="$(dirname "$(readlink -f "$0")")"
cd $SCRIPT_PATH/../

# Get all variants of this project to build all of them
get_variant project/$prj.mk variant

# Trigger build for each variant of this project
for ((i = 0; i < ${#variant[@]}; i++))
do
    clean_build
    echo -e "${LIGHTBLUE} start build ${GREEN} ${variant[i]} ${NC}" | echo_log true

    make $prj -k ${variant[i]} $CLANG $TOOLCHAIN BUILDROOT=$OUT_DIR -j24 2>&1 | echo_log $console
    check_file_exist "$LK_BIN"
done
