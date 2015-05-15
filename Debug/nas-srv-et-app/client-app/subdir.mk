################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../nas-srv-et-app/client-app/client.c \
../nas-srv-et-app/client-app/clientFunctions.c 

OBJS += \
./nas-srv-et-app/client-app/client.o \
./nas-srv-et-app/client-app/clientFunctions.o 

C_DEPS += \
./nas-srv-et-app/client-app/client.d \
./nas-srv-et-app/client-app/clientFunctions.d 


# Each subdirectory must supply rules for building sources it contributes
nas-srv-et-app/client-app/%.o: ../nas-srv-et-app/client-app/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


