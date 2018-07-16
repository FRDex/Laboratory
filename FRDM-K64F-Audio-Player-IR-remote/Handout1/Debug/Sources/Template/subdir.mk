################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/Template/LDM.c \
../Sources/Template/main.c 

OBJS += \
./Sources/Template/LDM.o \
./Sources/Template/main.o 

C_DEPS += \
./Sources/Template/LDM.d \
./Sources/Template/main.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/Template/%.o: ../Sources/Template/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D"CPU_MK64FN1M0VMD12" -I"../Sources" -I"../Project_Settings/Startup_Code" -I"../SDK/platform/CMSIS/Include" -I"../SDK/platform/devices" -I"../SDK/platform/devices/MK64F12/include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


