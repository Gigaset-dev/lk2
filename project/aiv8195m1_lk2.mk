TARGET := aiv8195m1

MEMBASE := 0x0
MEMSIZE := 0x300000
KERNEL_LOAD_OFFSET := 0x201000

LK_AS := BL2

MODULES += \
	app/blxboot \
	app/fastboot \
	app/fastboot/device_lock \
	app/fastboot/flash

GLOBAL_DEFINES += \
	SECURE_WORLD
