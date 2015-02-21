
#ifndef __CPTR_H__
#define __CPTR_H__

#include <main.h>

#define WIFI_RESET_GPIO C
#define WIFI_RESET_PIN 0

#define WIFI_NRELOAD_GPIO F
#define WIFI_NRELOAD_PIN 9

void cptr_init(void);
void cptr_wifi_reset(void);
void cptr_wifi_reload(void);

#endif

