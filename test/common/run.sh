#!/bin/sh
function test {
	echo Testing $1
	../../build/cmake/debug/xasm/$2 -fv -o$1.r $1 >$1.out 2>&1
	cat $1.r $1.out >$1.output 2>/dev/null
	rm $1.r $1.out 2>/dev/null
	diff $1.output $1.answer
	if [ $? -eq 0 ]; then
		rm $1.output
	fi
}

for asm in 0x10c/motor0x10c 680x0/motor68k 6502/motor6502 gameboy/motorgb mips/motormips schip/motorschip; do
	echo Using assembler $asm
	for i in *.asm; do
		test $i $asm
	done
done