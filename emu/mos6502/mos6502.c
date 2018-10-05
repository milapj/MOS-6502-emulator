#include <rc.h>
#include <base.h>
#include <membus.h>
#include <timekeeper.h>
#include <mos6502/vmcall.h>
#include <mos6502/mos6502.h>
#include <string.h>

uint8_t convert(uint8_t );
mos6502_step_result_t handle_vmcall(decode_info_t *);
void ADC_handler(mos6502_t *);
void ADC_ZP_handler(mos6502_t *);
void ADC_ZPX_handler(mos6502_t *);
void ADC_ABS_handler(mos6502_t *);
void ADC_ABSX_handler(mos6502_t *);
void ADC_ABSY_handler(mos6502_t *);
void ADC_IDX_IDR_handler(mos6502_t *);
void ADC_IDR_IDX_handler(mos6502_t *);
void AND_handler(mos6502_t *);
void AND_ZP_handler(mos6502_t *);
void AND_ZPX_handler(mos6502_t *);
void AND_ABS_handler(mos6502_t *);
void AND_ABSX_handler(mos6502_t *);
void AND_ABSY_handler(mos6502_t *);
void AND_IDX_IDR_handler(mos6502_t *);
void AND_IDR_IDX_handler(mos6502_t *);
void ASL_handler(mos6502_t *);
void ASL_ZP_handler(mos6502_t *);
void ASL_ZPX_handler(mos6502_t *);
void ASL_ABS_handler(mos6502_t *);
void ASL_ABSX_handler(mos6502_t *);
void BCC_handler(mos6502_t *);
void BCS_handler(mos6502_t *);
void BEQ_handler(mos6502_t *);
void BIT_handler(mos6502_t *);
void BIT_ABS_handler(mos6502_t *);
void BMI_handler(mos6502_t *);
void BNE_handler(mos6502_t *);
void BPL_handler(mos6502_t *);
void BRK_handler(mos6502_t *);
void BVC_handler(mos6502_t *);
void BVS_handler(mos6502_t *);
void CLC_handler(mos6502_t *);
void CLD_handler(mos6502_t *);
void CLI_handler(mos6502_t *);
void CLV_handler(mos6502_t *);
void CMP_handler(mos6502_t *);
void CMP_ZP_handler(mos6502_t *);
void CMP_ZPX_handler(mos6502_t *);
void CMP_ABS_handler(mos6502_t *);
void CMP_ABSX_handler(mos6502_t *);
void CMP_ABSY_handler(mos6502_t *);
void CMP_IDX_IDR_handler(mos6502_t *);
void CMP_IDR_IDX_handler(mos6502_t *);
void CPX_handler(mos6502_t *);
void CPX_ZP_handler(mos6502_t *);
void CPX_ABS_handler(mos6502_t *);
void CPY_handler(mos6502_t *);
void CPY_ZP_handler(mos6502_t *);
void CPY_ABS_handler(mos6502_t *);
void DEC_ZP_handler(mos6502_t *);
void DEC_ZPX_handler(mos6502_t *);
void DEC_ABS_handler(mos6502_t *);
void DEC_ABSX_handler(mos6502_t *);
void DEX_handler(mos6502_t *);
void DEY_handler(mos6502_t *);
void EOR_handler(mos6502_t *);
void EOR_ZP_handler(mos6502_t *);
void EOR_ZPX_handler(mos6502_t *);
void EOR_ABS_handler(mos6502_t *);
void EOR_ABSX_handler(mos6502_t *);
void EOR_ABSY_handler(mos6502_t *);
void EOR_IDX_IDR_handler(mos6502_t *);
void EOR_IDR_IDX_handler(mos6502_t *);
void INC_ZP_handler(mos6502_t *);
void INC_ZPX_handler(mos6502_t *);
void INC_ABS_handler(mos6502_t *);
void INC_ABSX_handler(mos6502_t *);
void INX_handler(mos6502_t *);
void INY_handler(mos6502_t *);
void JMP_ABS_handler(mos6502_t *);
void JMP_IDR_handler(mos6502_t *);
void TXS_handler(mos6502_t *);
void STX_handler(mos6502_t *);
void STX_ABS_handler(mos6502_t *);
void JSR_ABS_handler(mos6502_t *);
void PHA_handler(mos6502_t *);
void TAX_handler(mos6502_t *);
void TXA_handler(mos6502_t *);
void TAY_handler(mos6502_t *);
void TYA_handler(mos6502_t *);
void SEI_handler(mos6502_t *);
void LDA_handler(mos6502_t *);
void LDA_ZP_handler(mos6502_t *);
void LDA_ZPX_handler(mos6502_t *);
void LDA_ABS_handler(mos6502_t *);
void LDA_ABSX_handler(mos6502_t *);
void LDA_ABSY_handler(mos6502_t *);
void LDA_IDX_IDR_handler(mos6502_t *);
void LDA_IDR_IDX_handler(mos6502_t *);
void LDX_handler(mos6502_t *);
void LDX_ZP_handler(mos6502_t *);
void LDX_ZPY_handler(mos6502_t *);
void LDX_ABS_handler(mos6502_t *);
void LDX_ABSY_handler(mos6502_t *);
void LDY_handler(mos6502_t *);
void LDY_ZP_handler(mos6502_t *);
void LDY_ZPX_handler(mos6502_t *);
void LDY_ABS_handler(mos6502_t *);
void LDY_ABSX_handler(mos6502_t *);
void LSR_handler(mos6502_t *);
void LSR_ZP_handler(mos6502_t *);
void LSR_ZPX_handler(mos6502_t *);
void LSR_ABS_handler(mos6502_t *);
void LSR_ABSX_handler(mos6502_t *);
void NOP_handler(mos6502_t *);
void ORA_handler(mos6502_t *);
void ORA_ZP_handler(mos6502_t *);
void ORA_ZPX_handler(mos6502_t *);
void ORA_ABS_handler(mos6502_t *);
void ORA_ABSX_handler(mos6502_t *);
void ORA_ABSY_handler(mos6502_t *);
void ORA_IDX_IDR_handler(mos6502_t *);
void ORA_IDR_IDX_handler(mos6502_t *);
void PHP_handler(mos6502_t *);
void PLA_handler(mos6502_t *);
void PLP_handler(mos6502_t *);
void RTI_handler(mos6502_t *);
void RTS_handler(mos6502_t *);
void SEC_handler(mos6502_t *);
void SEC_handler(mos6502_t *);
void STA_ZP_handler(mos6502_t *);
void STA_ZPX_handler(mos6502_t *);
void STA_ABS_handler(mos6502_t *);
void STA_ABSX_handler(mos6502_t *);
void STA_ABSY_handler(mos6502_t *);
void STA_IDX_IDR_handler(mos6502_t *);
void STA_IDR_IDX_handler(mos6502_t *);
void STX_ZP_handler(mos6502_t *);
void STX_ZPY_handler(mos6502_t *);
void STX_ABS_handler(mos6502_t *);
void STY_ZP_handler(mos6502_t *);
void STY_ZPX_handler(mos6502_t *);
void STY_ABS_handler(mos6502_t *);
void TSX_handler(mos6502_t *);
void SED_handler(mos6502_t *);  
void ROL_handler(mos6502_t *);
void ROL_ZP_handler(mos6502_t *);
void ROL_ZPX_handler(mos6502_t *);
void ROL_ABS_handler(mos6502_t *);
void ROL_ABSX_handler(mos6502_t *);
void ROR_handler(mos6502_t *);
void ROR_ZP_handler(mos6502_t *);
void ROR_ZPX_handler(mos6502_t *);
void ROR_ABS_handler(mos6502_t *);
void ROR_ABSX_handler(mos6502_t *);
void SBC_handler(mos6502_t *);
void SBC_ZP_handler(mos6502_t *);
void SBC_ZPX_handler(mos6502_t *);
void SBC_ABS_handler(mos6502_t *);
void SBC_ABSX_handler(mos6502_t *);
void SBC_ABSY_handler(mos6502_t *);
void SBC_IDX_IDR_handler(mos6502_t *);
void SBC_IDR_IDX_handler(mos6502_t *);
void RMB0_handler(mos6502_t *);

static const uint8_t instr_cycles[256] = {
	7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
	2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
};

