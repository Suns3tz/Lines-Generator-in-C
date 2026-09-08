CC = gcc
NASM = nasm

OUTPUT = proyecto

C_SOURCES = \
	proyecto.c \
	algorithms.c \
	algorithms_pure.c \
	plots.c

C_OBJECTS = $(C_SOURCES:.c=.o)

ASM_SOURCE = bresenham.asm
ASM_OBJECT = bresenham.o

OBJECTS = $(C_OBJECTS) $(ASM_OBJECT)
DEPENDENCIES = $(C_OBJECTS:.o=.d)

CPPFLAGS = -I. -I/usr/local/Mesa-3.4/include
CFLAGS = -std=c11 -Wall -Wextra -O2 -MMD -MP
NASMFLAGS = -f elf64

LDFLAGS = -no-pie \
	-L/usr/local/Mesa-3.4/lib \
	-L/usr/X11R6/lib

LDLIBS = -lglut -lGLU -lGL -lm

.PHONY: all clean run

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(OUTPUT) $(OBJECTS) $(LDLIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(ASM_OBJECT): $(ASM_SOURCE)
	$(NASM) $(NASMFLAGS) $< -o $@

run: $(OUTPUT)
	./$(OUTPUT) 256 100 500

clean:
	rm -f $(OBJECTS) $(DEPENDENCIES) $(OUTPUT)

-include $(DEPENDENCIES)