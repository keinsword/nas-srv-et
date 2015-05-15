################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../nas-srv-et-lib/src/common-func.c \
../nas-srv-et-lib/src/crc.c \
../nas-srv-et-lib/src/errors.c \
../nas-srv-et-lib/src/protocol.c 

OBJS += \
./nas-srv-et-lib/src/common-func.o \
./nas-srv-et-lib/src/crc.o \
./nas-srv-et-lib/src/errors.o \
./nas-srv-et-lib/src/protocol.o 

C_DEPS += \
./nas-srv-et-lib/src/common-func.d \
./nas-srv-et-lib/src/crc.d \
./nas-srv-et-lib/src/errors.d \
./nas-srv-et-lib/src/protocol.d 


# Each subdirectory must supply rules for building sources it contributes
nas-srv-et-lib/src/%.o: ../nas-srv-et-lib/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


