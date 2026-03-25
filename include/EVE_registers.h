/**
 @file EVE_registers.h
/*
 * ============================================================================
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * https://brtchip.com/wp-content/uploads/2021/11/BRT_Software_License_Agreement.pdf ("the Licence Terms").
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
 *
 @brief Cross-generation EVE register address map.
 @details Provides a single unified set of EVE_REG_* macros that resolve to
     the correct hardware address for the selected generation at compile time.
     The generation is controlled by FT8XX_TYPE (via EVE_config.h / FT8xx.h),
     which sets EVE_API to 1–5.

     Usage:
         #include <EVE_config.h>   // sets FT8XX_TYPE
         #include <FT8xx.h>        // sets EVE_API, includes this file
         // or include directly after FT8xx.h is included.

     Macro convention:
         EVE_API_SELECT(a1, a2, a3, a4, a5)
             a1 = EVE1 address (FT800/FT801)
             a2 = EVE2 address (FT810–FT813, BT880–BT883)
             a3 = EVE3 address (BT815/BT816)
             a4 = EVE4 address (BT817/BT818)
             a5 = EVE5 address (BT820)

         EVE_REG_NOT_AVAILABLE (0ul) marks a register absent on that generation.
         Using an absent register at runtime is a logic error — guard call sites
         with IS_EVE_API() to make absence visible at compile time.

     NOTE: EVE3 and EVE4 (BT81x) share the same address map, so a3 == a4
     throughout this file. They are kept as separate columns so that if a
     future BT81x variant diverges the table stays correct.
 */
/*
 * ============================================================================
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * https://brtchip.com/wp-content/uploads/2021/11/BRT_Software_License_Agreement.pdf
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 * ============================================================================
 */

#ifndef _EVE_REGISTERS_H_
#define _EVE_REGISTERS_H_

/* -------------------------------------------------------------------------
 * Prerequisite: FT8xx.h must already be included so that EVE_API is defined
 * and the EVE_API_SELECT / IS_EVE_API macros are available.
 * ------------------------------------------------------------------------- */
#ifndef EVE_API
#error "EVE_registers.h requires FT8xx.h to be included first (defines EVE_API)."
#endif

/* =========================================================================
 * Sentinel value — register not available on this generation.
 * ========================================================================= */
#ifndef EVE_REG_NOT_AVAILABLE
#define EVE_REG_NOT_AVAILABLE  0ul
#endif

/* =========================================================================
 * EVE_API_SELECT — selects one of five address constants based on EVE_API.
 * The result is a (uint32_t) compile-time constant expression.
 * It CANNOT be used in #if directives; use IS_EVE_API() for that.
 * ========================================================================= */
#ifndef EVE_API_SELECT
#define EVE_API_SELECT(a1, a2, a3, a4, a5)             \
    ((EVE_API == 1) ? (a1) : (EVE_API == 2) ? (a2)     \
                           : (EVE_API == 3) ? (a3)     \
                           : (EVE_API == 4) ? (a4)     \
                                            : (a5))
#endif

/* =========================================================================
 *  Column header legend used in every table below:
 *  Name                    EVE1              EVE2           EVE3               EVE4            EVE5
 *                        FT800/801       FT81x/BT88x      BT815/816         BT817/818          BT820
 * ========================================================================= */

/* =========================================================================
 * CHIP ID REGISTER
 * ========================================================================= */

/** @brief Identification register — always reads as 0x7C */
#define EVE_REG_ID             EVE_API_SELECT(0x102400ul,  0x302000ul,  0x302000ul,  0x302000ul,  0x7f006000ul)

/* =========================================================================
 * DISPLAY REGISTERS
 * Controls the parallel display interface timing (PCLK, sync, blanking).
 * ========================================================================= */

/** @brief Pixel clock frequency divider
 *  @note Not present on EVE5 (BT820 uses a different clock architecture). */
#define EVE_REG_PCLK           EVE_API_SELECT(0x10246cul,  0x302070ul,  0x302070ul,  0x302070ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Pixel clock polarity (0 = rising edge, 1 = falling edge) */
#define EVE_REG_PCLK_POL       EVE_API_SELECT(0x102468ul,  0x30206cul,  0x30206cul,  0x30206cul,  0x7f0060b8ul)

/** @brief Horizontal total cycle count */
#define EVE_REG_HCYCLE         EVE_API_SELECT(0x102428ul,  0x30202cul,  0x30202cul,  0x30202cul,  0x7f00608cul)

/** @brief Horizontal display start offset */
#define EVE_REG_HOFFSET        EVE_API_SELECT(0x10242cul,  0x302030ul,  0x302030ul,  0x302030ul,  0x7f006090ul)

/** @brief Horizontal display pixel count */
#define EVE_REG_HSIZE          EVE_API_SELECT(0x102430ul,  0x302034ul,  0x302034ul,  0x302034ul,  0x7f006094ul)

/** @brief Horizontal sync fall offset */
#define EVE_REG_HSYNC0         EVE_API_SELECT(0x102434ul,  0x302038ul,  0x302038ul,  0x302038ul,  0x7f006098ul)

/** @brief Horizontal sync rise offset */
#define EVE_REG_HSYNC1         EVE_API_SELECT(0x102438ul,  0x30203cul,  0x30203cul,  0x30203cul,  0x7f00609cul)

/** @brief Vertical total cycle count */
#define EVE_REG_VCYCLE         EVE_API_SELECT(0x10243cul,  0x302040ul,  0x302040ul,  0x302040ul,  0x7f0060a0ul)

/** @brief Vertical display start offset */
#define EVE_REG_VOFFSET        EVE_API_SELECT(0x102440ul,  0x302044ul,  0x302044ul,  0x302044ul,  0x7f0060a4ul)

