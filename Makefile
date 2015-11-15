
all: 
	zcc +zxansi -O2 -vn zxmines2.c -o zxmines2.bin -lndos
	bin2tap zxmines2.bin zxmines2.tap
	rm -f zcc_opt.def

clean:
	rm -f *.bin *.i *.op* *.o *~
