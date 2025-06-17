# EVE-MCU-Dev Example Code Snippets

[Back](../README.md)

The `snippets` directory contains code that is used in the examples for the Python BT82x Development moduleEVE-MCU-Dev code. The following code is available in that directory:

## Contents

- [Widgets](#widgets)
  - [Seven Segment LED Widget](#Seven-Segment-LED-Widget)

## Widgets

There are several widgets in the directory:

| File/Folder | Description |
| --- | --- |
| [sevenseg](#Seven-Segment-LED-Widget) | Seven segement LED code |

### Seven Segment LED Widget

This widget will simulate a 7 segment LED display. Active LEDs will be drawn in the foreground colour and inactive ones as the background. The digit to be displayed is sent in the range 0-16. For values 0 to 9 the decimal number is shown, for 10 to 15 the letters 'a' to 'f' are shown for hexadecimale displays, and for 16 a dash '-' is displayed.

![Seven Segment LEDs](docs/segment123.png)

_Calling format:_

   `sevensegment(x, y, size, digit, fgcolour, bgcolour);`

_Parameters:_

-   uint32_t **x**, uint32_t **y**: Location of top left of the seven segment LED widget (in pixels).
-   uint16_t **size**: Size of a segment of the seven segment LED widget (in pixels).
-   uint8_t **digit**: Number to display on seven segment LED. 
-   uint32_t **fgcolour**: RGB colour for active segment.
-   uint32_t **bgcolour**: RGB colour for inactive segment.

_Example:_

```
#include "snippets/sevenseg/sevenseg.h"

void drawLED(void){
    // Draw a seven segment LED with bright red ON segments and dark red OFF segments
    sevensegment(200, 100, 90, int(number%10), 0xff0000, 0x200000);
}
```