/** @brief Vertical display line count */
#define EVE_REG_VSIZE          EVE_API_SELECT(0x102444ul,  0x302048ul,  0x302048ul,  0x302048ul,  0x7f0060a8ul)

/** @brief Vertical sync fall offset */
#define EVE_REG_VSYNC0         EVE_API_SELECT(0x102448ul,  0x30204cul,  0x30204cul,  0x30204cul,  0x7f0060acul)

/** @brief Vertical sync rise offset */
#define EVE_REG_VSYNC1         EVE_API_SELECT(0x10244cul,  0x302050ul,  0x302050ul,  0x302050ul,  0x7f0060b0ul)

/** @brief RGB output bit reduction (0 = 8-bit per channel)
 *  @note Not present on EVE5. */
#define EVE_REG_OUTBITS        EVE_API_SELECT(0x102458ul,  0x30205cul,  0x30205cul,  0x30205cul,  EVE_REG_NOT_AVAILABLE)

/** @brief Dither enable
 *  @note Not present on EVE5. */
#define EVE_REG_DITHER         EVE_API_SELECT(0x10245cul,  0x302060ul,  0x302060ul,  0x302060ul,  EVE_REG_NOT_AVAILABLE)

/** @brief RGB output swizzle
 *  @note Not present on EVE5. */
#define EVE_REG_SWIZZLE        EVE_API_SELECT(0x102460ul,  0x302064ul,  0x302064ul,  0x302064ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Output clock spreading
 *  @note Not present on EVE5. */
#define EVE_REG_CSPREAD        EVE_API_SELECT(0x102464ul,  0x302068ul,  0x302068ul,  0x302068ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Display rotation
 *  @note Not present on EVE5 (use render engine rotate instead). */
#define EVE_REG_ROTATE         EVE_API_SELECT(0x102454ul,  0x302058ul,  0x302058ul,  0x302058ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Display list swap control */
#define EVE_REG_DLSWAP         EVE_API_SELECT(0x102450ul,  0x302054ul,  0x302054ul,  0x302054ul,  0x7f0060b4ul)

/** @brief DISP output signal control
 *  @note EVE5 only. Guard usage with IS_EVE_API(5). */
#define EVE_REG_DISP           EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f0060e4ul)

/* =========================================================================
 * TOUCH REGISTERS — single-touch (resistive / basic capacitive)
 * ========================================================================= */

/** @brief Tag query result at REG_TAG_X / REG_TAG_Y */
#define EVE_REG_TAG            EVE_API_SELECT(0x102478ul,  0x30207cul,  0x30207cul,  0x30207cul,  0x7f0060c4ul)

/** @brief Tag query X coordinate */
#define EVE_REG_TAG_X          EVE_API_SELECT(0x102470ul,  0x302074ul,  0x302074ul,  0x302074ul,  0x7f0060bcul)

/** @brief Tag query Y coordinate */
#define EVE_REG_TAG_Y          EVE_API_SELECT(0x102474ul,  0x302078ul,  0x302078ul,  0x302078ul,  0x7f0060c0ul)

/** @brief Touchscreen sampling mode */
#define EVE_REG_TOUCH_MODE     EVE_API_SELECT(0x1024f0ul,  0x302104ul,  0x302104ul,  0x302104ul,  0x7f006158ul)

/** @brief Touchscreen raw (x,y) packed 16:16 */
#define EVE_REG_TOUCH_RAW_XY   EVE_API_SELECT(0x102508ul,  0x30211cul,  0x30211cul,  0x30211cul,  0x7f006164ul)

/** @brief Touchscreen screen (x,y) packed 16:16 */
#define EVE_REG_TOUCH_SCREEN_XY EVE_API_SELECT(0x102510ul, 0x302124ul,  0x302124ul,  0x302124ul,  0x7f006160ul)

/** @brief Touchscreen tag result 0 */
#define EVE_REG_TOUCH_TAG      EVE_API_SELECT(0x102518ul,  0x30212cul,  0x30212cul,  0x30212cul,  0x7f006178ul)

/** @brief Touchscreen (x,y) used for tag 0 lookup */
#define EVE_REG_TOUCH_TAG_XY   EVE_API_SELECT(0x102514ul,  0x302128ul,  0x302128ul,  0x302128ul,  0x7f006174ul)

/** @brief Touchscreen tag result 1
 *  @note Not present on EVE1. */
#define EVE_REG_TOUCH_TAG1     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302134ul,  0x302134ul,  0x302134ul,  0x7f006180ul)

/** @brief Touchscreen (x,y) for tag 1 lookup
 *  @note Not present on EVE1. */
#define EVE_REG_TOUCH_TAG1_XY  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302130ul,  0x302130ul,  0x302130ul,  0x7f00617cul)

/** @brief Touchscreen tag result 2
 *  @note Not present on EVE1. */
#define EVE_REG_TOUCH_TAG2     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x30213cul,  0x30213cul,  0x30213cul,  0x7f006188ul)

/** @brief Touchscreen (x,y) for tag 2 lookup
 *  @note Not present on EVE1. */
#define EVE_REG_TOUCH_TAG2_XY  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302138ul,  0x302138ul,  0x302138ul,  0x7f006184ul)

/** @brief Touchscreen tag result 3
 *  @note Not present on EVE1. */
#define EVE_REG_TOUCH_TAG3     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302144ul,  0x302144ul,  0x302144ul,  0x7f006190ul)

/** @brief Touchscreen (x,y) for tag 3 lookup
 *  @note Not present on EVE1. */
#define EVE_REG_TOUCH_TAG3_XY  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302140ul,  0x302140ul,  0x302140ul,  0x7f00618cul)

