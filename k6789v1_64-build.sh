make k6789v1_64 LK_AS=BL2_EXT TEE=teei
./mkimage  build-k6789v1_64/lk.bin ./bl2_ext.cfg > build-k6789v1_64/lk-bl2_ext.img
make k6789v1_64 LK_AS=AEE
./mkimage  build-k6789v1_64/lk.bin ./aee.cfg > build-k6789v1_64/lk-aee.img
make k6789v1_64
./mkimage  build-k6789v1_64/lk.bin ./lk.cfg > build-k6789v1_64/lk-lk.img
cat build-k6789v1_64/lk-aee.img build-k6789v1_64/lk-bl2_ext.img build-k6789v1_64/lk-lk.img main_dtb_header.bin > build-k6789v1_64/lk.img
 
