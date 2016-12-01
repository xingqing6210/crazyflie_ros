#include <stdint.h>

uint16_t single2half(float number)
{
    uint32_t num = *((uint32_t*)&number);
    uint32_t s = num>>31;
    uint32_t e = (num>>23)&0x0FF;
    
    if ((e==255) && (num&0x007fffff))
        return 0x7E00; // NaN
    if (e>(127+15))
        return s?0xFC00:0x7C00;  //+/- inf
    if (e<(127-15))
        return 0; //Do not handle generating subnormalised representation
    
    return (s<<15) | ((e-127+15)<<10) | (((num>>13)&0x3FF)+((num>>12)&0x01));
}