/** @brief Touchscreen tag result 4
 *  @note Not present on EVE1. */
#define EVE_REG_TOUCH_TAG4     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x30214cul,  0x30214cul,  0x30214cul,  0x7f006198ul)

/** @brief Touchscreen (x,y) for tag 4 lookup
 *  @note Not present on EVE1. */
#define EVE_REG_TOUCH_TAG4_XY  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302148ul,  0x302148ul,  0x302148ul,  0x7f006194ul)

/** @brief Touch transform matrix coefficient A (s15.16) */
#define EVE_REG_TOUCH_TRANSFORM_A EVE_API_SELECT(0x10251cul, 0x302150ul, 0x302150ul, 0x302150ul, 0x7f00619cul)

/** @brief Touch transform matrix coefficient B (s15.16) */
#define EVE_REG_TOUCH_TRANSFORM_B EVE_API_SELECT(0x102520ul, 0x302154ul, 0x302154ul, 0x302154ul, 0x7f0061a0ul)

/** @brief Touch transform matrix coefficient C (s15.16) */
#define EVE_REG_TOUCH_TRANSFORM_C EVE_API_SELECT(0x102524ul, 0x302158ul, 0x302158ul, 0x302158ul, 0x7f0061a4ul)

/** @brief Touch transform matrix coefficient D (s15.16) */
#define EVE_REG_TOUCH_TRANSFORM_D EVE_API_SELECT(0x102528ul, 0x30215cul, 0x30215cul, 0x30215cul, 0x7f0061a8ul)

/** @brief Touch transform matrix coefficient E (s15.16) */
#define EVE_REG_TOUCH_TRANSFORM_E EVE_API_SELECT(0x10252cul, 0x302160ul, 0x302160ul, 0x302160ul, 0x7f0061acul)

/** @brief Touch transform matrix coefficient F (s15.16) */
#define EVE_REG_TOUCH_TRANSFORM_F EVE_API_SELECT(0x102530ul, 0x302164ul, 0x302164ul, 0x302164ul, 0x7f0061b0ul)

/** @brief Touchscreen configuration register
 *  @note Not present on EVE1. */
#define EVE_REG_TOUCH_CONFIG   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302168ul,  0x302168ul,  0x302168ul,  0x7f0061b4ul)

/** @brief Resistive touch: ADC mode selection
 *  @note Not present on EVE5 (capacitive only). */
#define EVE_REG_TOUCH_ADC_MODE EVE_API_SELECT(0x1024f4ul,  0x302108ul,  0x302108ul,  0x302108ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Resistive touch: charge time
 *  @note Not present on EVE5. */
#define EVE_REG_TOUCH_CHARGE   EVE_API_SELECT(0x1024f8ul,  0x30210cul,  0x30210cul,  0x30210cul,  EVE_REG_NOT_AVAILABLE)

/** @brief Resistive touch: settle time
 *  @note Not present on EVE5. */
#define EVE_REG_TOUCH_SETTLE   EVE_API_SELECT(0x1024fcul,  0x302110ul,  0x302110ul,  0x302110ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Resistive touch: over-sample factor
 *  @note Not present on EVE5. */
#define EVE_REG_TOUCH_OVERSAMPLE EVE_API_SELECT(0x102500ul, 0x302114ul, 0x302114ul, 0x302114ul, EVE_REG_NOT_AVAILABLE)

/** @brief Resistive touch: RZ threshold
 *  @note Not present on EVE5. */
#define EVE_REG_TOUCH_RZTHRESH EVE_API_SELECT(0x102504ul,  0x302118ul,  0x302118ul,  0x302118ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Resistive touch: measured Z (resistance)
 *  @note Not present on EVE5. */
#define EVE_REG_TOUCH_RZ       EVE_API_SELECT(0x10250cul,  0x302120ul,  0x302120ul,  0x302120ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Resistive touch: fault register
 *  @note EVE2/3/4 only. */
#define EVE_REG_TOUCH_FAULT    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302170ul,  0x302170ul,  0x302170ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Resistive direct-touch XY
 *  @note Not present on EVE5. */
#define EVE_REG_TOUCH_DIRECT_XY   EVE_API_SELECT(0x102574ul, 0x30218cul, 0x30218cul, 0x30218cul, EVE_REG_NOT_AVAILABLE)

/** @brief Resistive direct-touch Z1Z2
 *  @note Not present on EVE5. */
#define EVE_REG_TOUCH_DIRECT_Z1Z2 EVE_API_SELECT(0x102578ul, 0x302190ul, 0x302190ul, 0x302190ul, EVE_REG_NOT_AVAILABLE)

/* =========================================================================
 * CAPACITIVE MULTI-TOUCH REGISTERS
 * Extended mode provides up to 5 touch points on capacitive panels.
 * ========================================================================= */

/** @brief 0 = single-touch (compatibility), 1 = multi-touch (extended) */
#define EVE_REG_CTOUCH_EXTENDED   EVE_API_SELECT(0x1024f4ul,  0x302108ul,  0x302108ul,  0x302108ul,  0x7f00615cul)

/** @brief Capacitive touch 0 screen (x,y) */
#define EVE_REG_CTOUCH_TOUCH0_XY  EVE_API_SELECT(0x102510ul,  0x302124ul,  0x302124ul,  0x302124ul,  0x7f006160ul)

/** @brief Capacitive touch 1 (x,y)
 *  @note EVE1: extended mode only via CTOUCH_TOUCH1_XY alias. */
#define EVE_REG_CTOUCH_TOUCH1_XY  EVE_API_SELECT(0x102508ul,  0x30211cul,  0x30211cul,  0x30211cul,  EVE_REG_NOT_AVAILABLE)

