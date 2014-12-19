
CFLAGS += -I$(IP_DIR)
C_SRCS += $(IP_DIR)/myip.c
C_SRCS += $(IP_DIR)/mytcp.c

#include $(IP_DIR)/dbg/dbg.mk
include $(IP_DIR)/icmp/icmp.mk
include $(IP_DIR)/ptp/ptp.mk
#include $(IP_DIR)/tftp/tftp.mk
include $(IP_DIR)/telnet/telnet.mk

