################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HARDWARE/Time_Interrupt/Time_Interrupt.c 

OBJS += \
./HARDWARE/Time_Interrupt/Time_Interrupt.o 

C_DEPS += \
./HARDWARE/Time_Interrupt/Time_Interrupt.d 


# Each subdirectory must supply rules for building sources it contributes
HARDWARE/Time_Interrupt/%.o HARDWARE/Time_Interrupt/%.su HARDWARE/Time_Interrupt/%.cyclo: ../HARDWARE/Time_Interrupt/%.c HARDWARE/Time_Interrupt/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../HARDWARE/Time_Interrupt -I../HARDWARE/Drive -I../HARDWARE/CAN -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-HARDWARE-2f-Time_Interrupt

clean-HARDWARE-2f-Time_Interrupt:
	-$(RM) ./HARDWARE/Time_Interrupt/Time_Interrupt.cyclo ./HARDWARE/Time_Interrupt/Time_Interrupt.d ./HARDWARE/Time_Interrupt/Time_Interrupt.o ./HARDWARE/Time_Interrupt/Time_Interrupt.su

.PHONY: clean-HARDWARE-2f-Time_Interrupt

