
CFLAGS += -DENABLE_PTP
CFLAGS += -I$(IP_DIR)/ptpd
C_SRCS += $(IP_DIR)/ptp/ptpd.c
C_SRCS += $(IP_DIR)/ptp/ptpd_msp.c

