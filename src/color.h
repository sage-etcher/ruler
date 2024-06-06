#ifndef COLOR_H
#define COLOR_H
#include "extern-c-helper.h"
EXTERN_C_START

/* c standard libraries */
#include <stdint.h>

/* RGB hex colors must have atleast 24bits */
typedef uint_fast8_t hex_value;
typedef uint_fast32_t color;

/* and off any extra bits */
#define HEX_MASK 0xFFFFFF
#define CLEAN_HEX(_x) ((_x) & HEX_MASK)

/* getter + setter */
#define HEX_R_OFFSET 0x10
#define HEX_G_OFFSET 0x08
#define HEX_B_OFFSET 0x00

#define HEX_R_MASK 0xFF << HEX_R_OFFSET
#define HEX_G_MASK 0xFF << HEX_G_OFFSET
#define HEX_B_MASK 0xFF << HEX_B_OFFSET

/* use macros */
#define GET_HEX_R(_v) (extract_hex (_v, HEX_R_MASK, HEX_R_OFFSET))
#define GET_HEX_G(_v) (extract_hex (_v, HEX_G_MASK, HEX_G_OFFSET))
#define GET_HEX_B(_v) (extract_hex (_v, HEX_B_MASK, HEX_B_OFFSET))

#define SET_HEX_R(_v,_x) (_v = insert_hex ((_v),(_x), HEX_R_MASK, HEX_R_OFFSET))
#define SET_HEX_G(_v,_x) (_v = insert_hex ((_v),(_x), HEX_G_MASK, HEX_G_OFFSET))
#define SET_HEX_B(_v,_x) (_v = insert_hex ((_v),(_x), HEX_B_MASK, HEX_B_OFFSET))


hex_value extract_hex (color hex, color bit_mask, uint_fast8_t bit_offset);
color     insert_hex  (color hex, hex_value value, color bitmask, uint_fast8_t bit_offset);


EXTERN_C_END
#endif /* COLOR_H */

/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
