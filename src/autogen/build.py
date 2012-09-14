#!/usr/bin/python
import sys
opcodes = set([])
	
class Table(object):
	def __init__(self,s):
		self.ranges = []
		rows = []
		self.ops = []
		fields = s.split('\n')
		self.name = fields[0].rstrip('\r')
		self.setBitmask(fields[1].split(","), fields[2].split(","))
		
		rows = fields[3:]
		for row in rows:
			for op in row.split(' '):
				self.ops.append(op.rstrip('\r'))
				opcodes.add(op.rstrip('\r'))
	
	def setBitmask(self, range0, range1):
		ret = 0
		
		for r in [range0,range1]:
			end, start = r
			self.ranges.append([int(start.rstrip('\r')), int(end.rstrip('\r'))+1])
			#print "end: %s, start: %s" % (end,start)
			for i in range(int(start.rstrip('\r')), int(end.rstrip('\r'))+1):
				ret |= (1 << i)
		
		self.bitmask = ret	
	
	def __repr__(self):
		bitmask = self.bitmask
		cases = ""
		
		for i,op in enumerate(self.ops):
			noCols = (2 ** (self.ranges[0][1] - self.ranges[0][0]))
			top = i % noCols
			left = (i - top)/noCols
			
			opcodeCaseValue = (left << self.ranges[1][0]) + (top << self.ranges[0][0])
			opcodeCaseBinaryStr = bin(opcodeCaseValue)
			
			if (op in tablesDict.keys()):
				subswitch = str(tablesDict[op])
				cases += \
				"""
					case 0x%(opcodeCaseValue)x: /*%(opcodeCaseBinaryStr)s*/
						%(subswitch)s;
						return;
						
				""" % locals()			
			
			else:
				cases += \
				"""
					case 0x%(opcodeCaseValue)x: /*%(opcodeCaseBinaryStr)s*/
						%(op)s(_cpu,op);
						return;
						
				""" % locals()
			
		bitmaskBinaryStr = bin(bitmask)
		
		switchCode = \
"""

/* %(bitmaskBinaryStr)s */
switch((op & 0x%(bitmask)x) >> 0)
{
	%(cases)s
	default:
	    printf("Unreachable from opcode: %%x",op);
}

""" % locals()
		
		return switchCode
		#return  self.name + '\n' + str(bin(self.bitmask)[2:]) + '\n' + str(self.ops)


genOpcodesC = False
if "--gen_opcodes_c" in sys.argv:
		genOpcodesC = True
		
rawTables = open("optables.txt").read().split('----')
rawTables = [t.strip() for t in rawTables]
tables = [ Table(t) for t in rawTables ]

tablesDict = {}

for table in tables:
	tablesDict[table.name] = table

startingTable = tablesDict["First"]
allOpcodes = set([])

#for t in tables:
#	print t

startingTableStr = str(startingTable)

tableSwitch = \
"""\
#include <stdio.h>
#include "../cpu.h"
#include "opcodes.h"

	void doOp(cpu* _cpu, uint32_t op) {
		%(startingTableStr)s
	}
""" % locals()

outFile = open("op_decode.c", "w")
outFile.write(tableSwitch)
outFile.close()

if genOpcodesC == True:
	opcodeFuncFile = open("opcodes.c","w")
	opcodeFuncFile.write("""#include <stdio.h>\n#include <stdlib.h>\n#include "cpu.h"\n#include "autogen/opcodes.h"\n\n""")

outFile = open("opcodes.h", "w")
outFile.write(\
"""\
#pragma once\n
""");
			 
for opcode in opcodes:
	if opcode == "" or opcode in tablesDict.keys():
		continue
	outFile.write("void %s(cpu* _cpu, int op);\n" % opcode)
	if genOpcodesC == True:
		opcodeFuncFile.write("""void %s(cpu* _cpu, int op) { printf("ERROR, unimplemented opcode: %s\\n"); exit(1); }\n\n""" % (opcode,opcode))
	
if genOpcodesC == True:
	opcodeFuncFile.close()
#print startingTable
	