uint8_t
convert(uint8_t operand){
  return operand >> 7 ? (~operand)+1 : operand;
}

static inline uint8_t
read8 (mos6502_t * cpu, uint16_t addr)
{
	return membus_read(cpu->bus, addr);
}

static inline void
write8 (mos6502_t * cpu, uint16_t addr, uint8_t val)
{
	membus_write(cpu->bus, addr, val);
}

static inline uint16_t
read16 (mos6502_t * cpu, uint16_t addr)
{
	uint16_t lo = (uint16_t)read8(cpu, addr);
	uint16_t hi = (uint16_t)read8(cpu, addr + 1);
	uint16_t val = lo | (uint16_t)(hi << 8);
	return val;
}

static inline uint16_t
buggy_read16 (mos6502_t * cpu, uint16_t addr)
{
	uint16_t first = addr;
	uint16_t secnd = (addr & 0xFF00) | (uint16_t)((uint8_t)addr + 1);

	uint16_t lo = (uint16_t)read8(cpu, first);
	uint16_t hi = (uint16_t)read8(cpu, secnd);
	uint16_t val = (uint16_t)(hi << 8) | lo;

	return val;
}

size_t
mos6502_instr_repr (mos6502_t * cpu, uint16_t addr, char * buffer, size_t buflen)
{
	// FILL ME IN

	// Delete this line when you're done
	buffer[0] = 0;
	return 0;
}

mos6502_step_result_t
mos6502_step (mos6502_t * cpu)
{
	uint8_t opcode = read8(cpu, cpu->pc);
	decode_info_t decode_info;
   	switch(opcode){
  case 0x80:
    decode_info.cpu = cpu;
    decode_info.opcode = opcode;
    decode_info.addr = cpu->pc+1;
    handle_vmcall(&decode_info);
    break;
  case 0x40:
    RTI_handler(cpu);
    break;
  case 0x60:
    RTS_handler(cpu);
    break;
    /* ADC Family */
  case 0x69:
    ADC_handler(cpu);
    break;
  case 0x65:
    ADC_ZP_handler(cpu);
  break;
  case 0x75:
    ADC_ZPX_handler(cpu);
    break;
  case 0x6D:
    ADC_ABS_handler(cpu);
    break;
  case 0x7D:
    ADC_ABSX_handler(cpu);
    break;
  case 0x79:
    ADC_ABSY_handler(cpu);
    break;
  case 0x61:
    ADC_IDX_IDR_handler(cpu);
    break;
  case 0x71:
    ADC_IDR_IDX_handler(cpu);
    break;
  /* AND Family */
  case 0x29:
    AND_handler(cpu);
    break;
  case 0x25:
    AND_ZP_handler(cpu);
    break;
  case 0x35:
    AND_ZPX_handler(cpu);
    break;
  case 0x2D:
    AND_ABS_handler(cpu);
    break;
  case 0x3D:
    AND_ABSX_handler(cpu);
    break;
  case 0x39:
    AND_ABSY_handler(cpu);
    break;
  case 0x21:
    AND_IDX_IDR_handler(cpu);
    break;
  case 0x31:
    AND_IDR_IDX_handler(cpu);
    break;
    /* ASL Family */
  case 0x0A:
    ASL_handler(cpu);
    break;
  case 0x06:
    ASL_ZP_handler(cpu);
    break;
  case 0x16:
    ASL_ZPX_handler(cpu);
    break;
  case 0x0E:
    ASL_ABS_handler(cpu);
    break;
  case 0x1E:
    ASL_ABSX_handler(cpu);
    break;
    /* Branch Family */
  case 0x10:
    BPL_handler(cpu);
    break;
  case 0x30:
    BMI_handler(cpu);
    break;
  case 0x50:
    BVC_handler(cpu);
    break;
  case 0x70:
    BVS_handler(cpu);
    break;
  case 0x90:
    BCC_handler(cpu);
    break;
  case 0xB0:
    BCS_handler(cpu);
    break;
  case 0xD0:
    BNE_handler(cpu);
    break;
  case 0xF0:
    BEQ_handler(cpu);
    break;
    /* BIT Family */
  case 0x24:
    BIT_handler(cpu);
    break;
  case 0x2C:
    BIT_ABS_handler(cpu);
    break;
  case 0x00:
    BRK_handler(cpu);
    break;
    /* CMP Family */
  case 0xC9:
    CMP_handler(cpu);
    break;
  case 0xC5:
    CMP_ZP_handler(cpu);
    break;
  case 0xD5:
    CMP_ZPX_handler(cpu);
    break;
  case 0xCD:
    CMP_ABS_handler(cpu);
    break;
  case 0xDD:
    CMP_ABSX_handler(cpu);
    break;
  case 0xD9:
    CMP_ABSY_handler(cpu);
    break;
  case 0xC1:
    CMP_IDX_IDR_handler(cpu);
    break;
  case 0xD1:
    CMP_IDR_IDX_handler(cpu);
    break;
    /* CPX Family */
  case 0xE0:
    CPX_handler(cpu);
    break;
  case 0xE4:
    CPX_ZP_handler(cpu);
    break;
  case 0xEC:
    CPX_ABS_handler(cpu);
    break;
    /* CPY Family */
  case 0xC0:
    CPY_handler(cpu);
    break;
  case 0xC4:
    CPY_ZP_handler(cpu);
    break;
  case 0xCC:
    CPY_ABS_handler(cpu);
    break;
    /* DEC Family */
  case 0xC6:
    DEC_ZP_handler(cpu);
    break;
  case 0xD6:
    DEC_ZPX_handler(cpu);
    break;
  case 0xCE:
    DEC_ABS_handler(cpu);
    break;
  case 0xDE:
    DEC_ABSX_handler(cpu);
    break;
    /* Register Instructions */
  case 0xCA:
    DEX_handler(cpu);
    break;
  case 0x88:
    DEY_handler(cpu);
    break;
  case 0xE8:
    INX_handler(cpu);
    break;
  case 0xC8:
    INY_handler(cpu);
    break;
  case 0xAA:
    TAX_handler(cpu);
    break;
  case 0x8A:
    TXA_handler(cpu);
    break;
  case 0xA8:
    TAY_handler(cpu);
    break;
  case 0x98:
    TYA_handler(cpu);
    break;
    /* EOR Family */
  case 0x49:
    EOR_handler(cpu);
    break;
  case 0x45:
    EOR_ZP_handler(cpu);
    break;
  case 0x55:
    EOR_ZPX_handler(cpu);
    break;
  case 0x4D:
    EOR_ABS_handler(cpu);
    break;
  case 0x5D:
    EOR_ABSX_handler(cpu);
    break;
  case 0x59:
    EOR_ABSY_handler(cpu);
    break;
  case 0x41:
    EOR_IDX_IDR_handler(cpu);
    break;
  case 0x51:
    EOR_IDR_IDX_handler(cpu);
    break;
    /* Flag Instruction */
  case 0x18:
    CLC_handler(cpu);
    break;
  case 0x38:
    SEC_handler(cpu);
    break;
  case 0x58:
    CLI_handler(cpu);
    break;
  case 0x78:
    SEI_handler(cpu);
    break;
  case 0xB8:
    CLV_handler(cpu);
    break;
  case 0xD8:
    CLD_handler(cpu);
    break;
  case 0xF8:
    SED_handler(cpu);
    break;     
    /* INC Family */
  case 0xE6:
    INC_ZP_handler(cpu);
    break;
  case 0xF6:
    INC_ZPX_handler(cpu);
    break;
  case 0xEE:
    INC_ABS_handler(cpu);
    break;
  case 0xFE:
    INC_ABSX_handler(cpu);
    break;
    /* JMP Family */
  case 0x4C:
    JMP_ABS_handler(cpu);
    break;
  case 0x6C:
    JMP_IDR_handler(cpu);
    break;
    /* JSR */
  case 0x20:
    JSR_ABS_handler(cpu);
    break;
    /* Stack Instructions */
  case 0x9A:
    TXS_handler(cpu);
    break;
  case 0xBA:
    TSX_handler(cpu);
    break;
  case 0x48:
    PHA_handler(cpu);
    break;
  case 0x68:
    PLA_handler(cpu);
    break;
  case 0x08:
    PHP_handler(cpu);
    break;
  case 0x28:
    PLP_handler(cpu);
    break;
    // LDA Family
  case 0xA9:
    LDA_handler(cpu);
    break;
  case 0xA5:
    LDA_ZP_handler(cpu);
    break;
  case 0xB5:
    LDA_ZPX_handler(cpu);
    break;
  case 0xAD:
    LDA_ABS_handler(cpu);
    break;
  case 0xBD:
    LDA_ABSX_handler(cpu);
    break;
  case 0xB9:
    LDA_ABSY_handler(cpu);
    break;
  case 0xA1:
    LDA_IDX_IDR_handler(cpu);
    break;
  case 0xB1:
    LDA_IDR_IDX_handler(cpu);
    break;
  case 0xEA:
    NOP_handler(cpu);
    break;
    // LDX Family
  case 0xA2:
    LDX_handler(cpu);
    break;
  case 0xA6:
    LDX_ZP_handler(cpu);
    break;
  case 0xB6:
    LDX_ZPY_handler(cpu);
    break;
  case 0xAE:
    LDX_ABS_handler(cpu);
    break;
  case 0xBE:
    LDX_ABSY_handler(cpu);
    break;
    // ORA Family
  case 0x09:
    ORA_handler(cpu);
    break;
  case 0x05:
    ORA_ZP_handler(cpu);
    break;
  case 0x15:
    ORA_ZPX_handler(cpu);
    break;
  case 0x0D:
    ORA_ABS_handler(cpu);
    break;
  case 0x1D:
    ORA_ABSX_handler(cpu);
    break;
  case 0x19:
    ORA_ABSY_handler(cpu);
    break;
  case 0x01:
    ORA_IDX_IDR_handler(cpu);
    break;
  case 0x11:
    ORA_IDR_IDX_handler(cpu);
    break;
    // STA Family 
  case 0x85:
    STA_ZP_handler(cpu);
    break;
  case 0x95:
    STA_ZPX_handler(cpu);
    break;
  case 0x8D:
    STA_ABS_handler(cpu);
    break;
  case 0x9D:
    STA_ABSX_handler(cpu);
    break;
  case 0x99:
    STA_ABSY_handler(cpu);
    break;
  case 0x81:
    STA_IDX_IDR_handler(cpu);
    break;
  case 0x91:
    STA_IDR_IDX_handler(cpu);
    break;
    // STX Family 
  case 0x86:
    STX_ZP_handler(cpu);
    break;
  case 0x96:
    STX_ZPY_handler(cpu);
    break;
  case 0x8E:
    STX_ABS_handler(cpu);
    break;
    // STY Family 
  case 0x84:
    STY_ZP_handler(cpu);
    break;
  case 0x94:
    STY_ZPX_handler(cpu);
    break;
  case 0x8C:
    STY_ABS_handler(cpu);
    break;
    // LDY Family
  case 0xA0:
    LDY_handler(cpu);
    break;
  case 0xA4:
    LDY_ZP_handler(cpu);
    break;
  case 0xB4:
    LDY_ZPX_handler(cpu);
    break;
  case 0xAC:
    LDY_ABS_handler(cpu);
    break;
  case 0xBC:
    LDY_ABSX_handler(cpu);
    break;
    // LSR Family
  case 0x4A:
    LSR_handler(cpu);
    break;
  case 0x46:
    LSR_ZP_handler(cpu);
    break;
  case 0x56:
    LSR_ZPX_handler(cpu);
    break;
  case 0x4E:
    LSR_ABS_handler(cpu);
    break;
  case 0x5E:
    LSR_ABSX_handler(cpu);
    break;
    // ROL Family
  case 0x2A:
    ROL_handler(cpu);
    break;
  case 0x26:
    ROL_ZP_handler(cpu);
    break;
  case 0x36:
    ROL_ZPX_handler(cpu);
    break;
  case 0x2E:
    ROL_ABS_handler(cpu);
    break;
  case 0x3E:
    ROL_ABSX_handler(cpu);
    break;
    // ROR Family
  case 0x6A:
    ROR_handler(cpu);
    break;
  case 0x66:
    ROR_ZP_handler(cpu);
    break;
  case 0x76:
    ROR_ZPX_handler(cpu);
    break;
  case 0x6E:
    ROR_ABS_handler(cpu);
    break;
  case 0x7E:
    ROR_ABSX_handler(cpu);
    break;
    // SBC Family
  case 0xE9:
    SBC_handler(cpu);
    break;
  case 0xE5:
    SBC_ZP_handler(cpu);
    break;
  case 0xF5:
    SBC_ZPX_handler(cpu);
    break;
  case 0xED:
    SBC_ABS_handler(cpu);
    break;
  case 0x02:
    NOP_handler(cpu);
    break;
  case 0x17:
    NOP_handler(cpu);
    break;
  default:
    printf("Illegal Opcode %x, quit + PC at->%x  ",opcode,cpu->pc);
    break;
  }
	mos6502_advance_clk(cpu, instr_cycles[opcode]);
	return MOS6502_STEP_RESULT_SUCCESS;
}
/*
MODE           SYNTAX       HEX LEN TIM
Immediate     ADC #$44      $69  2   2
Zero Page     ADC $44       $65  2   3
Zero Page,X   ADC $44,X     $75  2   4
Absolute      ADC $4400     $6D  3   4
Absolute,X    ADC $4400,X   $7D  3   4+
Absolute,Y    ADC $4400,Y   $79  3   4+
Indirect,X    ADC ($44,X)   $61  2   6
Indirect,Y    ADC ($44),Y   $71  2   5+
*/
void
ADC_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc+(uint8_t)1);
  uint16_t value = cpu->a + operand;
  value = cpu->p.c ? value + ( (cpu->p.c? 1 : 0) | 0x00) : value; 	
  /* Overflow */
  if( value > 0xFF){
    cpu->p.c = (value >> 8) & 0x01 ? 1: 0 ;  
    cpu->p.v = 1;
    cpu->a = value & 0xFF;
  } else{
    cpu->p.c = 0 ;
    cpu->a = value & 0xFF;
  }
  cpu->p.n = (cpu->a >> 7) & 0x01 ? 1 : 0;
  cpu->p.z = cpu->a == 0 ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
