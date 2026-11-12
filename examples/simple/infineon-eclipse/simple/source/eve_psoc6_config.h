#ifndef EVE_PSOC6_CONFIG_H
#define EVE_PSOC6_CONFIG_H

#include "cyhal.h"

/*
 * Default wiring on the breadboard headers of CY8CPROTO-062S2-43439.
 * Change these macros if a different SCB-compatible SPI pin group is used.
 */
#define EVE_PSOC6_MOSI_PIN       P5_0
#define EVE_PSOC6_MISO_PIN       P5_1
#define EVE_PSOC6_SCLK_PIN       P5_2
#define EVE_PSOC6_CS_PIN         P5_7
#define EVE_PSOC6_PD_PIN         P5_5
#define EVE_PSOC6_INT_PIN		 P5_6

#define EVE_PSOC6_SPI_INIT_HZ    (1000000UL)
#define EVE_PSOC6_SPI_RUN_HZ     (24000000UL)

#endif
