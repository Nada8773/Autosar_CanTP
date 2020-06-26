################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/stm32f1-stdperiph/misc.c \
../system/src/stm32f1-stdperiph/stm32f10x_gpio.c \
../system/src/stm32f1-stdperiph/stm32f10x_rcc.c 

OBJS += \
./system/src/stm32f1-stdperiph/misc.o \
./system/src/stm32f1-stdperiph/stm32f10x_gpio.o \
./system/src/stm32f1-stdperiph/stm32f10x_rcc.o 

C_DEPS += \
./system/src/stm32f1-stdperiph/misc.d \
./system/src/stm32f1-stdperiph/stm32f10x_gpio.d \
./system/src/stm32f1-stdperiph/stm32f10x_rcc.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/stm32f1-stdperiph/%.o: ../system/src/stm32f1-stdperiph/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I/CanTp_Test/Common -I/CanTp_Test/CommunicationStack/CanTp/include -I/CanTp_Test/CommunicationStack/CanIf/include -I/CanTp_Test/CommunicationStack/CanDrv/include -I/CanTp_Test/configuration -I/CanTp_Test/Det/include -I/CanTp_Test/CommunicationStack/PduR/include -I"D:\ITI\lectures\7-ES Turkey\Turkey Project\CanTp_Test\CommunicationStack\Com\include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


