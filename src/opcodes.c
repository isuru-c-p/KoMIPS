#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "autogen/opcodes.h"


//extra brackets make these macros play nice.
#define getRs(op) (((op)&0x3e00000) >> 21)

#define getRt(op) (((op)&0x1f0000) >> 16)   

#define getRd(op) (((op)&0xf800) >> 11)

#define getImm(op) ((op)&0x0000ffff)

#define getSHAMT(op) (((op)&0x7c0) >> 6)

#define getFunct(op) ((op)&0x3f)


void a(cpu* _cpu, int op) {
    fputs("ERROR bad opcode\n",stderr);
}

void ADD(cpu* _cpu, int op) {
    //We are ignoring overflow traps for now
    ADDU(_cpu,op);
    advancePC(_cpu);
}

void ADDI(cpu* _cpu, int op) {
    //We are ignoring overflow traps for now
    ADDIU(_cpu,op);
    advancePC(_cpu);
}

void ADDIU(cpu* _cpu, int op) {
    uint16_t imm = (uint16_t)getImm(op);
    _cpu->GPRs[getRt(op)] = (uint32_t)( (int32_t)_cpu->GPRs[getRs(op)] + (int32_t)((int16_t)imm)); 
    advancePC(_cpu);
}

void ADDU(cpu* _cpu, int op) { 
    _cpu->GPRs[getRd(op)] = _cpu->GPRs[getRs(op)] + _cpu->GPRs[getRt(op)];
    advancePC(_cpu);
}

void AND(cpu* _cpu, int op) { 
    _cpu->GPRs[getRd(op)] = _cpu->GPRs[getRs(op)] & _cpu->GPRs[getRt(op)];
    advancePC(_cpu);
}

void ANDI(cpu* _cpu, int op) {
    _cpu->GPRs[getRt(op)] = _cpu->GPRs[getRs(op)] & getImm(op); //& only bottom 16 bits
    advancePC(_cpu);
}

void b(cpu* _cpu, int op) {
    fputs("ERROR bad opcode\n",stderr);
}

//TODO delay slot, afvancePC
//name of branch, condition for branch, L if op does advancePC twice
#define BXXX(NAME,COND,L) \
void NAME(cpu* _cpu, int op){\
	int32_t offset = getSigned18((op&0x0000ffff) * 4);\
	int32_t rs_val = _cpu->GPRs[getRs(op)];\
	uint32_t addr = _cpu->PC + offset;\
	if(COND)\
	{\
		_cpu->PC = addr;\
	}\
	else\
	{\
		if(L){\
		}\
	}\
}

#define DO_DELAY_SLOT(CPU) \
    advancePC(CPU); \
    CPU->delay_slot = 1; \
    CPU->step(CPU); \
    CPU->delay_slot = 0;



