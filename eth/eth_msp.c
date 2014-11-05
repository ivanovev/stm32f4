
#include <main.h>
#include "eth/eth.h"
#include "eth/mdio.h"
#include "uart/uart.h"

extern void eth_reset(void);

#pragma message "ETH_MDC_GPIO: GPIO" STR(ETH_MDC_GPIO) " PIN" STR(ETH_MDC_PIN)
#pragma message "ETH_MDIO_GPIO: GPIO" STR(ETH_MDIO_GPIO) " PIN" STR(ETH_MDIO_PIN)
#pragma message "ETH_RESET_GPIO: GPIO" STR(ETH_RESET_GPIO) " PIN" STR(ETH_RESET_PIN)
#pragma message "ETH_REFCLK_GPIO: GPIO" STR(ETH_REFCLK_GPIO) " PIN" STR(ETH_REFCLK_PIN)
#pragma message "ETH_CRSDV_GPIO: GPIO" STR(ETH_CRSDV_GPIO) " PIN" STR(ETH_CRSDV_PIN)
#pragma message "ETH_RXD0_GPIO: GPIO" STR(ETH_RXD0_GPIO) " PIN" STR(ETH_RXD0_PIN)
#pragma message "ETH_RXD1_GPIO: GPIO" STR(ETH_RXD1_GPIO) " PIN" STR(ETH_RXD1_PIN)
#pragma message "ETH_TXEN_GPIO: GPIO" STR(ETH_TXEN_GPIO) " PIN" STR(ETH_TXEN_PIN)
#pragma message "ETH_TXD0_GPIO: GPIO" STR(ETH_TXD0_GPIO) " PIN" STR(ETH_TXD0_PIN)
#pragma message "ETH_TXD1_GPIO: GPIO" STR(ETH_TXD1_GPIO) " PIN" STR(ETH_TXD1_PIN)

void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    GPIO_InitTypeDef gpio_init;

    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();
    __GPIOH_CLK_ENABLE();

    GPIO_INIT(ETH_MDC_GPIO, ETH_MDC_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);
    GPIO_INIT(ETH_MDIO_GPIO, ETH_MDIO_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);

    GPIO_INIT(ETH_RESET_GPIO, ETH_RESET_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FAST, 0);
    HAL_GPIO_WritePin(GPIO(ETH_RESET_GPIO), PIN(ETH_RESET_PIN), GPIO_PIN_SET);

    GPIO_INIT(ETH_REFCLK_GPIO, ETH_REFCLK_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);

    GPIO_INIT(ETH_CRSDV_GPIO, ETH_CRSDV_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);
    GPIO_INIT(ETH_RXD0_GPIO, ETH_RXD0_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);
    GPIO_INIT(ETH_RXD1_GPIO, ETH_RXD1_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);

    GPIO_INIT(ETH_TXEN_GPIO, ETH_TXEN_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);
    GPIO_INIT(ETH_TXD0_GPIO, ETH_TXD0_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);
    GPIO_INIT(ETH_TXD1_GPIO, ETH_TXD1_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);

    __ETH_CLK_ENABLE();
    dbg_send_str3("HAL_ETH_MspInit", 1);
    eth_reset();
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef *heth)
{
    dbg_send_str3("HAL_ETH_MspDeInit", 1);
    __ETH_CLK_DISABLE();
}

