#pragma once

/**
* Copy the string src to dst, but no more than size - 1 bytes, and
* null-terminate dst.
*
* This function is the same as BSD strlcpy().
*
* @param dst destination buffer
* @param src source string
* @param size size of destination buffer
* @return the length of src
*
* @warning since the return value is the length of src, src absolutely
* _must_ be a properly 0-terminated string, otherwise this will read beyond
* the end of the buffer and possibly crash.
*/
size_t av_strlcpy(char *dst, const char *src, size_t size);
