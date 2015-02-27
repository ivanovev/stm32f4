
#include <main.h>
#include "eth.h"
#include "eth/myip/mytcp.h"
#include "gpio/led.h"

#ifdef ENABLE_MYIP
#ifdef ENABLE_PTP
#include "eth/myip/ptp/ptpd.h"
#endif
#endif

#pragma message "PHY_ADDRESS: " STR(PHY_ADDRESS)

/* Private variables ---------------------------------------------------------*/
__ALIGN_BEGIN ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;/* Ethernet Rx MA Descriptor */

__ALIGN_BEGIN ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;/* Ethernet Tx DMA Descriptor */

__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_END; /* Ethernet Receive Buffer */

__ALIGN_BEGIN uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_END; /* Ethernet Transmit Buffer */

__ALIGN_BEGIN static ethfrm_t iofrm __ALIGN_END;

ETH_HandleTypeDef heth;
#ifdef ENABLE_ICMP
extern uint8_t local_ipaddr[4];
extern uint8_t local_macaddr[6];
#endif

void eth_init(void)
{
#ifdef ENABLE_I2C
    eeprom_ipaddr_read(local_ipaddr);
    eeprom_macaddr_read(local_macaddr);
#endif

#ifdef ENABLE_ICMP
    myip_init();
    heth.Init.MACAddr = local_macaddr;
#endif

    heth.Instance = ETH;
    heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
    heth.Init.Speed = ETH_SPEED_100M;
    heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
    heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
    //heth.Init.RxMode = ETH_RXINTERRUPT_MODE;
    heth.Init.RxMode = ETH_RXPOLLING_MODE;
    heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
    heth.Init.PhyAddress = PHY_ADDRESS;

    if (HAL_ETH_Init(&heth) == HAL_OK)
    {
        dbg_send_str3("HAL_ETH_Init = ok", 1);
    }
    else
    {
        dbg_send_str3("HAL_ETH_Init = err", 1);
    }
    /* Initialize Tx Descriptors list: Chain Mode */
    HAL_ETH_DMATxDescListInit(&heth, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
    /* Initialize Rx Descriptors list: Chain Mode  */
    HAL_ETH_DMARxDescListInit(&heth, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
#ifdef ENABLE_PTP
    eth_ptp_start(&heth, PTP_FINE);
#endif
    HAL_ETH_Start(&heth);
}

void eth_deinit(void)
{
    HAL_ETH_Stop(&heth);
    HAL_ETH_DeInit(&heth);
}

void eth_reset(void)
{
    HAL_GPIO_WritePin(GPIO(ETH_RESET_GPIO), PIN(ETH_RESET_PIN), GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIO(ETH_RESET_GPIO), PIN(ETH_RESET_PIN), GPIO_PIN_SET);
}

uint16_t eth_input(ethfrm_t *frm)
{
    if(HAL_ETH_GetReceivedFrame(&heth) != HAL_OK)
        return 0;

    uint16_t i;
    uint16_t sz = heth.RxFrameInfos.length;
    ETH_DMADescTypeDef *dmarxdesc = heth.RxFrameInfos.LSRxDesc;
#ifdef ENABLE_PTP
    eth_ptpts_get(0, dmarxdesc);
    //eth_ptpts_rx(0, dmarxdesc);
#if 0
    if((dmarxdesc->Status & ETH_DMARXDESC_MAMPCE) && (dmarxdesc->ExtendedStatus & ETH_DMAPTPRXDESC_PTPMT))
    {
        //dmarxdesc = heth.RxFrameInfos.LSRxDesc;
        //dbg_send_hex2("rx status", dmarxdesc->Status);
        //dbg_send_hex2("rx ext status", dmarxdesc->ExtendedStatus);
        //uint16_t i;
        ptpts_t time;
        //eth_ptpts_now(&time);
        for(i = 0; i < ETH_RXBUFNB; i++)
        {
            eth_ptpts_get(&time, &DMARxDscrTab[i]);
            dbg_send_int2("rx.s", time.s);
            dbg_send_int2("rx.ns", time.ns);
        }
        eth_ptpts_get(0, dmarxdesc);
    }
#endif
#endif
    mymemcpy(frm->packet, (uint8_t*)heth.RxFrameInfos.buffer, sz);

    /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
    dmarxdesc = heth.RxFrameInfos.FSRxDesc;
    for(i = 0; i< (heth.RxFrameInfos).SegCount; i++)
    {
        dmarxdesc->Status = ETH_DMARXDESC_OWN;
        dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
    }
    /* Clear Segment_Count */
    (heth.RxFrameInfos).SegCount = 0;

    /* When Rx Buffer unavailable flag is set: clear it and resume reception */
    if (((heth.Instance)->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)
    {
        /* Clear RBUS ETHERNET DMA flag */
        (heth.Instance)->DMASR = ETH_DMASR_RBUS;
        /* Resume DMA reception */
        (heth.Instance)->DMARPDR = 0;
    }
    return sz;
}

void eth_output(ethfrm_t *frm, uint16_t sz)
{
    //io_send_int2("send_sz", sz);
    __IO ETH_DMADescTypeDef *dmatxdesc = heth.TxDesc;
    uint8_t *buf = (uint8_t *)(dmatxdesc->Buffer1Addr);
    mymemcpy(buf, frm->packet, sz);
    dmatxdesc->Status |= ETH_DMATXDESC_TTSE | ETH_DMATXDESC_IC;
    //dmatxdesc->Status &= ~ETH_DMATXDESC_TCH;
    HAL_ETH_TransmitFrame(&heth, sz);
}

uint8_t eth_io(void)
{
    led_toggle();
    iofrm.e.mac.type = 0;
    uint16_t sz = eth_input(&iofrm);
#ifdef ENABLE_MYIP
    sz = myip_eth_frm_handler(&iofrm, sz);
    if(sz)
    {
        eth_output(&iofrm, sz);
        return 1;
    }
#endif
    return 0;
}

#ifdef ENABLE_PTP
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *pheth)
{
    (void)pheth;
    //dbg_send_str3("HAL_ETH_RxCpltCallback", 1);
    if(pheth->Init.RxMode == ETH_RXINTERRUPT_MODE)
        eth_io();
}

void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *pheth)
{
    uint16_t i;
    ptpts_t time;
    __IO ETH_DMADescTypeDef *dmatxdesc = 0;
    for(i = 0; i < ETH_TXBUFNB; i++)
    {
        if(pheth->TxDesc == &DMATxDscrTab[i])
        {
            dmatxdesc = i ? &DMATxDscrTab[i - 1] : &DMATxDscrTab[ETH_TXBUFNB - 1];
            break;
        }
    }
    eth_ptpts_get(&time, dmatxdesc);
    myip_ptpd_save_ts(&time);
}
#endif

