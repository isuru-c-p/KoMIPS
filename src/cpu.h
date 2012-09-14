#ifndef _CPU_H_
#define _CPU_H_

#include <stdint.h>
#include "mmu.h"

typedef union _fpreg {
	float fd;
	float fs[2];
	uint64_t d;
	uint32_t w[2];
} fpreg;

typedef struct _cpu {
	uint32_t GPRs[32];
	fpreg FGRs[32];
	uint8_t mode;
	uint64_t pc;
	uint64_t MultHI;
	uint64_t MultLO;
	uint32_t FCR0;
	uint32_t FCR31;
	uint8_t LL:1;
    mmu* _mmu;
    void (*step)(struct _cpu* _cpu);
    void (*doOp)(struct _cpu* _cpu, uint32_t op);
} cpu;

typedef struct _cp0 {
	uint64_t EntryHi;
	uint64_t EntryLo;
	uint64_t PageMask;
	uint64_t Index;
	uint64_t Random;
	uint64_t Wired;
	uint64_t Context;
	uint64_t XContext;
	uint64_t BadVAddr;
	uint64_t LLAddr;
	uint64_t TagHi;
	uint64_t WatchHi;
	uint64_t Count;
	uint64_t SR;
	uint64_t CacheErr;
	uint64_t EPC;
	uint64_t TagLo;
	uint64_t WatchLo;
	uint64_t Compare;
	uint64_t Cause;
	uint64_t PErr;
	uint64_t ErrEPC;
} cp0;

void initCPU(cpu* _cpu);
void advancePC(cpu* _cpu);

#endif
