SECTIONS {
    .fb_oem_cmd : {
        __fb_oem_cmd_start = .;
        KEEP(*(.fb_oem_cmd))
        __fb_oem_cmd_end = .;
    }
}
INSERT AFTER .rodata;
