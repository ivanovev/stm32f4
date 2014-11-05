
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __arm__
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "util/myinit.h"
#endif

#include "util/macro.h"
#include "util/util.h"
#include "util/queue.h"

#include "stdint.h"
#include "stddef.h"
#include "pcl/picol.h"
#include "pcl/heap1.h"
#include "pcl/pcl.h"
#include "pcl/pcl_misc.h"
#include "pcl/pcl_sys.h"
#include "pcl/pcl_vfd.h"
#ifdef __arm__
#include "pcl/pcl_stm.h"
#endif

#endif

