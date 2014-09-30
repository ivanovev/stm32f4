
#include "mytelnetd.h"
#include "uart/uart.h"

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

TELNETD_STATE tds;
extern TCP_CON tcp_con;
uint8_t telnetd_prompt[] = "#> ";
uint8_t buf[IO_BUF_SZ];

void myip_telnetd_init(void)
{
    tds.qi.head = 0;
    tds.qi.tail = 0;
    tds.qo.head = 0;
    tds.qo.tail = 0;
    enqueue_str(&tds.qo, telnetd_prompt, sizeof(telnetd_prompt));
}

uint16_t myip_telnetd_io(uint8_t *data, uint16_t sz)
{
    if(!mystrncmp((char*)data, "exit", 4))
    {
        tcp_con.state = TCP_CON_CLOSE;
        return 0;
    }
    uint16_t i;
    for(i = 0; i < sz; i++)
    {
        if(data[i] == TELNET_IAC)
        {
            i += 2;
            continue;
        }
        enqueue(&tds.qi, data[i]);
    }
    return dequeue(&tds.qo, data, 0);
}

uint16_t telnetd_recv_str(char *buf)
{
    return dequeue(&tds.qi, (uint8_t*)buf, 0);
}

void telnetd_send_str(const char *str, uint16_t sz)
{
    enqueue_str(&tds.qo, (uint8_t*)str, sz);
}

