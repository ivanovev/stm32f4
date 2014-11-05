
#ifndef __VERSION_H__
#define __VERSION_H__

#include "stdint.h"

uint32_t    version(char *buf, int sz);
uint32_t    version_date(char *buf, int sz);
uint32_t    version_time(char *buf, int sz);

#endif /* __VERSION_H__ */

