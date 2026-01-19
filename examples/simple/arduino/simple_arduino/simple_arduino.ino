#include <SPI.h>

extern "C" {
#include <EVE.h>
#include "eve_example.h"
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Initialise the display
  Serial.print("Starting demo...\n");
  
  eve_example();
}
