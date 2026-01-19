# EVE-MCU-Dev Example Code Snippets

[Back](../README.md)

The `snippets` directory contains code that is used in the examples for the EVE-MCU-Dev code. The code can be included in example projects to extend functionality.

## Contents

- [Utilities](#Utilities)
  - [Calibration Utility](#calibration-utility)
  - [Touch Detection Utility](#touch-detection-utility)
  - [Tag Reading Utility](#tag-reading-utility)
- [Widgets](#widgets)
  - [Seven Segment LED Widget](#seven-segment-led-widget)
- [Dials](#dials)
  - [Flight Control Widgets](#flight-control-widgets)
- [Maths](#maths)
  - [Trigonometry using Furmans](#trigonometry-using-furmans)

## Utilities

### Calibration Utility

This utility can be used to calibrate the touch screen automatically.

If a supported panel is setup in [EVE_config.h](../../include/EVE_config.h) then a standard pre-configured touchscreen matrix is used. Currently DP-1012-01A and DP-1561-01A panels are supported. 

A callback to platform functions are required to store the configuration matrix. These need to store the calibration values using a non-volatile method. On PCs this can be in a regular file, on embedded systems this cabn be flash, NVRAM or any other appropriate methods. The structure `struct touchscreen_calibration` contains the values and is seven 32-bit values.

_Header File:_

   `#include "snippets/touch.h"`

_Source File:_

   `snippets/touch.c`

_Calling format:_

   `int eve_calibrate(void)`

_Parameters:_

-   None.

_Returns:_

-   int - zero if the calibration has been successful or non-zero if it fails. 

_Example:_

```
   eve_calibrate();
```

### Touch Detection Utility

This utility can be used to detect any touch on the screen. It does not return the touch TAG.

_Header File:_

   `#include "snippets/touch.h"`

_Source File:_

   `snippets/touch.c`

_Calling format:_

   `int eve_key_detect(void)`

_Parameters:_

-   None.

_Returns:_

-   int - zero if the screen is not pressed, non-zero if there is any touch detected. 

_Example:_

```
   eve_key_detect();
```

### Tag Reading Utility

This utility can be used to detect any touch on the screen and return the tag associated with the touch point.

_Header File:_

   `#include "snippets/touch.h"`

_Source File:_

   `snippets/touch.c`

_Calling format:_

   `int eve_read_tag(uint8_t *key)`

_Parameters:_

-   int8_t **tag**: Tag value of currently pressed point on screen.

_Returns:_

-   int - zero if the screen is not pressed, non-zero if there is any touch detected. 

_Example:_

```
   uint8_t tag_val = 0;
   if (eve_read_tag(&tag_val))
   {
      // Screen is pressed, action the tag stored in tag_val
   }
```

## Widgets

There is source code for widgets in the directory:

| Snippet | Description |
| --- | --- |
| [sevenseg](#Seven-Segment-LED-Widget) | Seven segement LED code |

### Seven Segment LED Widget

This widget will simulate a 7 segment LED display. Active LEDs will be drawn in the foreground colour and inactive ones as the background. The digit to be displayed is sent in the range 0-16. For values 0 to 9 the decimal number is shown, for 10 to 15 the letters 'a' to 'f' are shown for hexadecimal displays, and for 16 a dash '-' is displayed.

![Seven Segment LEDs](docs/segment123.png)

_Header File:_

   `#include "snippets/widgets/sevenseg.h"`

_Source File:_

   `snippets/widgets/sevenseg.c`

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
#include "snippets/widgets/sevenseg.h"

void drawLED(void){
    // Draw a seven segment LED with bright red ON segments and dark red OFF segments
    sevensegment(200, 100, 90, int(number%10), 0xff0000, 0x200000);
}
```

## Dials

This directory contains skeuomorphic dials.

| Snippet | Description |
| --- | --- |
| [flightcontrols](#flight-control-widgets) | Flight control widgets code |

### Flight Control Widgets

These widgets will simulate an aeroplane altitude indicator and attitude indicator. The file has both indicators included.

Both widgets require the [Trigonometry using Furmans](#Trigonometry-using-Furmans) utility to be compiled with the application.

#### Flight Control Altitude Indicator

This is a simulation of an altitude indicator. It reads from zero to 10000 feet. It has 2 hands measuring thousands and hundreds of feet.

The altitude is specified in the call. It is clamped to 0 to 10000 feet as a real-life altitude indicator would.

![Altitude Indicator](docs/altitude.png)

_Header File:_

   `#include "snippets/dials/flightcontrols.h"`

_Source File:_

   `snippets/dials/flightalt.c`

_Calling format:_

   `altwidget(x, y, radius, alt)`

_Parameters:_

-   uint32_t **x**, uint32_t **y**: Location of centre of the indicator (in pixels).
-   int16_t **radius**: Radius of the widget dial (in pixels).
-   int **alt**: Altitude to render.

_Example:_

```
#include "snippets/widgets/sevenseg.h"

altwidget(eve, 300, 300, 200, 4382);
```

#### Flight Control Attitude Indicator

This is a simulation of an attitude indicator. It displays pitch, roll and climb.

The pitch, roll and climb are specified in the call and are in furmans. The range of pitch and climb must be between 0xc000 furmans (-90 degrees) and 0x4000 (+90 degrees). Roll may be between 0x8000 (-180 degrees) and 0x7fff (+180 degrees). 

![Attitude Indicator](docs/attitude.png)

_Header File:_

   `#include "snippets/dials/flightcontrols.h"`

_Source File:_

   `snippets/dials/flightatt.c`

_Calling format:_

   `attwidget(x, y, radius, pitch, climb, roll)`

_Parameters:_

-   uint32_t **x**, uint32_t **y**: Location of centre of the indicator (in pixels).
-   int16_t **radius**: Radius of the widget dial (in pixels).
-   int **pitch**: Pitch angle in furmans.
-   int **climb**: Climb angle in furmans.
-   int **roll**: Roll angle in furmans.

_Example:_

```
altwidget(eve, 300, 300, 200, 0x1000, 0xe000, 0xd000);
```

## Maths

| Snippet | Description |
| --- | --- |
| [trig_furman](#Trigonometry-using-Furmans) | Furman Trigonometry |

### Trigonometry using Furmans

A utility is provided to perform trigonometery using angles in furmans rather than degrees or radians. 

The return value is an `int16_t` format ranging from -0x8000 to +0x7fff.

_Header File:_

   `#include "snippets/maths/trig_furman.h"`

_Source File:_

   `snippets/maths/trig_furman.c`

_Calling format:_

   `sin_furman(angle);`
   `cos_furman(angle);`

_Parameters:_

-   int16_t **angle**: Angle in furmans

_Example:_

```
uint16_t scale = sin_furman(0xd000);
```

#### Trigonometry Furman Macros

Macros are provided to turn degrees into furmans and vice versa. Care needs to be taken as the accuracy in degrees is integer which is much less than the accuracy of furmans.

_Calling format:_

   `FURMAN2DEG(furman);`
   `DEG2FURMAN(degrees);`

_Parameters:_

-   int16_t **furman**: Angle in furmans
-   int **degrees**: Angle in degrees

_Example:_

```
int16_t angle = DEG2FURMAN(0x4000); // angle = 90
```

#### Trigonometry Circle Macros

Macros are provided to turn a radius and degrees/furmans into components for X and Y vector. Note that the return value is signed.

_Calling format:_

   `CIRC_X(radius, furman);`
   `CIRC_X_DEG(radius, degrees);`
   `CIRC_Y(radius, furman);`
   `CIRC_Y_DEG(radius, degrees);`

_Parameters:_

-   int **radius**: Radius of vector
-   int16_t **furman**: Angle in furmans
-   int **degrees**: Angle in degrees

_Example:_

```
// angle = 45
int16_t vx = CIRC_X(0x2000); 
int16_t vy = CIRC_Y(0x2000); 
```