/** @brief Capacitive touch 2 (x,y) — EVE1/2/3/4 */
#define EVE_REG_CTOUCH_TOUCH2_XY  EVE_API_SELECT(0x102574ul,  0x30218cul,  0x30218cul,  0x30218cul,  EVE_REG_NOT_AVAILABLE)

/** @brief Capacitive touch 3 (x,y) — EVE1/2/3/4 */
#define EVE_REG_CTOUCH_TOUCH3_XY  EVE_API_SELECT(0x102578ul,  0x302190ul,  0x302190ul,  0x302190ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Capacitive touch 4 X — EVE1/2/3/4 */
#define EVE_REG_CTOUCH_TOUCH4_X   EVE_API_SELECT(0x102538ul,  0x30216cul,  0x30216cul,  0x30216cul,  EVE_REG_NOT_AVAILABLE)

/** @brief Capacitive touch 4 Y — EVE1/2/3/4 */
#define EVE_REG_CTOUCH_TOUCH4_Y   EVE_API_SELECT(0x10250cul,  0x302120ul,  0x302120ul,  0x302120ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Capacitive touch 4 (x,y) packed — EVE5 only */
#define EVE_REG_CTOUCH_TOUCH4_XY  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                              EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                              0x7f006170ul)

/** @brief Capacitive raw (x,y) — EVE5 extended mode (touch A) */
#define EVE_REG_CTOUCH_TOUCHA_XY  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                              EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                              0x7f006164ul)

/** @brief Capacitive touch B (x,y) — EVE5 extended mode */
#define EVE_REG_CTOUCH_TOUCHB_XY  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                              EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                              0x7f006168ul)

/** @brief Capacitive touch C (x,y) — EVE5 extended mode */
#define EVE_REG_CTOUCH_TOUCHC_XY  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                              EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                              0x7f00616cul)

/* =========================================================================
 * EHOST (External Host) TOUCH REGISTERS
 * Allow the host MCU to inject synthetic touch points.
 * EVE3/EVE4 only.
 * ========================================================================= */

/** @brief EHOST touch point X injection
 *  @note EVE3/4 only. */
#define EVE_REG_EHOST_TOUCH_X     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                              0x30210cul,  0x30210cul,  EVE_REG_NOT_AVAILABLE)

/** @brief EHOST touch point Y injection
 *  @note EVE3/4 only. */
#define EVE_REG_EHOST_TOUCH_Y     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                              0x302118ul,  0x302118ul,  EVE_REG_NOT_AVAILABLE)

/** @brief EHOST touch point ID
 *  @note EVE3/4 only. */
#define EVE_REG_EHOST_TOUCH_ID    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                              0x302114ul,  0x302114ul,  EVE_REG_NOT_AVAILABLE)

/** @brief EHOST touch acknowledgement
 *  @note EVE3/4 only. */
#define EVE_REG_EHOST_TOUCH_ACK   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                              0x302170ul,  0x302170ul,  EVE_REG_NOT_AVAILABLE)

/* =========================================================================
 * AUDIO / MEDIA REGISTERS
 * Sound effect synthesizer and PCM audio playback.
 * ========================================================================= */

/** @brief Sound effect select */
#define EVE_REG_SOUND          EVE_API_SELECT(0x102484ul,  0x302088ul,  0x302088ul,  0x302088ul,  0x7f0060d4ul)

/** @brief Start effect playback (write 1) */
#define EVE_REG_PLAY           EVE_API_SELECT(0x102488ul,  0x30208cul,  0x30208cul,  0x30208cul,  0x7f0060d8ul)

/** @brief Volume for PCM playback (EVE1/2/3/4) or left channel (EVE5) */
#define EVE_REG_VOL_PB         EVE_API_SELECT(0x10247cul,  0x302080ul,  0x302080ul,  0x302080ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Volume for synth sound */
#define EVE_REG_VOL_SOUND      EVE_API_SELECT(0x102480ul,  0x302084ul,  0x302084ul,  0x302084ul,  0x7f0060d0ul)

/** @brief Volume for playback left channel — EVE5 only */
#define EVE_REG_VOL_L_PB       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f0060c8ul)

/** @brief Volume for playback right channel — EVE5 only */
#define EVE_REG_VOL_R_PB       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f0060ccul)

/** @brief Audio playback RAM start address */
#define EVE_REG_PLAYBACK_START EVE_API_SELECT(0x1024a4ul,  0x3020b4ul,  0x3020b4ul,  0x3020b4ul,  0x7f00610cul)

/** @brief Audio playback sample length (bytes) */
#define EVE_REG_PLAYBACK_LENGTH EVE_API_SELECT(0x1024a8ul, 0x3020b8ul,  0x3020b8ul,  0x3020b8ul,  0x7f006110ul)

/** @brief Audio playback current read pointer */
#define EVE_REG_PLAYBACK_READPTR EVE_API_SELECT(0x1024acul, 0x3020bcul, 0x3020bcul,  0x3020bcul,  0x7f006114ul)

/** @brief Audio playback frequency (Hz) */
#define EVE_REG_PLAYBACK_FREQ  EVE_API_SELECT(0x1024b0ul,  0x3020c0ul,  0x3020c0ul,  0x3020c0ul,  0x7f006118ul)

/** @brief Audio playback format */
#define EVE_REG_PLAYBACK_FORMAT EVE_API_SELECT(0x1024b4ul, 0x3020c4ul,  0x3020c4ul,  0x3020c4ul,  0x7f00611cul)

/** @brief Audio playback loop enable */
#define EVE_REG_PLAYBACK_LOOP  EVE_API_SELECT(0x1024b8ul,  0x3020c8ul,  0x3020c8ul,  0x3020c8ul,  0x7f006120ul)

