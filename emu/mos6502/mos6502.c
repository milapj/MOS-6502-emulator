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
void (*lookup[16][16])(mos6502_t *cpu) =
  {
   {BRK_handler, ORA_IDX_IDR_handler, NULL, NULL, NULL, ORA_ZP_handler, ASL_ZP_handler, NULL, PHP_handler, ORA_handler, ASL_handler, NULL, NULL, ORA_ABS_handler, ASL_ABS_handler, NULL},
   {BPL_handler, ORA_IDR_IDX_handler, NULL, NULL, NULL, ORA_ZPX_handler, ASL_ZPX_handler, NULL, CLC_handler, ORA_ABSY_handler, NULL, NULL, NULL, ORA_ABSX_handler, ASL_ABSX_handler, NULL},
   {JSR_ABS_handler, AND_IDX_IDR_handler, NULL, NULL, BIT_handler, AND_ZP_handler, ROL_ZP_handler, NULL, PLP_handler, AND_handler, ROL_handler, NULL, BIT_ABS_handler, AND_ABS_handler, ROL_ABS_handler, NULL},
   {BMI_handler, AND_IDR_IDX_handler, NULL, NULL, NULL, AND_ZPX_handler, ROL_ZPX_handler, NULL, SEC_handler, AND_ABSY_handler, NULL, NULL, NULL, AND_ABSX_handler, ROL_ABSX_handler, NULL},
   {RTI_handler, EOR_IDX_IDR_handler, NULL, NULL, NULL, EOR_ZP_handler, LSR_ZP_handler, NULL, PHA_handler, EOR_handler, LSR_handler, NULL, JMP_ABS_handler, EOR_ABS_handler, LSR_ABS_handler, NULL},
   {BVC_handler, EOR_IDR_IDX_handler, NULL, NULL, NULL, EOR_ZPX_handler, LSR_ZPX_handler, NULL, CLI_handler, EOR_ABSY_handler, NULL, NULL, NULL, EOR_ABSX_handler, LSR_ABSX_handler, NULL},
   {RTS_handler, ADC_IDX_IDR_handler, NULL, NULL, NULL, ADC_ZP_handler, ROR_ZP_handler, NULL, PLA_handler, ADC_handler, ROR_handler, NULL, JMP_IDR_handler, ADC_ABS_handler, ROR_ABS_handler, NULL},
   {BVS_handler, ADC_IDR_IDX_handler, NULL, NULL, NULL, ADC_ZPX_handler, STX_ZP_handler, NULL, SEI_handler, ADC_ABSY_handler, NULL, NULL, NULL, ADC_ABSX_handler, ROR_ABSX_handler, NULL },
   {NULL, STA_IDX_IDR_handler, NULL, NULL, STY_ZP_handler, STA_ZP_handler, STX_ZP_handler, NULL, DEY_handler,NULL, TXA_handler, NULL, STY_ABS_handler, STA_ABS_handler, STX_ABS_handler, NULL},
   {BCC_handler, STA_IDR_IDX_handler, NULL, NULL, STY_ZPX_handler, STA_ZPX_handler, STX_ZPY_handler, NULL, TYA_handler, STA_ABSY_handler, TXS_handler, NULL, NULL, STA_ABSX_handler, NULL, NULL},
   {LDY_handler, LDA_IDX_IDR_handler, LDX_handler, NULL, LDY_ZP_handler, LDA_ZP_handler, LDX_ZP_handler, NULL, TAY_handler, LDA_handler, TAX_handler, NULL, LDY_ABS_handler, LDA_ABS_handler, LDX_ABS_handler, NULL},
   {BCS_handler, LDA_IDR_IDX_handler, NULL, NULL, LDY_ZPX_handler, LDA_ZPX_handler, LDX_ZPY_handler, NULL, CLV_handler, LDA_ABSY_handler, TSX_handler, NULL, LDY_ABSX_handler, LDA_ABSX_handler, LDX_ABSY_handler, NULL},
   {CPY_handler, CMP_IDX_IDR_handler, NULL, NULL, CPY_ZP_handler, CMP_ZP_handler, DEC_ZP_handler, NULL, INY_handler, CMP_handler, DEX_handler, NULL, CPY_ABS_handler, CMP_ABS_handler, DEC_ABS_handler, NULL},
   {BNE_handler, CMP_IDX_IDR_handler, NULL, NULL, NULL, CMP_ZPX_handler, DEC_ZPX_handler, NULL, CLD_handler, CMP_ABSY_handler, NULL, NULL, NULL, CMP_ABSX_handler, DEC_ABSX_handler, NULL},
   {CPX_handler, SBC_IDX_IDR_handler, NULL, NULL, CPX_ZP_handler, SBC_ZP_handler, INC_ZP_handler, NULL, INX_handler, SBC_handler, NOP_handler, NULL, CPX_ABS_handler, SBC_ABS_handler, INC_ABS_handler, NULL},
   {BEQ_handler, SBC_IDX_IDR_handler, NULL, NULL, NULL, SBC_ZPX_handler, INC_ZPX_handler, NULL, SED_handler, SBC_ABSY_handler, NULL, NULL, NULL, SBC_ABSX_handler, INC_ABSX_handler, NULL}
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
	char *instr_table[16][16] =
    { {"BRK","ORA X, ind","","","","ORA zpg","ASL zpg","","PHP","ORA","ASL_ABS","","","ORA abs","ASL abs",""},
      {"BPL","ORA ind, Y","","","","ORA zpg, X","ASL zpg, X","","CLC","ORA abs, Y","","","","ORA abs, X","ASL abs, X",""},
      {"JSR abs","AND X, ind","","","BIT zpg","AND zpg","ROL zpg","","PLP","AND","ROL A","","BIT abs","AND abs","ROL abs",""},
      {"BMI rel","AND ind,Y","","","","AND zpg,X","ROL zpg,X","","SEC",	"AND abs,Y","","","","AND abs,X","ROL abs,X","",},
      {"RTI impl","EOR X,ind","","","","EOR zpg","LSR zpg","","PHA impl","EOR","LSR A","","JMP abs","EOR abs","LSR abs",""},
      {"BVC rel","EOR ind,Y","","","","EOR zpg,X","LSR zpg,X","","CLI impl","EOR abs,Y","","","","EOR abs,X","LSR abs,X",""},
      {"RTS impl","ADC X,ind","","","","ADC zpg","ROR zpg","","PLA impl","ADC #","ROR A","","JMP ind","ADC abs","ROR abs",""},
      {"BVS rel","ADC ind,Y","","","","ADC zpg,X","ROR zpg,X","","SEI impl","ADC abs,Y","","","","ADC abs,X","ROR abs,X",""},
      {"","STA X,ind","","","STY zpg","STA zpg","STX zpg","","DEY impl","","TXA impl","","STY abs","STA abs","STX abs",""},
      {"BCC rel","STA ind,Y","","","STY zpg,X","STA zpg,X","STX zpg,Y","","TYA impl","STA abs,Y","TXS impl","","","STA abs,X","",""},     
      {"LDY #","LDA X,ind","LDX #","","LDY zpg","LDA zpg","LDX zpg","","TAY impl","LDA #","TAX impl","","LDY abs","LDA abs","LDX abs",""},
      {"BCS rel","LDA ind,Y","","","LDY zpg,X","LDA zpg,X","LDX zpg,Y","","CLV impl","LDA abs,Y","TSX impl","","LDY abs,X","LDA abs,X",	"LDX abs,Y",""},
      {"CPY #","CMP X,ind","","","CPY zpg","CMP zpg","DEC zpg","","INY impl","CMP #","DEX impl","","CPY abs","CMP abs","DEC abs",""},
      {"BNE rel","CMP ind,Y","","","","CMP zpg,X","DEC zpg,X","","CLD impl","CMP abs,Y","","","","CMP abs,X","DEC abs,X",""},
      {"CPX #","SBC X,ind","","","CPX zpg","SBC zpg","INC zpg","","INX impl","SBC #","NOP impl","","CPX abs","SBC abs","INC abs",""},
      {"BEQ rel","SBC ind,Y","","","","SBC zpg,X","INC zpg,X","","SED impl","SBC abs,Y","","","","SBC abs,X","INC abs,X",""}
    };

  uint8_t opcode = read8(cpu, addr);
  uint8_t hi = opcode >> 4;
  uint8_t lo = opcode & 0x0F;
  snprintf(buffer, buflen, "%s\n", instr_table[hi][lo]);
  return 0;
}

