; Lucas Myers, Summer 2018
; Supplies the 4-byte magic identifier for a Hawknest ROM

        .export __HEADER__ : absolute = 1 ; Linker referenced
        .segment "HEADER"
        .byte "HKNS"
