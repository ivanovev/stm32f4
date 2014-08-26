#!/usr/bin/env sh

openocd -f interface/stlink-v2.cfg -c "set TRANSPORT hla_swd" -f target/stm32f4x_stlink.cfg
#./src/openocd -s tcl -f interface/stlink-v2.cfg -c "transport select hla_swd" -f target/stm32f4x.cfg

