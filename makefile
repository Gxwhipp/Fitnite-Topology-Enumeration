fte	: fte.c fte.h
	cc -std=c11 -O3 -o fte fte.c -lm -pthread
