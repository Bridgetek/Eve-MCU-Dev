/**
 * @file EVE_commands.h
 * @brief Common definitions for EVE command options.
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
 */

#ifndef _EVE_COMMANDS_H_
#define _EVE_COMMANDS_H_


#ifndef EVE_API
#error "EVE_commands.h requires FT8xx.h to be included first (defines EVE_API)."
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

/* Sentinel for a register / constant absent on a given generation.
 * Value 0 is safe for register addresses (never a valid EVE address).
 * For CMD opcodes, guard usage sites with IS_EVE_API rather than relying
 * on this value. */
#ifndef EVE_REG_NOT_AVAILABLE
#define EVE_REG_NOT_AVAILABLE  0ul
#endif

/* =========================================================================
 * BITMAP ADDRESS MASK
 * Different address widths per generation.
 *   EVE1   (FT800/801): 20-bit  (0xFFFFF)
 *   EVE2   (FT810+)   : 22-bit  (0x3FFFFF)
 *   EVE3/4 (BT815+)   : 23-bit  (0x7FFFFF)
 *   EVE5   (BT820)    : 24-bit low word; upper bits via BITMAP_SOURCE_H
 * ========================================================================= */
#define EVE_ENC_BITMAP_ADDR_MASK \
    EVE_API_SELECT(0xFFFFFul, 0x3FFFFFul, 0x7FFFFFul, 0x7FFFFFul, 0xFFFFFFul)

/* =========================================================================
 * DISPLAY LIST ENCODING MACROS — COMMON TO ALL FIVE GENERATIONS
 * ========================================================================= */

#define EVE_ENC_ALPHA_FUNC(func,ref)        ((0x09ul<<24)|(((func)&0x7ul)<<8)|(((ref)&0xfful)<<0))
#define EVE_ENC_BEGIN(prim)                 ((0x1ful<<24)|(((prim)&0xful)<<0))

/** Bitmap handle.  EVE1-4: 5-bit field;  EVE5: 6-bit field. */
#define EVE_ENC_BITMAP_HANDLE(handle) \
    ((0x05ul<<24)|(((uint32_t)(handle) & EVE_API_SELECT(0x1ful,0x1ful,0x1ful,0x1ful,0x3ful))<<0))

#define EVE_ENC_BITMAP_LAYOUT(format,linestride,height) \
    ((0x07ul<<24)|(((format)&0x1ful)<<19)|(((linestride)&0x3fful)<<9)|(((height)&0x1fful)<<0))

#define EVE_ENC_BITMAP_SIZE(filter,wrapx,wrapy,width,height) \
    ((0x08ul<<24)|(((filter)&0x1ul)<<20)|(((wrapx)&0x1ul)<<19)|(((wrapy)&0x1ul)<<18)|\
     (((width)&0x1fful)<<9)|(((height)&0x1fful)<<0))

/** Bitmap source address (low 24 bits).  Use BITMAP_SOURCE_H on EVE5 for upper bits. */
#define EVE_ENC_BITMAP_SOURCE(addr)         ((0x01ul<<24)|((uint32_t)(addr)&EVE_ENC_BITMAP_ADDR_MASK))

#define EVE_ENC_BITMAP_TRANSFORM_C(c)       ((0x17ul<<24)|((((uint32_t)(c))&0xfffffful)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_F(f)       ((0x1aul<<24)|((((uint32_t)(f))&0xfffffful)<<0))
#define EVE_ENC_BLEND_FUNC(src,dst)         ((0x0bul<<24)|(((src)&0x7ul)<<3)|(((dst)&0x7ul)<<0))
#define EVE_ENC_CALL(dest)                  ((0x1dul<<24)|(((dest)&0xfffful)<<0))
#define EVE_ENC_CELL(cell)                  ((0x06ul<<24)|(((cell)&0x7ful)<<0))
#define EVE_ENC_CLEAR(c,s,t)                ((0x26ul<<24)|((((uint32_t)(c))&0x1ul)<<2)|((((uint32_t)(s))&0x1ul)<<1)|((((uint32_t)(t))&0x1ul)<<0))
#define EVE_ENC_CLEAR_COLOR(c)              ((0x02ul<<24)|(((uint32_t)(c))&0x00fffffful))
#define EVE_ENC_CLEAR_COLOR_A(alpha)        ((0x0ful<<24)|(((alpha)&0xfful)<<0))
#define EVE_ENC_CLEAR_COLOR_RGB(r,g,b)      ((0x02ul<<24)|(((r)&0xfful)<<16)|(((g)&0xfful)<<8)|(((b)&0xfful)<<0))
#define EVE_ENC_CLEAR_STENCIL(s)            ((0x11ul<<24)|((((uint32_t)(s))&0xfful)<<0))
#define EVE_ENC_CLEAR_TAG(s)                ((0x12ul<<24)|((((uint32_t)(s))&0xfful)<<0))
#define EVE_ENC_COLOR(c)                    ((0x04ul<<24)|(((uint32_t)(c))&0x00fffffful))
#define EVE_ENC_COLOR_A(alpha)              ((0x10ul<<24)|(((alpha)&0xfful)<<0))
#define EVE_ENC_COLOR_MASK(r,g,b,a)         ((0x20ul<<24)|((((uint32_t)(r))&0x1ul)<<3)|((((uint32_t)(g))&0x1ul)<<2)|((((uint32_t)(b))&0x1ul)<<1)|((((uint32_t)(a))&0x1ul)<<0))
#define EVE_ENC_COLOR_RGB(r,g,b)            ((0x04ul<<24)|(((r)&0xfful)<<16)|(((g)&0xfful)<<8)|(((b)&0xfful)<<0))
#define EVE_ENC_DISPLAY()                   ((0x00ul<<24))
#define EVE_ENC_END()                       ((0x21ul<<24))
#define EVE_ENC_JUMP(dest)                  ((0x1eul<<24)|(((dest)&0xfffful)<<0))
#define EVE_ENC_LINE_WIDTH(width)           ((0x0eul<<24)|(((width)&0xffful)<<0))
#define EVE_ENC_MACRO(m)                    ((0x25ul<<24)|((((uint32_t)(m))&0x1ul)<<0))
#define EVE_ENC_POINT_SIZE(size)            ((0x0dul<<24)|(((size)&0x1ffful)<<0))
#define EVE_ENC_RESTORE_CONTEXT()           ((0x23ul<<24))
#define EVE_ENC_RETURN()                    ((0x24ul<<24))
#define EVE_ENC_SAVE_CONTEXT()              ((0x22ul<<24))
#define EVE_ENC_STENCIL_FUNC(func,ref,mask) ((0x0aul<<24)|(((func)&0x7ul)<<16)|(((ref)&0xfful)<<8)|(((mask)&0xfful)<<0))
#define EVE_ENC_STENCIL_MASK(mask)          ((0x13ul<<24)|(((mask)&0xfful)<<0))
#define EVE_ENC_STENCIL_OP(sfail,spass)     ((0x0cul<<24)|(((sfail)&0x7ul)<<3)|(((spass)&0x7ul)<<0))

