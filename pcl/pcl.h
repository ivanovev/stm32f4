
#ifndef __PCL_H__
#define __PCL_H__

#include <main.h>
#include "pcl/picol.h"

void                pcl_init(void);
void                pcl_clear(void);
uint16_t            pcl_exec(char *cmd);
void                pcl_io(void);
struct picolInterp* pcl_get_interp(void);
void                pcl_gpio_exti_cb(void);

#endif

