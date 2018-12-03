#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "avstring.h"


size_t av_strlcpy(char *dst, const char *src, size_t size)
{
	size_t len = 0;
	while (++len < size && *src)
		*dst++ = *src++;
	if (len <= size)
		*dst = 0;
	return len + strlen(src) - 1;
}
