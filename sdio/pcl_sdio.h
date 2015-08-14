
#ifdef ENABLE_PCL
#ifndef __PCL_SDIO_H__
#define __PCL_SDIO_H__

#include <main.h>
#include "pcl/pcl.h"

extern struct   picolInterp *pcl_interp;

void            pcl_sdio_init(picolInterp *i);

#endif
#endif
