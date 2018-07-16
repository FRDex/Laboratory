################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/App.c \
../Sources/DAC.c \
../Sources/FTM.c \
../Sources/GPIO.c \
../Sources/IR_driver.c \
../Sources/PLAYER.c \
../Sources/PORT.c \
../Sources/SDHC.c \
../Sources/SysTick.c 

OBJS += \
./Sources/App.o \
./Sources/DAC.o \
./Sources/FTM.o \
./Sources/GPIO.o \
./Sources/IR_driver.o \
./Sources/PLAYER.o \
./Sources/PORT.o \
./Sources/SDHC.o \
./Sources/SysTick.o 

C_DEPS += \
./Sources/App.d \
./Sources/DAC.d \
./Sources/FTM.d \
./Sources/GPIO.d \
./Sources/IR_driver.d \
./Sources/PLAYER.d \
./Sources/PORT.d \
./Sources/SDHC.d \
./Sources/SysTick.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D"CPU_MK64FN1M0VMD12" -I"../Sources" -I"../Project_Settings/Startup_Code" -I"../SDK/platform/CMSIS/Include" -I"../SDK/platform/devices" -I"../SDK/platform/devices/MK64F12/include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