ADC_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)operand);
  uint16_t result = cpu->a + value;
  result = cpu->p.c ? result + ( (cpu->p.c? 1 : 0) | 0x00 ) : result;
  /* Overflow */
  if( result > 0xFF){
    cpu->p.c = (result >> 8) & 0x01 ? 1: 0 ;
    cpu->p.v = 1;
    cpu->a = result & 0xFF;
  } else{
    cpu->p.c = 0 ;
    cpu->a = result & 0xFF;
  }
  cpu->p.n = (cpu->a >> 7) & 0x01 ? 1 : 0;
  cpu->p.z = cpu->a == 0 ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
ADC_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)(operand+cpu->x));
  uint16_t result = cpu->a+value;
  result = cpu->p.c ? result + ( (cpu->p.c? 1 : 0) | 0x00 ) : result;
  /* Overflow */
  if( result > 0xFF){
    cpu->p.c = (result >> 8) & 0x01 ? 1: 0 ;
    cpu->p.v = 1;
    cpu->a = result & 0xFF;
  } else{
    cpu->p.c = 0 ;
    cpu->a = result & 0xFF;
  }
  cpu->p.n = (cpu->a >> 7) & 0x01 ? 1 : 0;
  cpu->p.z = cpu->a == 0 ? 1 : 0;
  cpu->pc += (uint8_t)0x2; 
}
void
ADC_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand);
  uint16_t result = cpu->a +value;
  result = cpu->p.c ? result + ( (cpu->p.c? 1 : 0) | 0x00 ) : result;
  /* Overflow */
  if( result > 0xFF){
    cpu->p.c = (result >> 8) & 0x01 ? 1: 0 ;
    cpu->p.v = 1;
    cpu->a = result & 0xFF;
  } else{
    cpu->p.c = 0 ;
    cpu->a = result & 0xFF;
  }
  cpu->p.n = (cpu->a >> 7) & 0x01 ? 1 : 0;
  cpu->p.z = cpu->a == 0 ? 1 : 0;
  cpu->pc += (uint8_t)0x3;
}
void
ADC_ABSX_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand+cpu->x);
  uint16_t result = cpu->a + value;
 result = cpu->p.c ? result + ( (cpu->p.c? 1 : 0) | 0x00 ) : result;
  /* Overflow */
  if( result > 0xFF){
    cpu->p.c = (result >> 8) & 0x01 ? 1: 0 ;
    cpu->p.v = 1;
    cpu->a = result & 0xFF;
  } else{
    cpu->p.c = 0 ;
    cpu->a = result & 0xFF;
  }
  cpu->p.n = (cpu->a >> 7) & 0x01 ? 1 : 0;
  cpu->p.z = cpu->a == 0 ? 1 : 0;
  cpu->pc += (uint8_t)0x3;
}
void
ADC_ABSY_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand+cpu->y);
  uint16_t result = cpu->a + value;
  result = cpu->p.c ? result + ( (cpu->p.c? 1 : 0) | 0x00 ) : result;
  /* Overflow */
  if( result > 0xFF){
    cpu->p.c = (result >> 8) & 0x01 ? 1: 0 ;
    cpu->p.v = 1;
    cpu->a = result & 0xFF;
  } else{
    cpu->p.c = 0 ;
    cpu->a = result & 0xFF;
  }
  cpu->p.n = (cpu->a >> 7) & 0x01 ? 1 : 0;
  cpu->p.z = cpu->a == 0 ? 1 : 0;
  cpu->pc += (uint8_t)0x3;
}
void
ADC_IDX_IDR_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t lo = operand + cpu->x;
  uint8_t hi = lo + 1;	
  uint16_t effective_addr = (hi << 8) | lo;
  uint8_t value = read8(cpu, effective_addr);
  uint16_t result = cpu->a + value;
  result = cpu->p.c ? result + ( (cpu->p.c? 1 : 0) | 0x00 ) : result;
  /* Overflow */
  if( result > 0xFF){
    cpu->p.c = (result >> 8) & 0x01 ? 1: 0 ;
    cpu->p.v = 1;
    cpu->a = result & 0xFF;
  } else{
    cpu->p.c = 0 ;
    cpu->a = result & 0xFF;
  }
  cpu->p.n = (cpu->a >> 7) & 0x01 ? 1 : 0;
  cpu->p.z = cpu->a == 0 ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