mos6502_step_result_t
mos6502_step (mos6502_t * cpu)
{
  uint8_t opcode = read8(cpu, cpu->pc);
  uint8_t hi = opcode >> 4;
  uint8_t lo = opcode & 0x0F;
  decode_info_t decode_info;
  switch(opcode){
  case 0x80:
    decode_info.cpu = cpu;
    decode_info.opcode = opcode;
    decode_info.addr = cpu->pc+1;
    handle_vmcall(&decode_info);
    break;
  default:
    (*lookup[hi][lo])(cpu);
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
void
TSX_handler(mos6502_t *cpu){
  cpu->x = cpu->sp;
  cpu->p.z = cpu->x == 0 ? 1: 0;
  cpu->p.n = (cpu->x >> 7) &0x1 ? 1:0;
  cpu->pc += (uint8_t)0x1;
}
void
SED_handler(mos6502_t *cpu){
  cpu->p.d = 1;
  cpu->pc += (uint8_t)0x1;
}
void
SEC_handler(mos6502_t *cpu){
  cpu->p.c = 1;
  cpu->pc += (uint8_t)0x1;
}
/*ROL & ROR Family
 *
 *modes	   	  syntax      opcode bytes  cyles
 *  accumulator   ROL A         2A    1     2
 *  zeropage      ROL oper      26    2     5
 *  zeropage,X    ROL oper,X    36    2     6
 *  absolute      ROL oper      2E    3     6
 *  absolute,X    ROL oper,X    3E    3     7
 *
 *
 *
 * */

void
ROL_handler(mos6502_t *cpu){
  uint8_t old = cpu->a ;
  cpu->a = cpu->a << 1;
  cpu->a = cpu->p.c ? cpu->a | 0x01 :cpu->a & 0xFD ;
  cpu->p.c = (old >> 7 & 0x01) ? 1 : 0 ; 
  cpu->p.z = cpu->a == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->a >> 7 & 0x01) ? 1 : cpu->p.n ;
  cpu->pc += (uint8_t)0x1;


}

void
ROL_ZP_handler(mos6502_t *cpu){

  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand);
  uint8_t old = value ;
  value = value << 1;
  value = cpu->p.c ? value | 0x01 : value & 0xFD; 
  write8(cpu, operand, value);
  cpu->p.c = (old >> 7 & 0x01) ? 1 : 0 ;
  cpu->p.z = value == 0 ? 1 : cpu->p.z;
  cpu->p.n = (value >> 7 & 0x01) ? 1 : cpu->p.n ;
  cpu->pc += (uint8_t)0x2;


}

