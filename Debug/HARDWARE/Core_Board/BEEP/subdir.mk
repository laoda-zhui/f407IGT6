################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HARDWARE/Core_Board/BEEP/Beep.c 

OBJS += \
./HARDWARE/Core_Board/BEEP/Beep.o 

C_DEPS += \
./HARDWARE/Core_Board/BEEP/Beep.d 


# Each subdirectory must supply rules for building sources it contributes
HARDWARE/Core_Board/BEEP/%.o HARDWARE/Core_Board/BEEP/%.su HARDWARE/Core_Board/BEEP/%.cyclo: ../HARDWARE/Core_Board/BEEP/%.c HARDWARE/Core_Board/BEEP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../HARDWARE/Task_Board/Photoresistance -I../HARDWARE/Task_Board/Infrared -I../HARDWARE/Task_Board/WheelLED -I../Core/Inc -I../HARDWARE/Task_Board/Photoresistance -I../HARDWARE/Task_Board/Infrared -I../HARDWARE/Task_Board/WheelLED -I../HARDWARE/Command -I../HARDWARE/Task_Board/bh1750 -I../HARDWARE/Task_Board/Voice -I../Task/Task1 -I../HARDWARE/Delay -I../HARDWARE/Task_Board/Ultrasonic -I../HARDWARE/Task_Board/BEEP -I../HARDWARE/Core_Board/LED -I../HARDWARE/Core_Board/BEEP -I../HARDWARE/Core_Board/RFID_RC522 -I../HARDWARE/Time_Interrupt -I../HARDWARE/Core_Board/KEY -I../HARDWARE/Drive -I../HARDWARE/CAN -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../HARDWARE/Command -I../HARDWARE/Task_Board/bh1750 -I../HARDWARE/Task_Board/Voice -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-HARDWARE-2f-Core_Board-2f-BEEP

clean-HARDWARE-2f-Core_Board-2f-BEEP:
	-$(RM) ./HARDWARE/Core_Board/BEEP/Beep.cyclo ./HARDWARE/Core_Board/BEEP/Beep.d ./HARDWARE/Core_Board/BEEP/Beep.o ./HARDWARE/Core_Board/BEEP/Beep.su

.PHONY: clean-HARDWARE-2f-Core_Board-2f-BEEP

