#pragma once
#include "StdAfx.h"

const float _2p23 = 8388608.0f;


void powFastSetTable
(
   unsigned int* const pTable,
   const unsigned int  precision
);

float powFastLookup
(
   const float         val,
   const float         ilog2,
   unsigned int* const pTable,
   const unsigned int  precision
);

#if 0
const int fprecision=4;
unsigned int* powtable = new unsigned int[2<<(fprecision-1)];
float myFastPow(const float         base, const float         val)
{
   const float ilog2 = log(base) * 1.44269504088896;
   /* build float bits */
   const int i = (int)( (val * (_2p23 * ilog2)) + (127.0f * _2p23) );

   /* replace mantissa with lookup */
   const int it = (i & 0xFF800000) | powtable[(i & 0x7FFFFF) >>           /* E */
      (23 - fprecision)];                                                /* F */

   /* convert bits to float */
   return *(const float*)( &it );
}
#endif