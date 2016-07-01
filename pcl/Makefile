
include ../scripts/util1.mk
C_SRCS = main.c
include $(ROOT_DIR)/gpio/gpio.mk
#include $(ROOT_DIR)/uart/uart.mk
#include $(ROOT_DIR)/usb/usb.mk
include pcl.mk
include ../scripts/util2.mk

test: $(BUILDDIR)
	gcc -w -g -I. -I.. -I../util -o $(BUILDDIR)/test picol.c ../util/heap1.c pcl_misc.c ../util/util.c -Wl,-Map=$(BUILDDIR)/test.map

