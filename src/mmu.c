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
		_mmu->mem[p_addr/4].bytes[p_addr%4] = byte; 
	}
	else if(KOMIPS_HOST_ENDIAN  == KOMIPS_BIG_ENDIAN)
	{
		_mmu->mem[p_addr/4].bytes[3-p_addr%4] = byte; 
	}
}

void writeVAByte(mmu* _mmu, uint32_t v_addr, uint8_t byte)
{
    uint32_t p_addr = _mmu->VAtoPA(_mmu, v_addr);
    _mmu->writePAByte(_mmu, p_addr, byte);
}

uint8_t readPAByte(mmu* _mmu, uint32_t p_addr)
{
 	if(KOMIPS_HOST_ENDIAN  == KOMIPS_LITTLE_ENDIAN)
	{
		return _mmu->mem[p_addr/4].bytes[p_addr%4];
	}
	else if(KOMIPS_HOST_ENDIAN  == KOMIPS_BIG_ENDIAN)
	{
		return _mmu->mem[p_addr/4].bytes[3-p_addr%4];
	}   
}

uint8_t readVAByte(mmu* _mmu, uint32_t v_addr)
{
    uint32_t p_addr = _mmu->VAtoPA(_mmu, v_addr);
    return readPAByte(_mmu, p_addr);
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

    if(fp == NULL)
    {
        printf("Error, opening %s for reading.\n", filename);
        exit(1);
    }

    typedef enum recordReadStateType {
        RECORD_HEADER = 0,
        RECORD_TYPE = 1,
        RECORD_LEN = 2,
        RECORD_ADDR = 3,
        RECORD_DATA = 4,
        RECORD_END = 5
    } recordReadStateType; 

    recordReadStateType recordReadState = RECORD_HEADER;

    int charCount = 0;
    char recordType = '\0';
    int recordLength = 0;
    int addrLength = 0;
    int dataLength = 0;
    const int addrLengthTable[10] = {2,2,3,4,2,4,3,4,4,4};
    uint32_t address = 0;
    

    while(!feof(fp))
    {
        int dataPtr = 0;
        char s_Length[3] = {'\0'};
        char* s_address;
        char* s_data;
        char s_byte[3] = {'\0'}; 
        uint8_t byte = 0;

        switch(recordReadState)
        {
            case RECORD_HEADER:
                fgetc(fp);
                break;

            case RECORD_TYPE:
                recordType = fgetc(fp);
                addrLength = addrLengthTable[((uint8_t)recordType)-((uint8_t)'0')];
                //printf("recordType: %c, addrLength: %d\n", recordType, addrLength);
                break;

            case RECORD_LEN:
                fread(s_Length, 1, 2, fp);  
                sscanf(s_Length, "%x", &recordLength); 
                break;

            case RECORD_ADDR:
                s_address = (char*)malloc((addrLength*2+1)*sizeof(char));
                s_address[addrLength*2] = '\0';
                fread(s_address, 1, addrLength*2, fp);
                sscanf(s_address, "%x", &address);
                //printf("address: %x, length: %d\n", address, addrLength*2);
                free(s_address); 
                break;

            case RECORD_DATA:
                dataLength = recordLength - addrLength - 1; 
                s_data = (char*)malloc((dataLength*2+1)*sizeof(char));
                s_data[dataLength*2] = '\0';

                switch(recordType)
                {
                    case '0':
                        fseek(fp, dataLength*2, SEEK_CUR);
                        break; 

                    case '7':
                    case '8':
                    case '9':
                         _mmu->entryPoint = address;
                         fseek(fp, 2, SEEK_CUR);
                         break;
                
                    default:
                        for(dataPtr = 0; dataPtr < dataLength; dataPtr++)
                        {
                            fread(s_byte, 1, 2, fp); 
                            sscanf(s_byte, "%x", &byte);
                            _mmu->writeVAByte(_mmu, address+dataPtr,byte);
                        }           
                }

                free(s_data);
                break;

            case RECORD_END:
                // skip checksum + new line char
                fseek(fp, 3, SEEK_CUR);
                break;
        }
    
        recordReadState = (recordReadState+1) % 6;   
    }
}
