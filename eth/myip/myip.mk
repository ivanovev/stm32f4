
CFLAGS += -DENABLE_MYIP

IP_DIR = $(ROOT_DIR)/eth/myip
CFLAGS += -I$(IP_DIR)

C_SRCS += $(IP_DIR)/myip.c
C_SRCS += $(IP_DIR)/mytcp.c

include $(ROOT_DIR)/eth/myip/icmp/icmp.mk
#include $(ROOT_DIR)/eth/myip/dbg/dbg.mk
#include $(ROOT_DIR)/eth/myip/ptp/ptp.mk
include $(ROOT_DIR)/eth/myip/tftp/tftp.mk
include $(ROOT_DIR)/eth/myip/stream/stream.mk
include $(ROOT_DIR)/eth/myip/telnet/telnet.mk