ADC_IDR_IDX_handler(mos6502_t *cpu){
  uint8_t first = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t second = first + 1;
  uint8_t lo = read8(cpu,first + cpu->y);
  uint8_t hi = read8(cpu, second);
  uint16_t effective_addr = (hi << 8) | lo;
  uint8_t value = read8(cpu, effective_addr);
  uint16_t result = cpu->a + value;
  result = cpu->p.c ? result + ( (cpu->p.c? 1 : 0) | 0x00 ) : result;
  /* Overflow */
  if( result > 0xFF){
    cpu->p.c = (result >> 8) & 0x01 ? 1: 0 ;
    cpu->p.v = 1;
    cpu->a = result & 0xFF;
  } else{
    cpu->p.c = 0 ;
    cpu->a = result & 0xFF;
  }
  cpu->p.n = (cpu->a >> 7) & 0x01 ? 1 : 0;
  cpu->p.z = cpu->a == 0 ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
AND_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  uint8_t res = (cpu->a & operand);
  cpu->a = res;
  cpu->p.z = !res ? 1 : cpu->p.z;
  cpu->p.n = (res >> 7) & 0x1 ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x2;
}
void
AND_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)operand);
  uint8_t res = (cpu->a & value);
  cpu->a = res;
  cpu->p.z = !res ? 1: cpu->p.z;
  cpu->p.n = (res >> 7) & 0x1 ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x2;
}
void
AND_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc+(uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)(operand+cpu->x));
  uint8_t res = (value) & cpu->a;
  cpu->a = res;
  cpu->p.z = !res ? 1 : cpu->p.z;
  cpu->p.n = (res >> 7) & 0x1 ? 1 : cpu->p.n; 
  cpu->pc += (uint8_t)0x2;
}
void
AND_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc+1);
  uint8_t value = read8(cpu, operand);
  uint8_t res = (value) & cpu->a;
  cpu->a = res;
  cpu->p.z = !res ? 1: cpu->p.z;
  cpu->p.n = ( res >> 7 ) & 0x1 ? 1: cpu->p.n;;
  cpu->pc += (uint8_t)0x3;
}
void
AND_ABSX_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc+1);
  uint8_t value = read8(cpu, operand+cpu->x);
  uint8_t res = (value) & cpu->a;
  cpu->a =res;
  cpu->p.z = !res ? 1 : cpu->p.z;
  cpu->p.n = (res >> 7) & 0x1 ? 1: cpu->p.n;
  cpu->pc += (uint8_t)0x3;
}

void
AND_ABSY_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc+1);
  uint8_t value = read8(cpu, operand+cpu->y);
  uint8_t res = (value) & cpu->a;
  cpu->a = res;
  cpu->p.z = !res ? 1: cpu->p.z;
  cpu->p.n = (res >> 7) & 0x1 ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x3;
}
void
AND_IDX_IDR_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t lo = operand + cpu->x;
  uint8_t hi = lo + 1;	
  uint16_t effective_addr = (hi << 8) | lo;
  uint8_t value = read8(cpu, effective_addr);
  uint8_t res = (value) & cpu->a;

  cpu->a = res;
  cpu->p.z = !res ? 1 : cpu->p.z;
  cpu->p.n = (res >> 7) & 0x1 ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x2;
}

void
AND_IDR_IDX_handler(mos6502_t *cpu){
  uint8_t first = read8(cpu, cpu->pc + 1);
  uint8_t secnd = first + 1;
  uint8_t lo = read8(cpu, first + cpu->y);
  uint8_t hi = read8(cpu, secnd);
  uint16_t effective_addr = (hi << 8) | lo;
  uint16_t effective_value = (uint16_t)read8(cpu, effective_addr);  
  uint8_t res = (effective_value) & cpu->a;
  cpu->a = res;
  cpu->p.z = !res ? 1 : cpu->p.z;
  cpu->p.n = (res >> 7) & 0x1 ? 1: cpu->p.n;
  cpu->pc += (uint8_t)0x2;
}
/*
MODE           SYNTAX       HEX LEN TIM
Accumulator   ASL A         $0A  1   2
Zero Page     ASL $44       $06  2   5
Zero Page,X   ASL $44,X     $16  2   6
Absolute      ASL $4400     $0E  3   6
Absolute,X    ASL $4400,X   $1E  3   7
*/
void
ASL_handler(mos6502_t *cpu){
  cpu->p.c = (cpu->a >> 7);
  cpu->a = (cpu->a << 1);
  cpu->p.z = cpu->a == 0 ? 1: cpu->p.z;
  cpu->p.n = (cpu->a >> 7 ) & 0x1 ? 1: cpu->p.n;
  cpu->pc += (uint8_t)0x1;
}
void
ASL_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc+(uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)operand);
  cpu->p.c = value >> 7;
  write8(cpu, operand, (value << 1) & 0x00FF);
  cpu->p.z = !((value << 1) & 0x00FF) ? 1: cpu->p.z;
  //  cpu->p.n =
  cpu->pc +=(uint8_t)0x2;
}
void
ASL_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc+(uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)(operand + cpu->x));
  cpu->p.c = value >> 7;
  write8(cpu, operand + cpu->x, (value << 1) & 0x00FF);
  cpu->p.z = !((value << 1) & 0x00FF) ? 1 : cpu->p.z;
  cpu->pc += (uint8_t)0x2;
}
void
ASL_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand);
  cpu->p.c = value >> 7;
  write8(cpu, operand, (value << 1) & 0x00FF);
  cpu->p.z = !((value<<1) & 0x00FF) ? 1: cpu->p.z;
  cpu->pc += (uint8_t)0x3;
}
void
ASL_ABSX_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand + cpu->x);
  cpu->p.c = value >> 7;
  write8(cpu, operand+(uint16_t)cpu->x, (value << 1) & 0x00FF);
  cpu->p.z = !((value <<1)&0x00FF)? 1: cpu->p.z;
  cpu->pc += (uint8_t)0x3;

}
/*
Mode           SYNTAX       HEX LEN TIM
Zero Page     BIT $44       $24  2   3
Absolute      BIT $4400     $2C  3   4
*/
void
BIT_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  uint8_t value = membus_read(cpu->bus, operand);
  cpu->p.z = value & cpu->a ? 1 : 0;
  cpu->p.n = (value >> 7) ? 1 : 0; // n is for the negative(sign) flag;
  cpu->p.v = (value >> 6) & 0x01 ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
