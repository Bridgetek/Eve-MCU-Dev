#include <SPI.h>

extern "C" {
#include "EVE.h"
#include "HAL.h"
#include "MCU.h"
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Initialise the display
  Serial.print("Initialise...\n");
  EVE_Init();

  Serial.print("Calibrating display...\n");
  EVE_LIB_BeginCoProList();
  EVE_CMD_DLSTART();
  EVE_CLEAR_COLOR_RGB(0, 0, 0);
  EVE_CLEAR(1, 1, 1);
  EVE_COLOR_RGB(255, 255, 255);
  EVE_CMD_TEXT(EVE_DISP_WIDTH / 2, EVE_DISP_HEIGHT / 2,
               28, EVE_OPT_CENTERX | EVE_OPT_CENTERY, "Please tap on the dots");
  EVE_CMD_CALIBRATE(0);
  EVE_LIB_EndCoProList();
  if (EVE_LIB_AwaitCoProEmpty() != 0) {
    Serial.print("Exception... ");
    while (1)
      ;
  }

  Serial.print("Starting Demo...\n");
  while (1)
  {
    Serial.print("Demo loop ");
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_CLEAR_COLOR_RGB(0, 0, 0);
    EVE_CLEAR(1, 1, 1);
    EVE_COLOR_RGB(255, 255, 255);
    EVE_CMD_TEXT((uint16_t)EVE_DISP_WIDTH / 2, (uint16_t)EVE_DISP_HEIGHT / 2,
                28ul, EVE_OPT_CENTERX, "Arduino");
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
  }
}