/** @brief Start audio playback (write 1) */
#define EVE_REG_PLAYBACK_PLAY  EVE_API_SELECT(0x1024bcul,  0x3020ccul,  0x3020ccul,  0x3020ccul,  0x7f006124ul)

/** @brief Audio playback pause (0 = play, 1 = pause)
 *  @note Not present on EVE1/2. */
#define EVE_REG_PLAYBACK_PAUSE EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x3025ecul,   0x3025ecul,  0x7f0065d0ul)

/** @brief Video playback control
 *  @note Not present on EVE1. */
#define EVE_REG_PLAY_CONTROL   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x30914eul,   0x30914eul,  0x7f004050ul)

/** @brief 32-bit mask of currently playing animations
 *  @note Not present on EVE1/2. */
#define EVE_REG_ANIM_ACTIVE    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x30902cul,   0x30902cul,  0x7f00402cul)

/* =========================================================================
 * MEDIA FIFO REGISTERS
 * Used with CMD_MEDIAFIFO for streaming media.
 * Not present on EVE1.
 * ========================================================================= */

/** @brief Media FIFO read offset */
#define EVE_REG_MEDIAFIFO_READ  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x309014ul,  0x309014ul,  0x309014ul,  0x7f004014ul)

/** @brief Media FIFO write offset */
#define EVE_REG_MEDIAFIFO_WRITE EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x309018ul,  0x309018ul,  0x309018ul,  0x7f004018ul)

/** @brief Media FIFO base address — EVE3/4 only */
#define EVE_REG_MEDIAFIFO_BASE  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                            0x30901cul,  0x30901cul,  EVE_REG_NOT_AVAILABLE)

/** @brief Media FIFO size — EVE3/4 only */
#define EVE_REG_MEDIAFIFO_SIZE  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                            0x309020ul,  0x309020ul,  EVE_REG_NOT_AVAILABLE)

/* =========================================================================
 * DISPLAY LIST / CO-PROCESSOR REGISTERS
 * ========================================================================= */

/** @brief Command buffer read pointer */
#define EVE_REG_CMD_READ       EVE_API_SELECT(0x1024e4ul,  0x3020f8ul,  0x3020f8ul,  0x3020f8ul,  0x7f00614cul)

/** @brief Command buffer write pointer */
#define EVE_REG_CMD_WRITE      EVE_API_SELECT(0x1024e8ul,  0x3020fcul,  0x3020fcul,  0x3020fcul,  0x7f006150ul)

/** @brief Command buffer DL offset */
#define EVE_REG_CMD_DL         EVE_API_SELECT(0x1024ecul,  0x302100ul,  0x302100ul,  0x302100ul,  0x7f006154ul)

/** @brief Co-processor bulk write port (CMDB method)
 *  @note Not present on EVE1. */
#define EVE_REG_CMDB_WRITE     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302578ul,  0x302578ul,  0x302578ul,  0x7f010000ul)

/** @brief Co-processor bulk space available
 *  @note Not present on EVE1. */
#define EVE_REG_CMDB_SPACE     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302574ul,  0x302574ul,  0x302574ul,  0x7f006594ul)

/* =========================================================================
 * TRACKING REGISTERS
 * ========================================================================= */

/** @brief Tracker register 0 */
#define EVE_REG_TRACKER        EVE_API_SELECT(0x109000ul,  0x309000ul,  0x309000ul,  0x309000ul,  0x7f004000ul)

/** @brief Tracker register 1
 *  @note Not present on EVE1. */
#define EVE_REG_TRACKER_1      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x309004ul,  0x309004ul,  0x309004ul,  0x7f004004ul)

/** @brief Tracker register 2
 *  @note Not present on EVE1. */
#define EVE_REG_TRACKER_2      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x309008ul,  0x309008ul,  0x309008ul,  0x7f004008ul)

/** @brief Tracker register 3
 *  @note Not present on EVE1. */
#define EVE_REG_TRACKER_3      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x30900cul,  0x30900cul,  0x30900cul,  0x7f00400cul)

/** @brief Tracker register 4
 *  @note Not present on EVE1. */
#define EVE_REG_TRACKER_4      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x309010ul,  0x309010ul,  0x309010ul,  0x7f004010ul)

/* =========================================================================
 * PWM (BACKLIGHT) REGISTERS
 * ========================================================================= */

/** @brief PWM output frequency (Hz) */
#define EVE_REG_PWM_HZ         EVE_API_SELECT(0x1024c0ul,  0x3020d0ul,  0x3020d0ul,  0x3020d0ul,  0x7f006128ul)

/** @brief PWM output duty cycle (0 = 0%, 128 = 100%) */
#define EVE_REG_PWM_DUTY       EVE_API_SELECT(0x1024c4ul,  0x3020d4ul,  0x3020d4ul,  0x3020d4ul,  0x7f00612cul)

/* =========================================================================
 * INTERRUPT REGISTERS
 * ========================================================================= */

/** @brief Interrupt flags (read to clear) */
#define EVE_REG_INT_FLAGS      EVE_API_SELECT(0x102498ul,  0x3020a8ul,  0x3020a8ul,  0x3020a8ul,  0x7f006100ul)

/** @brief Global interrupt enable */
#define EVE_REG_INT_EN         EVE_API_SELECT(0x10249cul,  0x3020acul,  0x3020acul,  0x3020acul,  0x7f006104ul)

/** @brief Interrupt enable mask */
#define EVE_REG_INT_MASK       EVE_API_SELECT(0x1024a0ul,  0x3020b0ul,  0x3020b0ul,  0x3020b0ul,  0x7f006108ul)

/* =========================================================================
 * CLOCK REGISTERS
 * ========================================================================= */

