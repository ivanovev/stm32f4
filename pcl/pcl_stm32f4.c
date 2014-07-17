
#include "main.h"
#include "gpio/gpio.h"
#include "uart/uart.h"
#ifdef HAL_ETH_MODULE_ENABLED
#include "eth/eth.h"
#endif

static GPIO_TypeDef *get_gpiox(char *a)
{
    char x = to_upper(a[4]);
    if(x == 'A')
        return GPIOA;
    if(x == 'B')
        return GPIOB;
    if(x == 'C')
        return GPIOC;
    if(x == 'D')
        return GPIOD;
    if(x == 'E')
        return GPIOE;
    if(x == 'F')
        return GPIOF;
    if(x == 'G')
        return GPIOG;
    if(x == 'H')
        return GPIOH;
    return 0;
}

static USART_TypeDef *get_uartx(char *a)
{
    char x = a[4];
    if(x == '1')
        return USART1;
    if(x == '2')
        return USART2;
    if(x == '3')
        return USART3;
    if(x == '4')
        return UART4;
    if(x == '5')
        return UART5;
    if(x == '6')
        return USART6;
    return 0;
}

COMMAND(gpio) {
    GPIO_TypeDef *gpiox = get_gpiox(argv[0]);
    ARITY(gpiox && (argc >= 2), "gpiox num [val]");
    uint32_t pin = 0, value = 0;
    volatile uint32_t *reg_ptr = gpio_get_reg_ptr(gpiox, argv[1]);
    if(reg_ptr)
    {
        if(argc == 2)
            value = gpio_get_reg(gpiox, argv[1]);
        else if(argc == 3)
            value = gpio_set_reg(gpiox, argv[1], str2int(argv[2]));
    }
    else if(('0' <= argv[1][0]) && (argv[1][0] <= '9'))
    {
        pin = str2int(argv[1]);
        if(argc == 2)
            value = HAL_GPIO_ReadPin(gpiox, 1 << pin);
        else if(argc == 3)
        {
            value = str2int(argv[2]);
            if(value)
                HAL_GPIO_WritePin(gpiox, 1 << pin, GPIO_PIN_SET);
            else
                HAL_GPIO_WritePin(gpiox, 1 << pin, GPIO_PIN_RESET);
        }
    }
    else
        return PICOL_ERR;
    return picolSetHexResult(i,value);
}

COMMAND(uart) {
    USART_TypeDef *uartx = get_uartx(argv[0]);
    ARITY(uartx && (argc >= 2), "uartx str");
    int32_t value;
    volatile uint32_t *reg_ptr = uart_get_reg_ptr(uartx, argv[1]);
    if(reg_ptr)
    {
        if(argc == 2)
            value = uart_get_reg(uartx, argv[1]);
        else if(argc == 3)
            value = uart_set_reg(uartx, argv[1], str2int(argv[2]));
    }
    else
        return PICOL_ERR;
    return picolSetHexResult(i,value);
}

#ifdef HAL_ETH_MODULE_ENABLED
COMMAND(mdio) {
    ARITY(argc >= 2, "mdio ...");
    uint32_t reg, value;
    reg = str2int(argv[1]);
    if(argc == 2)
    {
        value = mdio_read(reg);
    }
    return picolSetHex2Result(i,value);
}
#endif

void register_stm32f4_cmds(picolInterp *i)
{
#if 1
    picolRegisterCmd(i, "gpioa", picol_gpio);
    picolRegisterCmd(i, "gpiob", picol_gpio);
    picolRegisterCmd(i, "gpioc", picol_gpio);
    picolRegisterCmd(i, "gpiod", picol_gpio);
    picolRegisterCmd(i, "gpioe", picol_gpio);
    picolRegisterCmd(i, "gpiof", picol_gpio);
    picolRegisterCmd(i, "gpiog", picol_gpio);
    picolRegisterCmd(i, "gpioh", picol_gpio);

    picolRegisterCmd(i, "uart1", picol_uart);
    picolRegisterCmd(i, "uart2", picol_uart);
    picolRegisterCmd(i, "uart3", picol_uart);
    picolRegisterCmd(i, "uart4", picol_uart);
    picolRegisterCmd(i, "uart5", picol_uart);
    picolRegisterCmd(i, "uart6", picol_uart);
#endif
#ifdef HAL_ETH_MODULE_ENABLED
    picolRegisterCmd(i, "mdio", picol_mdio);
#endif
}

