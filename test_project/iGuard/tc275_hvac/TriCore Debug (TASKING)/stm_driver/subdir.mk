################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../stm_driver/stm.c" 

COMPILED_SRCS += \
"stm_driver/stm.src" 

C_DEPS += \
"./stm_driver/stm.d" 

OBJS += \
"stm_driver/stm.o" 


# Each subdirectory must supply rules for building sources it contributes
"stm_driver/stm.src":"../stm_driver/stm.c" "stm_driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/USER/AURIX-v1.10.6-workspace/p1/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"stm_driver/stm.o":"stm_driver/stm.src" "stm_driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-stm_driver

clean-stm_driver:
	-$(RM) ./stm_driver/stm.d ./stm_driver/stm.o ./stm_driver/stm.src

.PHONY: clean-stm_driver

