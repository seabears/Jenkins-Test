################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../dht22_driver/dht22.c" 

COMPILED_SRCS += \
"dht22_driver/dht22.src" 

C_DEPS += \
"./dht22_driver/dht22.d" 

OBJS += \
"dht22_driver/dht22.o" 


# Each subdirectory must supply rules for building sources it contributes
"dht22_driver/dht22.src":"../dht22_driver/dht22.c" "dht22_driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/USER/AURIX-v1.10.6-workspace/p1/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"dht22_driver/dht22.o":"dht22_driver/dht22.src" "dht22_driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-dht22_driver

clean-dht22_driver:
	-$(RM) ./dht22_driver/dht22.d ./dht22_driver/dht22.o ./dht22_driver/dht22.src

.PHONY: clean-dht22_driver

