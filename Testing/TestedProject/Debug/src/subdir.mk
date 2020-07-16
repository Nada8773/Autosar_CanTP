################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CanIf.c \
../src/CanTp.c \
../src/CanTp_LCfg.c \
../src/Det.c \
../src/ECUC.c \
../src/PduR.c \
../src/RCC.c \
../src/_write.c \
../src/main.c 

OBJS += \
./src/CanIf.o \
./src/CanTp.o \
./src/CanTp_LCfg.o \
./src/Det.o \
./src/ECUC.o \
./src/PduR.o \
./src/RCC.o \
./src/_write.o \
./src/main.o 

C_DEPS += \
./src/CanIf.d \
./src/CanTp.d \
./src/CanTp_LCfg.d \
./src/Det.d \
./src/ECUC.d \
./src/PduR.d \
./src/RCC.d \
./src/_write.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../" -I"../ThirdParty/include" -I"../ThirdParty/portable/GCC/ARM_CM3" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


