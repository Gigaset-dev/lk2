SECTIONS {
    .mboot_expdb : {
        __mboot_expdb = .;
        KEEP (*(.mboot_expdb))
        __mboot_expdb_end = .;
    }
}
INSERT AFTER .rodata;
