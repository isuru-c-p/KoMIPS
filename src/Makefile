default: emu

setup:
	cd autogen && ./build.py && cd ..
	
emu: setup ./autogen/op_decode.o opcodes.o cpu.o mmu.o koMIPS.o
	gcc ./autogen/op_decode.o opcodes.o cpu.o mmu.o koMIPS.o -o koMIPS
	
.PHONY: clean
	
clean:
	rm -rf ./autogen/*.o *.o
	rm -rf koMIPS
