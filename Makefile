# AnberDriver Makefile
#
# The aarch64 toolchain used is from 351ELEC (https://github.com/351ELEC/351ELEC).
#
# To build it:
# > DEVICE=RG351P ARCH=aarch64 ./script/build toolchain
#
# Alternatively:
# > DEVICE=RG351V ARCH=aarch64 ./script/build toolchain
#
CC_NATIVE    := gcc
CC_RG351P     := /work/351ELEC/build.351ELEC-RG351P.aarch64/toolchain/bin/aarch64-libreelec-linux-gnueabi-gcc
CC_RG351V     := /work/351ELEC/build.351ELEC-RG351V.aarch64/toolchain/bin/aarch64-libreelec-linux-gnueabi-gcc

CFLAGS       := -o2 -W -Wall
LDFLAGS      := -levdev
INCLUDES     := -Isrc

SRC_DIR      := src
SRCS         := main.c rg351-input.c

BUILD_DIR    := build
NATIVE_BUILD := $(BUILD_DIR)/native
RG351P_BUILD := $(BUILD_DIR)/rg351p
RG351V_BUILD := $(BUILD_DIR)/rg351v

OBJS         := $(SRCS:.c=.o)
OBJS_NATIVE  := $(addprefix $(BUILD_DIR)/native/,$(OBJS))
OBJS_RG351P  := $(addprefix $(BUILD_DIR)/rg351p/,$(OBJS))
OBJS_RG351V  := $(addprefix $(BUILD_DIR)/rg351v/,$(OBJS))

BINARY       := anberdriver

all: native rg351p #rg351v

# Native build
$(NATIVE_BUILD):
	mkdir -p $(NATIVE_BUILD)

native: $(OBJS_NATIVE)
	$(CC_NATIVE) $(OBJS_NATIVE) -o $(NATIVE_BUILD)/$(BINARY) $(LDFLAGS)

$(NATIVE_BUILD)/%.o : $(SRC_DIR)/%.c $(NATIVE_BUILD)
	$(CC_NATIVE) $(CFLAGS) $(INCLUDES) -c $< -o $@

# rg351p/m build
$(RG351P_BUILD):
	mkdir -p $(RG351P_BUILD)

rg351p: $(OBJS_RG351P)
	$(CC_RG351P) $(OBJS_RG351P) -o $(RG351P_BUILD)/$(BINARY) $(LDFLAGS)

$(RG351P_BUILD)/%.o : $(SRC_DIR)/%.c $(RG351P_BUILD)
	$(CC_RG351P) $(CFLAGS) $(INCLUDES) -c $< -o $@

# rg351v build
$(RG351V_BUILD):
	mkdir -p $(RG351V_BUILD)

rg351v: $(OBJS_RG351V)
	$(CC_RG351V) $(OBJS_RG351V) -o $(RG351V_BUILD)/$(BINARY) $(LDFLAGS)

$(RG351V_BUILD)/%.o : $(SRC_DIR)/%.c $(RG351V_BUILD)
	$(CC_RG351V) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR)
