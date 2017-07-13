OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS {
    . = 0x0;
    .text 0xe000 : {
        *(.entry.text);
        *(.text);
        vectors_start = . ;
        *(.rodata.vectors);
        vectors_end = . ;
        *(.rodata*);
        *(.data*);
    }
    .bss : {
        bss_start = . ;
        *(.bss*);
        bss_end = . ;
    }
    .reset 0xfff0 : {
        *(.reset.text);
    }
    .model 0xfffe : {
        *(.model);
    }
}