/** TAG value.  EVE1-4: 8-bit field;  EVE5: 24-bit field. */
#define EVE_ENC_TAG(s) \
    ((0x03ul<<24)|(((uint32_t)(s) & EVE_API_SELECT(0xfful,0xfful,0xfful,0xfful,0xfffffful))<<0))

#define EVE_ENC_TAG_MASK(mask)              ((0x14ul<<24)|(((mask)&0x1ul)<<0))
#define EVE_ENC_VERTEX2F(x,y)               ((0x01ul<<30)|((((uint32_t)(x))&0x7ffful)<<15)|((((uint32_t)(y))&0x7ffful)<<0))
#define EVE_ENC_VERTEX2II(x,y,handle,cell)  ((0x02ul<<30)|((((uint32_t)(x))&0x1fful)<<21)|((((uint32_t)(y))&0x1fful)<<12)|((((uint32_t)(handle))&0x1ful)<<7)|((((uint32_t)(cell))&0x7ful)<<0))

/* =========================================================================
 * DL MACROS WITH DIFFERENT BIT-FIELD LAYOUT BETWEEN GENERATIONS
 * The opcode is the same but field widths / positions differ.
 * EVE_API is a compile-time constant, so the dead branch is eliminated.
 * ========================================================================= */

/** Scissor clip size.  EVE1: 10-bit w/h at bits 10/0.  EVE2+: 12-bit at 12/0. */
#define EVE_ENC_SCISSOR_SIZE(width,height) \
    ((EVE_API==1) ? \
        ((0x1cul<<24)|(((uint32_t)(width)&0x3fful)<<10)|(((uint32_t)(height))&0x3fful)) : \
        ((0x1cul<<24)|(((uint32_t)(width)&0xffful)<<12)|(((uint32_t)(height))&0xffful)))

/** Scissor clip position.  EVE1: 9-bit x/y at bits 9/0.  EVE2+: 11-bit at 11/0. */
#define EVE_ENC_SCISSOR_XY(x,y) \
    ((EVE_API==1) ? \
        ((0x1bul<<24)|(((uint32_t)(x)&0x1fful)<<9)|(((uint32_t)(y))&0x1fful)) : \
        ((0x1bul<<24)|(((uint32_t)(x)&0x7fful)<<11)|(((uint32_t)(y))&0x7fful)))

/* =========================================================================
 * DL MACROS ADDED IN EVE2 (FT810 / BT880 onwards); not on EVE1
 * ========================================================================= */
#if IS_EVE_API(2,3,4,5)
#define EVE_ENC_BITMAP_LAYOUT_H(linestride,height)  ((0x28ul<<24)|(((linestride)&0x3ul)<<2)|(((height)&0x3ul)<<0))
#define EVE_ENC_BITMAP_SIZE_H(width,height)         ((0x29ul<<24)|(((width)&0x3ul)<<2)|(((height)&0x3ul)<<0))
#define EVE_ENC_NOP()                               ((0x2dul<<24))
#define EVE_ENC_PALETTE_SOURCE(addr)                ((0x2aul<<24)|(((addr)&0x3ffffful)<<0))
#define EVE_ENC_VERTEX_FORMAT(frac)                 ((0x27ul<<24)|(((frac)&0x7ul)<<0))
#define EVE_ENC_VERTEX_TRANSLATE_X(x)               ((0x2bul<<24)|(((uint32_t)(x)&0x1fffful)<<0))
#define EVE_ENC_VERTEX_TRANSLATE_Y(y)               ((0x2cul<<24)|(((uint32_t)(y)&0x1fffful)<<0))
#endif /* IS_EVE_API(2,3,4,5) */

/* =========================================================================
 * DL MACROS ADDED IN EVE3 (BT815/816 onwards)
 * ========================================================================= */
#if IS_EVE_API(3,4,5)

#define EVE_ENC_BITMAP_EXT_FORMAT(format)   ((0x2eul<<24)|(((format)&0xfffful)<<0))

/** Bitmap source with flash-or-RAM selector bit. */
#define EVE_ENC_BITMAP_SOURCE2(flash_or_ram,addr) \
    ((0x01ul<<24)|(((flash_or_ram)&0x1ul)<<23)|(((uint32_t)(addr)&(EVE_ENC_BITMAP_ADDR_MASK>>1))<<0))

#define EVE_ENC_BITMAP_SWIZZLE(r,g,b,a)     ((0x2ful<<24)|(((r)&0x7ul)<<9)|(((g)&0x7ul)<<6)|(((b)&0x7ul)<<3)|(((a)&0x7ul)<<0))

/* Extended bitmap transform variants (precision bit p selects fixed-point range) */
#define EVE_ENC_BITMAP_TRANSFORM_A_EXT(p,v) ((0x15ul<<24)|(((uint32_t)(p)&0x1ul)<<17)|(((uint32_t)(v)&0x1fffful)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_B_EXT(p,v) ((0x16ul<<24)|(((uint32_t)(p)&0x1ul)<<17)|(((uint32_t)(v)&0x1fffful)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_D_EXT(p,v) ((0x18ul<<24)|(((uint32_t)(p)&0x1ul)<<17)|(((uint32_t)(v)&0x1fffful)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_E_EXT(p,v) ((0x19ul<<24)|(((uint32_t)(p)&0x1ul)<<17)|(((uint32_t)(v)&0x1fffful)<<0))

