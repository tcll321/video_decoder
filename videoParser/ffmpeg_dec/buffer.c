#include "buffer.h"
#include "mem.h"

AVBufferRef *av_buffer_create(uint8_t *data, int size,
	void(*free)(void *opaque, uint8_t *data),
	void *opaque, int flags)
{
	AVBufferRef *ref = NULL;
	AVBuffer    *buf = NULL;

	buf = av_mallocz(sizeof(*buf));
	if (!buf)
		return NULL;

	buf->data = data;
	buf->size = size;
	buf->free = free ? free : av_buffer_default_free;
	buf->opaque = opaque;

	atomic_init(&buf->refcount, 1);

	if (flags & AV_BUFFER_FLAG_READONLY)
		buf->flags |= BUFFER_FLAG_READONLY;

	ref = av_mallocz(sizeof(*ref));
	if (!ref) {
		av_freep(&buf);
		return NULL;
	}

	ref->buffer = buf;
	ref->data = data;
	ref->size = size;

	return ref;
}

static void buffer_replace(AVBufferRef **dst, AVBufferRef **src)
{
	AVBuffer *b;

	b = (*dst)->buffer;

	if (src) {
		**dst = **src;
		av_freep(src);
	}
	else
		av_freep(dst);

	if (atomic_fetch_add_explicit(&b->refcount, -1, memory_order_acq_rel) == 1) {
		b->free(b->opaque, b->data);
		av_freep(&b);
	}
}

void av_buffer_unref(AVBufferRef **buf)
{
	if (!buf || !*buf)
		return;

	buffer_replace(buf, NULL);
}

void av_buffer_default_free(void *opaque, uint8_t *data)
{
	av_free(data);
}

AVBufferRef *av_buffer_alloc(int size)
{
	AVBufferRef *ret = NULL;
	uint8_t    *data = NULL;

	data = av_malloc(size);
	if (!data)
		return NULL;

	ret = av_buffer_create(data, size, av_buffer_default_free, NULL, 0);
	if (!ret)
		av_freep(&data);

	return ret;
}

AVBufferRef *av_buffer_allocz(int size)
{
	AVBufferRef *ret = av_buffer_alloc(size);
	if (!ret)
		return NULL;

	memset(ret->data, 0, size);
	return ret;
}
