################################################################################
# 自动生成的文件。不要编辑！
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# 此处定义了参与生成的所有源
-include sources.mk
-include HARDWARE/Time_Interrupt/subdir.mk
-include HARDWARE/Task_Board/bh1750/subdir.mk
-include HARDWARE/Task_Board/WheelLED/subdir.mk
-include HARDWARE/Task_Board/Ultrasonic/subdir.mk
-include HARDWARE/Task_Board/Infrared/subdir.mk
-include HARDWARE/Task_Board/BEEP/subdir.mk
-include HARDWARE/Drive/subdir.mk
-include HARDWARE/Delay/subdir.mk
-include HARDWARE/Core_Board/LED/subdir.mk
-include HARDWARE/Core_Board/KEY/subdir.mk
-include HARDWARE/Core_Board/BEEP/subdir.mk
-include HARDWARE/CAN/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := f407IGT6-can
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# 将这些工具调用的输入和输出添加到构建变量 
EXECUTABLES += \
f407IGT6-can.elf \

MAP_FILES += \
f407IGT6-can.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
f407IGT6-can.list \


# 所有目标
all: main-build

# 主构建目标
main-build: f407IGT6-can.elf secondary-outputs

# 工具调用
f407IGT6-can.elf f407IGT6-can.map: $(OBJS) $(USER_OBJS) C:\Users\zhui\AppData\Roaming\32\workspace\f407igt6_Car\f407IGT6\STM32F407IGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "f407IGT6-can.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\zhui\AppData\Roaming\32\workspace\f407igt6_Car\f407IGT6\STM32F407IGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="f407IGT6-can.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo '已结束构建目标： $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo '已结束构建： $@'
	@echo ' '

f407IGT6-can.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "f407IGT6-can.list"
	@echo '已结束构建： $@'
	@echo ' '

# 其他目标
clean:
	-$(RM) default.size.stdout f407IGT6-can.elf f407IGT6-can.list f407IGT6-can.map
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
