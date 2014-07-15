
#include "main.h"
#ifdef USE_ETH
#include "eth/eth.h"
#endif

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
    return 0;
}

COMMAND(gpio) {
    char buf[IO_BUF_SZ] = "";
    GPIO_TypeDef *GPIOx = get_gpiox(argv[1]);
    ARITY(GPIOx && (argc >= 2), "gpio xnum [val]");
    uint32_t pin = str2int(&(argv[1][1]));
    uint32_t value = 0;
    if(argc == 2)
    {
        value = HAL_GPIO_ReadPin(GPIOx, 1 << pin);
    }
    if(argc >= 3)
    {
        value = str2int(argv[2]);
        if(value)
            HAL_GPIO_WritePin(GPIOx, 1 << pin, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(GPIOx, 1 << pin, GPIO_PIN_RESET);
        int2str(value, buf, 16);
    }
    else
        return PICOL_ERR;
    return picolSetResult(i, buf);
}

#ifdef USE_ETH
COMMAND(mdio) {
    ARITY(argc >= 2, "mdio ...");
    char buf[MAXSTR] = "";
    uint32_t reg, val;
    reg = str2int(argv[1]);
    if(argc == 2)
    {
        val = mdio_read(reg);
        itoh(val, buf, 2);
    }
    return picolSetResult(i, buf);
}
#endif

void register_stm32f4_cmds(picolInterp *i)
{
    picolRegisterCmd(i, "gpio", picol_gpio);
#ifdef USE_ETH
    picolRegisterCmd(i, "mdio", picol_mdio);
#endif
}

