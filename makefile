###############################################################################
#Filename    : RMP
#Author      : hrs
#Date        : 29/11/2023 16:55:31
#Licence     : LGPL v3+; see COPYING for details.
#Description : Generic Makefile (based on gcc). This file is intended
#              to be used with STM32F405RG, and the GNU toolchain.
###############################################################################

# Config ######################################################################
TARGET=BENCHMARK
#USER
#BENCHMARK
CPU=-mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=neon-fp16 -mno-thumb-interwork
CDEFS=

CFLAGS=-O0 -specs=nano.specs -fsigned-char -fno-common -fno-strict-aliasing -fdata-sections -ffunction-sections -ffreestanding
AFLAGS=-fdata-sections -ffunction-sections  
WFLAGS=-Wall -Wno-strict-aliasing
DFLAGS=-g3
LFLAGS=-specs=nano.specs -nostartfiles -specs=nosys.specs -Wl,--gc-sections,--cref

OBJDIR=Object
PREFIX=arm-none-eabi-
# End Config ##################################################################

# Source ######################################################################
INCS+=-I  Include
INCS+=
                
CSRCS+= Source/rme_benchmark.c
#Source/rme_benchmark.c
ASRCS+=Source/rme_benchmark_asm.s
#u.s #Source/rme_benchmark_asm.s
LDSCRIPT=benchmark.ld
#user.ld #benchmark.ld
LIBS=-lc -lnosys
# End Source ##################################################################

# Toolchain ###################################################################
ifdef GCC
CC=$(GCC)/$(PREFIX)gcc
AS=$(GCC)/$(PREFIX)gcc -x assembler-with-cpp
CP=$(GCC)/$(PREFIX)objcopy
LD=$(GCC)/$(PREFIX)gcc
SZ=$(GCC)/$(PREFIX)size
else
CC=$(PREFIX)gcc
AS=$(PREFIX)gcc -x assembler-with-cpp
CP=$(PREFIX)objcopy
LD=$(PREFIX)gcc
SZ=$(PREFIX)size
endif

HEX=$(CP) -O ihex
BIN=$(CP) -O binary -S
# End Toolchain ###############################################################

# User ########################################################################
-include user
# End User ####################################################################

# Build #######################################################################
COBJS=$(CSRCS:%.c=%.o)
CDEPS=$(CSRCS:%.c=%.d)
AOBJS=$(ASRCS:%.s=%.o)
ADEPS=$(ASRCS:%.s=%.d)

DEP=$(OBJDIR)/$(notdir $(@:%.o=%.d))
LST=$(OBJDIR)/$(notdir $(@:%.o=%.lst))
OBJ=$(OBJDIR)/$(notdir $@)
MAP=$(OBJDIR)/$(TARGET).map

# Build all
all: mkdir $(COBJS) $(AOBJS) $(TARGET).elf $(TARGET).hex $(TARGET).bin copy

# Copy-to-tftp-folder
#Object\USER.bin ..\..\M7M01_Eukaron\Project\MFGCC-XC7Z020\Object\USER.bin
copy:
	copy Object\BENCHMARK.bin ..\..\M7M01_Eukaron\Project\MFGCC-XC7Z020\Object\BENCHMARK.bin
# Create output folder
mkdir:
	$(shell if [ ! -e $(OBJDIR) ];then mkdir -p $(OBJDIR); fi)

# Compile C sources
%.o:%.c
	@echo "    CC      $(notdir $<)"
	@$(CC) -c $(CPU) $(CDEFS) $(INCS) $(CFLAGS) $(DFLAGS) -MMD -MP -MF "$(DEP)" -Wa,-a,-ad,-alms="$(LST)" "$<" -o "$(OBJ)"

# Assemble ASM sources
%.o:%.s
	@echo "    AS      $(notdir $<)"
	@$(AS) -c $(CPU) $(INCS) $(AFLAGS) $(DFLAGS) "$<" -o "$(OBJ)"

# Link ELF target file and print size
$(TARGET).elf:$(COBJS) $(AOBJS)
	@echo "    LD [P]  $(notdir $@)"
	@$(LD) $(OBJDIR)/*.o $(CPU) $(LFLAGS) $(DFLAGS) -T $(LDSCRIPT) -Wl,-Map=$(MAP) $(LIBS) -o $(OBJ)
	@$(SZ) $(OBJ)

# Create hex/bin programming files
$(TARGET).hex:$(TARGET).elf
	@echo "    HEX     $(notdir $@)"
	@$(HEX) "$(OBJDIR)/$<" "$(OBJDIR)/$@"

$(TARGET).bin:$(TARGET).elf
	@echo "    BIN     $(notdir $@)"
	@$(BIN) "$(OBJDIR)/$<" "$(OBJDIR)/$@"

# Clean up
clean:
	#-rm -rf $(OBJDIR)
	-if exist $(OBJDIR) (rd /s /q $(OBJDIR))

# Dependencies
-include $(wildcard $(OBJDIR)/*.d)
# End Build ###################################################################

# End Of File #################################################################

# Copyright (C) Evo-Devo Instrum. All rights reserved #########################

