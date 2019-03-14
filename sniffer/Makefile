TARGET=sniffer
CC=gcc
CFLAGS=-Wall -g 
LIBS=

OBJ = $(patsubst src/%.c, %.o, $(wildcard */*.c) )
SRC = $(wildcard */*.c)
HEADERS = $(wildcard */*.h)

.PHONY: clean
.DEFAULT: $(TARGET)

$(TARGET): $(OBJ) $(HEADERS)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)
	rm *.o

%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

uninstall: clean

clean:
	@-(rm $(TARGET)) || true
