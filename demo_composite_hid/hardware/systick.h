#ifndef _SYSTICK_H
#define _SYSTICK_H

#include <stdio.h>

extern volatile uint32_t systick_cnt;

void systick_init(void);

#define GET_SYSTICK() (systick_cnt)

#endif
