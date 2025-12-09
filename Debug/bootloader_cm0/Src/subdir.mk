################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bootloader_cm0/Src/bl_cm0.c 

OBJS += \
./bootloader_cm0/Src/bl_cm0.o 

C_DEPS += \
./bootloader_cm0/Src/bl_cm0.d 


# Each subdirectory must supply rules for building sources it contributes
bootloader_cm0/Src/%.o bootloader_cm0/Src/%.su bootloader_cm0/Src/%.cyclo: ../bootloader_cm0/Src/%.c bootloader_cm0/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F030x8 -c -I../Core/Inc -I"C:/Users/Mert/STM32CubeIDE/workspace_1.15.0/NEW_WORKSPACE/f030_bootloader/crc/Inc" -I"C:/Users/Mert/STM32CubeIDE/workspace_1.15.0/NEW_WORKSPACE/f030_bootloader/bootloader_cm0/Inc" -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-bootloader_cm0-2f-Src

clean-bootloader_cm0-2f-Src:
	-$(RM) ./bootloader_cm0/Src/bl_cm0.cyclo ./bootloader_cm0/Src/bl_cm0.d ./bootloader_cm0/Src/bl_cm0.o ./bootloader_cm0/Src/bl_cm0.su

.PHONY: clean-bootloader_cm0-2f-Src

