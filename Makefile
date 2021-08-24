CC_NATIVE=gcc
CC_RG351=/work/351ELEC/build.351ELEC-RG351P.aarch64/toolchain/bin/aarch64-libreelec-linux-gnueabi-gcc

CFLAGS=-o2 -W -Wall
LDFLAGS=-levdev
INCLUDES=-Isrc

SRCS=src/main.c src/rg351.c

BINARY=anberjoycon

all: native rg351

native:
	$(CC_NATIVE) $(CFLAGS) $(INCLUDES) $(SRCS) -o $(BINARY).x86_64 $(LDFLAGS)

rg351:
	$(CC_RG351) $(CFLAGS) $(INCLUDES) $(SRCS) -o $(BINARY).aarch64 $(LDFLAGS)

clean:
	rm -f $(BINARY).x86_64 $(BINARY).aarch64
