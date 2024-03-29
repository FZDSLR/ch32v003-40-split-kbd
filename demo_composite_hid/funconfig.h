#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

#define FUNCONF_USE_HSE 0  			// external crystal on PA1 PA2
#define FUNCONF_USE_HSI 1    		// internal 24MHz clock oscillator
#define FUNCONF_USE_PLL 1 			// use PLL x2
#define FUNCONF_HSE_BYPASS 0 		// bypass the HSE when using an external clock source
									// requires enabled HSE

#define FUNCONF_USE_DEBUGPRINTF 1
#define FUNCONF_USE_UARTPRINTF 0
#define CH32V003                1
#define FUNCONF_SYSTICK_USE_HCLK 1
// #define FUNCONF_DEBUGPRINTF_TIMEOUT (1<<31) // Wait for a very very long time.

#endif

