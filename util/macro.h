
#ifndef __MACRO_H__
#define __MACRO_H__

#if 1
#define JOIN_(A,B) A ## B
#define JOIN(A,B) JOIN_(A, B)

#define JOIN3_(A, B, C) A ## B ## C
#define JOIN3(A, B, C) JOIN3_(A, B, C)

#define JOIN4_(A, B, C, D) A ## B ## C ## D
#define JOIN4(A, B, C, D) JOIN4_(A, B, C, D)

#define JOIN5_(A, B, C, D, E) A ## B ## C ## D ## E
#define JOIN5(A, B, C, D, E) JOIN5_(A, B, C, D, E)

#define GPIO(G) JOIN(GPIO, G)
#define PIN(P) JOIN(GPIO_PIN_, P)
#define PINSRC(P) JOIN(GPIO_PinSource, P)

#define STR(X) STR_HELPER(X)
#define STR_HELPER(X) #X

#define HTONS_16( x ) ((((x) >> 8) + ((x) << 8)) & 0x0000FFFF)
#define HTONS_32( x ) ( ((x >> 24) & 0x000000FF) + ((x >> 8) & 0x0000FF00) + \
                        ((x << 8) & 0x00FF0000) + ((x << 24) & 0xFF000000))

#define GPIO_INIT(gpio, pin, mode, pull, speed, alternate) do { \
    JOIN3(__GPIO, gpio, _CLK_ENABLE)(); \
    gpio_init.Pin = PIN(pin); \
    gpio_init.Mode = mode; \
    gpio_init.Pull = pull; \
    gpio_init.Speed = speed; \
    gpio_init.Alternate = alternate; \
    HAL_GPIO_Init(GPIO(gpio), &gpio_init); \
} while(0)

#define UART_PINMUX(txgpio, txpin, rxgpio, rxpin, uartaf) do { \
    GPIO_INIT(txgpio, txpin, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, uartaf); \
    GPIO_INIT(rxgpio, rxpin, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, uartaf); \
} while(0)

#define SPI_PINMUX(clkgpio, clkpin, mosigpio, mosipin, misogpio, misopin, spiaf) do { \
    GPIO_INIT(clkgpio, clkpin, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, spiaf); \
    GPIO_INIT(mosigpio, mosipin, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, spiaf); \
    GPIO_INIT(misogpio, misopin, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, spiaf); \
} while(0)

#define UART_NVIC_INIT(irqn, usart_it) do { \
    HAL_NVIC_SetPriority(irqn, 0, 1); \
    HAL_NVIC_EnableIRQ(irqn); \
} while(0)

#define ETH_PINMUX(gpio, pin) do { \
    GPIO_INIT(gpio, pin, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz); \
    GPIO_PinAFConfig(GPIO(gpio), PINSRC(pin), GPIO_AF_ETH); \
} while(0)

#define I2C_PINMUX(sclgpio, sclpin, sdagpio, sdapin, i2caf) do { \
    GPIO_INIT(sclgpio, sclpin, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FAST, i2caf); \
    GPIO_INIT(sdagpio, sdapin, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FAST, i2caf); \
} while(0)

#endif

#endif