/** @brief Frame counter since reset */
#define EVE_REG_FRAMES         EVE_API_SELECT(0x102404ul,  0x302004ul,  0x302004ul,  0x302004ul,  0x7f006004ul)

/** @brief Clock cycles since reset */
#define EVE_REG_CLOCK          EVE_API_SELECT(0x102408ul,  0x302008ul,  0x302008ul,  0x302008ul,  0x7f006008ul)

/** @brief Main clock frequency */
#define EVE_REG_FREQUENCY      EVE_API_SELECT(0x10240cul,  0x30200cul,  0x30200cul,  0x30200cul,  0x7f00600cul)

/** @brief Internal oscillator trim
 *  @note Not present on EVE5. */
#define EVE_REG_TRIM           EVE_API_SELECT(0x10256cul,  0x302180ul,  0x302180ul,  0x302180ul,  EVE_REG_NOT_AVAILABLE)

/* =========================================================================
 * MACRO REGISTERS
 * ========================================================================= */

/** @brief Display list macro command 0 */
#define EVE_REG_MACRO_0        EVE_API_SELECT(0x1024c8ul,  0x3020d8ul,  0x3020d8ul,  0x3020d8ul,  0x7f006130ul)

/** @brief Display list macro command 1 */
#define EVE_REG_MACRO_1        EVE_API_SELECT(0x1024ccul,  0x3020dcul,  0x3020dcul,  0x3020dcul,  0x7f006134ul)

/* =========================================================================
 * GPIO REGISTERS
 * ========================================================================= */

/** @brief GPIO read/write */
#define EVE_REG_GPIO           EVE_API_SELECT(0x102490ul,  0x302094ul,  0x302094ul,  0x302094ul,  0x7f0060e0ul)

/** @brief GPIO pin direction (1 = output) */
#define EVE_REG_GPIO_DIR       EVE_API_SELECT(0x10248cul,  0x302090ul,  0x302090ul,  0x302090ul,  0x7f0060dcul)

/** @brief Extended GPIO register
 *  @note Not present on EVE1 or EVE5. */
#define EVE_REG_GPIOX          EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x30209cul,  0x30209cul,  0x30209cul,  EVE_REG_NOT_AVAILABLE)

/** @brief Extended GPIO direction register
 *  @note Not present on EVE1 or EVE5. */
#define EVE_REG_GPIOX_DIR      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302098ul,  0x302098ul,  0x302098ul,  EVE_REG_NOT_AVAILABLE)

/* =========================================================================
 * CONTROL REGISTERS
 * ========================================================================= */

/** @brief Co-processor reset bits */
#define EVE_REG_CPURESET       EVE_API_SELECT(0x10241cul,  0x302020ul,  0x302020ul,  0x302020ul,  0x7f006088ul)

/** @brief SPI data width (0 = 1-bit, 1 = 2-bit, 2 = 4-bit)
 *  @note Not present on EVE1 or EVE5. */
#define EVE_REG_SPI_WIDTH      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302188ul,  0x302188ul,  0x302188ul,  EVE_REG_NOT_AVAILABLE)

/** @brief SPI early transmit
 *  @note Not present on EVE1 or EVE5. */
#define EVE_REG_SPI_EARLY_TX   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x30217cul,  0x30217cul,  0x30217cul,  EVE_REG_NOT_AVAILABLE)

/** @brief ROM font sub-selection
 *  @note Not present on EVE5. */
#define EVE_REG_ROMSUB_SEL     EVE_API_SELECT(0x1024e0ul,  0x3020f0ul,  0x3020f0ul,  0x3020f0ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Render mode
 *  @note Not present on EVE1 or EVE5. */
#define EVE_REG_RENDERMODE     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302010ul,  0x302010ul,  0x302010ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Adaptive frame rate control
 *  @note EVE3/4 only. */
#define EVE_REG_ADAPTIVE_FRAMERATE EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                               0x30257cul,  0x30257cul,  EVE_REG_NOT_AVAILABLE)

/* =========================================================================
 * SCREENSHOT / SNAPSHOT REGISTERS
 * ========================================================================= */

/** @brief Snapshot trigger (write 1)
 *  @note Not present on EVE1 or EVE5. */
#define EVE_REG_SNAPSHOT       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302018ul,  0x302018ul,  0x302018ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Snapshot Y scan line
 *  @note Not present on EVE1 or EVE5. */
#define EVE_REG_SNAPY          EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x302014ul,  0x302014ul,  0x302014ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Snapshot format
 *  @note Not present on EVE1 or EVE5. */
#define EVE_REG_SNAPFORMAT     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, 0x30201cul,  0x30201cul,  0x30201cul,  EVE_REG_NOT_AVAILABLE)

/** @brief Screenshot enable — EVE1 only */
#define EVE_REG_SCREENSHOT_EN  EVE_API_SELECT(0x102410ul,  EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE)

/** @brief Screenshot Y line — EVE1 only */
#define EVE_REG_SCREENSHOT_Y   EVE_API_SELECT(0x102414ul,  EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE)

/** @brief Screenshot start — EVE1 only */
#define EVE_REG_SCREENSHOT_START EVE_API_SELECT(0x102418ul, EVE_REG_NOT_AVAILABLE, \
                                             EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE)

/** @brief Screenshot busy flag — EVE1 only */
#define EVE_REG_SCREENSHOT_BUSY  EVE_API_SELECT(0x1024d8ul, EVE_REG_NOT_AVAILABLE, \
                                             EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE)

/** @brief Screenshot read data — EVE1 only */
#define EVE_REG_SCREENSHOT_READ  EVE_API_SELECT(0x102554ul, EVE_REG_NOT_AVAILABLE, \
                                             EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE)

