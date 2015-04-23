
#ifndef __ETH_H__
#define __ETH_H__

#ifdef ENABLE_MYIP
#include "myip.h"
#include "mytcp.h"
#endif

void        eth_init(void);
void        eth_deinit(void);
void        eth_reset(void);
void        eth_io(void);

#endif

