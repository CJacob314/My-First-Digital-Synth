.DEFAULT_GOAL := debug

clean:
	rm -f synth *.wav *.o

all:
	# g++ -c -O3 *.cpp -lc
	# gcc -o synth-debug *.o -lc
	# gcc -o synth *.o -lc -static
	# rm *.o
	make debug
	make prod

debug:
	g++ -g -o debug-synth -O0 *.cpp -lc

prod:
	g++ -g -o synth -O3 *.cpp -lc -static