#endif /* IS_EVE_API(3,4,5) */

/* =========================================================================
 * DL MACROS ADDED IN EVE5 (BT820 only)
 * ========================================================================= */
#if IS_EVE_API(5)
/** Upper 8 bits of a > 24-bit bitmap source address. */
#define EVE_ENC_BITMAP_SOURCE_H(addr)       ((0x31ul<<24)|((uint32_t)(addr)&0xfful))
#define EVE_ENC_BITMAP_ZORDER(o)            ((0x33ul<<24)|(((o)&0xfful)<<0))
#define EVE_ENC_PALLETE_SOURCE_H(addr)      ((0x32ul<<24)|((uint32_t)(addr)&0xfful))
#define EVE_ENC_REGION(y,h,dest)            ((0x34ul<<24)|(((y)&0x3ful)<<18)|(((h)&0x3ful)<<12)|(((dest)&0xffful)<<0))
#endif /* IS_EVE_API(5) */

/* =========================================================================
 * BITMAP TRANSFORM A/B/D/E — GENERATION-DEPENDENT ALIAS
 * EVE1/2: simple 17-bit form (no precision bit).
 * EVE3/4/5: alias to _EXT(0, value).  Use _EXT directly for the p=1 range.
 * ========================================================================= */
#if IS_EVE_API(1,2)
#define EVE_ENC_BITMAP_TRANSFORM_A(a)   ((0x15ul<<24)|((((uint32_t)(a))&0x1fffful)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_B(b)   ((0x16ul<<24)|((((uint32_t)(b))&0x1fffful)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_D(d)   ((0x18ul<<24)|((((uint32_t)(d))&0x1fffful)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_E(e)   ((0x19ul<<24)|((((uint32_t)(e))&0x1fffful)<<0))
#else
#define EVE_ENC_BITMAP_TRANSFORM_A(a)   EVE_ENC_BITMAP_TRANSFORM_A_EXT(0,(a))
#define EVE_ENC_BITMAP_TRANSFORM_B(b)   EVE_ENC_BITMAP_TRANSFORM_B_EXT(0,(b))
#define EVE_ENC_BITMAP_TRANSFORM_D(d)   EVE_ENC_BITMAP_TRANSFORM_D_EXT(0,(d))
#define EVE_ENC_BITMAP_TRANSFORM_E(e)   EVE_ENC_BITMAP_TRANSFORM_E_EXT(0,(e))
#endif /* IS_EVE_API(1,2) */

/* =========================================================================
 * CO-PROCESSOR COMMAND OPCODES
 *
 * All command opcodes follow the pattern 0xFFFFFF00 | byte_value.
 * - Single #define when the same opcode is used on all five generations.
 * - EVE_API_SELECT when the opcode changes between EVE1-4 and EVE5.
 * - IS_EVE_API guards for commands that exist on a subset of generations.
 * ========================================================================= */

/* Universal — identical opcode in all five generations */
#define EVE_ENC_CMD_DLSTART             0xffffff00ul
#define EVE_ENC_CMD_SWAP                0xffffff01ul
#define EVE_ENC_CMD_INTERRUPT           0xffffff02ul

/* Present in all 5 generations, opcode changes in EVE5 */
#define EVE_ENC_CMD_BGCOLOR      EVE_API_SELECT(0xffffff09ul,0xffffff09ul,0xffffff09ul,0xffffff09ul,0xffffff07ul)
#define EVE_ENC_CMD_FGCOLOR      EVE_API_SELECT(0xffffff0aul,0xffffff0aul,0xffffff0aul,0xffffff0aul,0xffffff08ul)
#define EVE_ENC_CMD_GRADIENT     EVE_API_SELECT(0xffffff0bul,0xffffff0bul,0xffffff0bul,0xffffff0bul,0xffffff09ul)
#define EVE_ENC_CMD_TEXT         EVE_API_SELECT(0xffffff0cul,0xffffff0cul,0xffffff0cul,0xffffff0cul,0xffffff0aul)
#define EVE_ENC_CMD_BUTTON       EVE_API_SELECT(0xffffff0dul,0xffffff0dul,0xffffff0dul,0xffffff0dul,0xffffff0bul)
#define EVE_ENC_CMD_KEYS         EVE_API_SELECT(0xffffff0eul,0xffffff0eul,0xffffff0eul,0xffffff0eul,0xffffff0cul)
#define EVE_ENC_CMD_PROGRESS     EVE_API_SELECT(0xffffff0ful,0xffffff0ful,0xffffff0ful,0xffffff0ful,0xffffff0dul)
#define EVE_ENC_CMD_SLIDER       EVE_API_SELECT(0xffffff10ul,0xffffff10ul,0xffffff10ul,0xffffff10ul,0xffffff0eul)
#define EVE_ENC_CMD_SCROLLBAR    EVE_API_SELECT(0xffffff11ul,0xffffff11ul,0xffffff11ul,0xffffff11ul,0xffffff0ful)
#define EVE_ENC_CMD_TOGGLE       EVE_API_SELECT(0xffffff12ul,0xffffff12ul,0xffffff12ul,0xffffff12ul,0xffffff10ul)
#define EVE_ENC_CMD_GAUGE        EVE_API_SELECT(0xffffff13ul,0xffffff13ul,0xffffff13ul,0xffffff13ul,0xffffff11ul)
#define EVE_ENC_CMD_CLOCK        EVE_API_SELECT(0xffffff14ul,0xffffff14ul,0xffffff14ul,0xffffff14ul,0xffffff12ul)
#define EVE_ENC_CMD_CALIBRATE    EVE_API_SELECT(0xffffff15ul,0xffffff15ul,0xffffff15ul,0xffffff15ul,0xffffff13ul)
#define EVE_ENC_CMD_SPINNER      EVE_API_SELECT(0xffffff16ul,0xffffff16ul,0xffffff16ul,0xffffff16ul,0xffffff14ul)
#define EVE_ENC_CMD_STOP         EVE_API_SELECT(0xffffff17ul,0xffffff17ul,0xffffff17ul,0xffffff17ul,0xffffff15ul)
#define EVE_ENC_CMD_MEMCRC       EVE_API_SELECT(0xffffff18ul,0xffffff18ul,0xffffff18ul,0xffffff18ul,0xffffff16ul)
#define EVE_ENC_CMD_REGREAD      EVE_API_SELECT(0xffffff19ul,0xffffff19ul,0xffffff19ul,0xffffff19ul,0xffffff17ul)
#define EVE_ENC_CMD_MEMWRITE     EVE_API_SELECT(0xffffff1aul,0xffffff1aul,0xffffff1aul,0xffffff1aul,0xffffff18ul)
#define EVE_ENC_CMD_MEMSET       EVE_API_SELECT(0xffffff1bul,0xffffff1bul,0xffffff1bul,0xffffff1bul,0xffffff19ul)
#define EVE_ENC_CMD_MEMZERO      EVE_API_SELECT(0xffffff1cul,0xffffff1cul,0xffffff1cul,0xffffff1cul,0xffffff1aul)
#define EVE_ENC_CMD_MEMCPY       EVE_API_SELECT(0xffffff1dul,0xffffff1dul,0xffffff1dul,0xffffff1dul,0xffffff1bul)
#define EVE_ENC_CMD_APPEND       EVE_API_SELECT(0xffffff1eul,0xffffff1eul,0xffffff1eul,0xffffff1eul,0xffffff1cul)
#define EVE_ENC_CMD_SNAPSHOT     EVE_API_SELECT(0xffffff1ful,0xffffff1ful,0xffffff1ful,0xffffff1ful,0xffffff1dul)
#define EVE_ENC_CMD_BITMAP_TRANSFORM \
                                 EVE_API_SELECT(0xffffff21ul,0xffffff21ul,0xffffff21ul,0xffffff21ul,0xffffff1ful)
