
#include "eth/eth.h"
#include "stream.h"

#define STREAM_IN   (1 << 0)
#define STREAM_OUT  (1 << 1)
#define STREAM_IO   (STREAM_IN | STREAM_OUT)

#define STREAM_CHUNK_SZ 512

struct stream_t
{
    uint8_t dir;
    uint8_t start;
    uint32_t counter, sz;
} st;

uint16_t myip_stream_init(void)
{
    st.dir = 0;
    st.counter = 0;
    st.sz = 0;
    st.start = 0;
}

uint16_t myip_stream_con_handler(uint8_t *in, uint16_t sz, uint8_t *out);
{
    if(st.start == 0)
        return;
    uint16_t i;
    uint32_t *ptr, chunk_sz = st.sz - st.counter;
    if(chunk_sz > STREAM_CHUNK_SZ)
        chunk_sz = STREAM_CHUNK_SZ;
    if(st.dir & STREAM_OUT)
    {
        for(i = 0; i < STREAM_CHUNK_SZ; i += 4)
        {
            ptr = (uint32_t*)&out[i];
            *ptr1 = 0;
        }
    }
    st.counter += STREAM_CHUNK_SZ;
    if(st.counter >= st.sz)
        st.start = 0;
    return STREAM_CHUNK_SZ;
}

