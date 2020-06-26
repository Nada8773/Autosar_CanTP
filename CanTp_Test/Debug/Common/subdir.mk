################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Common/Rte_criticalSection.c 

OBJS += \
./Common/Rte_criticalSection.o 

C_DEPS += \
./Common/Rte_criticalSection.d 


# Each subdirectory must supply rules for building sources it contributes
Common/%.o: ../Common/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I/CanTp_Test/Common -I/CanTp_Test/CommunicationStack/CanTp/include -I/CanTp_Test/CommunicationStack/CanIf/include -I/CanTp_Test/CommunicationStack/CanDrv/include -I/CanTp_Test/configuration -I/CanTp_Test/Det/include -I/CanTp_Test/CommunicationStack/PduR/include -I/CanTp_Test/CommunicationStack/Com/include -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


