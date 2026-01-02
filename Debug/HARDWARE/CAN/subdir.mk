################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HARDWARE/CAN/can_RxSolve.c \
../HARDWARE/CAN/can_Tx.c \
../HARDWARE/CAN/can_cmd.c 

OBJS += \
./HARDWARE/CAN/can_RxSolve.o \
./HARDWARE/CAN/can_Tx.o \
./HARDWARE/CAN/can_cmd.o 

C_DEPS += \
./HARDWARE/CAN/can_RxSolve.d \
./HARDWARE/CAN/can_Tx.d \
./HARDWARE/CAN/can_cmd.d 


# Each subdirectory must supply rules for building sources it contributes
HARDWARE/CAN/%.o HARDWARE/CAN/%.su HARDWARE/CAN/%.cyclo: ../HARDWARE/CAN/%.c HARDWARE/CAN/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../HARDWARE/CAN -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-HARDWARE-2f-CAN

clean-HARDWARE-2f-CAN:
	-$(RM) ./HARDWARE/CAN/can_RxSolve.cyclo ./HARDWARE/CAN/can_RxSolve.d ./HARDWARE/CAN/can_RxSolve.o ./HARDWARE/CAN/can_RxSolve.su ./HARDWARE/CAN/can_Tx.cyclo ./HARDWARE/CAN/can_Tx.d ./HARDWARE/CAN/can_Tx.o ./HARDWARE/CAN/can_Tx.su ./HARDWARE/CAN/can_cmd.cyclo ./HARDWARE/CAN/can_cmd.d ./HARDWARE/CAN/can_cmd.o ./HARDWARE/CAN/can_cmd.su

.PHONY: clean-HARDWARE-2f-CAN

