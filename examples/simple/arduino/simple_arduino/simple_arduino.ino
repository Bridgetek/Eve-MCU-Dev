#include <SPI.h>

extern "C" {
#include "eve_example.h"
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Initialise the display
  Serial.print("Starting EVE...\n");
  
  /* Start example code */
  eve_example();
}