/* =========================================================================
 * TAP DETECTION REGISTERS
 * EVE1/2/3/4 only — not present on EVE5.
 * ========================================================================= */

/** @brief Tap detection CRC — EVE1/2/3/4 only */
#define EVE_REG_TAP_CRC        EVE_API_SELECT(0x102420ul,  0x302024ul,  0x302024ul,  0x302024ul,  EVE_REG_NOT_AVAILABLE)

/** @brief Tap detection mask — EVE1/2/3/4 only */
#define EVE_REG_TAP_MASK       EVE_API_SELECT(0x102424ul,  0x302028ul,  0x302028ul,  0x302028ul,  EVE_REG_NOT_AVAILABLE)

/* =========================================================================
 * FLASH CONTROLLER REGISTERS
 * Not present on EVE1 or EVE2.
 * ========================================================================= */

/** @brief Detected flash capacity in Mbytes */
#define EVE_REG_FLASH_SIZE     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x309024ul,   0x309024ul,  0x7f004024ul)

/** @brief Flash status (0=INIT 1=DETACHED 2=BASIC 3=FULL) */
#define EVE_REG_FLASH_STATUS   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x3025f0ul,   0x3025f0ul,  0x7f0065d4ul)

/** @brief Flash DTR (Double Transfer Rate) enable — EVE5 only */
#define EVE_REG_FLASH_DTR      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f0065ecul)

/* =========================================================================
 * I2S / AUDIO OUTPUT REGISTERS
 * EVE5 only.
 * ========================================================================= */

/** @brief I2S interface enable — EVE5 only */
#define EVE_REG_I2S_EN         EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f006714ul)

/** @brief I2S sample frequency — EVE5 only */
#define EVE_REG_I2S_FREQ       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f006718ul)

/** @brief I2S adjusted main frequency — EVE5 only */
#define EVE_REG_FREQUENCY_A    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f00671cul)

/** @brief I2S configuration registers — EVE5 only */
#define EVE_REG_I2S_CFG        EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f800800ul)

/** @brief I2S control registers — EVE5 only */
#define EVE_REG_I2S_CTL        EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f800804ul)

/** @brief I2S status — EVE5 only */
#define EVE_REG_I2S_STAT       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f800810ul)

/** @brief I2S pad configuration — EVE5 only */
#define EVE_REG_I2S_PAD_CFG    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f800814ul)

/* =========================================================================
 * RENDER ENGINE REGISTERS
 * EVE5 only — render-to-texture / off-screen rendering.
 * ========================================================================= */

/** @brief RE destination address — EVE5 only */
#define EVE_REG_RE_DEST        EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f006010ul)

/** @brief RE format (bitmap format) — EVE5 only */
#define EVE_REG_RE_FORMAT      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f006014ul)

/** @brief RE rotate control — EVE5 only */
#define EVE_REG_RE_ROTATE      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f006018ul)

/** @brief RE target width in pixels — EVE5 only */
#define EVE_REG_RE_W           EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f00601cul)

/** @brief RE target height in pixels — EVE5 only */
#define EVE_REG_RE_H           EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f006020ul)

/** @brief RE target dither enable — EVE5 only */
#define EVE_REG_RE_DITHER      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f006024ul)

/** @brief RE write path active — EVE5 only */
#define EVE_REG_RE_ACTIVE      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f006028ul)

/** @brief RE render counter — EVE5 only */
#define EVE_REG_RE_RENDERS     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           0x7f00602cul)

/* =========================================================================
 * SWAPCHAIN REGISTERS
 * EVE5 only — hardware triple/quad-buffering.
 * ========================================================================= */

/** @brief Swapchain 0 reset (write to reset) — EVE5 only */
#define EVE_REG_SC0_RESET      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006034ul)
/** @brief Swapchain 0 ring size 1–4 — EVE5 only */
#define EVE_REG_SC0_SIZE       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006038ul)
/** @brief Swapchain 0 pointer 0 — EVE5 only */
#define EVE_REG_SC0_PTR0       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f00603cul)
/** @brief Swapchain 0 pointer 1 — EVE5 only */
#define EVE_REG_SC0_PTR1       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006040ul)
/** @brief Swapchain 0 pointer 2 — EVE5 only */
#define EVE_REG_SC0_PTR2       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006044ul)
/** @brief Swapchain 0 pointer 3 — EVE5 only */
#define EVE_REG_SC0_PTR3       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006048ul)

/** @brief Swapchain 1 reset — EVE5 only */
#define EVE_REG_SC1_RESET      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f00604cul)
/** @brief Swapchain 1 ring size 1–4 — EVE5 only */
#define EVE_REG_SC1_SIZE       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006050ul)
/** @brief Swapchain 1 pointer 0 — EVE5 only */
#define EVE_REG_SC1_PTR0       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006054ul)
/** @brief Swapchain 1 pointer 1 — EVE5 only */
#define EVE_REG_SC1_PTR1       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006058ul)
/** @brief Swapchain 1 pointer 2 — EVE5 only */
#define EVE_REG_SC1_PTR2       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f00605cul)
/** @brief Swapchain 1 pointer 3 — EVE5 only */
#define EVE_REG_SC1_PTR3       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006060ul)
/** @brief Swapchain 1 input valid (read) — EVE5 only */
#define EVE_REG_SC1_I_VALID    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f00607cul)
/** @brief Swapchain 1 input ready (write) — EVE5 only */
#define EVE_REG_SC1_I_READY    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006080ul)
/** @brief Swapchain 1 input pointer (read) — EVE5 only */
#define EVE_REG_SC1_I_PTR      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006084ul)

