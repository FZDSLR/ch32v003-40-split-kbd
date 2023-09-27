#ifndef _KZ_TICKER_H
#define _KZ_TICKER_H

#include "systick.h"

#define kz_get_timestamp32() (GET_SYSTICK())
#define kz_get_timestamp16() (GET_SYSTICK() & 0x0000ffff)

#endif
