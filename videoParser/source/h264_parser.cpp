#include <assert.h>
#include <stdint.h>
#include "defines.h"
#include "AVCodecParser.h"
extern "C" {
#include "h264.h"
#include "get_bits.h"
#include "error.h"
#include "utils.h"
#include "avassert.h"
}

typedef struct H264ParseContext {
	ParseContext pc;
	H264ParamSets ps;
// 	H264DSPContext h264dsp;
// 	H264POCContext poc;
// 	H264SEIContext sei;
	int is_avc;
	int nal_length_size;
	int got_first;
	int picture_structure;
	uint8_t parse_history[6];
	int parse_history_count;
	int parse_last_mb;
	int64_t reference_dts;
	int last_frame_num, last_picture_structure;
} H264ParseContext;

int ff_h2645_extract_rbsp(const uint8_t *src, int length,
	H2645RBSP *rbsp, H2645NAL *nal, int small_padding)
{
	int i, si, di;
	uint8_t *dst;

	nal->skipped_bytes = 0;
#define STARTCODE_TEST                                                  \
        if (i + 2 < length && src[i + 1] == 0 && src[i + 2] <= 3) {     \
            if (src[i + 2] != 3 && src[i + 2] != 0) {                   \
                /* startcode, so we must be past the end */             \
                length = i;                                             \
            }                                                           \
            break;                                                      \
        }
#if HAVE_FAST_UNALIGNED
#define FIND_FIRST_ZERO                                                 \
        if (i > 0 && !src[i])                                           \
            i--;                                                        \
        while (src[i])                                                  \
            i++
#if HAVE_FAST_64BIT
	for (i = 0; i + 1 < length; i += 9) {
		if (!((~AV_RN64(src + i) &
			(AV_RN64(src + i) - 0x0100010001000101ULL)) &
			0x8000800080008080ULL))
			continue;
		FIND_FIRST_ZERO;
		STARTCODE_TEST;
		i -= 7;
	}
#else
	for (i = 0; i + 1 < length; i += 5) {
		if (!((~AV_RN32(src + i) &
			(AV_RN32(src + i) - 0x01000101U)) &
			0x80008080U))
			continue;
		FIND_FIRST_ZERO;
		STARTCODE_TEST;
		i -= 3;
	}
#endif /* HAVE_FAST_64BIT */
#else
	for (i = 0; i + 1 < length; i += 2) {
		if (src[i])
			continue;
		if (i > 0 && src[i - 1] == 0)
			i--;
		STARTCODE_TEST;
	}
#endif /* HAVE_FAST_UNALIGNED */

	if (i >= length - 1 && small_padding) { // no escaped 0
		nal->data =
			nal->raw_data = src;
		nal->size =
			nal->raw_size = length;
		return length;
	}
	else if (i > length)
		i = length;

	nal->rbsp_buffer = &rbsp->rbsp_buffer[rbsp->rbsp_buffer_size];
	dst = nal->rbsp_buffer;

	memcpy(dst, src, i);
	si = di = i;
	while (si + 2 < length) {
		// remove escapes (very rare 1:2^22)
		if (src[si + 2] > 3) {
			dst[di++] = src[si++];
			dst[di++] = src[si++];
		}
		else if (src[si] == 0 && src[si + 1] == 0 && src[si + 2] != 0) {
			if (src[si + 2] == 3) { // escape
				dst[di++] = 0;
				dst[di++] = 0;
				si += 3;

				if (nal->skipped_bytes_pos) {
					nal->skipped_bytes++;
					if (nal->skipped_bytes_pos_size < nal->skipped_bytes) {
						nal->skipped_bytes_pos_size *= 2;
						av_assert0(nal->skipped_bytes_pos_size >= nal->skipped_bytes);
						av_reallocp_array(&nal->skipped_bytes_pos,
							nal->skipped_bytes_pos_size,
							sizeof(*nal->skipped_bytes_pos));
						if (!nal->skipped_bytes_pos) {
							nal->skipped_bytes_pos_size = 0;
							return AVERROR(ENOMEM);
						}
					}
					if (nal->skipped_bytes_pos)
						nal->skipped_bytes_pos[nal->skipped_bytes - 1] = di - 1;
				}
				continue;
			}
			else // next start code
				goto nsc;
		}

		dst[di++] = src[si++];
	}
	while (si < length)
		dst[di++] = src[si++];

nsc:
	memset(dst + di, 0, AV_INPUT_BUFFER_PADDING_SIZE);

	nal->data = dst;
	nal->size = di;
	nal->raw_data = src;
	nal->raw_size = si;
	rbsp->rbsp_buffer_size += si;

	return si;
}

