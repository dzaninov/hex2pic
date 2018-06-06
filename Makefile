hex2pic.hex: hex2pic.o
	gplink -o hex2pic.hex hex2pic.o

hex2pic.o: hex2pic.asm
	gpasm -p 16f887 -c -o hex2pic.o hex2pic.asm

clean:
	rm -f *.cod *.hex *.lst *.o
