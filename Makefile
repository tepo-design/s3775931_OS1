.default: all

clean:
	rm -rf probA *.o *.dSYM

probA: probA.cpp
	gcc -g -Wall -o probA probA.cpp