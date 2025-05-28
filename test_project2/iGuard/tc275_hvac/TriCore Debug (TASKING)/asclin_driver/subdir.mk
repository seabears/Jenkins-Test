################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../asclin_driver/asclin.c" 

COMPILED_SRCS += \
"asclin_driver/asclin.src" 

C_DEPS += \
"./asclin_driver/asclin.d" 

OBJS += \
"asclin_driver/asclin.o" 


# Each subdirectory must supply rules for building sources it contributes
"asclin_driver/asclin.src":"../asclin_driver/asclin.c" "asclin_driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/USER/AURIX-v1.10.6-workspace/p1/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"asclin_driver/asclin.o":"asclin_driver/asclin.src" "asclin_driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-asclin_driver

clean-asclin_driver:
	-$(RM) ./asclin_driver/asclin.d ./asclin_driver/asclin.o ./asclin_driver/asclin.src

.PHONY: clean-asclin_driver

