
# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PROJ_NAME = $(shell basename `pwd`)
PWD = $(shell pwd)

TRGT=arm-none-eabi-
CC=$(TRGT)gcc
AS=$(TRGT)as
LD=$(TRGT)ld
OBJCOPY=$(TRGT)objcopy
READELF=$(TRGT)readelf
GDB=$(TRGT)gdb

C_SRCS += $(HAL_DIR)/stm32f4xx_hal.c
C_SRCS += $(HAL_DIR)/stm32f4xx_hal_rcc.c
C_SRCS += $(HAL_DIR)/stm32f4xx_hal_cortex.c
C_SRCS += $(HAL_DIR)/stm32f4xx_hal_dma.c
C_SRCS += $(UTIL_DIR)/system_stm32f4xx.c
C_SRCS += $(UTIL_DIR)/system_clock.c
C_SRCS += $(UTIL_DIR)/systick_it.c
C_SRCS += $(UTIL_DIR)/queue.c
C_SRCS += $(UTIL_DIR)/util.c
#C_SRCS += $(UTIL_DIR)/startup_stm32f4xx.c
#C_SRCS += $(UTIL_DIR)/syscalls.c

# add startup file to build
#A_SRCS += $(UTIL_DIR)/startup_stm32f4xx.s
A_SRCS += $(UTIL_DIR)/startup_stm32f407xx.s

INCDIR += $(HAL_DIR)/Inc
INCDIR += $(HAL_DIR)/../CMSIS/Device/ST/STM32F4xx/Include
INCDIR += $(HAL_DIR)/../CMSIS/Include
INCDIR += $(ROOT_DIR)
INCDIR += $(UTIL_DIR)

MCPU = -mcpu=cortex-m4
MTHUMB = -mthumb
#MTHUMB += -mthumb-interwork
CFLAGS += -O0 -g -Wall
#CFLAGS += -dD
CFLAGS += $(MCPU)
CFLAGS += $(MTHUMB)
CFLAGS += -nodefaultlibs
CFLAGS += -mlittle-endian
CFLAGS += -mfloat-abi=soft
#CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -funsigned-char
CFLAGS += -I.
CFLAGS += -DSTM32F407xx
IINCDIR = $(patsubst %,-I%,$(INCDIR))
CFLAGS += $(IINCDIR)

LDSCRIPT_RAM = $(UTIL_DIR)/stm32f4_ram.ld
LDSCRIPT_FLASH = $(UTIL_DIR)/stm32f4_flash.ld
LDSCRIPT = $(LDSCRIPT_RAM)
#LDSCRIPT = $(LDSCRIPT_FLASH)
ifeq ($(LDSCRIPT), $(LDSCRIPT_RAM))
    CFLAGS += -DVECT_TAB_SRAM
endif

LDFLAGS += $(MCPU)
LDFLAGS += $(MTHUMB)
#LDFLAGS += -nostartfiles
LDFLAGS += -nostdlib
LDFLAGS += -Wl,-T$(LDSCRIPT)
LDFLAGS += -Wl,-Map=build/$(PROJ_NAME).map
LDFLAGS += -Wl,--gc-sections
#LDFLAGS += -dead-strip

# Put your stlink folder here so make burn will work.
STLINK=~/stlink.git

BUILDDIR = $(PWD)/build
OUTFILES = $(BUILDDIR)/$(PROJ_NAME).elf $(BUILDDIR)/$(PROJ_NAME).hex $(BUILDDIR)/$(PROJ_NAME).bin

OBJDIR = $(BUILDDIR)/obj
C_OBJS  = $(addprefix $(OBJDIR)/, $(notdir $(C_SRCS:.c=.o)))
A_OBJS  = $(addprefix $(OBJDIR)/, $(notdir $(A_SRCS:.s=.o)))
OBJS = $(C_OBJS) $(A_OBJS)


VPATH     = $(sort $(dir $(C_SRCS)) $(dir $(A_SRCS)))

.PHONY: proj clean echo debug

all: proj

proj: $(OUTFILES)

$(OBJS): | $(OBJDIR)

$(BUILDDIR) $(OBJDIR):
	@mkdir -p $(OBJDIR)

%.elf: $(OBJS)
	@echo elf, ldscript: $(LDSCRIPT)
	@$(CC) $^ $(LDFLAGS) -o $@ 
	@$(READELF) -s $@ > $(@:.elf=.symtab)
	@echo "BOARD = $(BOARD)"

%.hex: %.elf
	@echo hex
	@$(OBJCOPY) --keep-file-symbols -O ihex $< $@

%.bin: %.elf
	@echo bin
	@$(OBJCOPY) --keep-file-symbols -O binary $< $@
	@wc -c build/$(PROJ_NAME).bin

$(C_OBJS) : $(OBJDIR)/%.o : %.c Makefile *.h
	@echo $<
	@$(CC) $< ${CFLAGS} -c -o $@

$(A_OBJS) : $(OBJDIR)/%.o : %.s Makefile
	@echo $<
	@$(CC) -c -o $@ $< ${CFLAGS}

clean:
	@echo $(CFLAGS)
	@rm -rf $(BUILDDIR)

# Flash the STM32F4
burn:
	$(STLINK)/st-flash write $(PROJ_NAME).bin 0x8000000

echo:
	@echo $(OBJDIR)
	@echo $(BUILDDIR)
	@echo $(INCDIR)
	@echo $(C_SRCS)
	@echo $(CFLAGS)
	@echo $(TTT)
	@echo $(OBJS)
	@echo $(PWD)

debug:
	$(GDB) -ex "target remote localhost:3333; monitor reset halt; load" build/$(PROJ_NAME).elf

