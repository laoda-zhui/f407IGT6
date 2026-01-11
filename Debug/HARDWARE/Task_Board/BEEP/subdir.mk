################################################################################
# 自动生成的文件。不要编辑！
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# 将这些工具调用的输入和输出添加到构建变量 
C_SRCS += \
../HARDWARE/Task_Board/BEEP/Task_Beep.c 

OBJS += \
./HARDWARE/Task_Board/BEEP/Task_Beep.o 

C_DEPS += \
./HARDWARE/Task_Board/BEEP/Task_Beep.d 


# 每个子目录必须为构建它所贡献的源提供规则
HARDWARE/Task_Board/BEEP/%.o HARDWARE/Task_Board/BEEP/%.su HARDWARE/Task_Board/BEEP/%.cyclo: ../HARDWARE/Task_Board/BEEP/%.c HARDWARE/Task_Board/BEEP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../HARDWARE/Task_Board/Infrared -I../HARDWARE/Task_Board/bh1750 -I../HARDWARE/Delay -I../HARDWARE/Task_Board/Ultrasonic -I../HARDWARE/Task_Board/BEEP -I../HARDWARE/Core_Board/LED -I../HARDWARE/Core_Board/BEEP -I../HARDWARE/Time_Interrupt -I../HARDWARE/Core_Board/KEY -I../HARDWARE/Drive -I../HARDWARE/CAN -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-HARDWARE-2f-Task_Board-2f-BEEP

clean-HARDWARE-2f-Task_Board-2f-BEEP:
	-$(RM) ./HARDWARE/Task_Board/BEEP/Task_Beep.cyclo ./HARDWARE/Task_Board/BEEP/Task_Beep.d ./HARDWARE/Task_Board/BEEP/Task_Beep.o ./HARDWARE/Task_Board/BEEP/Task_Beep.su

.PHONY: clean-HARDWARE-2f-Task_Board-2f-BEEP