BIT_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + 1);
  uint8_t value = read8(cpu, operand);
  cpu->p.z = value & cpu-> a ? 1: 0;
  cpu->p.n = (value >> 7) ? 1: 0;
  cpu->p.v = (value >> 6) & 0x01? 1: 0;
  cpu->pc += (uint8_t)0x3;
}
void
BCC_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc+1);
  uint8_t new_operand = convert(operand);
  cpu->pc += 0x2;
  if( (cpu->p).c == 0){
    cpu->pc = operand >> 7 ? cpu->pc - new_operand : cpu->pc + operand;
  }
}
void
BCS_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + 1);
  uint8_t new_operand = convert(operand);
  cpu->pc += 0x2;
  if( (cpu->p).c == 1){
    cpu->pc = operand >> 7 ? cpu->pc - new_operand : cpu->pc + operand;
  }
}
void
BEQ_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + 1);
  uint8_t new_operand = convert(operand);
  cpu->pc += 0x2;
  if( (cpu->p).z == 1){
    cpu->pc = operand >> 7 ? cpu->pc - new_operand : cpu->pc + operand;
  }
}

void
BMI_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + 1);
  uint8_t new_operand = convert(operand);
  cpu->pc += 0x2;
  if( (cpu->p).n == 1){
    cpu->pc = operand >> 7 ? cpu->pc - new_operand : cpu->pc + operand;
  }
}
void
BNE_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + 1);
  uint8_t new_operand = convert(operand);
  cpu->pc += 0x2;
  if( (cpu->p).z == 0){
    cpu->pc = operand >> 7 ? cpu->pc - new_operand : cpu->pc + operand;
  }
}

void
BPL_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + 1);
  uint8_t new_operand = convert(operand);
  cpu->pc += 0x2;
  if( cpu->p.n == 0){
    cpu->pc = operand >> 7 ? cpu->pc - new_operand : cpu->pc + operand;
  }
}

void
BRK_handler(mos6502_t *cpu){
  cpu->pc+=2; // In order to throw it way
  write8(cpu, cpu->sp--, (cpu->pc >> 8) & 0xFF);
  write8(cpu, cpu->sp--, (cpu->pc)& 0x00FF);
  write8(cpu, cpu->sp--, cpu->p.val);
  uint8_t lo = read8(cpu, 0xFFFE);
  uint8_t hi = read8(cpu, 0xFFFF);
  cpu->p.b = 1;
  uint8_t val = (hi << 8) | lo;
  cpu->pc = val;
}
void
BVC_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + 1);
  uint8_t new_operand = convert(operand);
  cpu->pc += 0x2;
  if( (cpu->p).v == 0){
    cpu->pc = operand >> 7 ? cpu->pc - new_operand : cpu->pc + operand;
  }
}
void
BVS_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + 1);
  uint8_t new_operand = convert(operand);
  cpu->pc += 0x2;
  if( (cpu->p).v == 1){
    cpu->pc = operand >> 7 ? cpu->pc - new_operand : cpu->pc + operand;
  }
}
void
CLC_handler(mos6502_t *cpu){
  cpu->p.c = 0;
  cpu->pc += (uint8_t)0x1;
}

void
CLI_handler(mos6502_t *cpu){
  cpu->p.i = 0;
  cpu->pc += (uint8_t)0x1;
}
void
CLV_handler(mos6502_t *cpu){
  cpu->p.v = 0;
  cpu->pc += (uint8_t)0x1;
} 
void
CLD_handler(mos6502_t *cpu){ 
  cpu->p.d = 0;
  cpu->pc = cpu->pc + (uint8_t)0x1;
}
/*
MODE           SYNTAX       HEX LEN TIM
Immediate     CMP #$44      $C9  2   2
Zero Page     CMP $44       $C5  2   3
Zero Page,X   CMP $44,X     $D5  2   4
Absolute      CMP $4400     $CD  3   4
Absolute,X    CMP $4400,X   $DD  3   4+
Absolute,Y    CMP $4400,Y   $D9  3   4+
Indirect,X    CMP ($44,X)   $C1  2   6
Indirect,Y    CMP ($44),Y   $D1  2   5+
*/
/*
If the value in the accumulator is equal or greater than the compared value,
the Carry will be set.
?
The equal (Z) and sign (S) flags will be set based on equality or lack thereof and the sign (i.e. A>=$80) of the accumulator.
 */
void
CMP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  if( cpu->a >= operand){
    cpu->p.c = 1;
  }
  cpu->p.n = (cpu->a - operand) >> 7 ? 1 : 0 ;
  cpu->p.z = cpu->a == operand ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
CMP_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)operand);
  if ( cpu->a >= value) {
    cpu->p.c = 1;
  }
  cpu->p.n = (cpu->a - value) >> 7 ? 1 : 0;
  cpu->p.z = cpu->a == value ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
CMP_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)(operand + cpu->x));
  if ( cpu->a >= value){
    cpu->p.c = 1;
  }
  cpu->p.n = (cpu->a - value) >> 7 ? 1 : 0;
  cpu->p.z = cpu->a == value ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
CMP_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand);
  if ( cpu-> a >= value){
    cpu->p.c = 1;
  }
  cpu->p.n = (cpu->a - value) >> 7 ? 1 : 0;
  cpu->p.z = cpu->a == value ? 1 : 0;
  cpu->pc += (uint8_t)0x3;
}
void
CMP_ABSX_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand+(uint16_t)cpu->x);
  if ( cpu->a >= value){
    cpu->p.c = 1;
  }
  cpu->p.n = (cpu->a - value) >> 7 ? 1 : 0;
  cpu->p.z = cpu->a == value ? 1 : 0;
  cpu->pc +=(uint8_t)0x3;
}
void
CMP_ABSY_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand+(uint16_t)cpu->y);
  if(cpu->a >= value){
    cpu->p.c = 1;
  }
  cpu->p.n = (cpu->a - value) >> 7 ? 1 : 0;
  cpu->p.z = cpu->a == value ? 1 : 0;
  cpu->pc += (uint8_t)0x3;
}
void
CMP_IDX_IDR_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t lo = operand + cpu->x;
  uint8_t hi = lo + 1;
  uint16_t effective_addr = (hi << 8) | lo;
  uint8_t value = read8(cpu, effective_addr);
  if( cpu->a >= value){
    cpu->p.c = 1;
  }
  cpu->p.n = (cpu->a - value) >> 7 ? 1 : 0;
  cpu->p.z = cpu->a == value ? 1 : 0;
  cpu->pc += (uint8_t)0x2;

}
void
CMP_IDR_IDX_handler(mos6502_t *cpu){
  uint8_t first = read8(cpu, cpu->pc + 1);
  uint8_t secnd = first + 1;
  uint8_t lo = read8(cpu, first + cpu->y);
  uint8_t hi = read8(cpu, secnd);
  uint16_t effective_addr = (hi << 8) | lo;
  uint8_t effective_value = read8(cpu, effective_addr);
  if( cpu->a >= effective_value){
    cpu->p.c = 1;
  }
  cpu->p.n = (cpu->a - effective_value) >> 7 ? 1 : 0;
  cpu->p.z = cpu->a == effective_value ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}

/*
MODE           SYNTAX       HEX LEN TIM
Immediate     CPX #$44      $E0  2   2
Zero Page     CPX $44       $E4  2   3
Absolute      CPX $4400     $EC  3   4
*/
void
CPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  if( cpu->x >= operand){
    cpu->p.c = 1;
  }
  cpu->p.n = (cpu->x - operand) >> 7 ? 1 : 0;
  cpu->p.z = cpu->x == operand ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
CPX_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)operand);
  if( cpu->x >= value){
    cpu->p.c =1;
  }
  cpu->p.n = (cpu->x - value) >> 7 ? 1 : 0;
  cpu->p.z = cpu->x == value ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
