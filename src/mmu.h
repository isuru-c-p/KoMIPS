#pragma once
#include <stdint.h>

#define MEM_SIZE 128000000UL

#define UART_BASE 0xbfd003f8
#define UART_RX_TX UART_BASE
#define UART_LSTAT (UART_BASE+0x5)
#define UART_END UART_BASE+0x38

#define KOMIPS_LITTLE_ENDIAN 1
#define KOMIPS_BIG_ENDIAN 2

#define KOMIPS_HOST_ENDIAN KOMIPS_BIG_ENDIAN
//#ifndef KOMIPS_HOST_ENDIAN 
//	#error "KOMIPS_HOST_ENDIAN must be defined"
//#endif

typedef struct _pfnEntry {
	uint32_t PFN:19;
	uint16_t C:1;
	uint16_t D:1;
	uint16_t V:1;
} pfnEntry;

typedef struct _tlbEntry {
	uint16_t MASK:12;
	uint32_t VPN2:19;
	uint8_t G:1;
	uint8_t ASID;
	pfnEntry pfn[2];
} tlbEntry;

typedef union _memword {
    uint32_t word;
    uint8_t bytes[4];
} memword;

typedef struct _mmu {
    memword* mem; 
    int mem_size;
    uint32_t entryPoint;
    tlbEntry tlb[16];
    void (*writePAByte)(struct _mmu* _mmu, uint32_t p_addr, uint8_t byte);
    void (*writeVAByte)(struct _mmu* _mmu, uint32_t p_addr, uint8_t byte);
    uint32_t (*VAtoPA)(struct _mmu* _mmu, uint32_t v_addr);
    void (*writePAWordAligned)(struct _mmu* _mmu, uint32_t p_addr, uint32_t word);
    void (*writeVAWordAligned)(struct _mmu* _mmu, uint32_t v_addr, uint32_t word);
    uint32_t (*readPAWordAligned)(struct _mmu* _mmu, uint32_t p_addr);
    uint32_t (*readVAWordAligned)(struct _mmu* _mmu, uint32_t v_addr);
} mmu;

//void writePAByte(mmu* _mmu, uint32_t p_addr, uint8_t byte);
//void writeVAByte(mmu* _mmu, uint32_t v_addr, uint8_t byte);
void initMMU(mmu* _mmu);
int loadSREC(mmu* _mmu, char* filename);

uint8_t readPAByte(mmu* _mmu, uint32_t p_addr);
uint8_t readVAByte(mmu* _mmu, uint32_t v_addr);
uint32_t readVAWordUnAligned(mmu* _mmu, uint32_t v_addr);
