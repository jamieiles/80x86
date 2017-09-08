OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS {
    . = 0x0;

    .text 0xc000 : {
        *(.entry.text);
        . = ALIGN(4);
        *(.text);
        . = ALIGN(4);
        vectors_start = . ;
        *(.rodata.vectors);
        vectors_end = . ;
        . = ALIGN(4);
        *(.rodata*);
    }

    _data_load = . ;

    .data 0xb000 : AT(_data_load) {
        stack_start = . ;
        data_start = . ;
        *(.data*);
        data_end = . ;

        . = ALIGN(4);
        bss_start = . ;
        *(.bss*);
        bss_end = . ;
    }

    .reset 0xfff0 : AT(0xfff0) {
        *(.reset.text);
    }

    .bios_date 0xfff6 : AT(0xfff6) {
        *(.bios_date);
    }

    .model 0xfffe : AT(0xfffe) {
        *(.model);
    }
}