CPX_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand);
  if ( cpu->x >= value){
    cpu->p.c=1;
  }
  cpu->p.n = (cpu->x - value) >> 7 ? 1 : 0;
  cpu->p.z = cpu->x == value ? 1 : 0;
  cpu->pc += (uint8_t)0x3;
}

void
CPY_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  if( cpu->y >= operand){
    cpu->p.c = 1;
  }
  cpu->p.n = (cpu->y - operand ) >> 7 ? 1: 0;
  cpu->p.z = cpu->y == operand ? 1 : 0;

  cpu->pc += (uint8_t)0x2;
}
void
CPY_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)operand);
  if( cpu->y >= value){
    cpu->p.c = 1;
  }
  cpu->p.n = (cpu->y - value) >> 7 ? 1 : 0;
  cpu->p.z = cpu->y == value ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
CPY_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, (uint16_t)operand);
  if(cpu->y >= value){
    cpu->p.c = 1;
  }
  cpu->p.n = (cpu->y - value) >> 7 ? 1 : 0;
  cpu->p.z = cpu->y == value ? 1 : 0;
  cpu->pc += (uint8_t)0x3;
}
void
DEC_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)+1); // it stores the value in the operand memory location
  uint8_t value = read8(cpu, operand);
  write8(cpu, operand, --value);
  cpu->p.n = value >> 7 ? 1 : 0;
  cpu->p.z = value == 0 ? 1 : 0;
  cpu->pc +=(uint8_t)0x2;
}
void
DEC_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)+1);
  uint8_t value = read8(cpu, (uint16_t)(operand+cpu->x));
  write8(cpu, operand, --value);
  cpu->p.n = value >> 7 ? 1 : 0;
  cpu->p.z = value == 0 ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
DEC_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc+(uint16_t)1);
  uint8_t value = read8(cpu, operand);
  write8(cpu, operand, --value);
  cpu->p.n = value >> 7 ? 1 : 0;
  cpu->p.z = value == 0 ? 1 : 0;
  cpu->pc += (uint8_t)0x3;
}
void
DEC_ABSX_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc +(uint16_t)1);
  uint8_t value = read8(cpu, operand+cpu->x);
  write8(cpu, operand + (uint16_t)cpu->x, --value);
  cpu->p.n = value >> 7 ? 1 : 0;
  cpu->p.z = value == 0 ? 1 : 0;
  cpu->pc += (uint8_t)0x3;
}
void
DEX_handler(mos6502_t *cpu){
  cpu->x--;
  cpu->p.n = cpu->x >> 7 ? 1 : 0;
  cpu->p.z = cpu->x == 0 ? 1 : 0;
  cpu->pc += (uint8_t)0x1;
}
void
DEY_handler(mos6502_t *cpu){
  cpu->y--;
  cpu->p.n = cpu->y >> 7 ? 1 : 0;
  cpu->p.z = cpu->y == 0 ? 1 : 0;
  cpu->pc += (uint8_t)0x1;
}
/*
MODE           SYNTAX       HEX LEN TIM
Immediate     EOR #$44      $49  2   2
Zero Page     EOR $44       $45  2   3
Zero Page,X   EOR $44,X     $55  2   4
Absolute      EOR $4400     $4D  3   4
Absolute,X    EOR $4400,X   $5D  3   4+
Absolute,Y    EOR $4400,Y   $59  3   4+
Indirect,X    EOR ($44,X)   $41  2   6
Indirect,Y    EOR ($44),Y   $51  2   5
*/
void
EOR_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t val = operand ^ cpu->a;
  cpu->p.z = !val ? 1 : cpu->p.z;
  cpu->a = val;
  cpu->pc += (uint8_t)0x2;
}
void
EOR_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)operand) ^ cpu->a;
  cpu->p.z = !value ? 1 : cpu->p.z;
  cpu->a = value;
  cpu->pc += (uint8_t)0x2;
}

void
EOR_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)(operand + cpu->x)) ^ cpu->a;
  cpu->p.z = !value ? 1 : cpu->p.z;
  cpu->a = value;
  cpu->pc += (uint8_t)0x2;
}
void
EOR_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand) ^ cpu->a;
  cpu->p.z = !value ? 1 : cpu->p.z;
  cpu->a = value;
  cpu->pc += (uint8_t)0x3;
}
void
EOR_ABSX_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand + cpu->x) ^ cpu->a;
  cpu->p.z = !value ? 1: cpu->p.z;
  cpu->a = value;
  cpu->pc += (uint8_t)0x3;
}
void
EOR_ABSY_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand + cpu->y) ^ cpu->a;
  cpu->p.z = !value ? 1: cpu->p.z;
  cpu->a = value;
  cpu->pc += (uint8_t)0x3;
}
void
EOR_IDX_IDR_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t lo = operand + cpu->x;
  uint8_t hi = lo + 1;
  uint16_t effective_addr = (hi << 8) | lo;
  uint8_t value = read8(cpu, effective_addr) ^ cpu->a;
  cpu->p.z = !value ? 1 : cpu->p.z;
  cpu-> a = value;
  cpu->pc += (uint8_t)0x2;
}
void
EOR_IDR_IDX_handler(mos6502_t *cpu){
  uint8_t first = read8(cpu, cpu->pc + 1);
  uint8_t secnd = first + 1;
  uint8_t lo = read8(cpu, first + cpu->y);
  uint8_t hi = read8(cpu, secnd);
  uint16_t effective_addr = (hi << 8) | lo;
  uint8_t effective_value = read8(cpu, effective_addr);
  cpu->p.z = !(effective_value) ? 1 : cpu->p.z;
  cpu->a = effective_value;
  cpu->pc += (uint8_t)0x2;
}
void
INC_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)operand);
  write8(cpu, operand, ++value);
  cpu->p.n = value >> 7 ? 1 : 0;
  cpu->p.z = value == 0 ? 1: 0;
  cpu->pc += (uint8_t)0x2;
}
void
INC_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)(operand + cpu->x));
  write8(cpu, operand, ++value);
  cpu->p.n = value >> 7 ? 1 : 0;
  cpu->p.z = value == 0 ? 1: 0;
  cpu->pc += (uint8_t)0x2;
}

void
INC_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand);
  write8(cpu, operand, ++value);
  cpu->p.n = value >> 7 ? 1 : 0;
  cpu->p.z = value == 0 ? 1: 0;
  cpu->pc += (uint8_t)0x3;
}
void
INC_ABSX_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand+cpu->x);
  write8(cpu, operand+(cpu->x), ++value);
  cpu->p.n = value >> 7 ? 1 : 0;
  cpu->p.z = value == 0 ? 1: 0;
  cpu->pc += (uint8_t)0x3;
}
void
INX_handler(mos6502_t *cpu){
  cpu->x++;
  cpu->p.n = cpu->x >> 7 ? 1 : 0;
  cpu->p.z = cpu->x == 0 ? 1 : 0;
  cpu->pc += 1;
}

void
INY_handler(mos6502_t *cpu){
  cpu->y++;
  cpu->p.n = cpu->y >> 7 ? 1 : 0;
  cpu->p.z = cpu->y == 0 ? 1 : 0;
  cpu->pc += 1;
}
void
JMP_ABS_handler(mos6502_t *cpu){
  uint8_t lo = read8(cpu, cpu->pc+1);
  uint8_t hi = read8(cpu, cpu->pc+2);
  cpu->pc = hi << 8 | lo;
}
void
JMP_IDR_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc+1);
  uint8_t low_value = read8(cpu, operand);
  uint8_t high_value = read8(cpu, operand + 1);
  uint16_t value = (high_value << 8) | low_value;
  cpu->pc = value;
}
void
LDX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  cpu->x = operand;
  cpu->p.z = cpu->x == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->x >> 7) ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x2;
}
void
LDX_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand);
  cpu->x = value;
  cpu->p.z = cpu->x == 0 ? 1: cpu->p.z;
  cpu->p.n = (cpu->x >> 7) ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x2;
}
void
LDX_ZPY_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand + (uint16_t)cpu->y);
  cpu->x = value;
  cpu->p.z = cpu->x == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->x >> 7) ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x2;
}
void
LDX_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand);
  cpu->x = value;
  cpu->p.z = cpu->x == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->x >> 7) ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x3;
}
void
LDX_ABSY_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand+(uint16_t)cpu->y);
  cpu->x = value;
  cpu->p.z = cpu->x == 0 ? 1: cpu->p.z;
  cpu->p.n = (cpu->x >> 7) ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x3;
}

