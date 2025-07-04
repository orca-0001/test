#CH32V003
BUILD_DIR       := out
OBJ_DIR         := obj
TARGET		    ?= user

CC_PREFIX 	    ?= arm-none-eabi-
CC				:= $(CC_PREFIX)gcc
LD	            := $(CC_PREFIX)g++
OC 				:= $(CC_PREFIX)objcopy
OD 				:= $(CC_PREFIX)objdump
SIZE 		    := $(CC_PREFIX)size
AR              := $(CC_PREFIX)ar


C_FLAGS 		:= -c -g -DTW32F003x8 -Os
CC_FLAGS 		:= -w -c -g -DTW32F003x8 -Os -mcpu=cortex-m0 -mthumb -ffunction-sections -fdata-sections  -fno-rtti -fno-exceptions -fpermissive -Wunused -Wuninitialized
LD_FLAGS		:= -Os -Wl,--gc-sections  -save-temps -mcpu=cortex-m0 -mthumb   -T Ld/memory.ld -Wl,-Map,out/HDUSBCore.map  -o GPIO_Toggle.axf   --specs=nano.specs    -lm -lgcc 
AR_FLAGS        := rcs
#启动汇编程序
#START_UP         :=  startup_ch32v00x

#所有头文件路径
INCDIRS 		:= 	Core \
					User \
					Peripheral/inc \
					Lib \
					myLib

#所有源文件路径
SRCDIRS			:=	.\
                    Startup   \
                    Core   \
					User \
					Peripheral/src \
					Lib \
					myLib

# $(patsubst <pattern>,<replacement>,<text>)
# 查找<text>中的单词是否符合模式<pattern>,如果匹配，则以<replacement>替换
# 为所有的头文件名前面加上-I
INCLUDE			:= $(patsubst %, -I %, $(INCDIRS))

# $(wildcard PATTERN...)  获取PATTERN下所有符合条件的文件
# $(foreach <var>,<list>,<text>) 
#   把参数<list>中的单词逐一取出放到参数<var>所指定的变量中，
#   然后再执行<text>所包含的表达式。每一次<text>会返回一个字符串，循环过程中，<text>
#   的所返回的每个字符串会以空格分隔，最后当整个循环结束时，<text>所返回的每个字符串
#   所组成的整个字符串（以空格分隔）将会是 foreach 函数的返回值。
# 获取SRCDIRS指定的文件夹下的所有.S文件
SFILES			:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
# 获取SRCDIRS指定的文件夹下的所有.c文件
CFILES			:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
# 获取SRCDIRS指定的文件夹下的所有.cpp文件
CPPFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))

# 排除_main.cpp
MCPPFILES       := $(filter-out ./_main.cpp, $(CPPFILES)) 

# 取出非目录部分
SFILENDIR		:= $(notdir  $(SFILES))
CFILENDIR		:= $(notdir  $(CFILES))
CPPFILENDIR		:= $(notdir  $(MCPPFILES))

# $(SFILENDIR:.S=.o):将SFILENDIR中的.S替换为.o
# $(patsubst <pattern>,<replacement>,<text>)
# 查找<text>中的单词是否符合模式<pattern>,如果匹配，则以<replacement>替换
SOBJS			:= $(patsubst %, obj/%, $(SFILENDIR:.S=.o))
COBJS			:= $(patsubst %, obj/%, $(CFILENDIR:.c=.o))
CPPOBJS			:= $(patsubst %, obj/%, $(CPPFILENDIR:.cpp=.o))

OBJS			+= $(SOBJS) $(COBJS) $(CPPOBJS) 
COREOBJS        := $(filter-out obj/syscall.o, $(OBJS)) 
COREOBJS        := $(filter-out obj/Twen32_handlers.o, $(COREOBJS)) 
COREOBJS        := $(filter-out obj/Twen32_startup.o, $(COREOBJS)) 
COREOBJS        := $(filter-out obj/main.o, $(COREOBJS)) 
COREOBJS        := $(filter-out obj/wiring.o, $(COREOBJS)) 
COREOBJS        := $(filter-out obj/HardwareSerial.o, $(COREOBJS)) 

VPATH			:= $(SRCDIRS)

#生成.A文件
#riscv-none-embed-ar rcs "obj/core.a" "obj/ch32v20x_adc.o" "obj/ch32v20x_bkp.o" "obj/ch32v20x_can.o" "obj/ch32v20x_crc.o" "obj/ch32v20x_dbgmcu.o" "obj/ch32v20x_dma.o" "obj/ch32v20x_exti.o" "obj/ch32v20x_flash.o" "obj/ch32v20x_gpio.o" "obj/ch32v20x_i2c.o" "obj/ch32v20x_it.o" "obj/ch32v20x_iwdg.o" "obj/ch32v20x_misc.o" "obj/ch32v20x_opa.o" "obj/ch32v20x_pwr.o" "obj/ch32v20x_rcc.o" "obj/ch32v20x_rtc.o" "obj/ch32v20x_spi.o" "obj/ch32v20x_tim.o" "obj/ch32v20x_usart.o" "obj/ch32v20x_wwdg.o" "obj/startup_ch32v20x_D8W.o" "obj/itoa.o" "obj/dtostrf.o" "obj/wiring.o"  "obj/HardwareSerial.o" "obj/Print.o" "obj/Stream.o" "obj/WString.o" "obj/core_riscv.o"

# $@--目标文件， $^--所有的依赖文件， $<--第一个依赖文件
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OC) -O ihex $(BUILD_DIR)/$(TARGET).elf $@
	$(OC) -O binary $(BUILD_DIR)/$(TARGET).elf   fw.bin
	$(OD) -d -S $(BUILD_DIR)/$(TARGET).elf > $(BUILD_DIR)/firmware.txt
	$(SIZE) --format=berkeley $(BUILD_DIR)/user.elf

$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(LD) $(LD_FLAGS) -o $@ $(OBJS)
	$(AR) $(AR_FLAGS) "out/core.a"  $(COREOBJS)


$(COBJS) : obj/%.o : %.c
	$(CC) $(C_FLAGS)  -I. $(INCLUDE)  -Wall -mthumb -ffunction-sections -nostdlib  -fdata-sections -Dprintf=iprintf -fmessage-length=0 -mcpu=cortex-m0  -fno-builtin  -o $@ $<

$(CPPOBJS) : obj/%.o : %.cpp 
	$(LD) $(CC_FLAGS) -g -I. $(INCLUDE) -c -o $@ $<

.PHONY : clean
clean:
	rm obj/*.o
	rm out/*.elf
	rm out/*.hex
	# rm out/*.bin
	rm out/*.txt
	
	
	