void BEQ(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BEQ\n"); exit(1); }
void BEQL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BEQL\n"); exit(1); }
void BGEZAL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BGEZAL\n"); exit(1); }
void BGEZALL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BGEZALL\n"); exit(1); }
void BGEZ(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BGEZ\n"); exit(1); }
void BGEZL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BGEZL\n"); exit(1); }
void BGTZ(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BGTZ\n"); exit(1); }
void BGTZL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BGTZL\n"); exit(1); }
void BLEZ(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BLEZ\n"); exit(1); }
void BLEZL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BLEZL\n"); exit(1); }
void BLTZAL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BLTZAL\n"); exit(1); }
void BLTZALL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BLTZALL\n"); exit(1); }
void BLTZ(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BLTZ\n"); exit(1); }
void BLTZL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BLTZL\n"); exit(1); }
void BNE(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BNE\n"); exit(1); }
void BNEL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BNEL\n"); exit(1); }
void BREAK(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: BREAK\n"); exit(1); }
void CACHE(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: CACHE\n"); exit(1); }
void CLO(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: CLO\n"); exit(1); }
void CLZ(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: CLZ\n"); exit(1); }
void DERET(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: DERET\n"); exit(1); }
void DIV(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: DIV\n"); exit(1); }
void DIVU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: DIVU\n"); exit(1); }
void ERET(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: ERET\n"); exit(1); }
void JAL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: JAL\n"); exit(1); }
void JALR(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: JALR\n"); exit(1); }

void J(cpu* _cpu, int op) { 
    DO_DELAY_SLOT(_cpu);
    _cpu->pc = (_cpu->pc & 0xf0000000) | ((op & 0x3ffffff) << 2);
}

void JR(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: JR\n"); exit(1); }
void LB(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: LB\n"); exit(1); }
void LBU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: LBU\n"); exit(1); }
void LH(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: LH\n"); exit(1); }
void LHU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: LHU\n"); exit(1); }
void LL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: LL\n"); exit(1); }

void LUI(cpu* _cpu, int op) { 
     _cpu->GPRs[getRt(op)] = getImm(op);
     advancePC(_cpu);
}

void LW(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: LW\n"); exit(1); }
void LWL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: LWL\n"); exit(1); }
void LWR(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: LWR\n"); exit(1); }
void MADD(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: MADD\n"); exit(1); }
void MADDU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: MADDU\n"); exit(1); }
void MFC0(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: MFC0\n"); exit(1); }
void MFHI(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: MFHI\n"); exit(1); }
void MFLO(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: MFLO\n"); exit(1); }
void MOVN(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: MOVN\n"); exit(1); }
void MOVZ(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: MOVZ\n"); exit(1); }
void MSUB(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: MSUB\n"); exit(1); }
void MSUBU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: MSUBU\n"); exit(1); }
void MTC0(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: MTC0\n"); exit(1); }
void MTHI(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: MTHI\n"); exit(1); }
void MTLO(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: MTLO\n"); exit(1); }

void MUL(cpu* _cpu, int op) { 
    _cpu->GPRs[getRd(op)] = _cpu->GPRs[getRs(op)] * _cpu->GPRs[getRt(op)];
    advancePC(_cpu);
}

void MULT(cpu* _cpu, int op) {

    union {
        int64_t big;
        uint32_t small[2];
    } data;

    data.big = (int64_t)_cpu->GPRs[getRs(op)] * (int64_t)_cpu->GPRs[getRt(op)];
    
    if(KOMIPS_HOST_ENDIAN  == KOMIPS_LITTLE_ENDIAN){
        _cpu->MultHI = data.small[1];
        _cpu->MultLO = data.small[0];
    
    } else if (KOMIPS_HOST_ENDIAN  == KOMIPS_BIG_ENDIAN) {
        _cpu->MultHI = data.small[0];
        _cpu->MultLO = data.small[1];
    }

    advancePC(_cpu);
}

void MULTU(cpu* _cpu, int op) {

    union {
        uint64_t big;
        uint32_t small[2];
    } data;

    data.big = (uint64_t)_cpu->GPRs[getRs(op)] * (uint64_t)_cpu->GPRs[getRt(op)];
    
    if(KOMIPS_HOST_ENDIAN  == KOMIPS_LITTLE_ENDIAN){
        _cpu->MultHI = data.small[1];
        _cpu->MultLO = data.small[0];
    
    } else if (KOMIPS_HOST_ENDIAN  == KOMIPS_BIG_ENDIAN) {
        _cpu->MultHI = data.small[0];
        _cpu->MultLO = data.small[1];
    }

    advancePC(_cpu);
}


void NOR(cpu* _cpu, int op) {
    _cpu->GPRs[getRt(op)] = ~(_cpu->GPRs[getRs(op)] | getImm(op)); // ~^ only bottom 16 bits
    advancePC(_cpu);
}

void OR(cpu* _cpu, int op) { 
    _cpu->GPRs[getRd(op)] = _cpu->GPRs[getRs(op)] | _cpu->GPRs[getRt(op)];
    advancePC(_cpu);
}

void ORI(cpu* _cpu, int op) {
    _cpu->GPRs[getRt(op)] = _cpu->GPRs[getRs(op)] | getImm(op); //^ only bottom 16 bits
    advancePC(_cpu);
}

void PREF(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: PREF\n"); exit(1); }
void SB(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SB\n"); exit(1); }
void SC(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SC\n"); exit(1); }
void SDBBP(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SDBBP\n"); exit(1); }
void SH(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SH\n"); exit(1); }
void SLL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SLL\n"); exit(1); }
void SLLV(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SLLV\n"); exit(1); }
void SLT(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SLT\n"); exit(1); }
void SLTI(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SLTI\n"); exit(1); }
void SLTIU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SLTIU\n"); exit(1); }
void SLTU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SLTU\n"); exit(1); }
void SRA(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SRA\n"); exit(1); }
void SRAV(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SRAV\n"); exit(1); }
void SRL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SRL\n"); exit(1); }
void SRLV(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SRLV\n"); exit(1); }
void SUB(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SUB\n"); exit(1); }
void SUBU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SUBU\n"); exit(1); }
void SW(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SW\n"); exit(1); }
void SWL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SWL\n"); exit(1); }
void SWR(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SWR\n"); exit(1); }
void SYNC(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SYNC\n"); exit(1); }
void SYSCALL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: SYSCALL\n"); exit(1); }
void TEQ(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TEQ\n"); exit(1); }
void TEQI(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TEQI\n"); exit(1); }
void TGE(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TGE\n"); exit(1); }
void TGEI(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TGEI\n"); exit(1); }
void TGEIU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TGEIU\n"); exit(1); }
void TGEU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TGEU\n"); exit(1); }
void TLBP(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TLBP\n"); exit(1); }
void TLBR(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TLBR\n"); exit(1); }
void TLBWI(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TLBWI\n"); exit(1); }
void TLBWR(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TLBWR\n"); exit(1); }
void TLT(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TLT\n"); exit(1); }
void TLTI(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TLTI\n"); exit(1); }
void TLTIU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TLTIU\n"); exit(1); }
void TLTU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TLTU\n"); exit(1); }
void TNE(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TNE\n"); exit(1); }
void TNEI(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: TNEI\n"); exit(1); }
void WAIT(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: WAIT\n"); exit(1); }

void XOR(cpu* _cpu, int op) { 
    _cpu->GPRs[getRd(op)] = _cpu->GPRs[getRs(op)] & _cpu->GPRs[getRt(op)];
    advancePC(_cpu);
}

void XORI(cpu* _cpu, int op) {
    _cpu->GPRs[getRt(op)] = _cpu->GPRs[getRs(op)] ^ getImm(op); //^ only bottom 16 bits
    advancePC(_cpu);
}

