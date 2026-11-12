#include <stdint.h>
#include "cybsp.h"
#include "cyhal.h"
#include "EVE.h"
#include "eve_example.h"

int main(void)
{
    cy_rslt_t result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
        CY_ASSERT(0);

    __enable_irq();
    eve_example();

    for (;;)
        cyhal_system_delay_ms(1000U);
}