static int h264_find_frame_end(H264ParseContext *p, const uint8_t *buf,int buf_size, void *logctx)
{
	return 0;
// 	int i, j;
// 	uint32_t state;
// 	ParseContext *pc = &p->pc;
// 
// 	int next_avc = p->is_avc ? 0 : buf_size;
// 	state = pc->state;
// 	if (state > 13)
// 		state = 7;
// 
// 	for (i = 0; i < buf_size; i++) {
// 		if (i >= next_avc) {
// 			int nalsize = 0;
// 			i = next_avc;
// 			for (j = 0; j < p->nal_length_size; j++)
// 				nalsize = (nalsize << 8) | buf[i++];
// 			if (nalsize <= 0 || nalsize > buf_size - i) {
// 				return buf_size;
// 			}
// 			next_avc = i + nalsize;
// 			state = 5;
// 		}
// 
// 		if (state == 7) {
// 			if (i < next_avc)
// 				state = 2;
// 		}
// 		else if (state <= 2) {
// 			if (buf[i] == 1)
// 				state ^= 5;            // 2->7, 1->4, 0->5
// 			else if (buf[i])
// 				state = 7;
// 			else
// 				state >>= 1;           // 2->1, 1->0, 0->0
// 		}
// 		else if (state <= 5) {
// 			int nalu_type = buf[i] & 0x1F;
// 			if (nalu_type == H264_NAL_SEI || nalu_type == H264_NAL_SPS ||
// 				nalu_type == H264_NAL_PPS || nalu_type == H264_NAL_AUD) {
// 				if (pc->frame_start_found) {
// 					i++;
// 					goto found;
// 				}
// 			}
// 			else if (nalu_type == H264_NAL_SLICE || nalu_type == H264_NAL_DPA ||
// 				nalu_type == H264_NAL_IDR_SLICE) {
// 				state += 8;
// 				continue;
// 			}
// 			state = 7;
// 		}
// 		else {
// 			unsigned int mb, last_mb = p->parse_last_mb;
// 			GetBitContext gb;
// 			p->parse_history[p->parse_history_count++] = buf[i];
// 
// 			init_get_bits(&gb, p->parse_history, 8 * p->parse_history_count);
// 			mb = get_ue_golomb_long(&gb);
// 			if (get_bits_left(&gb) > 0 || p->parse_history_count > 5) {
// 				p->parse_last_mb = mb;
// 				if (pc->frame_start_found) {
// 					if (mb <= last_mb) {
// 						i -= p->parse_history_count - 1;
// 						p->parse_history_count = 0;
// 						goto found;
// 					}
// 				}
// 				else
// 					pc->frame_start_found = 1;
// 				p->parse_history_count = 0;
// 				state = 7;
// 			}
// 		}
// 	}
// 	pc->state = state;
// 	if (p->is_avc)
// 		return next_avc;
// 	return END_NOT_FOUND;
// 
// found:
// 	pc->state = 7;
// 	pc->frame_start_found = 0;
// 	if (p->is_avc)
// 		return next_avc;
// 	return i - (state & 5);
}

