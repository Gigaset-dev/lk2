TARGET := tb8168p1_64_bsp

MEMBASE := 0x40000000
MEMSIZE := 0x16400000
KERNEL_LOAD_OFFSET := 0x16000000

PROJ_EXT := bl33

# Avoid using floating point registers
GLOBAL_COMPILEFLAGS += -mgeneral-regs-only -DWITH_NO_FP=1

MODULES += \
	lib/pl_boottags
