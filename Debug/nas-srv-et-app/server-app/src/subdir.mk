################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../nas-srv-et-app/server-app/src/netns-func.c \
../nas-srv-et-app/server-app/src/server-func.c \
../nas-srv-et-app/server-app/src/server.c 

OBJS += \
./nas-srv-et-app/server-app/src/netns-func.o \
./nas-srv-et-app/server-app/src/server-func.o \
./nas-srv-et-app/server-app/src/server.o 

C_DEPS += \
./nas-srv-et-app/server-app/src/netns-func.d \
./nas-srv-et-app/server-app/src/server-func.d \
./nas-srv-et-app/server-app/src/server.d 


# Each subdirectory must supply rules for building sources it contributes
nas-srv-et-app/server-app/src/%.o: ../nas-srv-et-app/server-app/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


