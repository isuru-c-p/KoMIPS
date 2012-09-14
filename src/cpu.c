#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"
#include "mmu.h"

void doOp(cpu* _cpu, uint32_t op);

void step(cpu* _cpu)
{
   //printf("step.\n");
   uint32_t op = _cpu->_mmu->readVAWordAligned(_cpu->_mmu,_cpu->pc);
   //printf("op: %x\n", op);
   _cpu->doOp(_cpu, op);
}

void initCPU(cpu* _cpu)
{
    _cpu->_mmu = (mmu*)malloc(sizeof(mmu));
    initMMU(_cpu->_mmu);
    _cpu->step = step;
    _cpu->doOp = doOp;
    return;
}
