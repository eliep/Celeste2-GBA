#ifndef PICO8_MATH_H
#define PICO8_MATH_H

#include <gba_base.h>

#define max2(X,Y) (((X) > (Y)) ? (X) : (Y))
#define max(X) (((X) > 0) ? (X) : 0)
#define min2(X,Y) (((X) < (Y)) ? (X) : (Y))
#define min(X) (((X) < 0) ? (X) : 0)

// returns -1 or 1 as int
#define sgn(num) (((num) >= 0) * 2 - 1)

#define abs_(num) ((num) >=0 ? (num) : -(num))

//signed 16.16
typedef s32 FIXED_16;
#define FIXED_SHIFT (16)
#define int2Fixed(x) ((FIXED_16)((x)<<FIXED_SHIFT))
#define float2Fixed(x) ((FIXED_16)((x)*(1<<FIXED_SHIFT)))
#define fixed2Float(x) (((float)(x))/(1<<FIXED_SHIFT))
#define fixed2Int(x) ((int)((x)>>FIXED_SHIFT))
// y is taken as only half as precise as x
#define mulFixed(x,y) ((FIXED_16)(((x)*(y>>(FIXED_SHIFT/2)))>>(FIXED_SHIFT/2)))
// x is taken as only half as precise as y
#define divFixed(x,y) ((FIXED_16)((((x)<<(FIXED_SHIFT/2))/(y))<<(FIXED_SHIFT/2)))
//get the decimal parts of a FIXED_16.
#define getDecimals(x) ((x)&((1<<FIXED_SHIFT)-1))
// don't have special type for FIXED_8, as it's only needed once for affine transformation
#define fixed16tofixed8(x) ((s16)(((x)>>8)&(0xFFFF)))

FIXED_16 sin_fixed(FIXED_16 x);
FIXED_16 cos_fixed(FIXED_16 x);
int mod_132(int x);
int mod_10(int x);

ARM_CODE inline unsigned positive_modulo(int n, unsigned m){
    //return (n % m) + (m * (n < 0));
    while(n<0) n+=m;
    while(n>=m) n-=m;
    return n;
}

ARM_CODE int rnd(int x);

#endif