CC=gcc
CFLAGS=-Wall -O3

OBJECTS=xy-other.o xy-list.o xy-string.o \
		xy-closure.o xy-gc.o xy-oper.o \
		xy-value.o xy-main.o

OUTPUT=libxy.a


all: $(OUTPUT)


$(OUTPUT): $(OBJECTS)
	ar rcs $(OUTPUT) $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(OUTPUT)