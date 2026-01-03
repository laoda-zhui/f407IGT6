################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HARDWARE/Drive/Motor.c \
../HARDWARE/Drive/PID.c 

OBJS += \
./HARDWARE/Drive/Motor.o \
./HARDWARE/Drive/PID.o 

C_DEPS += \
./HARDWARE/Drive/Motor.d \
./HARDWARE/Drive/PID.d 


# Each subdirectory must supply rules for building sources it contributes
HARDWARE/Drive/%.o HARDWARE/Drive/%.su HARDWARE/Drive/%.cyclo: ../HARDWARE/Drive/%.c HARDWARE/Drive/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../HARDWARE/Time_Interrupt -I../HARDWARE/Drive -I../HARDWARE/CAN -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-HARDWARE-2f-Drive

clean-HARDWARE-2f-Drive:
	-$(RM) ./HARDWARE/Drive/Motor.cyclo ./HARDWARE/Drive/Motor.d ./HARDWARE/Drive/Motor.o ./HARDWARE/Drive/Motor.su ./HARDWARE/Drive/PID.cyclo ./HARDWARE/Drive/PID.d ./HARDWARE/Drive/PID.o ./HARDWARE/Drive/PID.su

.PHONY: clean-HARDWARE-2f-Drive

