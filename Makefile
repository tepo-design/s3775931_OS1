CC=g++
CFLAGS=-I.
DEPS = probA.h probD.h
OBJ = simulation.cpp probA.cpp probD.cpp

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

simulation: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)