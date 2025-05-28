################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../fan_ctl_driver/fan_ctl.c" 

COMPILED_SRCS += \
"fan_ctl_driver/fan_ctl.src" 

C_DEPS += \
"./fan_ctl_driver/fan_ctl.d" 

OBJS += \
"fan_ctl_driver/fan_ctl.o" 


# Each subdirectory must supply rules for building sources it contributes
"fan_ctl_driver/fan_ctl.src":"../fan_ctl_driver/fan_ctl.c" "fan_ctl_driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/USER/AURIX-v1.10.6-workspace/p1/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"fan_ctl_driver/fan_ctl.o":"fan_ctl_driver/fan_ctl.src" "fan_ctl_driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-fan_ctl_driver

clean-fan_ctl_driver:
	-$(RM) ./fan_ctl_driver/fan_ctl.d ./fan_ctl_driver/fan_ctl.o ./fan_ctl_driver/fan_ctl.src

.PHONY: clean-fan_ctl_driver

