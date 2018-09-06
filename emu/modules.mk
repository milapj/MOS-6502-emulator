include $(EMU_SRC_DIR)/mos6502/modules.mk
include $(EMU_SRC_DIR)/nes/modules.mk

EMU_SRC += main.c \
	   shell.c \
	   ines.c \
	   rc.c \
	   timekeeper.c \
	   memory.c \
	   membus.c \
	   reset_manager.c \
	   fileio.c
