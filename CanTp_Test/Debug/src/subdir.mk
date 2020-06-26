################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Can.c \
../src/CanIf.c \
../src/CanIf_lCfg.c \
../src/CanTp.c \
../src/CanTp_LCfg.c \
../src/Can_Lcfg.c \
../src/Can_interrupt.c \
../src/Com.c \
../src/Com_Lcfg.c \
../src/Det.c \
../src/ECUC.c \
../src/PduR_Lcfg.c \
../src/Pdur.c \
../src/_write.c \
../src/main.c 

OBJS += \
./src/Can.o \
./src/CanIf.o \
./src/CanIf_lCfg.o \
./src/CanTp.o \
./src/CanTp_LCfg.o \
./src/Can_Lcfg.o \
./src/Can_interrupt.o \
./src/Com.o \
./src/Com_Lcfg.o \
./src/Det.o \
./src/ECUC.o \
./src/PduR_Lcfg.o \
./src/Pdur.o \
./src/_write.o \
./src/main.o 

C_DEPS += \
./src/Can.d \
./src/CanIf.d \
./src/CanIf_lCfg.d \
./src/CanTp.d \
./src/CanTp_LCfg.d \
./src/Can_Lcfg.d \
./src/Can_interrupt.d \
./src/Com.d \
./src/Com_Lcfg.d \
./src/Det.d \
./src/ECUC.d \
./src/PduR_Lcfg.d \
./src/Pdur.d \
./src/_write.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I/CanTp_Test/Common -I/CanTp_Test/CommunicationStack/CanTp/include -I/CanTp_Test/CommunicationStack/CanIf/include -I/CanTp_Test/CommunicationStack/CanDrv/include -I/CanTp_Test/configuration -I/CanTp_Test/Det/include -I/CanTp_Test/CommunicationStack/PduR/include -I"D:\ITI\lectures\7-ES Turkey\Turkey Project\CanTp_Test\CommunicationStack\Com\include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