/** @brief Swapchain 2 reset — EVE5 only */
#define EVE_REG_SC2_RESET      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006064ul)
/** @brief Swapchain 2 ring size — EVE5 only */
#define EVE_REG_SC2_SIZE       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006068ul)
/** @brief Swapchain 2 pointer 0 — EVE5 only */
#define EVE_REG_SC2_PTR0       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f00606cul)
/** @brief Swapchain 2 pointer 1 — EVE5 only */
#define EVE_REG_SC2_PTR1       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006070ul)
/** @brief Swapchain 2 pointer 2 — EVE5 only */
#define EVE_REG_SC2_PTR2       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006074ul)
/** @brief Swapchain 2 pointer 3 — EVE5 only */
#define EVE_REG_SC2_PTR3       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006078ul)
/** @brief Swapchain 2 status — EVE5 only */
#define EVE_REG_SC2_STATUS     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006780ul)
/** @brief Swapchain 2 address — EVE5 only */
#define EVE_REG_SC2_ADDR       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006784ul)

/* =========================================================================
 * SCANOUT REGISTERS
 * EVE5 only — controls pixel output from swapchain to display.
 * ========================================================================= */

/** @brief Scanout 0 enable — EVE5 only */
#define EVE_REG_SO_EN          EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006600ul)

/** @brief Scanout pixel mode — EVE5 only */
#define EVE_REG_SO_MODE        EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f0065f4ul)

/** @brief Scanout 0 source — EVE5 only */
#define EVE_REG_SO_SOURCE      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f0065f8ul)

/** @brief Scanout 0 format — EVE5 only */
#define EVE_REG_SO_FORMAT      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f0065fcul)

/* =========================================================================
 * LVDS REGISTERS
 * EVE5 only — LVDS transmitter / receiver interface.
 * ========================================================================= */

/** @brief LVDSRX enable — EVE5 only */
#define EVE_REG_LVDSRX_CORE_ENABLE  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006670ul)
/** @brief LVDSRX capture enable — EVE5 only */
#define EVE_REG_LVDSRX_CORE_CAPTURE EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006674ul)
/** @brief LVDSRX pixel setup — EVE5 only */
#define EVE_REG_LVDSRX_CORE_SETUP   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006678ul)
/** @brief LVDSRX destination frame address — EVE5 only */
#define EVE_REG_LVDSRX_CORE_DEST    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f00667cul)
/** @brief LVDSRX output pixel format — EVE5 only */
#define EVE_REG_LVDSRX_CORE_FORMAT  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006680ul)
/** @brief LVDSRX dither enable — EVE5 only */
#define EVE_REG_LVDSRX_CORE_DITHER  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006684ul)
/** @brief LVDSRX frame counter — EVE5 only */
#define EVE_REG_LVDSRX_CORE_FRAMES  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006698ul)
/** @brief LVDSRX system set-up — EVE5 only */
#define EVE_REG_LVDSRX_SETUP        EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f800500ul)
/** @brief LVDSRX analog block config — EVE5 only */
#define EVE_REG_LVDSRX_CTRL         EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f800504ul)
/** @brief LVDSRX status — EVE5 only */
#define EVE_REG_LVDSRX_STAT         EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f800508ul)
/** @brief LVDSTX enables — EVE5 only */
#define EVE_REG_LVDSTX_EN           EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f800300ul)
/** @brief LVDSTX PLL and clock config — EVE5 only */
#define EVE_REG_LVDSTX_PLLCFG       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f800304ul)
/** @brief LVDSTX channel 0 control — EVE5 only */
#define EVE_REG_LVDSTX_CTRL_CH0     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f800314ul)
/** @brief LVDSTX channel 1 control — EVE5 only */
#define EVE_REG_LVDSTX_CTRL_CH1     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f800318ul)
/** @brief LVDSTX status — EVE5 only */
#define EVE_REG_LVDSTX_STAT         EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f80031cul)
/** @brief LVDSTX error status — EVE5 only */
#define EVE_REG_LVDSTX_ERR_STAT     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                                EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f800320ul)

/* =========================================================================
 * EXTENT / WIDGET REGISTERS
 * EVE5 only — bounding box of the last widget rendered.
 * ========================================================================= */

/** @brief Previous widget extent X0 — EVE5 only */
#define EVE_REG_EXTENT_X0      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f00403cul)
/** @brief Previous widget extent Y0 — EVE5 only */
#define EVE_REG_EXTENT_Y0      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f004040ul)
/** @brief Previous widget extent X1 — EVE5 only */
#define EVE_REG_EXTENT_X1      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f004044ul)
/** @brief Previous widget extent Y1 — EVE5 only */
#define EVE_REG_EXTENT_Y1      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f004048ul)

/** @brief Object complete register (used with OPT_COMPLETEREG) — EVE5 only */
#define EVE_REG_OBJECT_COMPLETE EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                            EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f004038ul)

/* =========================================================================
 * EVE5 SYSTEM CONTROL REGISTERS
 * ========================================================================= */

/** @brief Boot control byte — EVE5 only */
#define EVE_REG_BOOT_CONTROL   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f006628ul)
/** @brief Miscellaneous system configuration — EVE5 only */
#define EVE_REG_SYS_CFG        EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f800420ul)
/** @brief System status — EVE5 only */
#define EVE_REG_SYS_STAT       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f800424ul)
/** @brief CHIP_ID info — EVE5 only */
#define EVE_REG_CHIP_ID        EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f800448ul)
/** @brief EVE boot status — EVE5 only */
#define EVE_REG_BOOT_STATUS    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f80044cul)
/** @brief DDR DRAM type setting — EVE5 only */
#define EVE_REG_DDR_TYPE       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, \
                                           EVE_REG_NOT_AVAILABLE, EVE_REG_NOT_AVAILABLE, 0x7f800454ul)

#endif /* _EVE_REGISTERS_H_ */
