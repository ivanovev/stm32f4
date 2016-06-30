
# Binaries will be generated with this name (.elf, .bin, .hex, etc)
# PROJ = $(shell basename `pwd`)
# PWD = $(shell pwd)

TRGT=arm-none-eabi-
CC=$(TRGT)gcc
AS=$(TRGT)as
LD=$(TRGT)ld
OBJCOPY=$(TRGT)objcopy
READELF=$(TRGT)readelf
GDB=$(TRGT)gdb

include $(ROOT_DIR)/util/util.mk

C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_rcc.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_cortex.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_dma.c

INCDIR += $(HAL_DIR)/Inc
INCDIR += $(CMSIS_DIR)/Device/ST/STM32F4xx/Include
INCDIR += $(CMSIS_DIR)/Include
INCDIR += $(ROOT_DIR)
INCDIR += $(ROOT_DIR)/util

MCPU = -mcpu=cortex-m4
MTHUMB = -mthumb
MTHUMB += -mthumb-interwork
#CFLAGS += -dD
CFLAGS += $(MCPU)
CFLAGS += $(MTHUMB)
CFLAGS += -std=c99
CFLAGS += -mlittle-endian
#CFLAGS += -mfloat-abi=soft
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -nodefaultlibs
CFLAGS += -funsigned-char
CFLAGS += -fshort-wchar
CFLAGS += -fno-builtin
CFLAGS += -fno-diagnostics-show-caret
CFLAGS += -I$(PWD)
CFLAGS += -DSTM32F407xx
CFLAGS += -DARM_MATH_CM4
#CFLAGS += -D__FPU_PRESENT=1
#CFLAGS += -D__FPU_USED
IINCDIR = $(patsubst %,-I%,$(INCDIR))
CFLAGS += $(IINCDIR)

LDFLAGS += $(MCPU)
LDFLAGS += $(MTHUMB)
LDFLAGS += -nostartfiles
LDFLAGS += -nostdlib
#LDFLAGS += -static
#LDFLAGS += -lgcc_s
LDFLAGS += -Wl,-T$(LDSCRIPT)
LDFLAGS += -Wl,-Map=build/$(PROJ).map
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -dead-strip
LDFLAGS += -Xlinker --wrap=memset
LDFLAGS += -Xlinker --wrap=memclr

# Put your stlink folder here so make burn will work.
STLINK=~/stlink.git

BUILDDIR = $(PWD)/build
OUTFILES = $(BUILDDIR)/$(PROJ).elf $(BUILDDIR)/$(PROJ).hex $(BUILDDIR)/$(PROJ).bin

OBJDIR = $(BUILDDIR)/obj
C_OBJS  = $(addprefix $(OBJDIR)/, $(notdir $(C_SRCS:.c=.o)))
A_OBJS  = $(addprefix $(OBJDIR)/, $(notdir $(A_SRCS:.s=.o)))
OBJS = $(C_OBJS) $(A_OBJS)

VPATH     = $(sort $(dir $(C_SRCS)) $(dir $(A_SRCS)))

.PHONY: proj clean echo debug

all: proj touch_version_c

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
	@wc -c build/$(PROJ).bin

$(C_OBJS) : $(OBJDIR)/%.o : %.c Makefile *.h
	@echo $<
	@$(CC) $< ${CFLAGS} -c -o $@

$(A_OBJS) : $(OBJDIR)/%.o : %.s Makefile
	@echo $<
	@$(CC) -c -o $@ $< ${CFLAGS}

touch_version_c:
	touch $(ROOT_DIR)/util/version.c

clean:
	@rm -rf $(BUILDDIR)

# Flash the STM32F4
burn:
	$(STLINK)/st-flash write $(PROJ).bin 0x8000000

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
	$(GDB) -ex "target remote localhost:3333; monitor reset halt; load" build/$(PROJ).elf

