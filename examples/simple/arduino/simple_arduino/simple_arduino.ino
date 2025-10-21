#include <SPI.h>

extern "C" {
#include <EVE.h>
#include "eve_example.h"
}

/**
 * @brief Functions used to store calibration data in file.
   @details Currently not used.
 */
//@{
int8_t platform_calib_init(void)
{
    return 1;
}

int8_t platform_calib_write(struct touchscreen_calibration *calib)
{
		(void)calib;
	  return 0;
}

int8_t platform_calib_read(struct touchscreen_calibration *calib)
{
		(void)calib;
    return -1;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Initialise the display
  Serial.print("Starting demo...\n");
  
  eve_example();
}
