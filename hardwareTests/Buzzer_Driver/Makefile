obj-m := buzz.o
buzz-objs:=buzzmodule.o utils.o
KDIR := /home/jmiranda/buildroot/buildroot-2021.02.5/output/build/linux-custom/
ARCH ?= arm
CROSS_COMPILE ?= /home/jmiranda/buildroot/buildroot-2021.02.5/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-



export ARCH CROSS_COMPILE


KBUILD_TARGETS	:= clean help modules modules_install

.PHONY: all $(KBUILD_TARGETS)

all: modules
$(KBUILD_TARGETS):
	#$(MAKE) -C $(KDIR) M=$(shell pwd) $@ EXTRA_CFLAGS="$(MY_CFLAGS)"
	$(MAKE) -C $(KDIR) M=$(shell pwd) $@
        
