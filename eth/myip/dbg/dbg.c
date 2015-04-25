
#include "eth/eth.h"
#include "dbg.h"

uint16_t myip_dbg_con_handler(uint8_t *in, uint16_t sz, uint8_t *out)
{
    if(sz == 0)
        return 0;
    uint16_t i;
    uint32_t *ptr1, *ptr2;
    //dbg_send_hex2("sz", (uint32_t)sz);
    for(i = 0; i < sz; i += 4)
    {
        ptr1 = (uint32_t*)&in[i];
        ptr2 = (uint32_t*)&out[i];
        //ptr2 = (uint32_t*)(HTONS_32(*ptr1));
        ptr1 = (uint32_t*)(*ptr1);
        //dbg_send_hex2("addr", (uint32_t)ptr2);
        *ptr2 = HTONS_32(*ptr1);
    }
    return sz;
}