/**
* Parse NAL units of found picture and decode some basic information.
*
* @param s parser context.
* @param avctx codec context.
* @param buf buffer with field/frame data.
* @param buf_size size of the buffer.
*/
static inline int parse_nal_units(H264ParseContext* p, const uint8_t * const buf, int buf_size)
{
	H2645RBSP rbsp = { NULL };
	H2645NAL nal = { NULL };
	int buf_index, next_avc;
	int state = -1, got_reset = 0;
	int ret;

	av_fast_padded_malloc(&rbsp.rbsp_buffer, (unsigned int*)&rbsp.rbsp_buffer_alloc_size, buf_size);
	if (!rbsp.rbsp_buffer)
		return AVERROR(ENOMEM);

	buf_index = 0;
	next_avc = 0;
	for (;;) {
		const SPS *sps;
		int src_length, consumed, nalsize = 0;

		if (buf_index >= next_avc) {
			nalsize = get_nalsize(p->nal_length_size, buf, buf_size, &buf_index);
			if (nalsize < 0)
				break;
			next_avc = buf_index + nalsize;
		}
		else {
			buf_index = find_start_code(buf, buf_size, buf_index, next_avc);
			if (buf_index >= buf_size)
				break;
			if (buf_index >= next_avc)
				continue;
		}
		src_length = next_avc - buf_index;

		state = buf[buf_index];
		switch (state & 0x1f) {
		case H264_NAL_SLICE:
		case H264_NAL_IDR_SLICE:
			// Do not walk the whole buffer just to decode slice header
			if ((state & 0x1f) == H264_NAL_IDR_SLICE || ((state >> 5) & 0x3) == 0) {
				/* IDR or disposable slice
				* No need to decode many bytes because MMCOs shall not be present. */
				if (src_length > 60)
					src_length = 60;
			}
			else {
				/* To decode up to MMCOs */
				if (src_length > 1000)
					src_length = 1000;
			}
			break;
		}
		consumed = ff_h2645_extract_rbsp(buf + buf_index, src_length, &rbsp, &nal, 1);
		if (consumed < 0)
			break;

		buf_index += consumed;

		ret = init_get_bits8(&nal.gb, nal.data, nal.size);
		if (ret < 0)
			goto fail;
		get_bits1(&nal.gb);
		nal.ref_idc = get_bits(&nal.gb, 2);
		nal.type = get_bits(&nal.gb, 5);

		switch (nal.type) {
		case H264_NAL_SPS:
			ff_h264_decode_seq_parameter_set(&nal.gb, &p->ps, 0);
			break;
		case H264_NAL_PPS:
			ff_h264_decode_picture_parameter_set(&nal.gb, &p->ps,
				nal.size_bits);
			break;
		case H264_NAL_SEI:
// 			ff_h264_sei_decode(&p->sei, &nal.gb, &p->ps, avctx);
			break;
		case H264_NAL_IDR_SLICE:
			break;
// 			s->key_frame = 1;

// 			p->poc.prev_frame_num = 0;
// 			p->poc.prev_frame_num_offset = 0;
// 			p->poc.prev_poc_msb =
// 				p->poc.prev_poc_lsb = 0;

			av_freep(&rbsp.rbsp_buffer);
			return 0; /* no need to evaluate the rest */
		}
	}

fail:
	av_freep(&rbsp.rbsp_buffer);

	return -1;
}

static int h264_parse(const uint8_t *buf, int buf_size, VideoInfo * info)
{
	H264ParseContext parseContext;
	int ret = parse_nal_units(&parseContext, buf, buf_size);
	info->codeWidth = 16 * parseContext.ps.sps->mb_width;
	info->codeHeight = 16 * parseContext.ps.sps->mb_height;
	info->width = info->codeWidth - (parseContext.ps.sps->crop_right + parseContext.ps.sps->crop_left);
	info->height = info->codeHeight - (parseContext.ps.sps->crop_top + parseContext.ps.sps->crop_bottom);
	if (info->width <= 0 || info->height <= 0) {
		info->width = info->codeWidth;
		info->height = info->codeHeight;
	}
	return ret;
}

static int h264_split(const uint8_t *buf, int buf_size)
{
    return 0;
}

static void h264_close()
{
}

static int init()
{
    return 0;
}

AVCodecParser ff_h264_parser = {
	{ AV_CODEC_TYPE_H264 },
	init,
	h264_parse,
	h264_close,
	h264_split,
};

// AVCodecParser ff_h264_parser = {
//     .codec_ids      = { AV_CODEC_TYPE_H264 },
//     .parser_init    = init,
//     .parser_parse   = h264_parse,
//     .parser_close   = h264_close,
//     .split          = h264_split,
// };