void
ROL_ZPX_handler(mos6502_t *cpu){

  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand + cpu->x);
  uint8_t old = value;
  value= value << 1;
  value = cpu->p.c ? value | 0x01 : value &  0xFD;  
  write8(cpu, operand + cpu->x, value);
  cpu->p.c = (old >> 7 & 0x01) ? 1 : 0 ;
  cpu->p.z = value == 0 ? 1 : cpu->p.z;
  cpu->p.n = (value >> 7 & 0x01) ? 1 : cpu->p.n ;
  cpu->pc += (uint8_t)0x2;
 
}

void
ROL_ABS_handler(mos6502_t *cpu){

  uint16_t operand = read16(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand);
  uint8_t old = value;
  value= value << 1;
  value = cpu->p.c ? value | 0x01 : value &  0xFD;
  write8(cpu, operand , value);
  cpu->p.c = (old >> 7 & 0x01) ? 1 : 0 ;
  cpu->p.z = value == 0 ? 1 : cpu->p.z;
  cpu->p.n = (value >> 7 & 0x01) ? 1 : cpu->p.n ;
  cpu->pc += (uint8_t)0x2;

}

void
ROL_ABSX_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand + (uint16_t)cpu->x);
  uint8_t old = value;
  value= value << 1;
  value = cpu->p.c ? value | 0x01 : value & 0xFD;
  write8(cpu, operand + cpu->x, value);
  cpu->p.c = (old >> 7 & 0x01) ? 1 : 0 ;
  cpu->p.z = value == 0 ? 1 : cpu->p.z;
  cpu->p.n = (value >> 7 & 0x01) ? 1 : cpu->p.n ;
  cpu->pc += (uint8_t)0x2;


}

/*
 *ROR Family
 *
 * */