void
LDA_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  cpu->a = operand;
  cpu->p.z = cpu->a == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->a >> 7) ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x2;
}
void
LDA_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc+1);
  uint8_t value = read8(cpu, (uint16_t)operand);
  cpu->a = value;
  cpu->p.z = cpu->a == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->a >> 7) ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x2;

}
void
LDA_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc+(uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)(operand+cpu->x));
  cpu->a = value;
  cpu->p.z = cpu->a == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->a >> 7) ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x2;
}
void
LDA_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc+1);
  uint8_t value = read8(cpu, operand);
  cpu->a = value;
  cpu->p.z = cpu->a == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->a >> 7)? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x3;
}
void
LDA_ABSX_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc+1);
  uint8_t value = read8(cpu, operand + cpu->x);
  cpu->a = value;
  cpu->p.z = cpu->a == 0 ? 1: cpu->p.z;
  cpu->p.n = (cpu->a >> 7) ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x3;
}
void
LDA_ABSY_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc+1);
  uint8_t value = read8(cpu, operand + cpu->y);
  cpu->a = value;
  cpu->p.z = cpu->a == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->a >> 7) ? 1: cpu->p.n;
  cpu->pc += (uint8_t)0x3;
}
void
LDA_IDX_IDR_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t lo = operand + cpu->x;
  uint8_t hi = lo + 1;
  uint16_t effective_addr = (hi << 8) | lo;
  uint8_t value = read8(cpu, effective_addr);
  cpu->a = value;
  cpu->p.z = cpu->a == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->a >> 7) ? 1: cpu->p.n;
  cpu->pc += (uint8_t)0x2;

}
void
LDA_IDR_IDX_handler(mos6502_t *cpu){
  uint8_t first = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t secnd = first + 1;
  uint8_t lo = read8(cpu, first + cpu->y);
  uint8_t hi = read8(cpu, secnd);
  uint16_t effective_addr = hi << 8 | (lo + cpu->y);
  uint8_t effective_value = read8(cpu, effective_addr);
  cpu->a = effective_value;
  cpu->p.z = cpu->a == 0 ? 1: cpu->p.z;
  cpu->p.n = (cpu->a >> 7) ? 1: cpu->p.n;
  cpu->pc += (uint8_t)0x2;
}
/*
 modes	       syntax	   opcode length
 accumulator   LSR A         4A    1     2
 zeropage      LSR oper      46    2     5
 zeropage,X    LSR oper,X    56    2     6
 absolute      LSR oper      4E    3     6
 absolute,X    LSR oper,X    5E    3     7
 */
void
LSR_handler(mos6502_t *cpu){
  cpu->p.c = (cpu->a &0x1) ? 1: 0 ;
  cpu->a = cpu->a >> 1;
  cpu->p.z = cpu->a == 0? 1: cpu->p.z ;
  cpu->p.n = (cpu->a >> 7) & 0x01 ? 1: cpu->p.n;
  cpu->pc += (uint8_t)0x1;
}

void
LSR_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)operand);
  cpu->p.c = (value & 0x1) ? 1: 0;
  write8(cpu, operand, (value >> 1) & 0xFF);
  cpu->p.z = ((value >> 1) && 0xFF) == 0 ? 1: cpu->p.z ;
  cpu->p.n = (value >> 7) & 0x01 ? 1: cpu->p.n;
  cpu->pc += (uint8_t)0x2;
}

void
LSR_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, (cpu->pc)+(uint8_t)1);
  uint8_t value = read8(cpu, (uint16_t)(operand + cpu->x));
  cpu->p.c = (value & 0x1) ? 1 : 0;
  write8(cpu, operand + cpu->x , (value >> 1) & 0xFF);
  cpu->p.z = ((value >> 1) && 0xFF) == 0 ? 1: cpu->p.z;
  cpu->p.n = (value >> 7) & 0x01 ? 1: cpu->p.n;
  cpu->pc += (uint8_t)0x2;
}

void
LSR_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand);
  cpu->p.c = (value & 0x1) ? 1: 0;
  write8(cpu, operand , (value >> 1) & 0xFF);
  cpu->p.z = ((value >> 1) && 0xFF) == 0 ? 1: cpu->p.z;
  cpu->p.n = (value >> 7) & 0x01 ? 1: cpu->p.n;
  cpu->pc += (uint8_t)0x3;

}
void
LSR_ABSX_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand + cpu->x);
  cpu->p.c = (value & 0x1) ? 1 : 0;
  write8(cpu, operand + cpu->x , (value >> 1) & 0xFF);
  cpu->p.z = ((value >> 1) && 0xFF) == 0 ? 1: cpu->p.z;
  cpu->p.n = (value >> 7) & 0x01 ? 1: cpu->p.n;
  cpu->pc += (uint8_t)0x3;
}
void
TXS_handler(mos6502_t *cpu){
  cpu->sp = cpu->x;
  cpu->pc += 1;
}

void
STA_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  write8(cpu, operand, cpu->a);
  cpu->pc += (uint8_t)0x2;
}
void
STA_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  write8(cpu, operand + cpu->x, cpu->a);
  cpu->pc += (uint8_t)0x2;
}
void
STA_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  write8(cpu, operand, cpu->a);
  cpu->pc += (uint8_t)0x3;
}
void
STA_ABSX_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  write8(cpu, operand + cpu->x, cpu->a);
  cpu->pc += (uint8_t)0x3;
}
void
STA_ABSY_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  write8(cpu, operand + cpu->y, cpu->a);
  cpu->pc += (uint8_t)0x3;
}
void
STA_IDX_IDR_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t lo = operand + cpu->x;
  uint8_t hi = lo + 1;
  uint8_t effective_addr = (hi << 8) | lo;
  write8(cpu, effective_addr,cpu->a);
  cpu->pc += (uint8_t)0x2;
}
void
STA_IDR_IDX_handler(mos6502_t *cpu){
  uint8_t first = read8(cpu, cpu->pc+1);
  uint8_t secnd = first + 1;
  uint8_t lo = read8(cpu, first + cpu->y);
  uint8_t hi = read8(cpu, secnd);
  uint16_t effective_addr = (hi << 8) | lo;
  write8(cpu, effective_addr, cpu->a);
  cpu->pc += (uint8_t)0x2;
}

void
STX_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  write8(cpu, (uint16_t)operand, cpu->x);
  cpu->pc += (uint8_t)0x2;
}
void
STX_ZPY_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  write8(cpu, (uint16_t)operand, cpu->x);
  cpu->pc += (uint8_t)0x1;
}
void
STX_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  write8(cpu, operand, cpu->x);
  cpu->pc += (uint8_t)0x3;
}
void
STY_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  write8(cpu, (uint16_t)operand, cpu->y);
  cpu->pc += (uint8_t)0x2;
}
void
STY_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  write8(cpu, (uint16_t)operand, cpu->y);
  cpu->pc += (uint8_t)0x2;
}
void
STY_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  write8(cpu, operand, cpu->y);
  cpu->pc += (uint8_t)0x3;
}
void
LDY_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  cpu->y = operand;
  cpu->p.z = cpu->y == 0 ? 1 : 0;
  cpu->p.n = (cpu->y >> 7 ) & 0x1 ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
LDY_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand);
  cpu->y = value;
  cpu->p.z = cpu->y == 0 ? 1 : 0;
  cpu->p.n = (cpu->y >> 7 ) & 0x1 ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}

