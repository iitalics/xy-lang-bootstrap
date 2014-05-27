XY=xy.exe src/main.xy
XYFLAGS=-t

CC=gcc
CFLAGS=-Iclib

LINK=gcc
LFLAGS=

PROGRAM=test-file.exe
SOURCE=test-file.xy
CSOURCE=bin/test-file.c
COBJECT=bin/test-file.o


all: $(PROGRAM)

$(CSOURCE): $(SOURCE)
	$(XY) $(SOURCE) $(XYFLAGS) -o $(CSOURCE)
	
# TODO fix this linking stuff
#  it should be: "-Lclib -lxy" instead of "clib/libxy.a"
$(PROGRAM): bin $(COBJECT) clib/libxy.a
	$(LINK) $(LFLAGS) -o $(PROGRAM) $(COBJECT) clib/libxy.a

bin: 
	mkdir bin
	
clean:
	rm -f $(PROGRAM) $(CSOURCE)