#pragma once
#include <stdatomic.h>
#include <stdint.h>

/**
* The buffer is always treated as read-only.
*/
#define BUFFER_FLAG_READONLY      (1 << 0)
/**
* The buffer was av_realloc()ed, so it is reallocatable.
*/
#define BUFFER_FLAG_REALLOCATABLE (1 << 1)

struct AVBuffer {
	uint8_t *data; /**< data described by this buffer */
	int      size; /**< size of data in bytes */

				   /**
				   *  number of existing AVBufferRef instances referring to this buffer
				   */
	atomic_uint refcount;

	/**
	* a callback for freeing the data
	*/
	void(*free)(void *opaque, uint8_t *data);

	/**
	* an opaque pointer, to be used by the freeing callback
	*/
	void *opaque;

	/**
	* A combination of BUFFER_FLAG_*
	*/
	int flags;
};

/**
* A reference counted buffer type. It is opaque and is meant to be used through
* references (AVBufferRef).
*/
typedef struct AVBuffer AVBuffer;

/**
* A reference to a data buffer.
*
* The size of this struct is not a part of the public ABI and it is not meant
* to be allocated directly.
*/
typedef struct AVBufferRef {
	AVBuffer *buffer;

	/**
	* The data buffer. It is considered writable if and only if
	* this is the only reference to the buffer, in which case
	* av_buffer_is_writable() returns 1.
	*/
	uint8_t *data;
	/**
	* Size of data in bytes.
	*/
	int      size;
} AVBufferRef;

/**
* Always treat the buffer as read-only, even when it has only one
* reference.
*/
#define AV_BUFFER_FLAG_READONLY (1 << 0)

/**
* Free a given reference and automatically free the buffer if there are no more
* references to it.
*
* @param buf the reference to be freed. The pointer is set to NULL on return.
*/
void av_buffer_unref(AVBufferRef **buf);

/**
* Default free callback, which calls av_free() on the buffer data.
* This function is meant to be passed to av_buffer_create(), not called
* directly.
*/
void av_buffer_default_free(void *opaque, uint8_t *data);

/**
* Allocate an AVBuffer of the given size using av_malloc().
*
* @return an AVBufferRef of given size or NULL when out of memory
*/
AVBufferRef *av_buffer_alloc(int size);

/**
* Same as av_buffer_alloc(), except the returned buffer will be initialized
* to zero.
*/
AVBufferRef *av_buffer_allocz(int size);

/**
* Create an AVBuffer from an existing array.
*
* If this function is successful, data is owned by the AVBuffer. The caller may
* only access data through the returned AVBufferRef and references derived from
* it.
* If this function fails, data is left untouched.
* @param data   data array
* @param size   size of data in bytes
* @param free   a callback for freeing this buffer's data
* @param opaque parameter to be got for processing or passed to free
* @param flags  a combination of AV_BUFFER_FLAG_*
*
* @return an AVBufferRef referring to data on success, NULL on failure.
*/
AVBufferRef *av_buffer_create(uint8_t *data, int size,
	void(*free)(void *opaque, uint8_t *data),
	void *opaque, int flags);
