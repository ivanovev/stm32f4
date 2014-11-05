
#include "util/util.h"

uint32_t version(char *buf, int sz)
{
#ifdef __DATE__
#ifdef __TIME__
    return mysnprintf(buf, sz, "%s %s", __DATE__, __TIME__);
#endif
#endif
}

uint32_t version_date(char *buf, int sz)
{
#ifdef __DATE__
    return mysnprintf(buf, sz, "%s", __DATE__);
#endif
}

uint32_t version_time(char *buf, int sz)
{
#ifdef __TIME__
    return mysnprintf(buf, sz, "%s", __TIME__);
#endif
}

