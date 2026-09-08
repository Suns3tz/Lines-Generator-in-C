CC = gcc
NASM = nasm

OBJECTS = proyecto.o plots.o bresenham.o
OUTPUT = proyecto

CFLAGS = -std=c11 -Wall -Wextra -O2 \
         -I/usr/local/Mesa-3.4/include

NASMFLAGS = -f elf64

LDFLAGS = -no-pie \
          -L/usr/local/Mesa-3.4/lib \
          -L/usr/X11R6/lib

LDLIBS = -lglut -lGLU -lGL -lm

.PHONY: all clean

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(OUTPUT) $(OBJECTS) $(LDLIBS)

proyecto.o: proyecto.c
	$(CC) $(CFLAGS) -c proyecto.c -o proyecto.o

plots.o: plots.c
	$(CC) $(CFLAGS) -c plots.c -o plots.o

bresenham.o: bresenham.asm
	$(NASM) $(NASMFLAGS) bresenham.asm -o bresenham.o

clean:
	rm -f $(OBJECTS) $(OUTPUT)