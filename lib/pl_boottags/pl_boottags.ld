SECTIONS {
    .pl_boottags : {
        __pl_boottags = .;
        KEEP (*(.pl_boottags))
        __pl_boottags_end = .;
    }

    .pl_boottags_to_be_updated : {
        __pl_boottags_to_be_updated = .;
        KEEP (*(.pl_boottags_to_be_updated))
        __pl_boottags_to_be_updated_end = .;
    }
}
INSERT AFTER .rodata;
