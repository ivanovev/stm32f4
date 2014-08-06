
#PROJ = led uart usb hcsr i2c adc pwm mdio
PROJ = $(shell ls -d */ 2>/dev/null | grep -v util | sed "s:/::")
.PHONY: clean $(PROJ)

BUILDS = $(shell find . -maxdepth 2 -name build)

dflt:
	@echo make $(PROJ)

$(PROJ):
	@echo $(CFLAGS)
	@make -C $@

clean: $(BUILDS)
	rm -rf $^