void
ROR_handler(mos6502_t *cpu){

  uint8_t old = cpu->a ;
  cpu->a = cpu->a >> 1;
  cpu->a = cpu->p.c ? cpu->a | 0x80 : cpu->a & 0x7F;
  cpu->p.c = (old & 0x01) ? 1 : 0 ;
  cpu->p.z = cpu->a == 0 ? 1 : cpu->p.z;
  cpu->p.n = (cpu->a >> 7 & 0x01) ? 1 : cpu->p.n ;
  cpu->pc += (uint8_t)0x1;

}
void
ROR_ZP_handler(mos6502_t *cpu){

  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand);
  uint8_t old = value ;
  value = value >> 1;
  value = cpu->p.c ? value | 0x80 : value & 0x7F;
  write8(cpu, operand, value);
  cpu->p.c = (old & 0x01) ? 1 : 0 ;
  cpu->p.z = value == 0 ? 1 : cpu->p.z;
  cpu->p.n = (value >> 7 & 0x01) ? 1 : cpu->p.n ;
  cpu->pc += (uint8_t)0x2;


}
void
ROR_ZPX_handler(mos6502_t *cpu){

  uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand + cpu->x);
  uint8_t old = value;
  value= value >> 1;
  value = cpu->p.c ? value | 0x80 : value & 0x7F;
  write8(cpu, operand + cpu->x, value);
  cpu->p.c = (old & 0x01) ? 1 : 0 ;
  cpu->p.z = value == 0 ? 1 : cpu->p.z;
  cpu->p.n = (value >> 7 & 0x01) ? 1 : cpu->p.n ;
  cpu->pc += (uint8_t)0x2;

}
void
ROR_ABS_handler(mos6502_t *cpu){

  uint16_t operand = read16(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand );
  uint8_t old = value;
  value= value >> 1;
  value = cpu->p.c ? value | 0x80 : value & 0x7F;
  write8(cpu, operand , value);
  cpu->p.c = (old & 0x01) ? 1 : 0 ;
  cpu->p.z = value == 0 ? 1 : cpu->p.z;
  cpu->p.n = (value >> 7 & 0x01) ? 1 : cpu->p.n ;
  cpu->pc += (uint8_t)0x3;

 }

void
ROR_ABSX_handler(mos6502_t *cpu){

  uint16_t operand = read16(cpu, cpu->pc + (uint8_t)1);
  uint8_t value = read8(cpu, operand + (uint16_t)cpu->x);
  uint8_t old = value;
  value= value >> 1;
  value = cpu->p.c ? value | 0x80 :value & 0x7F;
  write8(cpu, operand + cpu->x, value);
  cpu->p.c = (old & 0x01) ? 1 : 0 ;
  cpu->p.z = value == 0 ? 1 : cpu->p.z;
  cpu->p.n = (value >> 7 & 0x01) ? 1 : cpu->p.n ;
  cpu->pc += (uint8_t)0x3;

}
void
SBC_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + 1);
  uint16_t result = ((cpu->a - operand) - 1);
  
  cpu->a = cpu->p.c ? cpu->a - operand : (cpu->a - operand) - 1;
  //Overflow
  if( result > 0xFF) {
    cpu->p.c = 1;
    cpu->p.v = 1;
    cpu->a = (cpu->a - operand) - 0xFF;
  }
  cpu->p.n = (result >> 7) & 0x1 ? 1: 0;
  cpu->p.z = result == 0? 1:0;
  cpu->pc += 2;
}
void
SBC_ZP_handler(mos6502_t *cpu) {
  uint8_t operand = read8(cpu, cpu->pc + 1);
  uint8_t value = read8(cpu, (uint16_t)operand);
  uint16_t result = ((cpu->a) - value);
  cpu->a = cpu->p.c ? cpu->a -value : (cpu->a - value) - 1;
  //Overflow
  if( result > 0xFF) {
    cpu->p.c = 1;
    cpu->p.v = 1;
    cpu->a = (cpu->a - value) - 0xFF;
  }
  cpu->p.n = (result >> 7) & 0x1 ? 1: 0;
  cpu->p.z = result == 0? 1:0;
  cpu->pc += 2;
}

void
SBC_ZPX_handler(mos6502_t *cpu){
  uint8_t operand = read8(cpu, cpu->pc + 1);
  uint8_t value = read8(cpu, (uint16_t)(operand+cpu->x));
  uint16_t result = cpu->a-value;
  cpu->a = cpu->p.c ? cpu->a - value : (cpu->a - value) - 1;
  //Overflow
  if( result > 0xFF) {
    cpu->p.c = 1;
    cpu->p.v = 1;
    cpu->a = (cpu->a - value) - 0xFF;
  }
  cpu->p.n = (result >> 7) & 0x1 ? 1: 0;
  cpu->p.z = result == 0? 1:0;
  cpu->pc += (uint8_t)0x2;
}

