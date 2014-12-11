
CFLAGS += -DENABLE_PTP
CFLAGS += -I$(IP_DIR)/ptpd
C_SRCS += $(IP_DIR)/ptp/myptpd.c
C_SRCS += $(IP_DIR)/ptp/ptpd_msp.c