/** Inflate (decompress) data. NOTE: opcode 0x22 in EVE1-4; renumbered to 0x4a in EVE5. */
#define EVE_ENC_CMD_INFLATE      EVE_API_SELECT(0xffffff22ul,0xffffff22ul,0xffffff22ul,0xffffff22ul,0xffffff4aul)
#define EVE_ENC_CMD_GETPTR       EVE_API_SELECT(0xffffff23ul,0xffffff23ul,0xffffff23ul,0xffffff23ul,0xffffff20ul)
#define EVE_ENC_CMD_LOADIMAGE    EVE_API_SELECT(0xffffff24ul,0xffffff24ul,0xffffff24ul,0xffffff24ul,0xffffff21ul)
#define EVE_ENC_CMD_GETPROPS     EVE_API_SELECT(0xffffff25ul,0xffffff25ul,0xffffff25ul,0xffffff25ul,0xffffff22ul)
#define EVE_ENC_CMD_LOADIDENTITY EVE_API_SELECT(0xffffff26ul,0xffffff26ul,0xffffff26ul,0xffffff26ul,0xffffff23ul)
#define EVE_ENC_CMD_TRANSLATE    EVE_API_SELECT(0xffffff27ul,0xffffff27ul,0xffffff27ul,0xffffff27ul,0xffffff24ul)
#define EVE_ENC_CMD_SCALE        EVE_API_SELECT(0xffffff28ul,0xffffff28ul,0xffffff28ul,0xffffff28ul,0xffffff25ul)
#define EVE_ENC_CMD_ROTATE       EVE_API_SELECT(0xffffff29ul,0xffffff29ul,0xffffff29ul,0xffffff29ul,0xffffff26ul)
#define EVE_ENC_CMD_SETMATRIX    EVE_API_SELECT(0xffffff2aul,0xffffff2aul,0xffffff2aul,0xffffff2aul,0xffffff27ul)
/** CMD_SETFONT: opcode 0x2b in EVE1-4; renumbered to 0x36 in EVE5. */
#define EVE_ENC_CMD_SETFONT      EVE_API_SELECT(0xffffff2bul,0xffffff2bul,0xffffff2bul,0xffffff2bul,0xffffff36ul)
#define EVE_ENC_CMD_TRACK        EVE_API_SELECT(0xffffff2cul,0xffffff2cul,0xffffff2cul,0xffffff2cul,0xffffff28ul)
#define EVE_ENC_CMD_DIAL         EVE_API_SELECT(0xffffff2dul,0xffffff2dul,0xffffff2dul,0xffffff2dul,0xffffff29ul)
#define EVE_ENC_CMD_NUMBER       EVE_API_SELECT(0xffffff2eul,0xffffff2eul,0xffffff2eul,0xffffff2eul,0xffffff2aul)
#define EVE_ENC_CMD_SCREENSAVER  EVE_API_SELECT(0xffffff2ful,0xffffff2ful,0xffffff2ful,0xffffff2ful,0xffffff2bul)
#define EVE_ENC_CMD_SKETCH       EVE_API_SELECT(0xffffff30ul,0xffffff30ul,0xffffff30ul,0xffffff30ul,0xffffff2cul)
#define EVE_ENC_CMD_LOGO         EVE_API_SELECT(0xffffff31ul,0xffffff31ul,0xffffff31ul,0xffffff31ul,0xffffff2dul)
#define EVE_ENC_CMD_COLDSTART    EVE_API_SELECT(0xffffff32ul,0xffffff32ul,0xffffff32ul,0xffffff32ul,0xffffff2eul)
#define EVE_ENC_CMD_GETMATRIX    EVE_API_SELECT(0xffffff33ul,0xffffff33ul,0xffffff33ul,0xffffff33ul,0xffffff2ful)
#define EVE_ENC_CMD_GRADCOLOR    EVE_API_SELECT(0xffffff34ul,0xffffff34ul,0xffffff34ul,0xffffff34ul,0xffffff30ul)

/* EVE1 / EVE2 only */
#if IS_EVE_API(1,2)
/** Capacitive-screen sketch command (FT800/FT810 families only). */
#define EVE_ENC_CMD_CSKETCH             0xffffff35ul
#endif

