##########################################################################################################################
# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
# 	2022-11-03 - make ring buffer
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# 
# build options:
# 	targets:
# 		libringbuffer 	: build ring buffer as a static library
# 		ringbuffer		: build ring buffer executable
# 		test			: build test for ring buffer
# 		docs			: generate doxygen documentation
# 	
# 	build variables:
# 		build:
# 			Debug		: debug build, enables -DDEBUG pre-processor witch and adds debugging symbols to executable, optimization flags: -Og
# 			Release		: release build, with optimization flag -O2
# 			RelMinSize	: release build with minimum size, optimizatin flags: -O3
# 		
# 		platform:
# 			STM32		: build ring buffer for STM32F10xx, using arm-none-eabi-gcc (must be visible in path or supplied to make as GCC_PATH)
# 			Win			: build ring buffer for WIn, using gcc (muts be visible in bath)
# 
# ------------------------------------------------

######################################
# target
######################################
TARGET = ringbuffer


######################################
# building variables
######################################
# debug build?
ifeq ($(strip $(build)),)
build = Debug
endif

# platform
ifeq ($(strip $(platform)),)
platform = Win
endif

# optimization flags
ifeq ($(build), Release)
OPT = -O2

else ifeq ($(build), RelMinSize)
OPT = -Os

else
OPT = -Og
endif


#######################################
# paths
#######################################
# Build path
ROOT_BUILD_DIR = build
BUILD_DIR = $(ROOT_BUILD_DIR)/$(platform)/$(build)
LIB_BUILD_DIR = $(BUILD_DIR)/lib

#######################################
# Doxygen 
#######################################
DOCS_DIR = Docs
TEST_DIR = Test

ifeq ($(strip $(doxyfile)),)
DOXY_FILE = $(DOCS_DIR)/Doxyfile
endif


######################################
# source
######################################

# module sources
MODULE_SOURCES = \
Modules/ring_buffer/ring_buffer.c \


# platform specific sources
ifeq ($(platform), STM32)

# application sources
APP_SOURCES = \
Application/STM32/Core/Src/main.c \
Application/STM32/Core/Src/gpio.c \
Application/STM32/Core/Src/usart.c \
Application/STM32/Core/Src/stm32f1xx_it.c \
Application/STM32/Core/Src/system_stm32f1xx.c \
Application/STM32/Core/Src/sysmem.c \
Application/STM32/Core/Src/syscalls.c \

# platform sources
PLATFORM_SOURCES = \
Platform/STM32/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_gpio.c \
Platform/STM32/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_pwr.c \
Platform/STM32/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_exti.c \
Platform/STM32/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usart.c \
Platform/STM32/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dma.c \
Platform/STM32/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rcc.c \
Platform/STM32/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_utils.c \

else ifeq ($(platform), Win)

# application sources
APP_SOURCES = \
Application/Win/cORE/Src/main.c \

# platform sources
PLATFORM_SOURCES = \


endif

# # application sources
# APP_SOURCES = $(filter %.c,$(shell ls -R Application/$(platform)/Core/))

# # platform sources
# PLATFORM_SOURCES = $(filter %.h,$(shell ls -R Platform/$(platform)/Drivers/))

# module test sources
MODULE_TEST_SOURCES = \
$(TEST_DIR)/ring_buffer/test_ring_buffer.c \


# platfrm test runner sources
ifeq ($(platform), STM32)

PLATFORM_TEST_SOURCES = \
$(TEST_DIR)/Platform/STM32/ring_buffer/test_runner_ring_buffer.c \
$(TEST_DIR)/Platform/STM32/Common/Src/gpio.c \
$(TEST_DIR)/Platform/STM32/Common/Src/usart.c \
$(TEST_DIR)/Platform/STM32/Common/Src/stm32f1xx_it.c \
$(TEST_DIR)/Platform/STM32/Common/Src/syscalls.c \
$(TEST_DIR)/Platform/STM32/Common/Src/sysmem.c \
$(TEST_DIR)/Platform/STM32/Common/Src/system_stm32f1xx.c \

else ifeq ($(platform), Win)

PLATFORM_TEST_SOURCES = \
$(TEST_DIR)/Platform/Win/ring_buffer/test_runner_ring_buffer.c

endif


# unity sources
UNITY_SOURCES = \
Test/unity/src/unity.c \


# C sources to build
C_SOURCES += $(MODULE_SOURCES) 
C_SOURCES += $(PLATFORM_SOURCES)
C_SOURCES += $(APP_SOURCES)

C_TEST_SOURCES += $(MODULE_SOURCES)
C_TEST_SOURCES += $(PLATFORM_SOURCES)
C_TEST_SOURCES += $(MODULE_TEST_SOURCES)
C_TEST_SOURCES += $(PLATFORM_TEST_SOURCES)
C_TEST_SOURCES += $(UNITY_SOURCES)

# ASM sources 
ifeq ($(platform), STM32)

# startup code
ASM_SOURCES =  \
Platform/STM32/Startup/startup_stm32f103xb.s \


else ifeq ($(platform), Win)

ASM_SOURCES =  

endif

# Linker script
ifeq ($(platform), STM32)
LDSCRIPT = Platform/STM32/Linker/STM32F103CBTx_FLASH.ld
endif

#######################################
# includes
#######################################

# AS includes
AS_INCLUDES = 


# C includes
MODULE_INCLUDES = \
Modules/ring_buffer \
Modules/utils \


