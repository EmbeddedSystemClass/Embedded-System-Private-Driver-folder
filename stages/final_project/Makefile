# Add additional project sources like this:
# SRCS += X.c
#
# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)

#location of freertos source files
NRF24L01PLUS_PATH=$(SOURCELIB_ROOT)/components/hal/nrf24l01plus

#location of freertos source files
FREERTOS_PATH=$(SOURCELIB_ROOT)/components/os/FreeRTOS
FREERTOSCLI_PATH=$(SOURCELIB_ROOT)/components/os/FreeRTOS-Plus/Source/FreeRTOS-Plus-CLI

#Name of project
PROJ_NAME=main

#Your local source files
SRCS = $(PROJ_NAME).c
CFLAGS += -DENABLE_DEBUG_UART #enable USB Virtual Com Port
#CFLAGS += -DDEBUG #enable Debug
CFLAGS += -DS4527438_SYSMON_CONF_INC
CFLAGS += -DCOM_BASE_SETTING
#CFLAGS += -DSORTER1_SETTING
#CFLAGS += -DSORTER2_SETTING
#CFLAGS += -DSORTER3_SETTING
#CFLAGS += -DSORTER4_SETTING
#CFLAGS += -DORB1_SETTING
#CFLAGS += -DORB2_SETTING
#CFLAGS += -DORB3_SETTING
#CFLAGS += -DORB4_SETTING
CFLAGS += -I.
#CFLAGS += -DHAL_ADC_MODULE_ENABLED #enable USB Virtual Com Port

#Extra files from the src folder must ONLY be added to LIBSRCS
LIBSRCS = $(FREERTOS_PATH)/portable/MemMang/heap_2.c

###################################################
ROOT=$(shell pwd)

OBJS=$(SRCS:.c=.o)

###################################################

.PHONY: $(shell mkdir -p obj) lib proj all depend
all: proj

#Include freertos makefile
include $(FREERTOS_PATH)/filelist.mk

#Include freertos makefile
include $(FREERTOSCLI_PATH)/filelist.mk

#Include nrf24l01plus makefile
include $(NRF24L01PLUS_PATH)/nrf24l01plus.mk

#include cli makefile
include /home/csse3010/csse3010/mylib/cli/filelist.mk

#include hamming makefile
include /home/csse3010/csse3010/mylib/hamming/filelist.mk

#include pantilt makefile
include /home/csse3010/csse3010/mylib/radio/filelist.mk

#include atimer makefile
include /home/csse3010/csse3010/mylib/atimer/filelist.mk

#include nucelo-f429zi makefile - NOTE MUST BE LAST TO INCLUDE
include $(SOURCELIB_ROOT)/components/boards/nucleo-f429zi/board.mk

lib:
	$(MAKE) -C lib

proj: 	$(PROJ_NAME).elf

#compile and link file
$(PROJ_NAME).elf:	$(PROJOBJ) $(OBJS)
	$(CC) $(LDFLAGS) -o $@ -Llib $(OBJS) $(PROJOBJ) $(LD_SYS_LIBS)
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

%.o:
	$(CC) $(CFLAGS) -c $< -o $@

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^>>./.depend;

dfu:
	sudo dfu-util -d 0483:df11 -c 1 -i 0 -a 0 -s 0x08000000 -D $(PROJ_NAME).bin		#Program NP2

flash:
	sudo st-flash write $(PROJ_NAME).bin 0x8000000	#Program Nucleo

qemu:
	np2-qemu $(PROJ_NAME).elf

gdb:
	np2-gdb-qemu $(PROJ_NAME).elf

clean:
	rm -f *.o
	rm -f $(OBJDIR)/*.o
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).bin
	rm -f $(LIBNP2)
	rm -rf .depend
	rmdir $(OBJDIR)


include .depend
