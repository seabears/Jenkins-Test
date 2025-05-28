################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../PWM_driver/PWM.c" 

COMPILED_SRCS += \
"PWM_driver/PWM.src" 

C_DEPS += \
"./PWM_driver/PWM.d" 

OBJS += \
"PWM_driver/PWM.o" 


# Each subdirectory must supply rules for building sources it contributes
"PWM_driver/PWM.src":"../PWM_driver/PWM.c" "PWM_driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/USER/AURIX-v1.10.6-workspace/p1/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"PWM_driver/PWM.o":"PWM_driver/PWM.src" "PWM_driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-PWM_driver

clean-PWM_driver:
	-$(RM) ./PWM_driver/PWM.d ./PWM_driver/PWM.o ./PWM_driver/PWM.src

.PHONY: clean-PWM_driver