/* Commands added in EVE2; same opcode in EVE2/3/4; different in EVE5 */
#if IS_EVE_API(2,3,4,5)
#define EVE_ENC_CMD_SETROTATE   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,0xffffff36ul,0xffffff36ul,0xffffff36ul,0xffffff31ul)
#define EVE_ENC_CMD_SETBASE     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,0xffffff38ul,0xffffff38ul,0xffffff38ul,0xffffff33ul)
#define EVE_ENC_CMD_MEDIAFIFO   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,0xffffff39ul,0xffffff39ul,0xffffff39ul,0xffffff34ul)
#define EVE_ENC_CMD_PLAYVIDEO   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,0xffffff3aul,0xffffff3aul,0xffffff3aul,0xffffff35ul)
#define EVE_ENC_CMD_SETSCRATCH  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,0xffffff3cul,0xffffff3cul,0xffffff3cul,0xffffff37ul)
#define EVE_ENC_CMD_ROMFONT     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,0xffffff3ful,0xffffff3ful,0xffffff3ful,0xffffff39ul)
#define EVE_ENC_CMD_VIDEOSTART  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,0xffffff40ul,0xffffff40ul,0xffffff40ul,0xffffff3aul)
#define EVE_ENC_CMD_VIDEOFRAME  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,0xffffff41ul,0xffffff41ul,0xffffff41ul,0xffffff3bul)
#define EVE_ENC_CMD_SYNC        EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,0xffffff42ul,0xffffff42ul,0xffffff42ul,0xffffff3cul)
#define EVE_ENC_CMD_SETBITMAP   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,0xffffff43ul,0xffffff43ul,0xffffff43ul,0xffffff3dul)
#endif

/* Commands added in EVE2, present only in EVE2/3/4 (absent in EVE5) */
#if IS_EVE_API(2,3,4)
#define EVE_ENC_CMD_SNAPSHOT2           0xffffff37ul
#define EVE_ENC_CMD_SETFONT2            0xffffff3bul
#endif

/* Commands added in EVE3; present in EVE3/4 and EVE5 with different opcode */
#if IS_EVE_API(3,4,5)
#define EVE_ENC_CMD_FLASHERASE    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff44ul,0xffffff44ul,0xffffff3eul)
#define EVE_ENC_CMD_FLASHWRITE    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff45ul,0xffffff45ul,0xffffff3ful)
#define EVE_ENC_CMD_FLASHREAD     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff46ul,0xffffff46ul,0xffffff40ul)
#define EVE_ENC_CMD_FLASHUPDATE   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff47ul,0xffffff47ul,0xffffff41ul)
#define EVE_ENC_CMD_FLASHDETACH   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff48ul,0xffffff48ul,0xffffff42ul)
#define EVE_ENC_CMD_FLASHATTACH   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff49ul,0xffffff49ul,0xffffff43ul)
#define EVE_ENC_CMD_FLASHFAST     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff4aul,0xffffff4aul,0xffffff44ul)
#define EVE_ENC_CMD_FLASHSPIDESEL EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff4bul,0xffffff4bul,0xffffff45ul)
#define EVE_ENC_CMD_FLASHSPITX    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff4cul,0xffffff4cul,0xffffff46ul)
#define EVE_ENC_CMD_FLASHSPIRX    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff4dul,0xffffff4dul,0xffffff47ul)
#define EVE_ENC_CMD_FLASHSOURCE   EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff4eul,0xffffff4eul,0xffffff48ul)
#define EVE_ENC_CMD_FLASHPROGRAM  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff70ul,0xffffff70ul,0xffffff64ul)
#define EVE_ENC_CMD_ROTATEAROUND  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff51ul,0xffffff51ul,0xffffff4bul)
#define EVE_ENC_CMD_RESETFONTS    EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff52ul,0xffffff52ul,0xffffff4cul)
#define EVE_ENC_CMD_ANIMSTART     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff53ul,0xffffff53ul,0xffffff5ful)
#define EVE_ENC_CMD_ANIMSTOP      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff54ul,0xffffff54ul,0xffffff4dul)
#define EVE_ENC_CMD_ANIMXY        EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff55ul,0xffffff55ul,0xffffff4eul)
#define EVE_ENC_CMD_ANIMDRAW      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff56ul,0xffffff56ul,0xffffff4ful)
#define EVE_ENC_CMD_GRADIENTA     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff57ul,0xffffff57ul,0xffffff50ul)
#define EVE_ENC_CMD_FILLWIDTH     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff58ul,0xffffff58ul,0xffffff51ul)
#define EVE_ENC_CMD_APPENDF       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff59ul,0xffffff59ul,0xffffff52ul)
#define EVE_ENC_CMD_ANIMFRAME     EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff5aul,0xffffff5aul,0xffffff5eul)
/** Co-processor NOP. */
#define EVE_ENC_CMD_NOP           EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff5bul,0xffffff5bul,0xffffff53ul)
#endif /* IS_EVE_API(3,4,5) */

/* EVE3/4 only (absent in EVE5) */
#if IS_EVE_API(3,4)
#define EVE_ENC_CMD_CLEARCACHE          0xffffff4ful
#define EVE_ENC_CMD_INFLATE2            0xffffff50ul
#define EVE_ENC_CMD_VIDEOSTARTF         0xffffff5ful
#endif

/* Commands added in EVE4; present in EVE4 and EVE5 with different opcodes */
#if IS_EVE_API(4,5)
#define EVE_ENC_CMD_CALIBRATESUB  EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff60ul,0xffffff56ul)
#define EVE_ENC_CMD_TESTCARD      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff61ul,0xffffff57ul)
#define EVE_ENC_CMD_GETIMAGE      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff64ul,0xffffff58ul)
#define EVE_ENC_CMD_WAIT          EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff65ul,0xffffff59ul)
#define EVE_ENC_CMD_RETURN        EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff66ul,0xffffff5aul)
#define EVE_ENC_CMD_CALLLIST      EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff67ul,0xffffff5bul)
#define EVE_ENC_CMD_NEWLIST       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff68ul,0xffffff5cul)
#define EVE_ENC_CMD_ENDLIST       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff69ul,0xffffff5dul)
#define EVE_ENC_CMD_RUNANIM       EVE_API_SELECT(EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,EVE_REG_NOT_AVAILABLE,0xffffff6ful,0xffffff60ul)
#endif

