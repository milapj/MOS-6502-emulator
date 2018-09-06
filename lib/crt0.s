;
; Oliver Schmidt, 2013-05-16
;
; Startup code for cc65 (Hawknest version)
;

        .export _exit
        .export __STARTUP__ : absolute = 1 ; Mark as startup

        .import zerobss, callmain, copydata, initlib, donelib, exit
	.import _handle_nmi, _handle_irq, _write

	; Linker generated symbols
	.import __RAM_START__, __RAM_SIZE__
	.import __STACK_RAM_START__, __STACK_RAM_SIZE__

        .include "zeropage.inc"
        .segment "STARTUP"

start:
        cld     ; decimal flag off
        ldx     #$FF
        txs     ; cpu stack starts just below 0x100

        lda     #<(__STACK_RAM_START__ +  __STACK_RAM_SIZE__)
        ldx     #>(__STACK_RAM_START__ + __STACK_RAM_SIZE__)
        sta     sp ; this is the CC65 parameter stack pointer
        stx     sp+1

        jsr     zerobss
	jsr     copydata
        jsr     initlib
        jsr     callmain

_exit:  pha
        jsr     donelib
        pla
        jmp     exit

nmi:
	; save regs
	pha
	tya
	pha
	txa
	pha

	jsr _handle_nmi
	
	; restore regs
	pla
	tax
	pla
	tay
	pla
	
	rti

irq:
	; save regs
	pha
	tya
	pha
	txa
	pha

	jsr _handle_irq

	; restore regs
	pla
	tax
	pla
	tay
	pla

	rti

; [KCH] hardware interrupt vectors
.segment "VECTORS"
	.word nmi
	.word start
	.word irq
