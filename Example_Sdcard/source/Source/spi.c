/*
 * spi.c
 *
 *  Created on: 9 jul. 2024
 *      Author: agustin
 */

#include "Include/spi.h"

#if (!defined(USE_FREERTOS) && !defined(USE_NOT_FREERTOS))
#error "Se debe definir USE_FREERTOS o USE_NOT_FREERTOS"
#endif

#ifdef	USE_NOT_FREERTOS

#elif defined(USE_FREERTOS)

#endif


