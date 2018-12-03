#pragma once

#include "bswap.h"

#   if    defined(AV_RN24) && !defined(AV_RL24)
#       define AV_RL24(p) AV_RN24(p)
#   elif !defined(AV_RN24) &&  defined(AV_RL24)
#       define AV_RN24(p) AV_RL24(p)
#   endif

#   define AV_RN(s, p) (*((const __unaligned uint##s##_t*)(p)))
#   define AV_WN(s, p, v) (*((__unaligned uint##s##_t*)(p)) = (v))

#ifndef AV_RN16
#   define AV_RN16(p) AV_RN(16, p)
#endif

#ifndef AV_RN32
#   define AV_RN32(p) AV_RN(32, p)
#endif

#ifndef AV_RN64
#   define AV_RN64(p) AV_RN(64, p)
#endif

#ifndef AV_WN16
#   define AV_WN16(p, v) AV_WN(16, p, v)
#endif

#ifndef AV_WN32
#   define AV_WN32(p, v) AV_WN(32, p, v)
#endif

#ifndef AV_WN64
#   define AV_WN64(p, v) AV_WN(64, p, v)
#endif

#if AV_HAVE_BIGENDIAN
#   define AV_RB(s, p)    AV_RN##s(p)
#   define AV_WB(s, p, v) AV_WN##s(p, v)
#   define AV_RL(s, p)    av_bswap##s(AV_RN##s(p))
#   define AV_WL(s, p, v) AV_WN##s(p, av_bswap##s(v))
#else
#   define AV_RB(s, p)    av_bswap##s(AV_RN##s(p))
#   define AV_WB(s, p, v) AV_WN##s(p, av_bswap##s(v))
#   define AV_RL(s, p)    AV_RN##s(p)
#   define AV_WL(s, p, v) AV_WN##s(p, v)
#endif

#ifndef AV_RB32
#   define AV_RB32(p)    AV_RB(32, p)
#endif
#ifndef AV_WB32
#   define AV_WB32(p, v) AV_WB(32, p, v)
#endif

#ifndef AV_RL24
#   define AV_RL24(x)                           \
    ((((const uint8_t*)(x))[2] << 16) |         \
     (((const uint8_t*)(x))[1] <<  8) |         \
      ((const uint8_t*)(x))[0])
#endif
#ifndef AV_WL24
#   define AV_WL24(p, d) do {                   \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[2] = (d)>>16;           \
    } while(0)
#endif

#define AV_COPYU(n, d, s) AV_WN##n(d, AV_RN##n(s));

#ifndef AV_COPY16U
#   define AV_COPY16U(d, s) AV_COPYU(16, d, s)
#endif

#ifndef AV_COPY32U
#   define AV_COPY32U(d, s) AV_COPYU(32, d, s)
#endif

#ifndef AV_COPY64U
#   define AV_COPY64U(d, s) AV_COPYU(64, d, s)
#endif
