
#include <main.h>
#include "eth.h"
#include "eth/myip/mytcp.h"

#pragma message "PHY_ADDRESS: " STR(PHY_ADDRESS)

/* Private variables ---------------------------------------------------------*/
__ALIGN_BEGIN ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;/* Ethernet Rx MA Descriptor */

__ALIGN_BEGIN ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;/* Ethernet Tx DMA Descriptor */

__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_END; /* Ethernet Receive Buffer */

__ALIGN_BEGIN uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_END; /* Ethernet Transmit Buffer */

ETH_HandleTypeDef heth;

void eth_init(void)
{
    myip_init();
    uint8_t macaddress[6]= { MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5 };

    heth.Instance = ETH;
    heth.Init.MACAddr = macaddress;
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

uint16_t eth_input(ETH_FRAME *frm)
{
    if (HAL_ETH_GetReceivedFrame(&heth) != HAL_OK)
        return 0;

    uint16_t sz = heth.RxFrameInfos.length;
    mymemcpy(frm->packet, (uint8_t*)heth.RxFrameInfos.buffer, sz);
    uint32_t i = 0;
    __IO ETH_DMADescTypeDef *dmarxdesc = heth.RxFrameInfos.FSRxDesc;
#ifdef ENABLE_PTP
    eth_ptpts_get(0, dmarxdesc);
#endif

    /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
    //io_send_int2("seg_count", (heth.RxFrameInfos).SegCount);
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

void eth_output(ETH_FRAME *frm, uint16_t sz)
{
    //io_send_int2("send_sz", sz);
    uint8_t *buf = (uint8_t *)(heth.TxDesc->Buffer1Addr);
    mymemcpy(buf, frm->packet, sz);
    __IO ETH_DMADescTypeDef *dmatxdesc = heth.TxDesc;
    dmatxdesc->Status |= ETH_DMATXDESC_TTSE | ETH_DMATXDESC_IC;
    //dmatxdesc->Status &= ~ETH_DMATXDESC_TCH;
    HAL_ETH_TransmitFrame(&heth, sz);
}

uint8_t eth_io(void)
{
    static ETH_FRAME frm;
    frm.e.p.type = 0;
    uint16_t sz = eth_input(&frm);
    sz = myip_eth_frame_handler(&frm, sz);
    if(sz)
    {
        eth_output(&frm, sz);
        return 1;
    }
    return 0;
}

#ifdef ENABLE_PTP
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *pheth)
{
    (void)pheth;
}

void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *pheth)
{
    uint16_t i;
    __IO ETH_DMADescTypeDef *dmatxdesc = 0;
    for(i = 0; i < ETH_TXBUFNB; i++)
    {
        if(pheth->TxDesc == &DMATxDscrTab[i])
        {
            dmatxdesc = i ? &DMATxDscrTab[i - 1] : &DMATxDscrTab[ETH_TXBUFNB - 1];
            break;
        }
    }
    ptpts_t time;
    eth_ptpts_get(&time, dmatxdesc);
    myip_ptpd_save_t3(&time);
}
#endif

