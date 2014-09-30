
#include "pcl_stm32f4.h"
#include "util/util.h"

#ifdef MY_GPIO
#include "gpio/gpio.h"
#endif

#ifdef MY_ETH
#include "eth/mdio.h"
#include "eth/eth.h"
#include "eth/myip/mydatad.h"
#endif

#ifdef MY_FLASH
#include "flash/flash.h"
#endif

#ifdef MY_GPIO
static GPIO_TypeDef *get_gpiox(char *a)
{
    char x = to_upper(a[0]);
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
#endif

#ifdef MY_UART
static USART_TypeDef *get_uartx(char *a)
{
    char x = a[0];
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
#endif

#ifdef MY_FLASH
COMMAND(flash) {
    ARITY(argc >= 2, "flash mr|mw addr ...");
    uint32_t *ptr = (uint32_t*)str2int(argv[2]);
#if 1
    if(SUBCMD1("unlock"))
        return picolSetHexResult(i, HAL_FLASH_Unlock());
    if(SUBCMD1("lock"))
        return picolSetHexResult(i, HAL_FLASH_Lock());
    if(SUBCMD1("mr"))
        return picolSetHexResult(i, *ptr);
    if(SUBCMD1("mw"))
        return picolSetIntResult(i, flash_write((uint32_t)ptr, str2int(argv[3])));
#endif
    if(SUBCMD1("fsz1"))
        return picolSetIntResult(i, flash_fsz1());
    if(SUBCMD1("fsz2"))
        return picolSetIntResult(i, flash_fsz2());
#ifdef MY_ETH
    if(SUBCMD1("tx1"))
    {
        uint32_t sz = str2int(argv[2]);
        myip_datad_io_flash_tx(sz, 0);
        return picolSetResult(i, argv[2]);
    }
    if(SUBCMD1("rx2"))
    {
        uint32_t sz = str2int(argv[2]);
        myip_datad_io_flash_rx(sz + USER_FLASH_SZ/2, sz);
        return picolSetResult(i, argv[2]);
    }
#endif
    return PICOL_ERR;
}
#endif

COMMAND(mw) {
    ARITY(argc >= 3, "mw addr data");
    uint32_t *ptr = (uint32_t*)str2int(argv[1]);
    *ptr = str2int(argv[2]);
    return picolSetResult(i, argv[2]);
}

#ifdef MY_GPIO
COMMAND(gpio) {
    ARITY(argc >= 3, "gpio a|b|c... num [val]");
    GPIO_TypeDef *gpiox = get_gpiox(argv[1]);
    ARITY(gpiox, "gpio a|b|c... num [val]");
    uint32_t pin = 0, value = 0;
    volatile uint32_t *reg_ptr = gpio_get_reg_ptr(gpiox, argv[2]);
    if(reg_ptr)
    {
        if(argc == 3)
            value = gpio_get_reg(gpiox, argv[2]);
        else if(argc == 4)
            value = gpio_set_reg(gpiox, argv[2], str2int(argv[3]));
    }
    else if(('0' <= argv[2][0]) && (argv[2][0] <= '9'))
    {
        pin = str2int(argv[2]);
        if(argc == 3)
            value = HAL_GPIO_ReadPin(gpiox, 1 << pin);
        else if(argc == 4)
        {
            value = str2int(argv[3]);
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
#endif

#ifdef MY_UART
COMMAND(uart) {
    ARITY((argc >= 3), "uart 1|2|3 str");
    USART_TypeDef *uartx = get_uartx(argv[1]);
    ARITY(uartx, "uart 1|2|3 str");
    int32_t value;
    volatile uint32_t *reg_ptr = uart_get_reg_ptr(uartx, argv[2]);
    if(reg_ptr)
    {
        if(argc == 3)
            value = uart_get_reg(uartx, argv[2]);
        else if(argc == 4)
            value = uart_set_reg(uartx, argv[2], str2int(argv[3]));
    }
    else
        return PICOL_ERR;
    return picolSetHexResult(i,value);
}
#endif

#ifdef MY_ETH
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

COMMAND(eth) {
    ARITY(argc >= 2, "eth subcmd ...");
    if(SUBCMD1("reset")) {
        eth_reset();
    }
    else
        return PICOL_ERR;
    return PICOL_OK;
}
#endif

void register_stm32f4_cmds(picolInterp *i)
{
#ifdef MY_GPIO
    picolRegisterCmd(i, "gpio", picol_gpio);
#endif
#ifdef MY_FLASH
    picolRegisterCmd(i, "flash", picol_flash);
#endif
#ifdef MY_UART
    picolRegisterCmd(i, "uart", picol_uart);
#endif
#ifdef MY_ETH
    picolRegisterCmd(i, "mdio", picol_mdio);
    picolRegisterCmd(i, "eth", picol_eth);
#endif
}

