SECTIONS {
    .set_fdt : {
        __set_fdt = .;
        KEEP (*(.set_fdt))
        __set_fdt_end = .;
    }
}
INSERT AFTER .rodata;
