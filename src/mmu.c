#include "mmu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t VAtoPA(mmu* _mmu, uint32_t v_addr)
{
    uint32_t p_addr;

    switch((v_addr & 0xe0000000) >> 29)
    {
        case 0b100:
            p_addr = v_addr - 0x80000000;
            break;
            
        case 0b101:
            p_addr = v_addr - 0xa0000000; 
            break;

        default:
            printf("Error, translation not defined for VA: %x\n", v_addr);
            exit(1);
    }   

    return p_addr;
}

void writePAByte(mmu* _mmu, uint32_t p_addr, uint8_t byte)
{
	if(KOMIPS_HOST_ENDIAN  == KOMIPS_LITTLE_ENDIAN)
	{
		_mmu->mem[p_addr/4].bytes[3-p_addr%4] = byte; 
	}
	else if(KOMIPS_HOST_ENDIAN  == KOMIPS_BIG_ENDIAN)
	{
		_mmu->mem[p_addr/4].bytes[p_addr%4] = byte; 
	}
}

void writeVAByte(mmu* _mmu, uint32_t v_addr, uint8_t byte)
{
    uint32_t p_addr = _mmu->VAtoPA(_mmu, v_addr);
    _mmu->writePAByte(_mmu, p_addr, byte);
}

void writePAWordAligned(mmu* _mmu, uint32_t p_addr, uint32_t word)
{
    _mmu->mem[p_addr/4].word = word;
}

void writeVAWordAligned(mmu* _mmu, uint32_t v_addr, uint32_t word)
{
    uint32_t p_addr = _mmu->VAtoPA(_mmu, v_addr);
    _mmu->writePAWordAligned(_mmu, p_addr, word);
}

uint32_t readPAWordAligned(mmu* _mmu, uint32_t p_addr)
{
    //printf("readPAWordAligned: %x\n", p_addr);
    return _mmu->mem[p_addr/4].word;
}

uint32_t readVAWordAligned(mmu* _mmu, uint32_t v_addr)
{
    //printf("readVAWordAligned: %x\n", v_addr);
    uint32_t p_addr = _mmu->VAtoPA(_mmu, v_addr);
    return _mmu->readPAWordAligned(_mmu, p_addr);
}

void initMMU(mmu* _mmu)
{
    _mmu->writePAByte = writePAByte;
    _mmu->writeVAByte = writeVAByte;
    _mmu->VAtoPA = VAtoPA;
    _mmu->writePAWordAligned = writePAWordAligned;
    _mmu->writeVAWordAligned = writeVAWordAligned;
    _mmu->readPAWordAligned = readPAWordAligned;
    _mmu->readVAWordAligned = readVAWordAligned;
    _mmu->entryPoint = 0;
    _mmu->mem = (memword*)malloc(MEM_SIZE*sizeof(memword));
}

void loadSREC(mmu* _mmu, char* filename)
{
    FILE *fp;
    fp = fopen(filename, "r");
    char* nextLineChar;
    size_t len;
    uint32_t entryPoint = 0;

    if(fp == NULL)
    {
        printf("Error, opening %s for reading.\n", filename);
        exit(1); 
    }

    while((nextLineChar = fgetln(fp,&len)))
    {
        char t = nextLineChar[1];
        char s_count[3] = {'\0'};
        char s_addr[9] = {'\0'};
        uint32_t addr = 0;
        int count = 0;
        int dataEnd = len - 2;
        int dataLen = dataEnd - 8;
        char* data = (char*)malloc(dataLen*sizeof(char));
        memset((void*)data,'\0',dataLen*sizeof(char));
        

        if(nextLineChar[0] != 'S')
        {
            printf("Invalid SREC record!\n");
            exit(1);
        }    

        strncpy(s_count,(nextLineChar+2),2); 
        sscanf(s_count,"%x",&count);

        switch(t)
        {
            case '0':
                break;

            case '1':
                strncpy(s_addr,(nextLineChar+4),4);
                strncpy(data,(nextLineChar+8),dataEnd-8);
                break;

            case '2':
                strncpy(s_addr,(nextLineChar+4),6);
                strncpy(data,(nextLineChar+10),dataEnd-10);
                break;

            case '3':
                strncpy(s_addr,(nextLineChar+4),8);
                strncpy(data,(nextLineChar+12),dataEnd-12);
                break;

            case '7':
            case '8':
            case '9':
                count = count*2 - 2;
                strncpy(s_addr,(nextLineChar+4),count);
                sscanf(s_addr,"%x",&_mmu->entryPoint);  
                break; 

            default:
                printf("ERROR, unknown SREC record type: %c\n", t);
                exit(1);
                break; 
        }
       
        if((t == '1') || (t == '2') || (t == '3'))
        {
            //printf("Record: %c\n", t);
            int dataLen = strlen(data);
            if((dataLen % 2) != 0)
            {
                printf("Invalid SREC record length: %d\n", dataLen);
                exit(1);
            }

            sscanf(s_addr,"%x",&addr);
            //printf("addr: %x, s_addr: %s\n", addr, s_addr);
            for(int i = 0; i < dataLen; i+=2)
            {
                char s_dataByte[3] = {'\0'};
                uint8_t dataByte = 0;
                strncpy(s_dataByte,(nextLineChar+i),2);
                sscanf(s_dataByte,"%x",&dataByte);
                
                _mmu->writeVAByte(_mmu, addr+(i/2),dataByte);
            }
        } 

        
        free(data); 
    }
}
