################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cConsole.c \
../src/cCore.c \
../src/cInput.c \
../src/cMenu.c \
../src/cMenuDis.c \
../src/cXBorder.c \
../src/cXCore.c \
../src/cXMemDbg.c \
../src/cXStack.c \
../src/cXStackP.c \
../src/cXThread.c \
../src/cXTrace.c \
../src/pmTrace.c \
../src/pmXDebug.c 

OBJS += \
./src/cConsole.o \
./src/cCore.o \
./src/cInput.o \
./src/cMenu.o \
./src/cMenuDis.o \
./src/cXBorder.o \
./src/cXCore.o \
./src/cXMemDbg.o \
./src/cXStack.o \
./src/cXStackP.o \
./src/cXThread.o \
./src/cXTrace.o \
./src/pmTrace.o \
./src/pmXDebug.o 

C_DEPS += \
./src/cConsole.d \
./src/cCore.d \
./src/cInput.d \
./src/cMenu.d \
./src/cMenuDis.d \
./src/cXBorder.d \
./src/cXCore.d \
./src/cXMemDbg.d \
./src/cXStack.d \
./src/cXStackP.d \
./src/cXThread.d \
./src/cXTrace.d \
./src/pmTrace.d \
./src/pmXDebug.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


