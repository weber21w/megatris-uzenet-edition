###############################################################################
# Makefile for the project Megatris Uzenet Edition
###############################################################################

## General Flags
PROJECT = Megatris
GAME = Megatris
MCU = atmega644
TARGET = $(GAME).elf
CC = avr-gcc
INFO=../gameinfo.properties
 
## Kernel settings
KERNEL_DIR = ../../../kernel

## Escape spaces in mixer path (due to custom sounds.inc)
EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)
SPACE_ESC:= \\$(SPACE)
MIX_PATH:= $(realpath ../data/sounds.inc)
MIX_PATH_ESC:= $(subst $(SPACE),$(SPACE_ESC),$(MIX_PATH))

KERNEL_OPTIONS = -DVIDEO_MODE=1 -DSOUND_MIXER=1 -DINTRO_LOGO=0 -DTRUE_RANDOM_GEN=1 -DMODE1_FAST_VSYNC=1
KERNEL_OPTIONS += -DCONTROLLERS_VSYNC_READ=0 -DUART_RX_BUFFER_SIZE=256 -DUART_TX_BUFFER_SIZE=8 -DUART=1
KERNEL_OPTIONS += -DMUSIC_ENGINE=STREAM -DSONG_SPEED=1 -DNO_EEPROM_FORMAT=1 -DCPU_PLAYER=2 -DHEADLESS=1
KERNEL_OPTIONS += -DMIXER_WAVES=\"$(MIX_PATH_ESC)\" -DSCREEN_TILES_V=27

UZEBIN_DIR = ../../../bin
## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -Os -mcall-prologues -fsigned-char -ffunction-sections -fno-toplevel-reorder
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 
CFLAGS += $(KERNEL_OPTIONS)


## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=$(GAME).map 
LDFLAGS += -Wl,-gc-sections 


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

## Graphical and music resources
RESOURCES = ../data/font.inc ../data/graphics.inc ../data/music/Korobeiniki-3tracks.inc ../data/music/testrisnt.inc ../data/music/ending.inc ../data/music.inc 

## Objects that must be built in order to link
OBJECTS = uzeboxVideoEngineCore.o asmFunctions.o uzeboxCore.o uzeboxSoundEngine.o uzeboxSoundEngineCore.o uzeboxVideoEngine.o spiram.o keyboard.o $(GAME).o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)" 

## Build
all: $(RESOURCES) $(TARGET) $(GAME).hex $(GAME).eep $(GAME).lss $(GAME).uze size 

## Rebuild graphics resources
../data/font.inc: ../data/font.png ../data/gconvert-font.xml
	$(UZEBIN_DIR)/gconvert ../data/gconvert-font.xml

../data/graphics.inc: ../data/graphics.png ../data/gconvert-tiles.xml
	$(UZEBIN_DIR)/gconvert ../data/gconvert-tiles.xml


## Rebuild music resources
../data/music/Korobeiniki-3tracks.inc: ../data/Korobeiniki-3tracks.mid
	$(UZEBIN_DIR)/midiconv ../data/Korobeiniki-3tracks.mid ../data/Korobeiniki-3tracks.inc -vsong_korobeiniki

../data/music/testrisnt.inc: ../data/testrisnt__0.mid
	$(UZEBIN_DIR)/midiconv ../data/testrisnt__0.mid ../data/testrisnt.inc -vsong_testrisnt

../data/music/ending.inc: ../data/ending2.mid
	$(UZEBIN_DIR)/midiconv ../data/ending2.mid ../data/ending.inc -vsong_ending -f8

../data/music.inc: ../data/compress-music.cfg
	$(UZEBIN_DIR)/mconvert ../data/compress-music.cfg


## Compile Kernel files
uzeboxVideoEngineCore.o: $(KERNEL_DIR)/uzeboxVideoEngineCore.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

uzeboxSoundEngineCore.o: $(KERNEL_DIR)/uzeboxSoundEngineCore.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

uzeboxCore.o: $(KERNEL_DIR)/uzeboxCore.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzeboxSoundEngine.o: $(KERNEL_DIR)/uzeboxSoundEngine.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzeboxVideoEngine.o: $(KERNEL_DIR)/uzeboxVideoEngine.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<


## Compile game sources
asmFunctions.o: ../asmFunctions.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

spiram.o: $(KERNEL_DIR)/spiram.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

keyboard.o: $(KERNEL_DIR)/keyboard.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

$(GAME).o: ../$(GAME).c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<


##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

%.uze: $(TARGET)
	-$(UZEBIN_DIR)/packrom $(GAME).hex $@ $(INFO)

UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')
AVRSIZEFLAGS := -A ${TARGET}
ifneq (,$(findstring MINGW,$(UNAME)))
AVRSIZEFLAGS := -C --mcu=${MCU} ${TARGET}
endif

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}


## Clean target
.PHONY: clean
clean:
	-rm -rf $(RESOURCES) $(OBJECTS) $(GAME).* dep/* *.uze


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

