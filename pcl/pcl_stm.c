
#include "pcl_stm.h"
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

#ifdef MY_I2C
#include "i2c/eeprom.h"
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
    uint32_t sz = 0;
#if 1
    if(SUBCMD1("unlock"))
        return picolSetHexResult(i, HAL_FLASH_Unlock());
    if(SUBCMD1("lock"))
        return picolSetHexResult(i, HAL_FLASH_Lock());
    if(SUBCMD1("mr"))
        return picolSetHexResult(i, *ptr);
    if(SUBCMD1("mw"))
        return picolSetIntResult(i, flash_write((uint32_t)ptr, str2int(argv[3])));
    if(SUBCMD1("fsz0"))
    {
        return picolSetIntResult(i, flash_fsz0());
    }
    if(SUBCMD1("fsz1"))
        return picolSetIntResult(i, flash_fsz1());
    if(SUBCMD1("crc0"))
    {
        if(argc >= 3)
            sz = str2int(argv[2]);
        return picolSetHexResult(i, flash_crc0(sz));
    }
    if(SUBCMD1("crc1"))
    {
        if(argc >= 3)
            sz = str2int(argv[2]);
        return picolSetHexResult(i, flash_crc1(sz));
    }
    if(SUBCMD1("erase1"))
        return picolSetIntResult(i, flash_erase1());
    if(SUBCMD1("pclupd"))
        return picolSetHexResult(i, pcl_load());
#endif
#ifdef MY_ETH
    if(SUBCMD1("tx0"))
    {
        sz = flash_fsz0();
        myip_datad_io_flash_tx(sz, 0);
        return picolSetIntResult(i, sz);
    }
    if(SUBCMD1("tx1"))
    {
        sz = flash_fsz1();
        myip_datad_io_flash_tx(sz, USER_FLASH_SZ/2);
        return picolSetIntResult(i, sz);
    }
    if(SUBCMD1("rx1"))
    {
        uint32_t sz = str2int(argv[2]);
        flash_erase1();
        if(SUBCMD3("fw"))
            myip_datad_io_flash_rx(sz, USER_FLASH_SZ/2, RESET_FWUPG);
        else if(SUBCMD3("pcl"))
            myip_datad_io_flash_rx(sz, USER_FLASH_SZ/2, 0);
        else
            return PICOL_ERR;
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
    int32_t value = 0;
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
    uint32_t reg, value = 0;
    reg = str2int(argv[1]);
    if(argc == 2)
    {
        value = mdio_read(reg);
    }
    return picolSetHex4Result(i,value);
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

#ifdef MY_I2C
COMMAND(eeprom) {
    ARITY(argc > 3, "eeprom read|write addr ...");
    uint16_t addr = str2int(argv[2]);
    uint8_t buf[MAXSTR];
    uint8_t buf1[32];
    uint16_t sz = 0, j, k;
    if(SUBCMD1("read")) {
        sz = str2int(argv[3]);
        if(eeprom_read_data(addr, buf1, sz) == 0)
            return PICOL_ERR;
        for(j = 0, k = 0; j < sz; j++)
        {
            if(j)
                buf[k++] = ' ';
            itoh(buf1[j], &(buf[k]), 1);
            k += 4;
        }
        return picolSetResult(i, buf);
    }
    if(SUBCMD1("write")) {
        sz = argc - 3;
        for(j = 0, k = 0; j < sz; j++)
            buf1[j] = str2int(argv[j + 3]);
        if(eeprom_write_data(addr, buf1, sz) == 0)
            return PICOL_ERR;
        return PICOL_OK;
    }
    return PICOL_ERR;
}
#endif

void pcl_stm_init(picolInterp *i)
{
#ifdef MY_GPIO
    picolRegisterCmd(i, "gpio", picol_gpio, 0);
#endif
#ifdef MY_FLASH
    picolRegisterCmd(i, "flash", picol_flash, 0);
#endif
#ifdef MY_UART
    picolRegisterCmd(i, "uart", picol_uart, 0);
#endif
#ifdef MY_ETH
    picolRegisterCmd(i, "mdio", picol_mdio, 0);
    picolRegisterCmd(i, "eth", picol_eth, 0);
#endif
#ifdef MY_I2C
    picolRegisterCmd(i, "eeprom", picol_eeprom, 0);
#endif
}
