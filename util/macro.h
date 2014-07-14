
#if 1
#define JOIN_(A,B) A ## B
#define JOIN(A,B) JOIN_(A, B)

#define JOIN3_(A, B, C) A ## B ## C
#define JOIN3(A, B, C) JOIN3_(A, B, C)

#define GPIO(G) JOIN(GPIO, G)
#define PIN(P) JOIN(GPIO_PIN_, P)
#define PINSRC(P) JOIN(GPIO_PinSource, P)

#define STR(X) STR_HELPER(X)
#define STR_HELPER(X) #X

#define GPIO_INIT(gpio, pin, mode, pull, speed, alternate) do { \
    JOIN3(__GPIO, gpio, _CLK_ENABLE)(); \
    gpio_init.Pin = PIN(pin); \
    gpio_init.Mode = mode; \
    gpio_init.Pull = pull; \
    gpio_init.Speed = speed; \
    gpio_init.Alternate = alternate; \
    HAL_GPIO_Init(GPIO(gpio), &gpio_init); \
} while(0)

#define UART_PINMUX(uartn, txgpio, txpin, rxgpio, rxpin, uartaf) do { \
    GPIO_INIT(txgpio, txpin, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, uartaf); \
    GPIO_INIT(rxgpio, rxpin, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, uartaf); \
} while(0)

#define UART_INIT(uartn, baud) do { \
    RCC_APB1PeriphClockCmd(JOIN(RCC_APB1Periph_UART, uartn), ENABLE); \
    uart_init.USART_BaudRate = baud; \
    uart_init.USART_WordLength = USART_WordLength_8b; \
    uart_init.USART_StopBits = USART_StopBits_1; \
    uart_init.USART_Parity = USART_Parity_No; \
    uart_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; \
    uart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None; \
    USART_Cmd(JOIN(UART, uartn), ENABLE); \
    USART_Init(JOIN(UART, uartn), &uart_init); \
} while(0)

#define UART_NVIC_INIT(uartn, irqn, usart_it) do { \
    HAL_NVIC_SetPriority(irqn, 0, 1); \
    HAL_NVIC_EnableIRQ(irqn); \
} while(0)

#define ETH_PINMUX(gpio, pin) do { \
    GPIO_INIT(gpio, pin, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz); \
    GPIO_PinAFConfig(GPIO(gpio), PINSRC(pin), GPIO_AF_ETH); \
} while(0)

#endif

