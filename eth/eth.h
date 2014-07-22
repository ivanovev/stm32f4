
#ifndef __ETH_H__
#define __ETH_H__

#include "main.h"
#include "eth/myip/myip.h"

#define ETH_RESET_GPIO G
#define ETH_RESET_PIN 6
#define ETH_REFCLK_GPIO A
#define ETH_REFCLK_PIN 1

#define ETH_MDC_GPIO C
#define ETH_MDC_PIN 1
#define ETH_MDIO_GPIO A
#define ETH_MDIO_PIN 2

#define ETH_CRSDV_GPIO A
#define ETH_CRSDV_PIN 7
#define ETH_RXD0_GPIO C
#define ETH_RXD0_PIN 4
#define ETH_RXD1_GPIO C
#define ETH_RXD1_PIN 5

#define ETH_TXEN_GPIO G
#define ETH_TXEN_PIN 11
#define ETH_TXD0_GPIO G
#define ETH_TXD0_PIN 13
#define ETH_TXD1_GPIO G
#define ETH_TXD1_PIN 14

void        eth_init(void);
void        eth_reset(void);
uint16_t    eth_input(ETH_FRAME *frm);
void        eth_output(ETH_FRAME *frm, uint16_t sz);
void        eth_io(void);

#endif

