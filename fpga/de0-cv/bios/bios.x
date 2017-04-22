OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS {
    . = 0x0;
    .text : {
        *(.entry.text);
        *(.text);
        *(.rodata*);
        *(.bss*);
        *(.data*);
    }
}
