/**
 @file trig_furman.h
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019        Initial version FT81x and FT80x
 * Mar 2020        BT815/6 support added
 * Mar 2021        BT817/8 support added
 * Jun 2025        BT82x support added
 *
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

/* Note that EVE furman angles are 16.16 format and have integer furmans in the
 * upper 16-bits and fractional furmans in the lower 16-bits. */

 /* USE_TRIG_TABLE
 * This can be used on embedded systems or compilers which do not have a built-in 
 * math.h implementation or floating point. If it is not set then the built-in
 * compiler functions defined in math.h are used.
 */
//#undef USE_TRIG_TABLE

/* Convert degrees into furmans
 * Input degrees (modulus of 360 is used as the input)
 * Returns furmans within range 0x0000 to 0xffff
 * Note a furman value of zero returns zero degrees
 */
#define DEG2FURMAN(deg) ((0x10000 * ((deg) % 360)) / 360)

/* Convert furmans into degrees
 * Input range is 0x0000 to 0xffff (input is masked to this range)
 * Returns degrees within range -180 degrees to + 180 degrees
 * Note a furman value of zero is zero degrees
 */
#define FURMAN2DEG(fur) (((360 * (((fur) + 0x8000) & 0xffff)) / 0x10000) - 180)

/* Calculate X and Y components of circumferece
 * Input raduis and angle in degrees
 * Returns X or Y component of vector
 */
#define CIRC_X_DEG(radius, deg) ((radius) * sin_furman((deg) * 0x10000 / 360) / 0x8000)
#define CIRC_Y_DEG(radius, deg) ((radius) * cos_furman((deg) * 0x10000 / 360) / 0x8000)

/* Calculate X and Y components of circumferece
 * Input raduis and angle in furmans
 * Returns X or Y component of vector
 */
#define CIRC_X(radius, fur) ((radius) * sin_furman(fur) / 0x8000)
#define CIRC_Y(radius, fur) ((radius) * cos_furman(fur) / 0x8000)

/* Calculate sine of furman angle
 * Input angle in furman
 * Returns sine values from -0x7fff to + 0x7fff
 */
int16_t sin_furman(uint16_t furman16);

/* Calculate cosine of furman angle
 * Input angle in furman
 * Returns cosine values from -0x7fff to + 0x7fff
 */
int16_t cos_furman(uint16_t furman16);
