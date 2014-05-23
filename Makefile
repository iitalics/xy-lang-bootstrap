XY=xy.exe src/main.xy
XYFLAGS=

CC=gcc
CFLAGS=-Iclib

LINK=gcc
LFLAGS=-Lclib -lxy

PROGRAM=test-file.exe
SOURCE=test-file.xy
CSOURCE=bin/test-file.c
COBJECT=bin/test-file.o


all: $(PROGRAM)

$(CSOURCE): $(SOURCE)
	$(XY) $(SOURCE) $(XYFLAGS) -o $(CSOURCE)
	
$(PROGRAM): $(COBJECT)
	$(LINK) $(LFLAGS) -o $(PROGRAM) $(COBJECT) clib/libxy.a
	
	
clean:
	rm -f $(PROGRAM) $(CSOURCE)