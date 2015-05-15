################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../nas-srv-et-app/client-app/src/client-func.c \
../nas-srv-et-app/client-app/src/client.c 

OBJS += \
./nas-srv-et-app/client-app/src/client-func.o \
./nas-srv-et-app/client-app/src/client.o 

C_DEPS += \
./nas-srv-et-app/client-app/src/client-func.d \
./nas-srv-et-app/client-app/src/client.d 


# Each subdirectory must supply rules for building sources it contributes
nas-srv-et-app/client-app/src/%.o: ../nas-srv-et-app/client-app/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


