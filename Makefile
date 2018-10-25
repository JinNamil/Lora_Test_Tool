OBJDIR	:= obj
DEPDIR	:= dep
BUILD	:= debug

CPU		:= cortex-m0
CORE	:= armv6-m
ENDIAN	:= little-endian

PREFIX=arm-none-eabi-
CC=$(PREFIX)gcc
LD=$(PREFIX)gcc
AR=$(PREFIX)ar
AS=$(PREFIX)as
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump
V:=@


ROOT_PATH	:=	../..
APP_NAME	:= $(notdir $(shell pwd))

vpath %.c	$(ROOT_PATH)/application/$(APP_NAME)
APP_INC		:= -I$(ROOT_PATH)/application/$(APP_NAME)
APP_SRC		:= main.c \
			   ewbm_lt_comm.c \
			   ewbm_lt_flash.c
APP_LD		:= ms500.ld


#
# Header File Define
#
CMSIS_INC		:= -I$(ROOT_PATH)/Drivers/CMSIS/include
DEVICE_INC		:= -I$(ROOT_PATH)/Drivers/MS500/Device/inc
DRIVER_INC		:= -I$(ROOT_PATH)/Drivers/MS500/Drivers/inc
SECURITY_INC	:= -I$(ROOT_PATH)/Drivers/MS500/Security/inc
UTILS_INC		:= -I$(ROOT_PATH)/Drivers/MS500/Utils/inc
CRYPTO_INC		:= -I$(ROOT_PATH)/Service/Crypto/inc

INCLUDES := $(CMSIS_INC)
INCLUDES += $(DEVICE_INC)
INCLUDES += $(DRIVER_INC)
INCLUDES += $(SECURITY_INC)
INCLUDES += $(UTILS_INC)
INCLUDES += $(CRYPTO_INC)
INCLUDES += $(FIDO_INC)
INCLUDES += $(APP_INC)
INCLUDES := $(strip $(INCLUDES))

#
# Source Code
#
vpath %.c	$(ROOT_PATH)/Drivers/MS500/Device/src
vpath %.c	$(ROOT_PATH)/Drivers/MS500/Drivers/src
vpath %.c	$(ROOT_PATH)/Drivers/MS500/Security/src
vpath %.c	$(ROOT_PATH)/Drivers/MS500/Utils/src

DEVICE_SRC	 := startup_MS500.c \
				system_MS500.c

DRIVER_SRC	 := hal_clock.c \
				hal_delay.c \
				hal_dma.c \
				hal_flash.c \
				hal_gpio.c \
				hal_i2c.c \
				hal_irq.c \
				hal_pinmap.c \
				hal_pmu.c \
				hal_rtc.c \
				hal_sdioh.c \
				hal_sdios.c \
				hal_spi.c \
				hal_syscon.c \
				hal_tmr.c \
				hal_uart.c \
				hal_wwdt.c \

SECURITY_SRC := hal_spacc.c \
				hal_trng.c \
				hal_pka.c

UTILS_SRC	 := util_printf.c

SOURCES := $(DEVICE_SRC)
SOURCES += $(DRIVER_SRC)
SOURCES += $(UTILS_SRC)
SOURCES += $(SECURITY_SRC)
SOURCES += $(FIDO_SRC)
SOURCES += $(APP_SRC)
SOURCES := $(strip $(SOURCES))

LIBS := $(LIBC) $(LIBGCC) $(LIBM) $(LIBNOSYS) $(CRT0) $(CRTI)
LIBS += $(ROOT_PATH)/Service/Crypto/lib/libCrypto.a

DEFINES  := -DMS500
MCUFLAG  := -mcpu=$(CPU) -march=$(CORE) -m$(ENDIAN) -mthumb -msoft-float
LD_FLAGS := -T$(APP_LD) -O2 $(MCUFLAG) -Wl,--gc-sections,-Map=$(BUILD)/$(APP_NAME).map
AS_FLAGS := $(MCUFLAG) $(INCLUDES)
C_FLAGS  := -O2 $(MCUFLAG) $(INCLUDES)

OBJS := $(strip $(patsubst %.c,$(OBJDIR)/%.o,$(SOURCES)))
DEPS := $(strip $(patsubst %.c,$(DEPDIR)/%.dep,$(SOURCES)))

.SUFFIXES: .o .s
$(OBJDIR)/%.o : %.s
	@if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR) ; fi
	@echo "Assembly Compile : $<"
	$(V)$(AS) $(AS_FLAGS) -c $< -o $@

.SUFFIXES: .o .c
$(OBJDIR)/%.o : %.c
	@if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR) ; fi
	@echo "C Compile : $<"
	$(V)$(CC) $(C_FLAGS) $(DEFINES) -c $< -o $@



.SUFFIXES: %.dep .c
$(DEPDIR)/%.dep : %.c
	@echo "Dependency Check : $<"
	@if [ ! -d $(DEPDIR) ]; then mkdir -p $(DEPDIR) ; fi
	$(V)$(CC) $(C_FLAGS) -M $< > $@


.SUFFIXES: %.dep .s
$(DEPDIR)/%.dep : %.s
	@echo "Dependency Check : $<"
	@if [ ! -d $(DEPDIR) ]; then mkdir -p $(DEPDIR) ; fi
	$(V)$(CC) $(C_FLAGS) -M $< > $@

.PHONY: all clean bin asm
all:$(APP_NAME) bin asm
$(APP_NAME):$(OBJS)
	@if [ ! -d $(BUILD) ]; then mkdir -p $(BUILD) ; fi
	@echo "Linking $(BUILD)/$(APP_NAME).elf"
	$(LD) $(LD_FLAGS) -o $(BUILD)/$(APP_NAME).elf $^ $(LIBS)

dep: $(DEPS)

bin:
	$(OBJCOPY) --gap-fill=0xff -O binary  $(BUILD)/$(APP_NAME).elf $(BUILD)/$(APP_NAME).bin

asm:
	$(OBJDUMP) --disassemble-all $(BUILD)/$(APP_NAME).elf > $(BUILD)/$(APP_NAME).S

clean:
	-rm -rf $(OBJDIR)
	-rm -rf $(DEPDIR)
	-rm -rf $(BUILD)


test:
	@echo "DEFINES : $(DEFINES)"
	@echo "LIBS: $(LIBS)"
	@echo "OBJS: $(OBJS)"
	@echo "DEPS: $(DEPS)"
	@echo "CFLAGS: $(C_FLAGS)"
