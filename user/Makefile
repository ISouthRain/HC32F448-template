#====================================================================#
# Output directory, 输出目录
OUTPUT_DIR = ../build/Debug
# 生成目标文件
TARGET = HC32F448KCTI
# 程序地址
ADDRESS = 0x0

#====================================================================#
# Output files
EXECUTABLE = $(OUTPUT_DIR)/$(TARGET).elf
BIN_IMAGE = $(OUTPUT_DIR)/$(TARGET).bin
MAP_FILE = $(OUTPUT_DIR)/$(TARGET).map

#====================================================================#
# Cross Compiler
TOOL = arm-none-eabi-
CC = $(TOOL)gcc
OBJCOPY = $(TOOL)objcopy

#====================================================================#
# Flags
CFLAGS = -mfloat-abi=soft -Og -Wall -g --specs=nosys.specs --specs=nano.specs -ffunction-sections -fdata-sections 
# 生成 map 文件
CFLAGS += -Wl,-Map=$(MAP_FILE)
# 构架
CFLAGS += -mcpu=cortex-m4
# 宏定义
CFLAGS += -D USE_DDL_DRIVER
CFLAGS += -D HC32F448

# hc32f448-flash
# 链接文件
CFLAGS += -Wl,-T,../cmsis/Device/HDSC/hc32f4xx/Source/GCC/linker/HC32F448xC.ld

#====================================================================#
# Libraries
# hc32f448 libraries
ST_LIB = ../hc32_ll_driver
BSP_LIB = ../bsp
# CMSIS libraries
CFLAGS += -I../cmsis/Include
# StdPeriph includes
CFLAGS += -I$(ST_LIB)/inc
CFLAGS += -I../cmsis/Device/HDSC/hc32f4xx/Include/
CFLAGS += -I.
CFLAGS += -I./
CFLAGS += -I$(BSP_LIB)/ev_hc32f448_lqfp80
CFLAGS += -I$(BSP_LIB)/components/24cxx
CFLAGS += -I$(BSP_LIB)/components/gt9xx
CFLAGS += -I$(BSP_LIB)/components/nt35510
CFLAGS += -I$(BSP_LIB)/components/tca9539
CFLAGS += -I$(BSP_LIB)/components/w25qxx

#====================================================================#
# Setup system clock
SRC = ../cmsis/Device/HDSC/hc32f4xx/Source/system_hc32f448.c
# StdPeriph
SRC += $(wildcard $(ST_LIB)/src/*.c)
# BSPPeriph
SRC += $(wildcard $(BSP_LIB)/ev_hc32f448_lqfp80/*.c)
SRC += $(wildcard $(BSP_LIB)/components/24cxx/*.c)
SRC += $(wildcard $(BSP_LIB)/components/gt9xx/*.c)
SRC += $(wildcard $(BSP_LIB)/components/nt35510/*.c)
SRC += $(wildcard $(BSP_LIB)/components/tca9539/*.c)
SRC += $(wildcard $(BSP_LIB)/components/w25qxx/*.c)

# # Major programs
SRC += $(wildcard ./*.c)
# #StdPeriph
# SRC+=$(ST_LIB)/src/misc.c \
# 	$(ST_LIB)/src/stm32f10x_rcc.c \
# 	$(ST_LIB)/src/stm32f10x_gpio.c \
# #Major programs
# SRC+=./main.c \

#====================================================================#
# STM32 startup file
STARTUP = ../cmsis/Device/HDSC/hc32f4xx/Source/GCC/startup_hc32f448.S

#====================================================================#
# Make rules

# Make all
all: $(BIN_IMAGE)

$(BIN_IMAGE): $(EXECUTABLE)
	$(OBJCOPY) -O binary $^ $@

STARTUP_OBJ = $(OUTPUT_DIR)/startup_hc32f448.o

$(STARTUP_OBJ): $(STARTUP) 
	$(CC) $(CFLAGS) $^ -c -o $@

$(EXECUTABLE): $(SRC) $(STARTUP_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Make clean
clean:
	rm -rf $(OUTPUT_DIR)

# Make flash
flash:
	st-flash write $(BIN_IMAGE) $(ADDRESS)

openocd:
	openocd -f "../commom/openocd.cfg"

gdb:
	arm-none-eabi-gdb -x ../commom/gdb_init.gdb

gdbtui:
	arm-none-eabi-gdb -tui -x ../commom/gdb_init.gdb

#====================================================================
.PHONY: all clean flash openocd gdb gdbtui
