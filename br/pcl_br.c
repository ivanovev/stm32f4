
#ifdef ENABLE_PCL

#include <main.h>
#include "pcl_br.h"

extern UART_HandleTypeDef alt_huart;

COMMAND(alt_uart) {

    uint32_t j;
    static uint32_t timeout = 500;
    char buf[IO_BUF_SZ];
    buf[0] = 0;
    for(j = 1; j < argc; j++)
    {
        if(j > 1)
            mystrncat(buf, " ", IO_BUF_SZ);
        mystrncat(buf, argv[j], IO_BUF_SZ);
    }
    mystrncat(buf, "\n", IO_BUF_SZ);
    j = HAL_UART_Transmit(&alt_huart, (uint8_t*)buf, mystrnlen(buf, IO_BUF_SZ), timeout);
    return picolSetIntResult(i, j);
}

void pcl_br_init(picolInterp *i)
{
    picolRegisterCmd(i, "alt_uart", picol_alt_uart, 0);
}

#endif

