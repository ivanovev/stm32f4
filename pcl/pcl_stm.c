
#include "pcl_stm.h"

#ifdef ENABLE_FLASH
extern uint16_t pcl_load(picolInterp *i, uint32_t addr);

COMMAND(flash) {
    ARITY(argc >= 2, "flash cmd [addr]");
    uint32_t sz = 0;
#if 1
    if(argc >= 3)
    {
        uint32_t *ptr = (uint32_t*)str2int(argv[2]);
        if(SUBCMD1("mr"))
            return picolSetHex4Result(i, *ptr);
        if(argc >= 4)
        {
            if(SUBCMD1("mw"))
                return picolSetIntResult(i, flash_write((uint32_t)ptr, str2int(argv[3])));
        }
    }
    if(SUBCMD1("unlock"))
        return picolSetHexResult(i, HAL_FLASH_Unlock());
    if(SUBCMD1("lock"))
        return picolSetHexResult(i, HAL_FLASH_Lock());
    if(SUBCMD1("fsz0"))
        return picolSetIntResult(i, flash_fsz0());
    if(SUBCMD1("fsz1"))
        return picolSetIntResult(i, flash_fsz1());
    if(SUBCMD1("crc0"))
    {
        if(argc >= 3)
            sz = str2int(argv[2]);
        return picolSetHex4Result(i, flash_crc0(sz));
    }
    if(SUBCMD1("crc1"))
    {
        if(argc >= 3)
            sz = str2int(argv[2]);
        return picolSetHex4Result(i, flash_crc1(sz));
    }
    if(SUBCMD1("erase1"))
        return picolSetIntResult(i, flash_erase1());
    if(SUBCMD1("pclupd"))
        return picolSetHex4Result(i, pcl_load(i, USER_FLASH_MID_ADDR));
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

#ifdef ENABLE_GPIO
static GPIO_TypeDef *get_gpio_instance(char *a)
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

COMMAND(gpio) {
    ARITY(argc >= 2, "gpio a|b|c... num [val]");
    GPIO_TypeDef *gpiox = get_gpio_instance(argv[1]);
    if(gpiox == 0)
        return PICOL_ERR;
    ARITY(gpiox, "gpio a|b|c... num [val]");
    uint32_t pin = 0, value = 0;
    char op = 0;
    if(argc >= 3)
    {
        volatile uint32_t *preg = gpio_get_reg_ptr(gpiox, argv[2]);
        if(preg)
        {
            if(argc == 4)
                *preg = str2int(argv[3]);
            return picolSetHex4Result(i,*preg);
        }
    }
    if(('0' <= argv[1][1]) && (argv[1][1] <= '9'))
    {
        pin = str2int(&(argv[1][1]));
        if(argc == 2)
            op = 'r';
        else
        {
            op = 'w';
            value = str2int(argv[2]);
        }
    }
    else if (argc >= 3)
    {
        if(('0' <= argv[2][0]) && (argv[2][0] <= '9'))
        {
            pin = str2int(argv[2]);
            if(argc == 3)
                op = 'r';
            else
            {
                op = 'w';
                value = str2int(argv[3]);
            }
        }
    }
    if(op == 'r')
        value = HAL_GPIO_ReadPin(gpiox, 1 << pin);
    else if(op == 'w')
    {
        if(value)
            HAL_GPIO_WritePin(gpiox, 1 << pin, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(gpiox, 1 << pin, GPIO_PIN_RESET);
    }
    else
        return PICOL_ERR;
    return picolSetHexResult(i,value);
}

COMMAND(btn) {
    ARITY(argc >= 3, "btn [a n ...] ...");
    uint8_t j;
    GPIO_TypeDef *gpiox = 0;
    for(j = 1; j < argc; j++)
    {
        if(gpiox && ('0' <= argv[j][0]) && (argv[j][0] <= '9'))
            btn_irq_init(gpiox, str2int(argv[j]));
        else
            gpiox = get_gpio_instance(argv[j]);
    }
    return PICOL_OK;
}
#endif

#ifdef ENABLE_ADC
COMMAND(adc) {
    ARITY((argc >= 2), "adc cmd ...");
    if(SUBCMD1("start"))
    {
        if(argc == 2)
            adc_start();
        else
            adc_start_sz(str2int(argv[2]));
        return PICOL_OK;
    }
    if(SUBCMD1("counter"))
    {
        return picolSetIntResult(i, adc_data_counter());
    }
    if(SUBCMD1("stop"))
    {
        adc_stop();
        return PICOL_OK;
    }
    if(SUBCMD1("read"))
    {
        return picolSetHex2Result(i, adc_read());
    }
    return PICOL_ERR;
}
#endif

#ifdef ENABLE_CAN
extern volatile uint32_t rxcounter;
extern CAN_HandleTypeDef hcan;
COMMAND(can) {
    ARITY((argc >= 2), "can cmd");
    uint32_t j = 0, k = 0;
    volatile uint32_t *reg_ptr = 0;
    if(SUBCMD1("init"))
    {
        j = CAN_MODE_NORMAL;
        if(SUBCMD2("loopback"))
            j = CAN_MODE_LOOPBACK;
        if(SUBCMD2("silent"))
            j = CAN_MODE_SILENT;
        can_reset(j);
        return picolSetHex1Result(i, HAL_CAN_GetState(&hcan));
    }
    if(SUBCMD1("mode"))
    {
        reg_ptr = can_get_reg_ptr("btr");
        if((*reg_ptr & CAN_MODE_SILENT_LOOPBACK) == CAN_MODE_SILENT_LOOPBACK)
            return picolSetResult(i, "silent_loopback");
        if(*reg_ptr & CAN_MODE_SILENT)
            return picolSetResult(i, "silent");
        if(*reg_ptr & CAN_MODE_LOOPBACK)
            return picolSetResult(i, "loopback");
        return picolSetResult(i, "normal");
    }
    if(SUBCMD1("rx"))
    {
        can_rx_start();
        return picolSetHex1Result(i, HAL_CAN_GetState(&hcan));
    }
    if(SUBCMD1("err"))
    {
        return picolSetHex4Result(i, HAL_CAN_GetError(&hcan));
    }
    if(SUBCMD1("stdid"))
    {
        if(argc >= 3)
            j = str2int(argv[2]);
        return picolSetHex4Result(i, can_msg_stdid(j));
    }
    if(SUBCMD1("dlc"))
    {
        if(argc >= 3)
            j = str2int(argv[2]);
        return picolSetHex4Result(i, can_msg_dlc((uint8_t)j));
    }
    if(SUBCMD1("state"))
    {
        return picolSetHex1Result(i, HAL_CAN_GetState(&hcan));
    }
    if(SUBCMD1("test"))
    {
        k = can_msg_dlc(0);
        if(argc == 3)
            k = str2int(argv[2]);
        if(can_send_test(k) != HAL_OK)
            return PICOL_ERR;
        return picolSetIntResult(i, j);
    }
    if(SUBCMD1("rxcounter"))
    {
        return picolSetIntResult(i, can_rx_counter());
    }
    reg_ptr = can_get_reg_ptr(argv[1]);
    if(reg_ptr)
    {
        if(argc == 2)
            return picolSetHex4Result(i, *reg_ptr);
        if(argc == 3)
            *reg_ptr = str2int(argv[2]);
        return picolSetResult(i, argv[2]);
    }
    return PICOL_ERR;
}
#endif

#ifdef ENABLE_DAC
COMMAND(dac) {
    ARITY((argc >= 2), "dac cmd ...");
    if(SUBCMD1("start"))
    {
        dac_start_sin();
        return PICOL_OK;
    }
    if(SUBCMD1("stop"))
    {
        dac_stop();
        return PICOL_OK;
    }
    if(SUBCMD1("write"))
    {
        ARITY((argc >= 3), "dac write value");
        return picolSetHex2Result(i, dac_write((uint16_t)str2int(argv[2])));
    }
    return PICOL_ERR;
}
#endif

#ifdef ENABLE_UART
COMMAND(uart) {
    ARITY((argc >= 2), "uart 1|2|3... str");
    char buf[IO_BUF_SZ];
    uint32_t value = 0, j = 0;
    static uint32_t timeout = 500;
    if(SUBCMD1("timeout"))
    {
        if(argc == 3)
            timeout = str2int(argv[2]);
        return picolSetIntResult(i, timeout);
    }
    value = str2int(argv[1]);
    ARITY((0 < value) && (value <= 6) && (argc >= 3), "uart 1|2|3... str");
    UART_HandleTypeDef huart;
    uart_get_handle(&huart, value, HAL_UART_STATE_READY);
    if(SUBCMD2("init"))
    {
        ARITY((argc >= 4), "uart 1|2|3... init baudrate [8n1]");
        HAL_UART_DeInit(&huart);
        huart.Init.BaudRate = str2int(argv[3]);
        return picolSetIntResult(i, (HAL_UART_DeInit(&huart) == HAL_OK) ? 0 : 1);
    }
    volatile uint32_t *reg_ptr = uart_get_reg_ptr(huart.Instance, argv[2]);
    if(reg_ptr)
    {
        if(argc == 3)
            value = uart_get_reg(huart.Instance, argv[2]);
        else if(argc == 4)
            value = uart_set_reg(huart.Instance, argv[2], str2int(argv[3]));
        return picolSetHexResult(i, value);
    }
    buf[0] = 0;
    for(j = 2; j < argc; j++)
    {
        if(!mystrncmp(argv[j], "\\n", 2))
        {
            mystrncat(buf, "\n", IO_BUF_SZ);
            continue;
        }
        if(!mystrncmp(argv[j], "\\r", 2))
        {
            mystrncat(buf, "\r", IO_BUF_SZ);
            continue;
        }
        if(j > 2)
            mystrncat(buf, " ", IO_BUF_SZ);
        mystrncat(buf, argv[j], IO_BUF_SZ);
    }
    HAL_UART_Transmit(&huart, (uint8_t*)buf, mystrnlen(buf, IO_BUF_SZ), timeout);
    mymemset(buf, 0, IO_BUF_SZ);
    HAL_UART_Receive(&huart, (uint8_t*)buf, IO_BUF_SZ, timeout);
    return picolSetResult(i, buf);
}
#endif

#ifdef ENABLE_I2C
COMMAND(i2c) {
    if(SUBCMD1("init"))
    {
        i2c_init();
        return PICOL_OK;
    }
#if 1
    if(SUBCMD1("send1"))
    {
        char buf1[2] = {0x12, 0x34};
        i2c_send(0xAE, (uint8_t*)buf1, sizeof(buf1));
        return PICOL_OK;
    }
#endif
    ARITY(argc >= 3, "i2c addr data");
    uint8_t buf[MAXSTR];
    uint16_t addr = str2int(argv[1]);
    uint16_t len = str2bytes(argv[2], buf, sizeof(buf));
    uint32_t ret = i2c_send(addr, buf, len);
    return picolSetHexResult(i, ret);
}
#ifdef ENABLE_EEPROM
COMMAND(eeprom) {
    ARITY(argc >= 3, "eeprom read|write addr ...");
    uint16_t addr = str2int(argv[2]);
    uint8_t buf[MAXSTR];
    uint8_t buf1[32];
    uint16_t sz = 0, j, k;
    uint32_t *tmpi = (uint32_t*)buf1;
    if(SUBCMD1("read")) {
        if(argc > 3)
            sz = str2int(argv[3]);
        else
            sz = 1;
        if(eeprom_read_data(addr, buf1, sz) == 0)
            return PICOL_ERR;
        for(j = 0, k = 0; j < sz; j++)
        {
            if(j)
                buf[k++] = ' ';
            itoh(buf1[j], (char*)&(buf[k]), 1);
            k += 4;
        }
        return picolSetResult(i, (char*)buf);
    }
    if(SUBCMD1("write")) {
        if(SUBCMD2("enable"))
        {
            uint8_t en = 0;
            if(argc > 3)
            {
                en = (uint8_t)str2int(argv[3]);
                eeprom_write_enable(en);
            }
            else
                en = eeprom_write_enable(2);
            return picolSetIntResult(i, en);
        }
        sz = argc - 3;
        for(j = 0; j < sz; j++)
            buf1[j] = str2int(argv[j + 3]);
        if(eeprom_write_data(addr, buf1, sz) == 0)
            return PICOL_ERR;
        return PICOL_OK;
    }
    if(SUBCMD1("readint")) {
        if(eeprom_read_data(addr, buf1, 4) == 0)
            return PICOL_ERR;
        return picolSetIntResult(i, *tmpi);
    }
    if(SUBCMD1("writeint")) {
        *tmpi = str2int(argv[3]);
        if(eeprom_write_data(addr, buf1, 4) == 0)
            return PICOL_ERR;
        return picolSetResult(i, argv[3]);
    }
    return PICOL_ERR;
}
#endif
#endif

#ifdef ENABLE_SPI
COMMAND(spi) {
    ARITY(argc >= 3, "spi nspi data ...");
    uint8_t nspi = 0;
    if(argv[1][0] == '1') nspi = 1;
    if(argv[1][0] == '2') nspi = 2;
    if(argv[1][0] == '3') nspi = 3;
    char buf[IO_BUF_SZ];
    char buf2[IO_BUF_SZ];
    uint32_t tmp = 0;
    if(nspi && (argv[1][1] == '.') && (mystrnlen(argv[1], 8) >= 4))
    {
        GPIO_TypeDef *csgpiox = get_gpio_instance(&(argv[1][2]));
        if(!csgpiox)
            return PICOL_ERR;
        uint8_t csgpion = str2int(&(argv[1][3]));
        uint16_t len = str2bytes(argv[2], (uint8_t*)buf, IO_BUF_SZ);
        tmp = spi_send(nspi, csgpiox, csgpion, (uint8_t*)buf, len);
        bytes2str(buf, buf2, len);
        return picolSetResult(i, buf2);
    }
    if(nspi)
    {
        volatile uint32_t *preg = spi_get_reg_ptr(nspi, argv[2]);
        if(preg)
        {
            if(argc == 4)
                *preg = str2int(argv[3]);
            return picolSetHex4Result(i, *preg);
        }
        if(SUBCMD2("cpha"))
        {
            ARITY(argc >= 3, "spi nspi cpha [0|1]");
            tmp = (uint32_t)spi_cr1_bits(nspi, 0, 0, (argc >= 4) ? str2int(argv[3]) : -1);
            return picolSetIntResult(i, tmp);
        }
        if(SUBCMD2("cpol"))
        {
            ARITY(argc >= 3, "spi nspi cpol [0|1]");
            tmp = (uint32_t)spi_cr1_bits(nspi, 1, 1, (argc >= 4) ? str2int(argv[3]) : -1);
            return picolSetIntResult(i, tmp);
        }
    }
    return PICOL_ERR;
}
#endif

void pcl_stm_init(picolInterp *i)
{
#ifdef ENABLE_GPIO
    picolRegisterCmd(i, "gpio", picol_gpio, 0);
    picolRegisterCmd(i, "btn", picol_btn, 0);
#endif
#ifdef ENABLE_ADC
    picolRegisterCmd(i, "adc", picol_adc, 0);
#endif
#ifdef ENABLE_CAN
    picolRegisterCmd(i, "can", picol_can, 0);
#endif
#ifdef ENABLE_DAC
    picolRegisterCmd(i, "dac", picol_dac, 0);
#endif
#ifdef ENABLE_FLASH
    picolRegisterCmd(i, "flash", picol_flash, 0);
#endif
#ifdef ENABLE_I2C
    picolRegisterCmd(i, "i2c", picol_i2c, 0);
#ifdef ENABLE_EEPROM
    picolRegisterCmd(i, "eeprom", picol_eeprom, 0);
#endif
#endif
#ifdef ENABLE_SPI
    picolRegisterCmd(i, "spi", picol_spi, 0);
#endif
#ifdef ENABLE_UART
    picolRegisterCmd(i, "uart", picol_uart, 0);
#endif
}

