
#ifdef ENABLE_PCL

#include <main.h>
#include "pcl_br.h"

extern UART_HandleTypeDef alt_huart;

COMMAND(alt_uart) {

    uint32_t j, v = 0;
    static uint32_t timeout = 1000;
    char buf[IO_BUF_SZ];
    buf[0] = 0;
    for(j = 1; j < argc; j++)
    {
        if(j > 1)
            mystrncat(buf, " ", IO_BUF_SZ);
        mystrncat(buf, argv[j], IO_BUF_SZ);
    }
    mystrncat(buf, "\n", IO_BUF_SZ);
    HAL_UART_Transmit(&alt_huart, (uint8_t*)buf, mystrnlen(buf, IO_BUF_SZ), timeout);
    mymemset(buf, 0, IO_BUF_SZ);
    for(j = 0; j < IO_BUF_SZ; j++)
    {
        v = HAL_UART_Receive(&alt_huart, (uint8_t*)(&buf[j]), 1, timeout);
        if(v != HAL_OK)
        {
            dbg_send_str2("uart recv err");
            break;
        }
        if(buf[j] == '\n')
        {
            buf[j] = 0;
            break;
        }
    }
    return picolSetResult(i, buf);
}

void pcl_br_init(picolInterp *i)
{
    picolRegisterCmd(i, "alt_uart", picol_alt_uart, 0);
}

#endif

