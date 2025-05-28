################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../mq135_driver/mq135.c" 

COMPILED_SRCS += \
"mq135_driver/mq135.src" 

C_DEPS += \
"./mq135_driver/mq135.d" 

OBJS += \
"mq135_driver/mq135.o" 


# Each subdirectory must supply rules for building sources it contributes
"mq135_driver/mq135.src":"../mq135_driver/mq135.c" "mq135_driver/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc27xd "-fC:/Users/USER/AURIX-v1.10.6-workspace/p1/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc27xd -Y0 -N0 -Z0 -o "$@" "$<"
"mq135_driver/mq135.o":"mq135_driver/mq135.src" "mq135_driver/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-mq135_driver

clean-mq135_driver:
	-$(RM) ./mq135_driver/mq135.d ./mq135_driver/mq135.o ./mq135_driver/mq135.src

.PHONY: clean-mq135_driver

