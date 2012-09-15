#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "mmu.h"

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <path to srec>\n", argv[0]);
        exit(1);
    }

    cpu* _cpu = (cpu*)malloc(sizeof(cpu));
    initCPU(_cpu);
    if(loadSREC(_cpu->_mmu, argv[1])){
        fputs("failed to load srec...\n",stderr);
        return 1;
    }
    _cpu->pc = 0;
    _cpu->pc = _cpu->_mmu->entryPoint;
    printf("Finished loading SREC, running from entryPoint: %llx\n", _cpu->pc);

    while(1)
    {
        _cpu->step(_cpu);
    } 

    free(_cpu);
} 