# platform specific includes
ifeq ($(platform), STM32)

PLATFORM_INCLUDES = \
Platform/$(platform)/Drivers/STM32F1xx_HAL_Driver/Inc \
Platform/$(platform)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
Platform/$(platform)/Drivers/CMSIS/Include \

else ifeq ($(platform), Win)

PLATFORM_INCLUDES = \

endif


# application includes
APP_INCLUDES = \
Application/$(platform)/Core/Inc \


# module test includes
MODULE_TEST_INCLUDES = \
Test \
Test/ring_buffer \

# platform test includes
PLATFORM_TEST_INCLUDES = \
$(TEST_DIR)/Platform/$(platform)/Common/Inc


# unity includes
UNITY_INCLUDES = \
Test/unity/src \


C_INCLUDES += $(MODULE_INCLUDES)
C_INCLUDES += $(PLATFORM_INCLUDES)
C_INCLUDES += $(APP_INCLUDES)
C_INCLUDES := $(addprefix -I,$(C_INCLUDES))

C_TEST_INCLUDES += $(MODULE_INCLUDES)
C_TEST_INCLUDES += $(PLATFORM_INCLUDES)
C_TEST_INCLUDES += $(PLATFORM_TEST_INCLUDES)
C_TEST_INCLUDES += $(MODULE_TEST_INCLUDES)
C_TEST_INCLUDES += $(UNITY_INCLUDES)
C_TEST_INCLUDES := $(addprefix -I,$(C_TEST_INCLUDES))

#######################################
# binaries
#######################################

ifeq ($(strip $(PREFIX)),)
ifeq ($(platform), STM32)
PREFIX = arm-none-eabi-
endif
endif

# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
AR = $(GCC_PATH)/$(PREFIX)ar
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
AR = $(PREFIX)ar
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
ifeq ($(platform), STM32)
CPU = -mcpu=cortex-m3
endif

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi


# mcu
ifeq ($(platform), STM32)
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
endif

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
ifeq ($(platform), STM32)
C_DEFS =  \
-DUSE_FULL_LL_DRIVER \
-DUSE_FULL_ASSERT=1U \
-DHSE_VALUE=8000000 \
-DHSE_STARTUP_TIMEOUT=100 \
-DLSE_STARTUP_TIMEOUT=5000 \
-DLSE_VALUE=32768 \
-DHSI_VALUE=8000000 \
-DLSI_VALUE=40000 \
-DVDD_VALUE=3300 \
-DPREFETCH_ENABLE=1 \
-DSTM32F103xB
endif

ifeq ($(build), Debug)
C_DEFS += -DDEBUG

else ifeq ($(build), Test)
C_DEFS += -DDEBUG
endif

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -Wextra -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(C_TEST_INCLUDES) $(OPT) -Wall -Wextra -Wpedantic -fdata-sections -ffunction-sections

ifeq ($(build), Debug)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################

# libraries
ifeq ($(platform), STM32)
LIBS = -lc -lm -lnosys 
endif
LIBDIR =

# linker flags
ifeq ($(platform), STM32)
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
endif

# output/executable
ifeq ($(platform), STM32)

EXECUTABLES = $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin 
TEST_EXECUTABLES = $(BUILD_DIR)/test_$(TARGET).elf $(BUILD_DIR)/test_$(TARGET).hex $(BUILD_DIR)/test_$(TARGET).bin

else ifeq ($(platform), Win)

EXECUTABLES = $(BUILD_DIR)/$(TARGET).exe
TEST_EXECUTABLES = $(BUILD_DIR)/test_$(TARGET).exe

endif

# default action: build all
all: $(TARGET) lib$(TARGET) test 

$(TARGET): $(EXECUTABLES)

lib$(TARGET): $(LIB_BUILD_DIR)/lib$(TARGET).a

test: $(TEST_EXECUTABLES)


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

TEST_OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_TEST_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_TEST_SOURCES)))

TEST_OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# object files target for C files
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	@ECHO compiling file: $<
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@
	@ECHO 

# object files targets for assembly files
$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@ECHO compiling file: $<
	$(AS) -c $(CFLAGS) $< -o $@
	@ECHO 

# ELF target
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

# hex target
$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

# binary target
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir -p $@	

# Win executable target
$(BUILD_DIR)/$(TARGET).exe: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/test_$(TARGET).elf: $(TEST_OBJECTS) Makefile
	$(CC) $(TEST_OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

# Win test executable target
$(BUILD_DIR)/test_$(TARGET).exe: $(TEST_OBJECTS) Makefile
	@ECHO
	$(CC) $(TEST_OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@
	@ECHO

# library objects
LIB_OBJECTS = $(addprefix $(LIB_BUILD_DIR)/,$(notdir $(MODULE_SOURCES:.c=.o)))

# library objects
$(LIB_BUILD_DIR)/%.o: %.c Makefile | $(LIB_BUILD_DIR) 
	$(CC) -c $(CFLAGS) -fPIC $< -o $@

# library target
$(LIB_BUILD_DIR)/lib$(TARGET).a: $(LIB_OBJECTS) Makefile
	$(AR) -rcs $@ $<

# library output dir
$(LIB_BUILD_DIR):
	mkdir -p $@	

docs:
	doxygen $(DOXY_FILE)

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

clean_all:
	-rm -fR $(ROOT_BUILD_DIR)

clean_docs:
	-rm -fR $(DOCS_DIR)/Doxygen/html

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean clean_all docs

# *** EOF ***
