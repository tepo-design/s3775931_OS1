.default: all

clean:
	rm -rf probA *.o *.dSYM

probA: probA.cpp
	g++ -g -Wall -o probA probA.cpp