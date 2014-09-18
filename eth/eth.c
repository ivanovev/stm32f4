
#include "main.h"
#include "eth.h"

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
    heth.Init.RxMode = ETH_RXPOLLING_MODE;
    heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
    heth.Init.PhyAddress = PHY_ADDRESS;

    if (HAL_ETH_Init(&heth) == HAL_OK)
    {
        io_send_str3("HAL_ETH_Init = ok", 1);
    }
    /* Initialize Tx Descriptors list: Chain Mode */
    HAL_ETH_DMATxDescListInit(&heth, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
    /* Initialize Rx Descriptors list: Chain Mode  */
    HAL_ETH_DMARxDescListInit(&heth, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
    HAL_ETH_Start(&heth);
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
    io_send_int2("send_sz", sz);
    __IO ETH_DMADescTypeDef *DmaTxDesc = heth.TxDesc;
    uint8_t *buf = (uint8_t *)(heth.TxDesc->Buffer1Addr);
    mymemcpy(buf, frm->packet, sz);
    HAL_ETH_TransmitFrame(&heth, sz);
}

void eth_io(void)
{
    static ETH_FRAME frm;
    frm.e.p.type = 0;
    uint16_t sz = eth_input(&frm);
    //MAC_FRAME *frm = myip_get_mac_frame(buf, sz);
    //io_send_hex4("src", frm.e.p.src, 6);
    //io_send_hex4("dst", frm.e.p.dst, 6);
    //io_send_hex4("type", (uint8_t*)(&frm.e.p.type), 2);
    switch(frm.e.p.type)
    {
        case ARP_FRAME_TYPE:
            sz = myip_handle_arp_frame(&frm, sz);
            break;
        case IP_FRAME_TYPE:
            sz = myip_handle_ip_frame(&frm, sz);
            break;
        default:
            sz = myip_handle_tcp_frame(&frm, sz);
            break;
    }
    if(sz)
        eth_output(&frm, sz);
}

