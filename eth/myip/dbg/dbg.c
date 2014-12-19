
#include "eth.h"
#include "dbg.h"

uint16_t myip_dbg_con_handler(uint8_t *data, uint16_t sz)
{
    if(sz == 0)
        return 0;
    uint16_t i;
    uint32_t *ptr1, *ptr2;
    for(i = 0; i < sz; i += 4)
    {
        ptr1 = (uint32_t*)&data[i];
        ptr2 = (uint32_t*)(HTONS_32(*ptr1));
        dbg_send_hex2("addr", (uint32_t)ptr2);
        *ptr1 = HTONS_32(*ptr2);
    }
    return sz;
}

