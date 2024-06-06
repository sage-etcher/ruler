#include "color.h"


hex_value
extract_hex (color hex, color bit_mask, uint_fast8_t bit_offset)
{
    return ((hex & bit_mask) >> bit_offset); 
}


color
insert_hex (color hex, hex_value value, color bitmask, uint_fast8_t bit_offset)
{
    return ((hex & (~bitmask)) | ((value << bit_offset) & bitmask));
}


