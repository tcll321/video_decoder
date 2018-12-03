#pragma once
#include "attributes.h"

#define av_restrict __restrict

//rounded division & shift
#define RSHIFT(a,b) ((a) > 0 ? ((a) + ((1<<(b))>>1))>>(b) : ((a) + ((1<<(b))>>1)-1)>>(b))
/* assume b>0 */
#define ROUNDED_DIV(a,b) (((a)>0 ? (a) + ((b)>>1) : (a) - ((b)>>1))/(b))
/* Fast a/(1<<b) rounded toward +inf. Assume a>=0 and b>=0 */
#define AV_CEIL_RSHIFT(a,b) (!av_builtin_constant_p(b) ? -((-(a)) >> (b)) \
                                                       : ((a) + (1<<(b)) - 1) >> (b))
/* Backwards compat. */
#define FF_CEIL_RSHIFT AV_CEIL_RSHIFT

#define FFUDIV(a,b) (((a)>0 ?(a):(a)-(b)+1) / (b))
#define FFUMOD(a,b) ((a)-(b)*FFUDIV(a,b))

/**
* Absolute value, Note, INT_MIN / INT64_MIN result in undefined behavior as they
* are not representable as absolute values of their type. This is the same
* as with *abs()
* @see FFNABS()
*/
#define FFABS(a) ((a) >= 0 ? (a) : (-(a)))
#define FFSIGN(a) ((a) > 0 ? 1 : -1)

/**
* Negative Absolute value.
* this works for all integers of all types.
* As with many macros, this evaluates its argument twice, it thus must not have
* a sideeffect, that is FFNABS(x++) has undefined behavior.
*/
#define FFNABS(a) ((a) <= 0 ? (a) : (-(a)))

/**
* Comparator.
* For two numerical expressions x and y, gives 1 if x > y, -1 if x < y, and 0
* if x == y. This is useful for instance in a qsort comparator callback.
* Furthermore, compilers are able to optimize this to branchless code, and
* there is no risk of overflow with signed types.
* As with many macros, this evaluates its argument multiple times, it thus
* must not have a side-effect.
*/
#define FFDIFFSIGN(x,y) (((x)>(y)) - ((x)<(y)))

#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMAX3(a,b,c) FFMAX(FFMAX(a,b),c)
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define FFMIN3(a,b,c) FFMIN(FFMIN(a,b),c)
#define FFSWAP(type,a,b) do{type SWAP_tmp= b; b= a; a= SWAP_tmp;}while(0)
#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))

#define MKTAG(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
#define MKBETAG(a,b,c,d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))

/**
* Clip a signed integer value into the amin-amax range.
* @param a value to clip
* @param amin minimum value of the clip range
* @param amax maximum value of the clip range
* @return clipped value
*/
static av_always_inline av_const int av_clip_c(int a, int amin, int amax)
{
#if defined(HAVE_AV_CONFIG_H) && defined(ASSERT_LEVEL) && ASSERT_LEVEL >= 2
	if (amin > amax) abort();
#endif
	if (a < amin) return amin;
	else if (a > amax) return amax;
	else               return a;
}

#ifndef av_ceil_log2
#   define av_ceil_log2     av_ceil_log2_c
#endif
#ifndef av_clip
#   define av_clip          av_clip_c
#endif