void
LDY_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand + (uint16_t)cpu->x);
  cpu->y = value;
  cpu->p.z = cpu->y == 0 ? 1 : 0;
  cpu->p.n = (cpu->y >> 7 ) & 0x1 ? 1 : 0;
  cpu->pc += (uint8_t)0x2;
}
void
LDY_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand);
  cpu->y = value;
  cpu->p.z = cpu->y == 0 ? 1 : 0;
  cpu->p.n = (cpu->y >> 7 ) & 0x1 ? 1 : 0;
  cpu->pc += (uint8_t)0x3;
}
void
LDY_ABSX_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand + cpu->x);
  cpu->y = value;
  cpu->p.z = cpu->y == 0 ? 1 : 0;
  cpu->p.n = (cpu->y >> 7 ) & 0x1 ? 1 : 0;
  cpu->pc += (uint8_t)0x3;
}
void
NOP_handler(mos6502_t *cpu){
  cpu->pc += (uint8_t)0x1;
}
void
ORA_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  cpu->a |= operand;
  cpu->p.n = (cpu->a >> 7) & 0x1 ? 1: cpu->p.n;
  cpu->p.z = cpu->a == 0 ? 1 : cpu->p.z;
  cpu->pc += (uint8_t)0x2;
}
void
ORA_ZP_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand);
  cpu->a |= value;
  cpu->p.n = (cpu->a >> 7) &0x1 ? 1 : cpu->p.n;
  cpu->p.z = (cpu->a == 0) ? 1 : cpu->p.z;
  cpu->pc += (uint8_t)0x2;
}
void
ORA_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand + cpu->x);
  cpu->a |= value;
  cpu->p.n = (cpu->a  >> 7) & 0x1 ? 1: 0;
  cpu->p.z = (cpu->a == 0) ? 1: 0;
  cpu->pc += (uint8_t)0x2;
}
void
ORA_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + 1);
  uint8_t value = read8(cpu, operand);
  cpu->a |= value;
  cpu->p.n = (cpu->a >> 7) &0x1 ? 1: 0;
  cpu->p.z = (cpu->a == 0) ? 1: 0;
  cpu->pc += (uint8_t)0x3;
}
void
ORA_ABSX_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + 1);
  uint8_t value = read8(cpu, operand-cpu->x);
  cpu->a |= value;
  cpu->p.n = (cpu->a >> 7) &0x1 ? 1 : 0;
  cpu->p.z = (cpu->a== 0) ? 1: 0;
  cpu->pc += (uint8_t)0x3;
}
void
ORA_ABSY_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + 1);
  uint8_t value = read8(cpu, operand-cpu->y);
  cpu->a |= value;
  cpu->p.n = (cpu->a >> 7) &0x1 ? 1 : 0;
  cpu->p.z = (cpu->a == 0) ? 1: 0;
  cpu->pc += (uint8_t)0x3;
}
void
ORA_IDX_IDR_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t lo = operand + cpu->x;
  uint8_t hi = lo + 1;
  uint16_t effective_addr = (hi << 8) | lo;
  uint8_t value = read8(cpu, effective_addr);
  cpu->a |= value;
  cpu->p.n = (cpu->a >> 7) &0x1 ? 1: 0;
  cpu->p.z = (cpu->a == 0)  ?1: 0;
  cpu->pc += (uint8_t)0x3;
}
void
ORA_IDR_IDX_handler(mos6502_t *cpu){
  uint8_t first = read8(cpu, cpu->pc + 1);
  uint8_t secnd = first + 1;
  uint8_t lo = read8(cpu, first + cpu->y);
  uint8_t hi = read8(cpu, secnd);
  uint16_t effective_addr = (hi << 8) | lo;
  uint8_t effective_value = read8(cpu, effective_addr);
  cpu->a |= effective_value;
  cpu->p.n = (cpu->a  >> 7 ) & 0x1 ? 1: 0;
  cpu->p.z = (cpu->a == 0) ? 1: 0;
  cpu->pc += (uint8_t)0x3;
}
void
JSR_ABS_handler(mos6502_t *cpu){
  uint8_t lo = (cpu->pc + 3) & 0xFF;
  uint8_t hi = (cpu->pc + 3) >> 8;
  uint16_t effectivesp = cpu->sp | 0x0100; 

  write8(cpu, effectivesp , hi);
  write8(cpu, --effectivesp , lo);
  --effectivesp;
  cpu->sp = effectivesp & 0xFF;
  cpu->pc = read16(cpu, cpu->pc+1);
}
void
PHA_handler(mos6502_t *cpu){
  uint16_t effective_sp = 0x0100 | cpu->sp;
  write8(cpu, effective_sp--, cpu->a);
  cpu->sp = effective_sp & 0xFF;
  cpu->pc += (uint8_t)0x1;
}
void
PLA_handler(mos6502_t *cpu){
  uint16_t effective_sp = 0x0100 | cpu->sp;
  cpu->a = read8(cpu, ++effective_sp);
  cpu->sp = effective_sp & 0xFF;
  cpu->p.z = cpu->a == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->a >> 7) & 0x1 ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x1;
}
void
PHP_handler(mos6502_t *cpu){
  uint16_t effective_sp = 0x100 | cpu->sp;
  write8(cpu, effective_sp--, cpu->p.val);
  cpu->sp = effective_sp & 0xFF;
  cpu->pc += (uint8_t)0x1;
}
void
PLP_handler(mos6502_t *cpu){
  uint16_t effective_sp = 0x100 | cpu->sp;
  cpu->p.val = read8(cpu,++effective_sp);
  cpu->sp = effective_sp & 0xFF;
  cpu->pc += (uint8_t)0x1;
}
void
TAX_handler(mos6502_t *cpu){
  cpu->x = cpu->a;
  cpu->p.z = cpu->x == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->x >> 7) & 0x1 ? 1 : cpu->p.n;
  cpu->pc += (uint8_t)0x1;
}
void
TXA_handler(mos6502_t *cpu){
  cpu->a = cpu->x;
  cpu->p.z = cpu->a == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->a >> 7) & 0x1 ? 1 : cpu->p.n;
  cpu->pc +=(uint8_t)0x1;
}
void
TAY_handler(mos6502_t *cpu){
  cpu->y = cpu->a;
  cpu->p.z = cpu->y == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->y >> 7) & 0x1 ? 1 : cpu->p.n;
  cpu->pc +=(uint8_t)0x1;
}

void
TYA_handler(mos6502_t *cpu){
  cpu->a = cpu->y;
  cpu->p.z = cpu->a == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->a >> 7) & 0x1 ? 1 : cpu->p.n;
  cpu->pc +=(uint8_t)0x1;
}
void
SEI_handler(mos6502_t *cpu){
  cpu->p.i = 1;
  cpu->pc += 0x01;
}
void
RTI_handler(mos6502_t *cpu){
  cpu->p.v = read8(cpu, cpu->sp++);
  uint8_t lo = read8(cpu, cpu->sp++);
  uint8_t hi = read8(cpu, cpu->sp);
  cpu->pc = (hi << 8) | lo;
}
void
RTS_handler(mos6502_t *cpu){
  uint16_t effective_sp = 0x0100 | cpu->sp;
  uint8_t lo = read8(cpu, ++effective_sp);
  uint8_t hi = read8(cpu, ++effective_sp);
  cpu->sp = effective_sp & 0xFF;
  uint16_t val = (hi << 8) | lo;
  cpu->pc = val;
}

/////
void
CLD_handler(mos6502_t *cpu){
  printf("CLD handler\n");
  cpu->p.d = 0;
  printf("%d\n", cpu->p.d);
  cpu->pc = cpu->pc + (uint16_t)1;
}

void
LDX_handler(mos6502_t *cpu){
  printf("LDX handler\n");
  printf("operand : %x\n", read16(cpu, cpu->pc));
  cpu->pc = cpu->pc + (uint16_t)2;
}

void
 LDA_handler(mos6502_t *cpu){
   printf("LDA handler\n");
   // uint8_t operand = (0x00FF) && read16(cpu, cpu->pc);
  printf("operand : %x\n", read16(cpu, cpu->pc));
   // cpu->a = value;// How to figure out the value;
  cpu->pc = cpu->pc + (uint16_t)2;
 }

void
 TXS_handler(mos6502_t *cpu){
   printf("TXS handler\n");
   cpu->sp = cpu->x;
   printf("X register : %x , A register : %x Y register : %x\n", cpu->x, cpu->a, cpu->y);
   cpu->pc = cpu->pc + (uint8_t)1;
}

void
 STA_handler(mos6502_t *cpu){
   printf("STA handler\n");
   // memory = cpu->a;// How to figure out the memory;
   printf("operand : %x\n", read16(cpu, cpu->pc));
   cpu->pc = cpu->pc + (uint16_t)2;
}
