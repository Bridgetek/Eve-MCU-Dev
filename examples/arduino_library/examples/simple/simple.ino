/**
 @file simple.ino
 */
/*
 * ============================================================================
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

#include "eve_example.h"

/**
 @brief EVE library handle.
 @details This is the one instance of the EVE library. Available as a global to other files.
 */
BtEve5 eve;

extern const uint8_t font0[];
const EVE_GPU_FONT_HEADER *font0_hdr = (const EVE_GPU_FONT_HEADER *)font0;

/**
 * @brief Functions used to store calibration data in file.
   @details Currently not used.
 */
//@{
int8_t platform_calib_init(void) {
  return 1;
}

int8_t platform_calib_write(struct touchscreen_calibration *calib) {
  (void)calib;
  return 0;
}

int8_t platform_calib_read(struct touchscreen_calibration *calib) {
  (void)calib;
  return -1;
}

void setup() {
  Serial.begin(9600);

  uint32_t font_end;

  // Setup the EVE library (WUXGA is 1920x1200 pixels)
  eve.setup(WUXGA, EVE_RAM_G_1_GBIT);
  // Initialise the display
  eve.Init();

  // Calibrate the display
  Serial.print("Calibrating display...\n");
  if (eve_calibrate() != 0) {
    Serial.print("Exception...\n");
    while (1)
      ;
  }

  // Load fonts and images
  Serial.print("Loading font...\n");
  font_end = eve_init_fonts();
  Serial.print("Loading images...\n");
  eve_load_images(font_end);

  Serial.print("Starting demo...\n");
}

uint32_t counter = 0;

void loop() {
  uint8_t key;
  int8_t i;
  uint32_t units;

  // Comment this line if the counter needs to increment continuously.
  // Uncomment and it will increment by one each press.
  //while (eve_read_tag(&key) != 0);

  eve.LIB_BeginCoProList();
  eve.CMD_DLSTART();
  eve.CLEAR_COLOR_RGB(0, 0, 0);
  eve.CLEAR(1, 1, 1);
  eve.COLOR_RGB(255, 255, 255);

  eve.BEGIN(eve.BEGIN_BITMAPS);
#if IS_EVE_API(2, 3, 4, 5)
  // Set origin on canvas using EVE_VERTEX_TRANSLATE.
  eve.VERTEX_TRANSLATE_X(((EVE_DISP_WIDTH / 2) - (eve_img_bridgetek_logo_width / 2)) * 16);
  eve.VERTEX2II(0, 0, BITMAP_BRIDGETEK_LOGO, 0);
  eve.VERTEX_TRANSLATE_X(0);
#else
  // Place directly on canvas eve.VERTEX_TRANSLATE not available.
  eve.VERTEX2II((EVE_DISP_WIDTH / 2) - (eve_img_bridgetek_logo_width / 2), 0, BITMAP_BRIDGETEK_LOGO, 0);
#endif

  eve.CMD_TEXT(EVE_DISP_WIDTH / 2, eve_img_bridgetek_logo_height,
               28, eve.OPT_CENTERX, "Touch the counter");

  eve.TAG(100);

  eve.COLOR_RGB(255, 0, 0);

  eve.BEGIN(eve.BEGIN_BITMAPS);
  units = 1;

#if IS_EVE_API(2, 3, 4, 5)
  eve.VERTEX_TRANSLATE_Y((EVE_DISP_HEIGHT / 2) * 16);

  for (i = 0; i < 5; i++) {
    eve.VERTEX_TRANSLATE_X((((EVE_DISP_WIDTH - (font0_hdr->FontWidthInPixels * 5)) / 2) - (font0_hdr->FontWidthInPixels) + (font0_hdr->FontWidthInPixels * (5 - i))) * 16);
    eve.VERTEX2II(0, 0, FONT_CUSTOM, ((counter / units) % 10) + 1);  //+1 as in the converted font the number '0' is in position 1 in the font table
    units *= 10;
  }
#else
  for (i = 0; i < 5; i++) {
    eve.VERTEX2II((((EVE_DISP_WIDTH - (font0_hdr->FontWidthInPixels * 5)) / 2) - (font0_hdr->FontWidthInPixels) + (font0_hdr->FontWidthInPixels * (5 - i))),
                  (EVE_DISP_HEIGHT / 2), FONT_CUSTOM, ((counter / units) % 10) + 1);  //+1 as in the converted font the number '0' is in position 1 in the font table
    units *= 10;
  }
#endif

  eve.DISPLAY();
  eve.CMD_SWAP();
  eve.LIB_EndCoProList();
  eve.LIB_AwaitCoProEmpty();

  while (eve_read_tag(&key) == 0)
    ;

  if (key == 100) {
    counter++;
    if (counter == 100000) {
      counter = 0;
    }
  }
}
