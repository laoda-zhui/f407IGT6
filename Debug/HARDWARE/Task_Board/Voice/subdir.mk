################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HARDWARE/Task_Board/Voice/Voice.c 

OBJS += \
./HARDWARE/Task_Board/Voice/Voice.o 

C_DEPS += \
./HARDWARE/Task_Board/Voice/Voice.d 


# Each subdirectory must supply rules for building sources it contributes
HARDWARE/Task_Board/Voice/%.o HARDWARE/Task_Board/Voice/%.su HARDWARE/Task_Board/Voice/%.cyclo: ../HARDWARE/Task_Board/Voice/%.c HARDWARE/Task_Board/Voice/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../HARDWARE/Task_Board/Infrared -I../HARDWARE/Task_Board/WheelLED -I../HARDWARE/Task_Board/Photoresistance -I../HARDWARE/Task_Board/bh1750 -I../HARDWARE/Task_Board/Voice -I../Task/Task1 -I../HARDWARE/Delay -I../HARDWARE/Task_Board/Ultrasonic -I../HARDWARE/Task_Board/BEEP -I../HARDWARE/Core_Board/LED -I../HARDWARE/Core_Board/BEEP -I../HARDWARE/Core_Board/RFID_RC522 -I../HARDWARE/Time_Interrupt -I../HARDWARE/Core_Board/KEY -I../HARDWARE/Drive -I../HARDWARE/CAN -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-HARDWARE-2f-Task_Board-2f-Voice

clean-HARDWARE-2f-Task_Board-2f-Voice:
	-$(RM) ./HARDWARE/Task_Board/Voice/Voice.cyclo ./HARDWARE/Task_Board/Voice/Voice.d ./HARDWARE/Task_Board/Voice/Voice.o ./HARDWARE/Task_Board/Voice/Voice.su

.PHONY: clean-HARDWARE-2f-Task_Board-2f-Voice