/* EVE4 only */
#if IS_EVE_API(4)
#define EVE_ENC_CMD_ANIMFRAMERAM        0xffffff6dul
#define EVE_ENC_CMD_ANIMSTARTRAM        0xffffff6eul
#define EVE_ENC_CMD_APILEVEL            0xffffff63ul
#define EVE_ENC_CMD_FONTCACHE           0xffffff6bul
#define EVE_ENC_CMD_FONTCACHEQUERY      0xffffff6cul
#define EVE_ENC_CMD_HSF                 0xffffff62ul
#define EVE_ENC_CMD_PCLKFREQ            0xffffff6aul
#endif

/* EVE5 only */
#if IS_EVE_API(5)
#define EVE_ENC_CMD_APBREAD             0xffffff62ul
#define EVE_ENC_CMD_APBWRITE            0xffffff63ul
#define EVE_ENC_CMD_ARC                 0xffffff87ul
#define EVE_ENC_CMD_CGRADIENT           0xffffff8aul
#define EVE_ENC_CMD_COPYLIST            0xffffff88ul
#define EVE_ENC_CMD_DDRSHUTDOWN         0xffffff65ul
#define EVE_ENC_CMD_DDRSTARTUP          0xffffff66ul
#define EVE_ENC_CMD_ENABLEREGION        0xffffff7eul
#define EVE_ENC_CMD_ENDREGION           0xffffff90ul
#define EVE_ENC_CMD_FENCE               0xffffff68ul
#define EVE_ENC_CMD_FSDIR               0xffffff8eul
#define EVE_ENC_CMD_FSOPTIONS           0xffffff6dul
#define EVE_ENC_CMD_FSREAD              0xffffff71ul
#define EVE_ENC_CMD_FSSIZE              0xffffff80ul
#define EVE_ENC_CMD_FSSOURCE            0xffffff7ful
#define EVE_ENC_CMD_GLOW                0xffffff8bul
#define EVE_ENC_CMD_GRAPHICSFINISH      0xffffff6bul
#define EVE_ENC_CMD_I2SSTARTUP          0xffffff69ul
#define EVE_ENC_CMD_LOADASSET           0xffffff81ul
#define EVE_ENC_CMD_LOADPATCH           0xffffff82ul
#define EVE_ENC_CMD_LOADWAV             0xffffff85ul
#define EVE_ENC_CMD_PLAYWAV             0xffffff79ul
#define EVE_ENC_CMD_REGION              0xffffff8ful
#define EVE_ENC_CMD_REGWRITE            0xffffff86ul
#define EVE_ENC_CMD_RENDERTARGET        0xffffff8dul
#define EVE_ENC_CMD_RESTORECONTEXT      0xffffff7dul
#define EVE_ENC_CMD_RESULT              0xffffff89ul
#define EVE_ENC_CMD_SAVECONTEXT         0xffffff7cul
#define EVE_ENC_CMD_SDATTACH            0xffffff6eul
#define EVE_ENC_CMD_SDBLOCKREAD         0xffffff6ful
#define EVE_ENC_CMD_SKIPCOND            0xffffff8cul
#define EVE_ENC_CMD_TEXTDIM             0xffffff84ul
#define EVE_ENC_CMD_WAITCHANGE          0xffffff67ul
#define EVE_ENC_CMD_WAITCOND            0xffffff78ul
#define EVE_ENC_CMD_WATCHDOG            0xffffff83ul
/* Swapchain destination tokens */
#define EVE_SWAPCHAIN_0                 0xffff00fful
#define EVE_SWAPCHAIN_1                 0xffff01fful
#define EVE_SWAPCHAIN_2                 0xffff02fful
#endif /* IS_EVE_API(5) */

/* =========================================================================
 * BITMAP FORMATS
 * ========================================================================= */

/* Common to all generations */
#define EVE_FORMAT_ARGB1555             0x0ul
#define EVE_FORMAT_L1                   0x1ul
#define EVE_FORMAT_L4                   0x2ul
#define EVE_FORMAT_L8                   0x3ul
#define EVE_FORMAT_RGB332               0x4ul
#define EVE_FORMAT_ARGB2                0x5ul
#define EVE_FORMAT_ARGB4                0x6ul
#define EVE_FORMAT_RGB565               0x7ul
#define EVE_FORMAT_PALETTED             0x8ul
#define EVE_FORMAT_TEXT8X8              0x9ul
#define EVE_FORMAT_TEXTVGA              0xaul
#define EVE_FORMAT_BARGRAPH             0xbul

/* Added in EVE2 */
#if IS_EVE_API(2,3,4,5)
#define EVE_FORMAT_PALETTED565          0xeul
#define EVE_FORMAT_PALETTED4444         0xful
#define EVE_FORMAT_PALETTED8            0x10ul
#endif

/* Added in EVE3 */
#if IS_EVE_API(3,4,5)
#define EVE_FORMAT_L2                   0x11ul
/** Marker for extended GL / ASTC formats — use with EVE_ENC_BITMAP_EXT_FORMAT. */
#define EVE_GLFORMAT                    0x1ful
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR   0x93b0ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_5x4_KHR   0x93b1ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_5x5_KHR   0x93b2ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_6x5_KHR   0x93b3ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_6x6_KHR   0x93b4ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_8x5_KHR   0x93b5ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_8x6_KHR   0x93b6ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_8x8_KHR   0x93b7ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x5_KHR  0x93b8ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x6_KHR  0x93b9ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x8_KHR  0x93baul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x10_KHR 0x93bbul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_12x10_KHR 0x93bcul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_12x12_KHR 0x93bdul
#endif

