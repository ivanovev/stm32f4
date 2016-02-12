
#include "eth/eth.h"
#include "telnetd.h"

#define STATE_NORMAL 0
#define STATE_IAC    1
#define STATE_WILL   2
#define STATE_WONT   3
#define STATE_DO     4
#define STATE_DONT   5
#define STATE_CLOSE  6

#define TELNET_IAC   255
#define TELNET_WILL  251
#define TELNET_WONT  252
#define TELNET_DO    253
#define TELNET_DONT  254

struct {
    Queue qi;
    Queue qo;
} tds;

uint8_t telnetd_prompt[] = "#> ";

void myip_telnetd_init(void)
{
    tds.qi.head = 0;
    tds.qi.tail = 0;
    tds.qo.head = 0;
    tds.qo.tail = 0;
#ifndef ENABLE_PCL
    enqueue_str(&tds.qo, telnetd_prompt, sizeof(telnetd_prompt));
#else
    enqueue_str(&tds.qi, (uint8_t*)"\n", 1);
#endif
}

uint16_t myip_telnetd_con_handler(uint8_t *in, uint16_t sz, uint8_t *out)
{
#ifndef ENABLE_PCL
    if(!mystrncmp((char*)in, "exit", 4))
    {
        myip_tcp_con_close();
        return 0;
    }
    else if(sz > 0)
    {
        if(in[0] != TELNET_IAC)
            enqueue_str(&tds.qo, telnetd_prompt, sizeof(telnetd_prompt));
    }
#else
    uint16_t i;
    for(i = 0; i < sz; i++)
    {
        if(in[i] == TELNET_IAC)
        {
            i += 2;
            continue;
        }
        enqueue(&tds.qi, in[i]);
    }
#endif
    return dequeue(&tds.qo, out, 0);
}

uint16_t telnetd_recv_str(char *buf)
{
    return dequeue(&tds.qi, (uint8_t*)buf, 0);
}

void telnetd_send_str(const char *str, uint16_t sz)
{
    enqueue_str(&tds.qo, (uint8_t*)str, sz);
}

