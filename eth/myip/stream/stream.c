
#include "eth/eth.h"
#include "stream.h"

#define STREAM_CHUNK_SZ 512

struct stream_t
{
    uint8_t dir;
    uint32_t counter;
    uint8_t src_ipaddr[4], dst_ipaddr[4];
} st;

void myip_stream_init(void)
{
    st.dir = 0;
    st.counter = 0;
    mymemset(st.src_ipaddr, 0, 4);
    mymemset(st.dst_ipaddr, 0, 4);
}

void myip_stream_start(uint8_t dir)
{
    st.dir |= dir;
    if(st.dir & STREAM_OUT)
        st.counter = 0;
}

void myip_stream_stop(uint8_t dir)
{
    st.dir &= ~dir;
}

const uint8_t* myip_stream_in_src(uint8_t ipaddr[])
{
    if(ipaddr)
        mymemcpy(st.src_ipaddr, ipaddr, 4);
    return st.src_ipaddr;
}

const uint8_t* myip_stream_out_dst(uint8_t ipaddr[])
{
    if(ipaddr)
        mymemcpy(st.dst_ipaddr, ipaddr, 4);
    return st.dst_ipaddr;
}

uint8_t myip_stream_status(void)
{
    return st.dir;
}

uint16_t myip_stream_con_handler(uint8_t *in, uint16_t sz, uint8_t *out)
{
    if(st.dir == 0)
        return 0;
    uint16_t i;
    uint32_t *ptr;
    if(st.dir & STREAM_OUT)
    {
#if 0
        for(i = 0; i < STREAM_CHUNK_SZ; i += 4)
        {
            ptr = (uint32_t*)&out[i];
            *ptr = i;
        }
#endif
        st.counter += STREAM_CHUNK_SZ;
        return STREAM_CHUNK_SZ;
    }
    return 0;
}

uint16_t myip_stream_frm_handler(ethfrm_t *in, uint16_t sz, uint16_t con_index, ethfrm_t *out)
{
    udpfrm_t* ufrmi = (udpfrm_t*)in;
    udpfrm_t* ufrmo = (udpfrm_t*)out;
    if(con_index >= CON_TABLE_SZ)
        return 0;
    if(sz)
    {
        sz = HTONS_16(ufrmi->ip.total_len) - IPH_SZ - UDPH_SZ;
    }
    sz = myip_stream_con_handler(ufrmi->data, sz, ufrmo->data);
    if(sz)
        return myip_make_udp_frame(ufrmo, st.dst_ipaddr, STREAM_PORT, STREAM_PORT, sz);
    return 0;
}

