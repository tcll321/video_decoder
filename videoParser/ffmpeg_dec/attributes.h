#pragma once

#ifdef __GNUC__
#    define AV_GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > (x) || __GNUC__ == (x) && __GNUC_MINOR__ >= (y))
#    define AV_GCC_VERSION_AT_MOST(x,y)  (__GNUC__ < (x) || __GNUC__ == (x) && __GNUC_MINOR__ <= (y))
#else
#    define AV_GCC_VERSION_AT_LEAST(x,y) 0
#    define AV_GCC_VERSION_AT_MOST(x,y)  0
#endif

#ifndef av_always_inline
#if AV_GCC_VERSION_AT_LEAST(3,1)
#    define av_always_inline __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#    define av_always_inline __forceinline
#else
#    define av_always_inline inline
#endif
#endif

#ifndef av_extern_inline
#if defined(__ICL) && __ICL >= 1210 || defined(__GNUC_STDC_INLINE__)
#    define av_extern_inline extern inline
#else
#    define av_extern_inline inline
#endif
#endif

#if AV_GCC_VERSION_AT_LEAST(3,4)
#    define av_warn_unused_result __attribute__((warn_unused_result))
#else
#    define av_warn_unused_result
#endif

#if AV_GCC_VERSION_AT_LEAST(3,1)
#    define av_noinline __attribute__((noinline))
#elif defined(_MSC_VER)
#    define av_noinline __declspec(noinline)
#else
#    define av_noinline
#endif

#if AV_GCC_VERSION_AT_LEAST(3,1) || defined(__clang__)
#    define av_pure __attribute__((pure))
#else
#    define av_pure
#endif

#if AV_GCC_VERSION_AT_LEAST(2,6) || defined(__clang__)
#    define av_const __attribute__((const))
#else
#    define av_const
#endif

#if AV_GCC_VERSION_AT_LEAST(4,3) || defined(__clang__)
#    define av_cold __attribute__((cold))
#else
#    define av_cold
#endif

#if AV_GCC_VERSION_AT_LEAST(4,1) && !defined(__llvm__)
#    define av_flatten __attribute__((flatten))
#else
#    define av_flatten
#endif

#if AV_GCC_VERSION_AT_LEAST(3,1)
#    define attribute_deprecated __attribute__((deprecated))
#elif defined(_MSC_VER)
#    define attribute_deprecated __declspec(deprecated)
#else
#    define attribute_deprecated
#endif

#if defined(__GNUC__) || defined(__clang__)
#    define av_unused __attribute__((unused))
#else
#    define av_unused
#endif
