# Disable built-in rules and suffixes
MAKEFLAGS += -rR
.SUFFIXES:

# Final output name
override OUTPUT := myos

# Compiler settings
CC := cc
CFLAGS := -g -O2 -pipe
CPPFLAGS :=

# NASM settings
NASMFLAGS := -F dwarf -g

# Linker settings
LDFLAGS :=

# Clang detection
override CC_IS_CLANG := $(shell ! $(CC) --version 2>/dev/null | grep 'clang' >/dev/null 2>&1; echo $$?)
ifeq ($(CC_IS_CLANG),1)
    override CC += -target x86_64-unknown-none
endif

# Internal compiler flags
override CFLAGS += \
    -Wall -Wextra -std=gnu11 -ffreestanding -fno-stack-protector -fno-stack-check \
    -fno-PIC -m64 -march=x86-64 -mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone \
    -mcmodel=kernel

# Internal preprocessor flags
override CPPFLAGS := \
    -Isrc/include \
    $(CPPFLAGS) \
    -DLIMINE_API_REVISION=3 \
    -MMD -MP

# Internal linker flags
override LDFLAGS += \
    -Wl,-m,elf_x86_64 \
    -Wl,--build-id=none \
    -nostdlib -static \
    -z max-page-size=0x1000 \
    -T linker.ld

# Source files
override SRCFILES := $(shell find src -type f \( -name "*.c" -o -name "*.S" -o -name "*.asm" \) | LC_ALL=C sort)
override RELSRCFILES := $(patsubst src/%,%,$(SRCFILES))
override CFILES := $(filter %.c,$(RELSRCFILES))
override ASFILES := $(filter %.S,$(RELSRCFILES))
override NASMFILES := $(filter %.asm,$(RELSRCFILES))

# Object files
override OBJ := obj/font.o \
	$(addprefix obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))

# Header dependency tracking
override HEADER_DEPS := $(addprefix obj/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

# Default target
.PHONY: all
all: iso_root/boot/$(OUTPUT)

# Include dependencies
-include $(HEADER_DEPS)

# Build final binary + embed PSF section
iso_root/boot/$(OUTPUT): GNUmakefile linker.ld $(OBJ) font
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@
	objcopy --set-section-flags .psf_font=alloc,readonly $@ $@

# Compile .c
obj/%.c.o: src/%.c GNUmakefile
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compile .S
obj/%.S.o: src/%.S GNUmakefile
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compile .asm
obj/%.asm.o: src/%.asm GNUmakefile
	mkdir -p "$$(dirname $@)"
	nasm $(NASMFLAGS) $< -o $@

# Compile PSF font to .o
obj/font.o: font
	mkdir -p "$$(dirname $@)"
	objcopy -I binary -O elf64-x86-64 -B i386 $< $@

# Clean target
.PHONY: clean
clean:
	rm -rf bin obj iso_root/boot/$(OUTPUT) iso_root
