#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifdef DEVKITARM
override DEVKITARM := $(subst \,/,$(DEVKITARM))
override DEVKITARM := $(subst \,/,$(DEVKITARM))
endif

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

export TARGET		:=	$(shell basename $(CURDIR))
export TOPDIR		:=	$(CURDIR)
export LIBNDS		:=	$(DEVKITARM)/libnds

export NDS_VERSION := 0.3

TODAY := $(shell date +%Y%m%d)

.PHONY: $(TARGET).arm7 $(TARGET).arm9

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).ds.gba

$(TARGET).ds.gba	: $(TARGET).nds

#---------------------------------------------------------------------------------
$(TARGET).nds	:	$(TARGET).arm7 $(TARGET).arm9
	ndstool	-c $(TARGET).nds -7 $(TARGET).arm7 -9 $(TARGET).arm9

#---------------------------------------------------------------------------------
$(TARGET).arm7	: arm7/$(TARGET).elf
$(TARGET).arm9	: arm9/$(TARGET).elf

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7
	
#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	rm -f $(TARGET).ds.gba $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9

images: logo.png
	grit $< -gb -gB8 -p!
	@mv -f logo.s logo.h arm9/source

cflash.img:
	@echo "You must create the disk image to test with an emulator"
	@false

test: $(TARGET).nds cflash.img
	desmume --cflash=cflash.img $(TARGET).nds

install: $(TARGET).nds
	@cp $(TARGET).nds $(TARGET)_SC.nds
	dldi-scsd-moon $(TARGET)_SC.nds
	mount /usbkey && mv $(TARGET)_SC.nds /usbkey/nds/ && umount /usbkey/

binary-package:
	@rm -f opentyriands-$(TODAY).zip
	zip -r opentyriands-$(TODAY).zip $(TARGET).nds COPYING README tyrian/ -x@exclude.txt -x exclude.txt

source-package:
	@rm -f opentyriands-source-$(TODAY).tar.bz2
	tar cvjf opentyriands-source-$(TODAY).tar.bz2 * -Xexclude.txt --exclude=exclude.txt --exclude=tyrian/* --exclude=*.nds
