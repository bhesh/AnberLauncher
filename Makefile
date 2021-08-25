CC_NATIVE   := gcc
CC_RG351    := /work/351ELEC/build.351ELEC-RG351P.aarch64/toolchain/bin/aarch64-libreelec-linux-gnueabi-gcc

CFLAGS      := -o2 -W -Wall
LDFLAGS     := -levdev
INCLUDES    := -Isrc

SRC_DIR     := src
SRCS        := main.c rg351.c

BUILD_DIR   := build
BINARY      := anberjoycon

OBJS        := $(SRCS:.c=.o)
OBJS_NATIVE := $(addprefix $(BUILD_DIR)/native/,$(OBJS))
OBJS_RG351  := $(addprefix $(BUILD_DIR)/rg351/,$(OBJS))

all: $(BUILD_DIR) native rg351

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/native
	mkdir -p $(BUILD_DIR)/rg351

native: $(OBJS_NATIVE) $(BUILD_DIR)
	$(CC_NATIVE) $(OBJS_NATIVE) -o $(BUILD_DIR)/native/$(BINARY) $(LDFLAGS)

rg351: $(OBJS_RG351) $(BUILD_DIR)
	$(CC_RG351) $(OBJS_RG351) -o $(BUILD_DIR)/rg351/$(BINARY) $(LDFLAGS)

$(BUILD_DIR)/native/%.o : $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC_NATIVE) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/rg351/%.o : $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC_RG351) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf build/
