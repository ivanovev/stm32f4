
PROJ = $(shell ls -d */ 2>/dev/null | grep -v "scripts\|util" | sed "s:/::")
.PHONY: distclean commit $(PROJ) stats menuconfig

BUILDS = $(shell find . -maxdepth 2 -name build)

dflt: all

$(PROJ):
	@make -C $@

distclean: $(BUILDS)
	rm -rf $^

commit: distclean
	git add ./* && git commit && git push

stats:
	@find . -name "*.c" -print0 | xargs -0 cat | awk "NF" | wc

menuconfig:
	sh scripts/menuconfig.sh

include scripts/util1.mk
include main.mk
#include $(ROOT_DIR)/adc/adc.mk
#include $(ROOT_DIR)/can/can.mk
#include $(ROOT_DIR)/dac/dac.mk
#include $(ROOT_DIR)/display/display.mk
#include $(ROOT_DIR)/dma/dma.mk
#include $(ROOT_DIR)/dsp/dsp.mk
include $(ROOT_DIR)/eth/eth.mk
include $(ROOT_DIR)/flash/flash.mk
include $(ROOT_DIR)/gpio/gpio.mk
#include $(ROOT_DIR)/i2c/i2c.mk
#include $(ROOT_DIR)/rng/rng.mk
include $(ROOT_DIR)/pcl/pcl.mk
include $(ROOT_DIR)/spi/spi.mk
include $(ROOT_DIR)/tim/tim.mk
#include $(ROOT_DIR)/uart/uart.mk
#include $(ROOT_DIR)/usb/usb.mk
include scripts/util2.mk

