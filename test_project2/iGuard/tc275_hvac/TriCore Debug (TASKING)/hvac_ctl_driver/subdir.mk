################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../hvac_ctl_driver/hvac_ctl.c" 

COMPILED_SRCS += \
"hvac_ctl_driver/hvac_ctl.src" 

C_DEPS += \
"./hvac_ctl_driver/hvac_ctl.d" 

OBJS += \
"hvac_ctl_driver/hvac_ctl.o" 


# Each subdirectory must supply rules for building sources it contributes
"hvac_ctl_driver/hvac_ctl.src":"../hvac_ctl_driver/hvac_ctl.c" "hvac_ctl_driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/USER/AURIX-v1.10.6-workspace/p1/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"hvac_ctl_driver/hvac_ctl.o":"hvac_ctl_driver/hvac_ctl.src" "hvac_ctl_driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-hvac_ctl_driver

clean-hvac_ctl_driver:
	-$(RM) ./hvac_ctl_driver/hvac_ctl.d ./hvac_ctl_driver/hvac_ctl.o ./hvac_ctl_driver/hvac_ctl.src

.PHONY: clean-hvac_ctl_driver

