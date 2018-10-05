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
	printf("%x\n", opcode);

   	switch(opcode){
	case 0xD8:
	  CLD_handler(cpu);
	case 0xB5:
 	case 0xAD:
 	case 0xBD:
 	case 0xB9:
 	case 0xA1:
 	case 0xB1:
 	case 0xA9:
 	  LDA_handler(cpu);
 	  break;
 	case 0x9A:
 	  TXS_handler(cpu);
 	  break;
 	case 0x85:
 	  STA_handler(cpu);
 	  break;
 	default:
 	  break;
 	}
	mos6502_advance_clk(cpu, instr_cycles[opcode]);
	return MOS6502_STEP_RESULT_SUCCESS;
}

/* Immediate $69
   Zero Page $65
   Zero Page $75 */
void
ADC_handler(mos6502_t *cpu){
 }
void
AND_handler(mos6502_t *cpu){
}
void
ASL_handler(mos6502_t *cpu){
}
void
BCC_handler(mos6502_t *cpu){
}
void
BCS_handler(mos6502_t *cpu){
}
void
BEQ_handler(mos6502_t *cpu){
}
void
BIT_handler(mos6502_t *cpu){
}
void
BMI_handler(mos6502_t *cpu){
}
void
BNE_handler(mos6502_t *cpu){
}
void
PPL_handler(mos6502_t *cpu){
}
void
BPL_handler(mos6502_t *cpu){
}
void
BRK_handler(mos6502_t *cpu){
}
void
BVC_handler(mos6502_t *cpu){
}
void
BVS_handler(mos6502_t *cpu){
}
void
CLC_handler(mos6502_t *cpu){
}
 void
CLI_handler(mos6502_t *cpu){
}
void
CLV_handler(mos6502_t *cpu){
}
void
CMP_handler(mos6502_t *cpu){
}
void
CPX_handler(mos6502_t *cpu){
}
void
CPY_handler(mos6502_t *cpu){
}
void
DEC_handler(mos6502_t *cpu){
}
void
DEX_handler(mos6502_t *cpu){
}
void
DEY_handler(mos6502_t *cpu){
}
void
EOR_handler(mos6502_t *cpu){
}
 void
INC_handler(mos6502_t *cpu){
}
void
INX_handler(mos6502_t *cpu){
}
void
INY_handler(mos6502_t *cpu){
}
void
JMP_handler(mos6502_t *cpu){
}

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