/* Added in EVE5 */
#if IS_EVE_API(5)
#define EVE_FORMAT_RGB8                 0x13ul
#define EVE_FORMAT_ARGB8                0x14ul
#define EVE_FORMAT_PALETTEDARGB8        0x15ul
#define EVE_FORMAT_RGB6                 0x16ul
#define EVE_FORMAT_ARGB6                0x17ul
#define EVE_FORMAT_LA1                  0x18ul
#define EVE_FORMAT_LA2                  0x19ul
#define EVE_FORMAT_LA4                  0x1aul
#define EVE_FORMAT_LA8                  0x1bul
#define EVE_FORMAT_YCBCR                0x1cul
#endif

/* =========================================================================
 * PRIMITIVE BEGIN MODES  (EVE_ENC_BEGIN argument)
 * ========================================================================= */
#define EVE_BEGIN_BITMAPS               0x1ul
#define EVE_BEGIN_POINTS                0x2ul
#define EVE_BEGIN_LINES                 0x3ul
#define EVE_BEGIN_LINE_STRIP            0x4ul
#define EVE_BEGIN_EDGE_STRIP_R          0x5ul
#define EVE_BEGIN_EDGE_STRIP_L          0x6ul
#define EVE_BEGIN_EDGE_STRIP_A          0x7ul
#define EVE_BEGIN_EDGE_STRIP_B          0x8ul
#define EVE_BEGIN_RECTS                 0x9ul

/* =========================================================================
 * TEST / COMPARE FUNCTIONS  (ALPHA_FUNC / STENCIL_FUNC)
 * ========================================================================= */
#define EVE_TEST_NEVER                  0x0ul
#define EVE_TEST_LESS                   0x1ul
#define EVE_TEST_LEQUAL                 0x2ul
#define EVE_TEST_GREATER                0x3ul
#define EVE_TEST_GEQUAL                 0x4ul
#define EVE_TEST_EQUAL                  0x5ul
#define EVE_TEST_NOTEQUAL               0x6ul
#define EVE_TEST_ALWAYS                 0x7ul

/* =========================================================================
 * FILTER MODES  (BITMAP_SIZE filter argument)
 * ========================================================================= */
#define EVE_FILTER_NEAREST              0x0ul
#define EVE_FILTER_BILINEAR             0x1ul

/* =========================================================================
 * WRAP MODES  (BITMAP_SIZE wrapx / wrapy)
 * ========================================================================= */
#define EVE_WRAP_BORDER                 0x0ul
#define EVE_WRAP_REPEAT                 0x1ul

/* =========================================================================
 * BLEND FUNCTIONS  (BLEND_FUNC src / dst)
 * ========================================================================= */
#define EVE_BLEND_ZERO                  0x0ul
#define EVE_BLEND_ONE                   0x1ul
#define EVE_BLEND_SRC_ALPHA             0x2ul
#define EVE_BLEND_DST_ALPHA             0x3ul
#define EVE_BLEND_ONE_MINUS_SRC_ALPHA   0x4ul
#define EVE_BLEND_ONE_MINUS_DST_ALPHA   0x5ul

/* =========================================================================
 * STENCIL OPERATIONS  (STENCIL_OP sfail / spass)
 * ========================================================================= */
#define EVE_STENCIL_ZERO                0x0ul
#define EVE_STENCIL_KEEP                0x1ul
#define EVE_STENCIL_REPLACE             0x2ul
#define EVE_STENCIL_INCR                0x3ul
#define EVE_STENCIL_DECR                0x4ul
#define EVE_STENCIL_INVERT              0x5ul

/* =========================================================================
 * COMMAND OPTIONS (OPT_* widget / command option flags)
 * ========================================================================= */

/* Common to all generations */
#define EVE_OPT_3D                      0x0ul
#define EVE_OPT_CENTER                  0x600ul
#define EVE_OPT_CENTERX                 0x200ul
#define EVE_OPT_CENTERY                 0x400ul
#define EVE_OPT_FLAT                    0x100ul
#define EVE_OPT_FULLSCREEN              0x8ul
#define EVE_OPT_MONO                    0x1ul
#define EVE_OPT_NOBACK                  0x1000ul
#define EVE_OPT_NODL                    0x2ul
#define EVE_OPT_NOHANDS                 0xc000ul
#define EVE_OPT_NOHM                    0x4000ul
#define EVE_OPT_NOPOINTER               0x4000ul
#define EVE_OPT_NOSECS                  0x8000ul
#define EVE_OPT_NOTEAR                  0x4ul
#define EVE_OPT_NOTICKS                 0x2000ul
#define EVE_OPT_RGB565                  0x0ul
#define EVE_OPT_RIGHTX                  0x800ul
#define EVE_OPT_SIGNED                  0x100ul

/* EVE2+ options */
#if IS_EVE_API(2,3,4,5)
#define EVE_OPT_FULLSPEED               0x0ul
#define EVE_OPT_HALFSPEED               0x4ul
#define EVE_OPT_QUARTERSPEED            0x8ul
#define EVE_OPT_MEDIAFIFO               0x10ul
#define EVE_OPT_SOUND                   0x20ul
#endif

/* EVE3+ options */
#if IS_EVE_API(3,4,5)
#define EVE_OPT_DITHER                  0x100ul
#define EVE_OPT_FILL                    0x2000ul
#define EVE_OPT_FLASH                   0x40ul
#define EVE_OPT_FORMAT                  0x1000ul
#define EVE_OPT_OVERLAY                 0x80ul
#endif

/* EVE5-only options */
#if IS_EVE_API(5)
#define EVE_OPT_BASELINE                0x8000ul
#define EVE_OPT_CASESENSITIVE           0x2ul
#define EVE_OPT_COMPLETEREG             0x1000ul
#define EVE_OPT_DIRECT                  0x800ul
#define EVE_OPT_DIRSEP_WIN              0x4ul
#define EVE_OPT_DIRSEP_UNIX             0x8ul
#define EVE_OPT_FS                      0x2000ul
#define EVE_OPT_IS_SD                   0x20ul
#define EVE_OPT_IS_MMC                  0x10ul
#define EVE_OPT_SFNLOWER                0x1ul
#define EVE_OPT_TRUECOLOR               0x200ul
#define EVE_OPT_YCBCR                   0x400ul
#define EVE_OPT_1BIT                    0x0ul
#define EVE_OPT_4BIT                    0x2ul
#define EVE_OPT_TOUCH_100KHZ            0x800ul
#define EVE_OPT_TOUCH_400KHZ            0x0ul
#define EVE_OPT_TOUCH_FOCALTECH         0x1ul
#define EVE_OPT_TOUCH_GOODIX            0x2ul
#define EVE_OPT_TOUCH_AR1021            0x3ul
#define EVE_OPT_TOUCH_ILI2511           0x4ul
#define EVE_OPT_TOUCH_TSC2007           0x5ul
#define EVE_OPT_TOUCH_QUICKSIM          0x8000ul
#endif /* IS_EVE_API(5) */