void
SBC_ABS_handler(mos6502_t *cpu){
  uint16_t operand = read16(cpu, cpu->pc + (uint16_t)1);
  uint8_t value = read8(cpu, operand);
  uint16_t result = cpu->a - value;
  cpu->a = cpu->p.c ? cpu->a - value : (cpu->a - value) - 1;
  //Overflow
  if( result > 0xFF) {
    cpu->p.c = 1;
    cpu->p.v = 1;
    cpu->a = (cpu->a - value) - 0xFF;
  }
  cpu->p.n = (result >> 7) & 0x1 ? 1: 0;
  cpu->p.z = result == 0? 1:0;
  cpu->pc += (uint8_t)0x3;
}

void
SBC_ABSX_handler(mos6502_t *cpu){
 uint16_t operand = read16(cpu, cpu->pc+(uint16_t)1);
 uint8_t value = read8(cpu, operand+cpu->x);
 uint16_t result = cpu->p.c ? cpu->a - value : cpu->a - value - 1;
 cpu->a = cpu->p.c ? cpu->a - value : (cpu->a - value) - 1;
 //Overflow
 if( result > 0xFF) {
   cpu->p.c = 1;
   cpu->p.v = 1;
   cpu->a = (cpu->a - value) - 0xFF;
 }
 cpu->p.n = (result >> 7) & 0x1 ? 1 : 0;
 cpu->p.z = result == 0 ? 1 : 0;
 cpu->pc += (uint8_t)0x3; 
}

void
SBC_ABSY_handler(mos6502_t *cpu){ 
 uint16_t operand = read16(cpu, cpu->pc+(uint16_t)1);
 uint8_t value = read8(cpu, operand+cpu->y);
 uint16_t result = cpu->p.c ? cpu->a - value : cpu->a - value - 1;
 cpu->a = cpu->p.c ? cpu->a - value : (cpu->a - value) - 1;
 //Overflow
 if( result > 0xFF){
   cpu->p.c = 1;
   cpu->p.v = 1;
   cpu->a = (cpu->a - value) - 0xFF;
 }
 cpu->p.n = (result >> 7) & 0x1 ? 1 : 0;
 cpu->p.z = result == 0 ? 1 : 0;
 cpu->pc += (uint8_t) 0x3;
}

void
SBC_IDX_IDR_handler(mos6502_t *cpu){
 uint8_t operand = read8(cpu, cpu->pc + (uint8_t)1);
 uint8_t lo = operand + cpu->x;
 uint8_t hi = lo + 1;
 uint16_t effective_addr = (hi << 8) | lo;
 uint8_t value = read8(cpu, effective_addr);
 uint16_t result = cpu->p.c ? cpu->a - value : cpu->a - value - 1;
 cpu->a = cpu->p.c ? cpu->a - value : (cpu->a - value) - 1;
 //Overflow
 if( result > 0xFF) {
   cpu->p.c = 1;
   cpu->p.v = 1;
   cpu->a = (cpu->a - value) - 0xFF;
 }
 cpu->p.n = (result >> 7) ? 1: 0;
 cpu->p.z = (result == 0) ? 1: 0;
 cpu->pc += (uint8_t)0x02;
}

void
SBC_IDR_IDX_handler(mos6502_t *cpu){
  uint8_t first = read8(cpu, cpu->pc + 1);
  uint8_t secnd = first + 1;
  uint8_t lo = read8(cpu, first + cpu->y);
  uint8_t hi = read8(cpu, secnd);
  uint16_t effective_addr = (hi << 8 ) | lo;
  uint8_t effective_value = read8(cpu, effective_addr);
  uint16_t result = cpu->p.c ? cpu->a - effective_value :cpu->a -effective_value - 1;
 cpu->a = cpu->p.c ? cpu->a - effective_value : (cpu->a - effective_value) - 1;
 //Overflow
 if( result > 0xFF) {
   cpu->p.c = 1;
   cpu->p.v = 1;
   cpu->a = (cpu->a - effective_value) - 0xFF;
 }
 cpu->p.n = (result >> 7) ? 1: 0;
 cpu->p.z = (result == 0) ? 1: 0;
 cpu->pc += (uint8_t)0x02;
}
