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

#define getSigned16(OP) \
    ((int32_t)((int16_t)(OP)))
    
//return the unsigned word containing the sign extended 18 bit value
static uint32_t signExtend18(uint32_t value){
	value = value & 0x0003ffff;
	if (value&0x00020000 > 0 )
	    return value | 0xfffc0000;
	else
        return value;
}

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


#define DO_DELAY_SLOT(CPU) \
    do{ \
    advancePC(CPU); \
    CPU->delay_slot = 1; \
    CPU->step(CPU); \
    CPU->delay_slot = 0;\
    } while(0)


//TODO delay slot, afvancePC
//name of branch, condition for branch, L if op does advancePC twice

//TODO only eval offset etc... if branch is taken

#define BXXX(NAME,COND,L,LINK) \
void NAME(cpu* _cpu, int op){\
	uint32_t offset = signExtend18((op&0x0000ffff) * 4);\
	uint32_t addr = _cpu->pc + offset;\
	DO_DELAY_SLOT(_cpu); \
	if (LINK){_cpu->GPRs[31] = _cpu->pc + 4;}\
	if(COND)\
	{\
		_cpu->pc = addr;\
	}\
	else\
	{\
		if(L){\
		    advancePC(_cpu);\
		}\
	}\
}


BXXX(BEQ,_cpu->GPRs[getRs(op)] == _cpu->GPRs[getRt(op)],0,0);
BXXX(BEQL,_cpu->GPRs[getRs(op)] == _cpu->GPRs[getRt(op)],1,0);
BXXX(BGEZAL,(int32_t)_cpu->GPRs[getRs(op)] >= 0 ,0,1);
BXXX(BGEZALL,(int32_t)_cpu->GPRs[getRs(op)] >= 0,1,1);
BXXX(BGEZ,(int32_t)_cpu->GPRs[getRs(op)] == 0 ,0,0);
BXXX(BGEZL,(int32_t)_cpu->GPRs[getRs(op)] == 0,1,0);
BXXX(BGTZ,(int32_t)_cpu->GPRs[getRs(op)]  > 0 ,0,0);
BXXX(BGTZL,(int32_t)_cpu->GPRs[getRs(op)] > 0,1,0);
BXXX(BLEZ,(int32_t)_cpu->GPRs[getRs(op)]  <= 0 ,0,0);
BXXX(BLEZL,(int32_t)_cpu->GPRs[getRs(op)] <= 0,1,0);
BXXX(BLTZAL,(int32_t)_cpu->GPRs[getRs(op)]  < 0 ,0,1);
BXXX(BLTZALL,(int32_t)_cpu->GPRs[getRs(op)] < 0,1,1);
BXXX(BLTZ,(int32_t)_cpu->GPRs[getRs(op)]  < 0 ,0,0);
BXXX(BLTZL,(int32_t)_cpu->GPRs[getRs(op)] < 0,1,0);
BXXX(BNE,_cpu->GPRs[getRs(op)] != _cpu->GPRs[getRt(op)],0,0);
BXXX(BNEL,_cpu->GPRs[getRs(op)] != _cpu->GPRs[getRt(op)],1,0);

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
    _cpu->pc = ((_cpu->pc-4) & 0xf0000000) | ((op & 0x3ffffff) << 2);
}

void JR(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: JR\n"); exit(1); }
void LB(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: LB\n"); exit(1); }
void LBU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: LBU\n"); exit(1); }
void LH(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: LH\n"); exit(1); }
void LHU(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: LHU\n"); exit(1); }
void LL(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: LL\n"); exit(1); }

void LUI(cpu* _cpu, int op) { 
     _cpu->GPRs[getRt(op)] = (getImm(op) << 16);
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

void SW(cpu* _cpu, int op) { 
    _cpu->_mmu->writeVAWordAligned(_cpu->_mmu, _cpu->GPRs[getRs(op)] + getSigned16(getImm(op)), _cpu->GPRs[getRt(op)]);
    advancePC(_cpu);
}

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

