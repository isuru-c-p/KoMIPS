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
    if((v_addr >= UART_BASE) && (v_addr <= UART_END))
    {
        //printf("UART write, addr: 0x%x (offset: 0x%x)\n", v_addr, (v_addr - UART_BASE));
        if(v_addr == UART_RX_TX)
        {
            printf("%c",byte);        
        }
        return;
    } 

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
    if((v_addr >= UART_BASE) && (v_addr <= UART_END))
    {
        //printf("UART read, addr: 0x%x (offset: 0x%x)\n", v_addr, (v_addr - UART_BASE));
        if(v_addr == UART_LSTAT)
        {
            // THRE & TEMT are set
            return  (1 << 5) | (1 << 6); 
        } 
        return  (1 << 5) | (1 << 6); 
    }

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

uint32_t readVAWordUnAligned(mmu* _mmu, uint32_t v_addr)
{
   return  readVAByte(_mmu,v_addr+3)       +
           (readVAByte(_mmu,v_addr+2) << 8)  +
           (readVAByte(_mmu,v_addr+1) << 16) +
           (readVAByte(_mmu,v_addr)   << 24);
}

void writeVAWordUnAligned(mmu* _mmu, uint32_t v_addr, uint32_t value)
{
    writeVAByte(_mmu, v_addr+3, value & 0xff);
    writeVAByte(_mmu, v_addr+2, (value & 0xff00) >> 8); 
    writeVAByte(_mmu, v_addr+1, (value & 0xff0000) >> 16); 
    writeVAByte(_mmu, v_addr, (value & 0xff000000) >> 24); 
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


static int isHexChar(char c){
    if(c >= '0' && c <= '9')
        return 1;
    if(c >= 'a' && c <= 'f')
        return 1;
    if(c >= 'A' && c <= 'F')
        return 1;
    return 0;
}

static int srecReadType(FILE * f){
    int c = fgetc(f);
    if(c == EOF){
        return -1; // 0 type srecord will trigger a skip, which will terminate
    }
    if(c != 'S'){
        return -2;
    }
    c = fgetc(f) - '0';
    if(c >= 0 && c <= 9)
        return c;
    return -2;
}

static int srecReadByte(FILE * f, uint8_t * count){
    char chars[3];
    int i;
    
    for(i = 0 ; i < 2; i++){
        chars[i] = fgetc(f);
        if(!isHexChar(chars[i]))
            return 1;
    }
    chars[2] = 0;
    *count = (uint8_t) strtoul(&chars[0],0,16);
    return 0;
}

static int srecReadAddress(FILE * f, uint32_t * addr){
    char chars[9];
    int i;
    
    for(i = 0 ; i < 8; i++){
        chars[i] = fgetc(f);
        if(!isHexChar(chars[i]))
            return 1;
    }
    chars[8] = 0;
    
    *addr = strtoul(&chars[0],0,16); 
    return 0;
}

static void srecSkipToNextLine(FILE * f){
    while(1){
        int c = fgetc(f);
        if(c == EOF || c == '\n'){
            break;
        }
    }
}

static int srecLoadData(FILE * f,mmu* _mmu,uint32_t addr,uint32_t count){
    
    uint8_t b;
    while(count--){
        if(srecReadByte(f,&b)){
            return 1;
        }
        
        _mmu->writeVAByte(_mmu,addr++,b); //TODO check for exception/mapped address
    }
    return 0;
}

int loadSREC(mmu* _mmu, char* filename)
{

    uint32_t addr;
    uint8_t count;

    FILE * f = fopen(filename,"r");
    if(!f){
        fputs("srecLoader: failed to open file.\n",stderr);
        return 1;
    }
    
    while(!feof(f)){
        switch(srecReadType(f)){
            
            case -1:
                //EOF
                break;
            case 0:
                srecSkipToNextLine(f);
                break;
            case 3:
                if(srecReadByte(f,&count)){
                    fputs("srecLoader: failed to parse bytecount.\n",stderr);
                    return 1;
                }
                if(srecReadAddress(f,&addr)){
                    fputs("srecLoader: failed to parse address.\n",stderr);
                    return 1;
                }
                if(srecLoadData(f,_mmu,addr,count-5)){
                    fputs("srecLoader: failed to load data.\n",stderr);
                    return 1;
                }
                srecSkipToNextLine(f);
                break;
            case 7:
                if(srecReadByte(f,&count)){
                    fputs("srecLoader: failed to parse bytecount.\n",stderr);
                    return 1;
                }
                if(srecReadAddress(f,&addr)){
                    fputs("srecLoader: failed to parse address.\n",stderr);
                    return 1;
                }
                _mmu->entryPoint = addr;
                srecSkipToNextLine(f); 
                break;

            default:
                fputs("Bad/Unsupported srec type\n",stderr);
                return 1;
        }
    }
    
    return 0;
}

