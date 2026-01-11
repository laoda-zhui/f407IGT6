################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HARDWARE/BEEP/Beep.c 

OBJS += \
./HARDWARE/BEEP/Beep.o 

C_DEPS += \
./HARDWARE/BEEP/Beep.d 


# Each subdirectory must supply rules for building sources it contributes
HARDWARE/BEEP/%.o HARDWARE/BEEP/%.su HARDWARE/BEEP/%.cyclo: ../HARDWARE/BEEP/%.c HARDWARE/BEEP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../HARDWARE/LED -I../HARDWARE/Time_Interrupt -I../HARDWARE/KEY -I../HARDWARE/Drive -I../HARDWARE/CAN -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-HARDWARE-2f-BEEP

clean-HARDWARE-2f-BEEP:
	-$(RM) ./HARDWARE/BEEP/Beep.cyclo ./HARDWARE/BEEP/Beep.d ./HARDWARE/BEEP/Beep.o ./HARDWARE/BEEP/Beep.su

.PHONY: clean-HARDWARE-2f-BEEP

