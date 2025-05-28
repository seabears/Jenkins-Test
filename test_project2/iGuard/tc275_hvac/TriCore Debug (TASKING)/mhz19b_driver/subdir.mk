################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../mhz19b_driver/mhz19b.c" 

COMPILED_SRCS += \
"mhz19b_driver/mhz19b.src" 

C_DEPS += \
"./mhz19b_driver/mhz19b.d" 

OBJS += \
"mhz19b_driver/mhz19b.o" 


# Each subdirectory must supply rules for building sources it contributes
"mhz19b_driver/mhz19b.src":"../mhz19b_driver/mhz19b.c" "mhz19b_driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/USER/AURIX-v1.10.6-workspace/p1/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"mhz19b_driver/mhz19b.o":"mhz19b_driver/mhz19b.src" "mhz19b_driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-mhz19b_driver

clean-mhz19b_driver:
	-$(RM) ./mhz19b_driver/mhz19b.d ./mhz19b_driver/mhz19b.o ./mhz19b_driver/mhz19b.src

.PHONY: clean-mhz19b_driver

