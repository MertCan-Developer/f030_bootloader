################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../crc/Src/crc.c 

OBJS += \
./crc/Src/crc.o 

C_DEPS += \
./crc/Src/crc.d 


# Each subdirectory must supply rules for building sources it contributes
crc/Src/%.o crc/Src/%.su crc/Src/%.cyclo: ../crc/Src/%.c crc/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F030x8 -c -I../Core/Inc -I"C:/Users/Mert/STM32CubeIDE/workspace_1.15.0/NEW_WORKSPACE/f030_bootloader/crc/Inc" -I"C:/Users/Mert/STM32CubeIDE/workspace_1.15.0/NEW_WORKSPACE/f030_bootloader/bootloader_cm0/Inc" -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-crc-2f-Src

clean-crc-2f-Src:
	-$(RM) ./crc/Src/crc.cyclo ./crc/Src/crc.d ./crc/Src/crc.o ./crc/Src/crc.su

.PHONY: clean-crc-2f-Src

