
#include "main.h"

ETH_HandleTypeDef heth;

void eth_init(void)
{
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

    HAL_ETH_Init(&heth);
    HAL_ETH_Start(&heth);
}

