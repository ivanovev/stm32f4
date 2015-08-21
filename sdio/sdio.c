
#include <main.h>
#ifdef ENABLE_PCL
#include "sdio/pcl_sdio.h"
#endif

static SD_HandleTypeDef hsd;
HAL_SD_CardInfoTypedef SDCardInfo;

static HAL_SD_ErrorTypedef mysdinit(SD_HandleTypeDef *hsd)
{
    SD_InitTypeDef tmpinit;

    /* Allocate lock resource and initialize it */
    hsd->Lock = HAL_UNLOCKED;
    /* Initialize the low level hardware (MSP) */
    HAL_SD_MspInit(hsd);

    /* Default SDIO peripheral configuration for SD card initialization */
    tmpinit.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
    tmpinit.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
    tmpinit.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
    tmpinit.BusWide             = SDIO_BUS_WIDE_1B;
    tmpinit.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    tmpinit.ClockDiv            = SDIO_INIT_CLK_DIV;

    /* Initialize SDIO peripheral interface with default configuration */
    SDIO_Init(hsd->Instance, tmpinit);

    __HAL_SD_SDIO_DISABLE();

    /* Set Power State to ON */
    SDIO_PowerState_ON(hsd->Instance);

    /* 1ms: required power up waiting time before starting the SD initialization 
     *      sequence */
    HAL_Delay(1);

    /* Enable SDIO Clock */
    __HAL_SD_SDIO_ENABLE();

    return SD_OK;
}

void sdio_init(void)
{
    dbg_send_str2("sdio_init");
    hsd.Instance = SDIO;
    hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
    hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd.Init.ClockDiv = SDIO_INIT_CLK_DIV;

    if(HAL_SD_Init(&hsd, &SDCardInfo) != SD_OK)
    //if(mysdinit(&hsd) != SD_OK)
    {
        dbg_send_str2("HAL_SD_Init error");
        //return;
    }

    //HAL_SD_WideBusOperation_Config(&hsd, SDIO_BUS_WIDE_4B);
#ifdef ENABLE_PCL
    pcl_sdio_init(pcl_interp);
#endif
}

void sdio_write(uint8_t *data, uint32_t sz)
{
    HAL_SD_WriteBlocks_DMA(&hsd, (uint32_t*)data, 0, 512, sz/512);
}

#ifndef SDIO_CMD0TIMEOUT
#define SDIO_CMD0TIMEOUT                ((uint32_t)0x00010000)
#endif

#ifndef SDIO_STATIC_FLAGS
#define SDIO_STATIC_FLAGS               ((uint32_t)(SDIO_FLAG_CCRCFAIL | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_CTIMEOUT |\
                                                                SDIO_FLAG_DTIMEOUT | SDIO_FLAG_TXUNDERR | SDIO_FLAG_RXOVERR  |\
                                                                SDIO_FLAG_CMDREND  | SDIO_FLAG_CMDSENT  | SDIO_FLAG_DATAEND  |\
                                                                SDIO_FLAG_DBCKEND))
#endif

HAL_SD_ErrorTypedef sdio_cmd(uint32_t cmd, uint32_t *data)
{
    SDIO_CmdInitTypeDef  cmd_init;
    uint32_t timeout = SDIO_CMD0TIMEOUT, tmp;
    cmd_init.Argument         = *data;
    cmd_init.CmdIndex         = cmd;
    if((cmd == 0) || (cmd == 11))
        cmd_init.Response         = SDIO_RESPONSE_NO;
    else if((cmd == 2) || (cmd == 9))
        cmd_init.Response         = SDIO_RESPONSE_LONG;
    else
        cmd_init.Response         = SDIO_RESPONSE_SHORT;
    cmd_init.WaitForInterrupt = SDIO_WAIT_NO;
    cmd_init.CPSM             = SDIO_CPSM_ENABLE;

    __HAL_SD_SDIO_CLEAR_FLAG(&hsd, SDIO_STATIC_FLAGS);
    SDIO_SendCommand(hsd.Instance, &cmd_init);

    if(cmd_init.Response == SDIO_RESPONSE_NO)
    {
        tmp = __HAL_SD_SDIO_GET_FLAG(&hsd, SDIO_FLAG_CMDSENT);
        while((timeout > 0) && (!tmp))
        {
            tmp = __HAL_SD_SDIO_GET_FLAG(&hsd, SDIO_FLAG_CMDSENT);
            timeout--;
        }
        if(timeout == 0)
            return SD_CMD_RSP_TIMEOUT;
    }
    else
    {
        tmp = __HAL_SD_SDIO_GET_FLAG(&hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT);
        while((!tmp) && (timeout > 0))
        {
            tmp = __HAL_SD_SDIO_GET_FLAG(&hsd, SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT);
            timeout--;
        }
        tmp = __HAL_SD_SDIO_GET_FLAG(&hsd, SDIO_FLAG_CTIMEOUT);
        if((timeout == 0) || tmp)
            return SD_CMD_RSP_TIMEOUT;
        if(cmd_init.Response == SDIO_RESPONSE_SHORT)
            *data = SDIO_GetResponse(SDIO_RESP1);
    }
    return SD_OK;
}

const volatile uint32_t* sdio_get_reg_ptr(const char *reg)
{
    if(!mystrncmp(reg, "cmd", 3))
        return &(hsd.Instance->CMD);
    if(!mystrncmp(reg, "arg", 3))
        return &(hsd.Instance->ARG);
    if(!mystrncmp(reg, "sta", 3))
        return &(hsd.Instance->STA);
    if(!mystrncmp(reg, "respcmd", 7))
        return &(hsd.Instance->RESPCMD);
    if(!mystrncmp(reg, "resp1", 5))
        return &(hsd.Instance->RESP1);
    if(!mystrncmp(reg, "icr", 3))
        return &(hsd.Instance->ICR);
    if(!mystrncmp(reg, "clkcr", 5))
        return &(hsd.Instance->CLKCR);
    return 0;
}

uint32_t sdio_get_reg_bits(const char *reg, uint8_t n1, uint8_t n2)
{
    uint32_t *ptr = (uint32_t*)sdio_get_reg_ptr(reg);
    if(!ptr)
        return 0;
    uint32_t mask = ((1 << (n2 - n1 + 1)) - 1) << n1;
    return (*ptr & mask) >> n1;
}

uint32_t sdio_set_reg_bits(const char *reg, uint8_t n1, uint8_t n2, uint32_t v)
{
    uint32_t *ptr = (uint32_t*)sdio_get_reg_ptr(reg);
    if(!ptr)
        return 0;
    uint32_t mask = ((1 << (n2 - n1 + 1)) - 1) << n1;
    uint32_t ret = *ptr & ~mask;
    ret = ret | ((v << n1) & mask);
    *ptr = ret;
    return ret;
}