/* =========================================================================
 * TOUCH MODE  (REG_TOUCH_MODE values)
 * ========================================================================= */
#define EVE_TOUCHMODE_OFF               0x0ul
#define EVE_TOUCHMODE_ONESHOT           0x1ul
#define EVE_TOUCHMODE_FRAME             0x2ul
#define EVE_TOUCHMODE_CONTINUOUS        0x3ul

/* Capacitive multi-touch extended mode */
#define EVE_CTOUCH_MODE_EXTENDED        0x0ul
#define EVE_CTOUCH_MODE_COMPATIBILITY   0x1ul

/* EVE5 conical-gradient shape modes */
#if IS_EVE_API(5)
#define EVE_CGRADIENT_CORNER_ZERO       0x0ul
#define EVE_CGRADIENT_EDGE_ZERO         0x1ul
#endif

/* =========================================================================
 * DISPLAY LIST SWAP OPTIONS  (REG_DLSWAP)
 * ========================================================================= */
#define EVE_DLSWAP_DONE                 0x0ul
#define EVE_DLSWAP_LINE                 0x1ul
#define EVE_DLSWAP_FRAME                0x2ul

/* =========================================================================
 * INTERRUPT FLAG BITS  (REG_INT_FLAGS / REG_INT_MASK)
 * ========================================================================= */
#define EVE_INT_SWAP                    0x1ul
#define EVE_INT_TOUCH                   0x2ul
#define EVE_INT_TAG                     0x4ul
#define EVE_INT_SOUND                   0x8ul
#define EVE_INT_PLAYBACK                0x10ul
#define EVE_INT_CMDEMPTY                0x20ul
#define EVE_INT_CMDFLAG                 0x40ul
#define EVE_INT_CONVCOMPLETE            0x80ul

/* =========================================================================
 * AUDIO PLAYBACK SAMPLE FORMATS  (REG_PLAYBACK_FORMAT)
 * ========================================================================= */
#define EVE_SAMPLES_LINEAR              0x0ul
#define EVE_SAMPLES_ULAW                0x1ul
#define EVE_SAMPLES_ADPCM               0x2ul
#define EVE_SAMPLES_S16                 0x3ul   /* signed 16-bit PCM, EVE3+ */
#define EVE_SAMPLES_S16S                0x4ul   /* signed 16-bit stereo, EVE5 */

/** Aliases for older code using the FT80x-era names. */
#define EVE_LINEAR_SAMPLES              EVE_SAMPLES_LINEAR
#define EVE_ULAW_SAMPLES                EVE_SAMPLES_ULAW
#define EVE_ADPCM_SAMPLES               EVE_SAMPLES_ADPCM

#define EVE_VOL_ZERO                    0x0ul

/* =========================================================================
 * COLOUR CHANNEL IDENTIFIERS
 * ========================================================================= */
#define EVE_RED                         0x2ul
#define EVE_GREEN                       0x3ul
#define EVE_BLUE                        0x4ul
#define EVE_ALPHA                       0x5ul

/* =========================================================================
 * ADC MODE  (REG_TOUCH_ADC_MODE on resistive panels)
 * ========================================================================= */
#define EVE_ADC_SINGLE_ENDED            0x0ul
#define EVE_ADC_DIFFERENTIAL            0x1ul

/* =========================================================================
 * ANIMATION LOOP MODES  (CMD_ANIMSTART loop argument)
 * ========================================================================= */
#define EVE_ANIM_ONCE                   0x0ul
#define EVE_ANIM_LOOP                   0x1ul
#define EVE_ANIM_HOLD                   0x2ul

/* =========================================================================
 * FLASH STATUS  (REG_FLASH_STATUS, EVE3+)
 * ========================================================================= */
#define EVE_FLASH_STATUS_INIT           0x0ul
#define EVE_FLASH_STATUS_DETACHED       0x1ul
#define EVE_FLASH_STATUS_BASIC          0x2ul
#define EVE_FLASH_STATUS_FULL           0x3ul

/* =========================================================================
 * ROM FONT TABLES
 * Provides pixel widths, heights and bitmap formats of built-in ROM fonts.
 * EVE5 extends the table to 34 entries (adds fonts 32, 33, 34).
 * ========================================================================= */
#if IS_EVE_API(1,2,3,4)
#define EVE_ROMFONT_MAX     31
#define EVE_ROMFONT_WIDTHS  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    8,8,8,8,10,13,14,17,24,30, \
    12,16,18,22,28,36}
#define EVE_ROMFONT_HEIGHTS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    8,8,16,16,13,17,20,22,29,38, \
    16,20,25,28,36,49}
#define EVE_ROMFONT_FORMATS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1, \
    EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1, \
    EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4, \
    EVE_FORMAT_L4}
#else /* EVE5 */
#define EVE_ROMFONT_MAX     34
#define EVE_ROMFONT_WIDTHS  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    8,8,8,8,10,13,14,17,24,30, \
    14,15,18,22,28,38,48,62,83}
#define EVE_ROMFONT_HEIGHTS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    8,8,16,16,13,17,20,22,29,38, \
    16,18,22,27,33,46,58,74,98}
#define EVE_ROMFONT_FORMATS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1, \
    EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1, \
    EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4, \
    EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4}
#endif /* IS_EVE_API(1,2,3,4) */

/** Compatibility alias used in some older examples. */
#define FTPOINTS  EVE_BEGIN_POINTS

#endif /* _EVE_COMMANDS_H_ */
