/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * H.264 common definitions
 */

#ifndef AVCODEC_H264_H
#define AVCODEC_H264_H

#define QP_MAX_NUM (51 + 6*6)           // The maximum supported qp


#include "common.h"
#include "pixfmt.h"
#include "get_bits.h"
#include "intreadwrite.h"
#include "buffer.h"
#include "golomb.h"
#include "pixfmt.h"
#include "mathops.h"

#define MIN_LOG2_MAX_FRAME_NUM    4

#define EXTENDED_SAR       255

#define H264_MAX_PICTURE_COUNT 36

#define MAX_MMCO_COUNT         66

#define MAX_DELAYED_PIC_COUNT  16

static const uint8_t default_scaling4[2][16] = {
	{ 6, 13, 20, 28, 13, 20, 28, 32,
	20, 28, 32, 37, 28, 32, 37, 42 },
	{ 10, 14, 20, 24, 14, 20, 24, 27,
	20, 24, 27, 30, 24, 27, 30, 34 }
};

static const uint8_t default_scaling8[2][64] = {
	{ 6, 10, 13, 16, 18, 23, 25, 27,
	10, 11, 16, 18, 23, 25, 27, 29,
	13, 16, 18, 23, 25, 27, 29, 31,
	16, 18, 23, 25, 27, 29, 31, 33,
	18, 23, 25, 27, 29, 31, 33, 36,
	23, 25, 27, 29, 31, 33, 36, 38,
	25, 27, 29, 31, 33, 36, 38, 40,
	27, 29, 31, 33, 36, 38, 40, 42 },
	{ 9, 13, 15, 17, 19, 21, 22, 24,
	13, 13, 17, 19, 21, 22, 24, 25,
	15, 17, 19, 21, 22, 24, 25, 27,
	17, 19, 21, 22, 24, 25, 27, 28,
	19, 21, 22, 24, 25, 27, 28, 30,
	21, 22, 24, 25, 27, 28, 30, 32,
	22, 24, 25, 27, 28, 30, 32, 33,
	24, 25, 27, 28, 30, 32, 33, 35 }
};

/* maximum number of MBs in the DPB for a given level */
static const int level_max_dpb_mbs[][2] = {
	{ 10, 396 },
	{ 11, 900 },
	{ 12, 2376 },
	{ 13, 2376 },
	{ 20, 2376 },
	{ 21, 4752 },
	{ 22, 8100 },
	{ 30, 8100 },
	{ 31, 18000 },
	{ 32, 20480 },
	{ 40, 32768 },
	{ 41, 32768 },
	{ 42, 34816 },
	{ 50, 110400 },
	{ 51, 184320 },
	{ 52, 184320 },
};

/**
* Rational number (pair of numerator and denominator).
*/
typedef struct AVRational {
	int num; ///< Numerator
	int den; ///< Denominator
} AVRational;

static const AVRational ff_h264_pixel_aspect[17] = {
	{ 0,  1 },
	{ 1,  1 },
	{ 12, 11 },
	{ 10, 11 },
	{ 16, 11 },
	{ 40, 33 },
	{ 24, 11 },
	{ 20, 11 },
	{ 32, 11 },
	{ 80, 33 },
	{ 18, 11 },
	{ 15, 11 },
	{ 64, 33 },
	{ 160, 99 },
	{ 4,  3 },
	{ 3,  2 },
	{ 2,  1 },
};

const uint8_t ff_h264_dequant4_coeff_init[6][3] = {
	{ 10, 13, 16 },
	{ 11, 14, 18 },
	{ 13, 16, 20 },
	{ 14, 18, 23 },
	{ 16, 20, 25 },
	{ 18, 23, 29 },
};

const uint8_t ff_h264_dequant8_coeff_init_scan[16] = {
	0, 3, 4, 3, 3, 1, 5, 1, 4, 5, 2, 5, 3, 1, 5, 1
};

const uint8_t ff_h264_dequant8_coeff_init[6][6] = {
	{ 20, 18, 32, 19, 25, 24 },
	{ 22, 19, 35, 21, 28, 26 },
	{ 26, 23, 42, 24, 33, 31 },
	{ 28, 25, 45, 26, 35, 33 },
	{ 32, 28, 51, 30, 40, 38 },
	{ 36, 32, 58, 34, 46, 43 },
};

const uint8_t ff_h264_quant_rem6[QP_MAX_NUM + 1] = {
	0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2,
	3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5,
	0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2,
	3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5,
	0, 1, 2, 3,
};

const uint8_t ff_h264_quant_div6[QP_MAX_NUM + 1] = {
	0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3,  3,  3,
	3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6,  6,  6,
	7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 10, 10, 10,
	10,10,10,11,11,11,11,11,11,12,12,12,12,12,12,13,13,13, 13, 13, 13,
	14,14,14,14,
};

#define QP(qP, depth) ((qP) + 6 * ((depth) - 8))

#define CHROMA_QP_TABLE_END(d)                                          \
    QP(0,  d), QP(1,  d), QP(2,  d), QP(3,  d), QP(4,  d), QP(5,  d),   \
    QP(6,  d), QP(7,  d), QP(8,  d), QP(9,  d), QP(10, d), QP(11, d),   \
    QP(12, d), QP(13, d), QP(14, d), QP(15, d), QP(16, d), QP(17, d),   \
    QP(18, d), QP(19, d), QP(20, d), QP(21, d), QP(22, d), QP(23, d),   \
    QP(24, d), QP(25, d), QP(26, d), QP(27, d), QP(28, d), QP(29, d),   \
    QP(29, d), QP(30, d), QP(31, d), QP(32, d), QP(32, d), QP(33, d),   \
    QP(34, d), QP(34, d), QP(35, d), QP(35, d), QP(36, d), QP(36, d),   \
    QP(37, d), QP(37, d), QP(37, d), QP(38, d), QP(38, d), QP(38, d),   \
    QP(39, d), QP(39, d), QP(39, d), QP(39, d)

const uint8_t ff_h264_chroma_qp[7][QP_MAX_NUM + 1] = {
	{ CHROMA_QP_TABLE_END(8) },
	{ 0, 1, 2, 3, 4, 5,
	CHROMA_QP_TABLE_END(9) },
	{ 0, 1, 2, 3,  4,  5,
	6, 7, 8, 9, 10, 11,
	CHROMA_QP_TABLE_END(10) },
	{ 0,  1, 2, 3,  4,  5,
	6,  7, 8, 9, 10, 11,
	12,13,14,15, 16, 17,
	CHROMA_QP_TABLE_END(11) },
	{ 0,  1, 2, 3,  4,  5,
	6,  7, 8, 9, 10, 11,
	12,13,14,15, 16, 17,
	18,19,20,21, 22, 23,
	CHROMA_QP_TABLE_END(12) },
	{ 0,  1, 2, 3,  4,  5,
	6,  7, 8, 9, 10, 11,
	12,13,14,15, 16, 17,
	18,19,20,21, 22, 23,
	24,25,26,27, 28, 29,
	CHROMA_QP_TABLE_END(13) },
	{ 0,  1, 2, 3,  4,  5,
	6,  7, 8, 9, 10, 11,
	12,13,14,15, 16, 17,
	18,19,20,21, 22, 23,
	24,25,26,27, 28, 29,
	30,31,32,33, 34, 35,
	CHROMA_QP_TABLE_END(14) },
};

/*
 * Table 7-1 ¨C NAL unit type codes, syntax element categories, and NAL unit type classes in
 * T-REC-H.264-201704
 */
enum {
    H264_NAL_UNSPECIFIED     = 0,
    H264_NAL_SLICE           = 1,
    H264_NAL_DPA             = 2,
    H264_NAL_DPB             = 3,
    H264_NAL_DPC             = 4,
    H264_NAL_IDR_SLICE       = 5,
    H264_NAL_SEI             = 6,
    H264_NAL_SPS             = 7,
    H264_NAL_PPS             = 8,
    H264_NAL_AUD             = 9,
    H264_NAL_END_SEQUENCE    = 10,
    H264_NAL_END_STREAM      = 11,
    H264_NAL_FILLER_DATA     = 12,
    H264_NAL_SPS_EXT         = 13,
    H264_NAL_PREFIX          = 14,
    H264_NAL_SUB_SPS         = 15,
    H264_NAL_DPS             = 16,
    H264_NAL_RESERVED17      = 17,
    H264_NAL_RESERVED18      = 18,
    H264_NAL_AUXILIARY_SLICE = 19,
    H264_NAL_EXTEN_SLICE     = 20,
    H264_NAL_DEPTH_EXTEN_SLICE = 21,
    H264_NAL_RESERVED22      = 22,
    H264_NAL_RESERVED23      = 23,
    H264_NAL_UNSPECIFIED24   = 24,
    H264_NAL_UNSPECIFIED25   = 25,
    H264_NAL_UNSPECIFIED26   = 26,
    H264_NAL_UNSPECIFIED27   = 27,
    H264_NAL_UNSPECIFIED28   = 28,
    H264_NAL_UNSPECIFIED29   = 29,
    H264_NAL_UNSPECIFIED30   = 30,
    H264_NAL_UNSPECIFIED31   = 31,
};


enum {
    // 7.4.2.1.1: seq_parameter_set_id is in [0, 31].
    H264_MAX_SPS_COUNT = 32,
    // 7.4.2.2: pic_parameter_set_id is in [0, 255].
    H264_MAX_PPS_COUNT = 256,

    // A.3: MaxDpbFrames is bounded above by 16.
    H264_MAX_DPB_FRAMES = 16,
    // 7.4.2.1.1: max_num_ref_frames is in [0, MaxDpbFrames], and
    // each reference frame can have two fields.
    H264_MAX_REFS       = 2 * H264_MAX_DPB_FRAMES,

    // 7.4.3.1: modification_of_pic_nums_idc is not equal to 3 at most
    // num_ref_idx_lN_active_minus1 + 1 times (that is, once for each
    // possible reference), then equal to 3 once.
    H264_MAX_RPLM_COUNT = H264_MAX_REFS + 1,

    // 7.4.3.3: in the worst case, we begin with a full short-term
    // reference picture list.  Each picture in turn is moved to the
    // long-term list (type 3) and then discarded from there (type 2).
    // Then, we set the length of the long-term list (type 4), mark
    // the current picture as long-term (type 6) and terminate the
    // process (type 0).
    H264_MAX_MMCO_COUNT = H264_MAX_REFS * 2 + 3,

    // A.2.1, A.2.3: profiles supporting FMO constrain
    // num_slice_groups_minus1 to be in [0, 7].
    H264_MAX_SLICE_GROUPS = 8,

    // E.2.2: cpb_cnt_minus1 is in [0, 31].
    H264_MAX_CPB_CNT = 32,

    // A.3: in table A-1 the highest level allows a MaxFS of 139264.
    H264_MAX_MB_PIC_SIZE = 139264,
    // A.3.1, A.3.2: PicWidthInMbs and PicHeightInMbs are constrained
    // to be not greater than sqrt(MaxFS * 8).  Hence height/width are
    // bounded above by sqrt(139264 * 8) = 1055.5 macroblocks.
    H264_MAX_MB_WIDTH    = 1055,
    H264_MAX_MB_HEIGHT   = 1055,
    H264_MAX_WIDTH       = H264_MAX_MB_WIDTH  * 16,
    H264_MAX_HEIGHT      = H264_MAX_MB_HEIGHT * 16,
};

typedef struct H2645NAL {
	uint8_t *rbsp_buffer;

	int size;
	const uint8_t *data;

	/**
	* Size, in bits, of just the data, excluding the stop bit and any trailing
	* padding. I.e. what HEVC calls SODB.
	*/
	int size_bits;

	int raw_size;
	const uint8_t *raw_data;

	GetBitContext gb;

	/**
	* NAL unit type
	*/
	int type;

	/**
	* HEVC only, nuh_temporal_id_plus_1 - 1
	*/
	int temporal_id;

	int skipped_bytes;
	int skipped_bytes_pos_size;
	int *skipped_bytes_pos;
	/**
	* H.264 only, nal_ref_idc
	*/
	int ref_idc;
} H2645NAL;

typedef struct H2645RBSP {
	uint8_t *rbsp_buffer;
	int rbsp_buffer_alloc_size;
	int rbsp_buffer_size;
} H2645RBSP;

/* an input packet split into unescaped NAL units */
typedef struct H2645Packet {
	H2645NAL *nals;
	H2645RBSP rbsp;
	int nb_nals;
	int nals_allocated;
} H2645Packet;


/**
* Sequence parameter set
*/
typedef struct SPS {
	unsigned int sps_id;
	int profile_idc;
	int level_idc;
	int chroma_format_idc;
	int transform_bypass;              ///< qpprime_y_zero_transform_bypass_flag
	int log2_max_frame_num;            ///< log2_max_frame_num_minus4 + 4
	int poc_type;                      ///< pic_order_cnt_type
	int log2_max_poc_lsb;              ///< log2_max_pic_order_cnt_lsb_minus4
	int delta_pic_order_always_zero_flag;
	int offset_for_non_ref_pic;
	int offset_for_top_to_bottom_field;
	int poc_cycle_length;              ///< num_ref_frames_in_pic_order_cnt_cycle
	int ref_frame_count;               ///< num_ref_frames
	int gaps_in_frame_num_allowed_flag;
	int mb_width;                      ///< pic_width_in_mbs_minus1 + 1
									   ///< (pic_height_in_map_units_minus1 + 1) * (2 - frame_mbs_only_flag)
	int mb_height;
	int frame_mbs_only_flag;
	int mb_aff;                        ///< mb_adaptive_frame_field_flag
	int direct_8x8_inference_flag;
	int crop;                          ///< frame_cropping_flag

									   /* those 4 are already in luma samples */
	unsigned int crop_left;            ///< frame_cropping_rect_left_offset
	unsigned int crop_right;           ///< frame_cropping_rect_right_offset
	unsigned int crop_top;             ///< frame_cropping_rect_top_offset
	unsigned int crop_bottom;          ///< frame_cropping_rect_bottom_offset
	int vui_parameters_present_flag;
	AVRational sar;
	int video_signal_type_present_flag;
	int full_range;
	int colour_description_present_flag;
	enum AVColorPrimaries color_primaries;
	enum AVColorTransferCharacteristic color_trc;
	enum AVColorSpace colorspace;
	int timing_info_present_flag;
	uint32_t num_units_in_tick;
	uint32_t time_scale;
	int fixed_frame_rate_flag;
	short offset_for_ref_frame[256]; // FIXME dyn aloc?
	int bitstream_restriction_flag;
	int num_reorder_frames;
	int scaling_matrix_present;
	uint8_t scaling_matrix4[6][16];
	uint8_t scaling_matrix8[6][64];
	int nal_hrd_parameters_present_flag;
	int vcl_hrd_parameters_present_flag;
	int pic_struct_present_flag;
	int time_offset_length;
	int cpb_cnt;                          ///< See H.264 E.1.2
	int initial_cpb_removal_delay_length; ///< initial_cpb_removal_delay_length_minus1 + 1
	int cpb_removal_delay_length;         ///< cpb_removal_delay_length_minus1 + 1
	int dpb_output_delay_length;          ///< dpb_output_delay_length_minus1 + 1
	int bit_depth_luma;                   ///< bit_depth_luma_minus8 + 8
	int bit_depth_chroma;                 ///< bit_depth_chroma_minus8 + 8
	int residual_color_transform_flag;    ///< residual_colour_transform_flag
	int constraint_set_flags;             ///< constraint_set[0-3]_flag
	uint8_t data[4096];
	size_t data_size;
} SPS;

/**
* Picture parameter set
*/
typedef struct PPS {
	unsigned int sps_id;
	int cabac;                  ///< entropy_coding_mode_flag
	int pic_order_present;      ///< pic_order_present_flag
	int slice_group_count;      ///< num_slice_groups_minus1 + 1
	int mb_slice_group_map_type;
	unsigned int ref_count[2];  ///< num_ref_idx_l0/1_active_minus1 + 1
	int weighted_pred;          ///< weighted_pred_flag
	int weighted_bipred_idc;
	int init_qp;                ///< pic_init_qp_minus26 + 26
	int init_qs;                ///< pic_init_qs_minus26 + 26
	int chroma_qp_index_offset[2];
	int deblocking_filter_parameters_present; ///< deblocking_filter_parameters_present_flag
	int constrained_intra_pred;     ///< constrained_intra_pred_flag
	int redundant_pic_cnt_present;  ///< redundant_pic_cnt_present_flag
	int transform_8x8_mode;         ///< transform_8x8_mode_flag
	uint8_t scaling_matrix4[6][16];
	uint8_t scaling_matrix8[6][64];
	uint8_t chroma_qp_table[2][QP_MAX_NUM + 1];  ///< pre-scaled (with chroma_qp_index_offset) version of qp_table
	int chroma_qp_diff;
	uint8_t data[4096];
	size_t data_size;

	uint32_t dequant4_buffer[6][QP_MAX_NUM + 1][16];
	uint32_t dequant8_buffer[6][QP_MAX_NUM + 1][64];
	uint32_t(*dequant4_coeff[6])[16];
	uint32_t(*dequant8_coeff[6])[64];
} PPS;

#define MAX_SPS_COUNT          32
#define MAX_PPS_COUNT         256
#define MAX_LOG2_MAX_FRAME_NUM    (12 + 4)

typedef struct H264ParamSets {
	AVBufferRef *sps_list[MAX_SPS_COUNT];
	AVBufferRef *pps_list[MAX_PPS_COUNT];

	AVBufferRef *pps_ref;
	AVBufferRef *sps_ref;
	/* currently active parameters sets */
	const PPS *pps;
	const SPS *sps;
} H264ParamSets;

static void remove_pps(H264ParamSets *s, int id)
{
	av_buffer_unref(&s->pps_list[id]);
}

/**
* Decode SPS
*/
int ff_h264_decode_seq_parameter_set(GetBitContext *gb, H264ParamSets *ps, int ignore_truncation);

/**
* Decode PPS
*/
int ff_h264_decode_picture_parameter_set(GetBitContext *gb, H264ParamSets *ps, int bit_length);

const uint8_t *avpriv_find_start_code(const uint8_t *av_restrict p,
	const uint8_t *end,
	uint32_t *av_restrict state)
{
	int i;

	if (p >= end)
		return end;

	for (i = 0; i < 3; i++) {
		uint32_t tmp = *state << 8;
		*state = tmp + *(p++);
		if (tmp == 0x100 || p == end)
			return p;
	}

	while (p < end) {
		if (p[-1] > 1) p += 3;
		else if (p[-2]) p += 2;
		else if (p[-3] | (p[-1] - 1)) p++;
		else {
			p++;
			break;
		}
	}

	p = FFMIN(p, end) - 4;
	*state = AV_RB32(p);

	return p + 4;
}

static inline int find_start_code(const uint8_t *buf, int buf_size,
	int buf_index, int next_avc)
{
	uint32_t state = -1;

	buf_index = avpriv_find_start_code(buf + buf_index, buf + next_avc + 1, &state) - buf - 1;

	return FFMIN(buf_index, buf_size);
}

static inline int get_nalsize(int nal_length_size, const uint8_t *buf,
	int buf_size, int *buf_index)
{
	int i, nalsize = 0;

	if (*buf_index >= buf_size - nal_length_size) {
		// the end of the buffer is reached, refill it
		return -1;
	}

	for (i = 0; i < nal_length_size; i++)
		nalsize = ((unsigned)nalsize << 8) | buf[(*buf_index)++];
	if (nalsize <= 0 || nalsize > buf_size - *buf_index) {
		return -1;
	}
	return nalsize;
}

static inline int decode_hrd_parameters(GetBitContext *gb, SPS *sps)
{
	int cpb_count, i;
	cpb_count = get_ue_golomb_31(gb) + 1;

	if (cpb_count > 32U) {
		return AVERROR_INVALIDDATA;
	}

	get_bits(gb, 4); /* bit_rate_scale */
	get_bits(gb, 4); /* cpb_size_scale */
	for (i = 0; i < cpb_count; i++) {
		get_ue_golomb_long(gb); /* bit_rate_value_minus1 */
		get_ue_golomb_long(gb); /* cpb_size_value_minus1 */
		get_bits1(gb);          /* cbr_flag */
	}
	sps->initial_cpb_removal_delay_length = get_bits(gb, 5) + 1;
	sps->cpb_removal_delay_length = get_bits(gb, 5) + 1;
	sps->dpb_output_delay_length = get_bits(gb, 5) + 1;
	sps->time_offset_length = get_bits(gb, 5);
	sps->cpb_cnt = cpb_count;
	return 0;
}

// #include "pixdesc.c"
// const char *av_color_primaries_name(enum AVColorPrimaries primaries)
// {
// 	return (unsigned)primaries < AVCOL_PRI_NB ?
// 		color_primaries_names[primaries] : NULL;
// }
// 
// const char *av_color_transfer_name(enum AVColorTransferCharacteristic transfer)
// {
// 	return (unsigned)transfer < AVCOL_TRC_NB ?
// 		color_transfer_names[transfer] : NULL;
// }
// 
// const char *av_color_space_name(enum AVColorSpace space)
// {
// 	return (unsigned)space < AVCOL_SPC_NB ?
// 		color_space_names[space] : NULL;
// }

static inline int decode_vui_parameters(GetBitContext *gb, SPS *sps)
{
	int aspect_ratio_info_present_flag;
	unsigned int aspect_ratio_idc;

	aspect_ratio_info_present_flag = get_bits1(gb);

	if (aspect_ratio_info_present_flag) {
		aspect_ratio_idc = get_bits(gb, 8);
		if (aspect_ratio_idc == EXTENDED_SAR) {
			sps->sar.num = get_bits(gb, 16);
			sps->sar.den = get_bits(gb, 16);
		}
		else if (aspect_ratio_idc < FF_ARRAY_ELEMS(ff_h264_pixel_aspect)) {
			sps->sar = ff_h264_pixel_aspect[aspect_ratio_idc];
		}
		else {
			return AVERROR_INVALIDDATA;
		}
	}
	else {
		sps->sar.num =
			sps->sar.den = 0;
	}

	if (get_bits1(gb))      /* overscan_info_present_flag */
		get_bits1(gb);      /* overscan_appropriate_flag */

	sps->video_signal_type_present_flag = get_bits1(gb);
	if (sps->video_signal_type_present_flag) {
		get_bits(gb, 3);                 /* video_format */
		sps->full_range = get_bits1(gb); /* video_full_range_flag */

		sps->colour_description_present_flag = get_bits1(gb);
		if (sps->colour_description_present_flag) {
			sps->color_primaries = AVColorPrimaries(get_bits(gb, 8)); /* colour_primaries */
			sps->color_trc = AVColorTransferCharacteristic(get_bits(gb, 8)); /* transfer_characteristics */
			sps->colorspace = AVColorSpace(get_bits(gb, 8)); /* matrix_coefficients */

											   // Set invalid values to "unspecified"
// 			if (!av_color_primaries_name(sps->color_primaries))
// 				sps->color_primaries = AVCOL_PRI_UNSPECIFIED;
// 			if (!av_color_transfer_name(sps->color_trc))
// 				sps->color_trc = AVCOL_TRC_UNSPECIFIED;
// 			if (!av_color_space_name(sps->colorspace))
// 				sps->colorspace = AVCOL_SPC_UNSPECIFIED;
		}
	}

	/* chroma_location_info_present_flag */
	if (get_bits1(gb)) {
		/* chroma_sample_location_type_top_field */
		get_ue_golomb(gb) + 1;
		get_ue_golomb(gb);  /* chroma_sample_location_type_bottom_field */
	}

	if (show_bits1(gb) && get_bits_left(gb) < 10) {
		return 0;
	}

	sps->timing_info_present_flag = get_bits1(gb);
	if (sps->timing_info_present_flag) {
		unsigned num_units_in_tick = get_bits_long(gb, 32);
		unsigned time_scale = get_bits_long(gb, 32);
		if (!num_units_in_tick || !time_scale) {
			sps->timing_info_present_flag = 0;
		}
		else {
			sps->num_units_in_tick = num_units_in_tick;
			sps->time_scale = time_scale;
		}
		sps->fixed_frame_rate_flag = get_bits1(gb);
	}

	sps->nal_hrd_parameters_present_flag = get_bits1(gb);
	if (sps->nal_hrd_parameters_present_flag)
		if (decode_hrd_parameters(gb, sps) < 0)
			return AVERROR_INVALIDDATA;
	sps->vcl_hrd_parameters_present_flag = get_bits1(gb);
	if (sps->vcl_hrd_parameters_present_flag)
		if (decode_hrd_parameters(gb, sps) < 0)
			return AVERROR_INVALIDDATA;
	if (sps->nal_hrd_parameters_present_flag ||
		sps->vcl_hrd_parameters_present_flag)
		get_bits1(gb);     /* low_delay_hrd_flag */
	sps->pic_struct_present_flag = get_bits1(gb);
	if (!get_bits_left(gb))
		return 0;
	sps->bitstream_restriction_flag = get_bits1(gb);
	if (sps->bitstream_restriction_flag) {
		get_bits1(gb);     /* motion_vectors_over_pic_boundaries_flag */
		get_ue_golomb(gb); /* max_bytes_per_pic_denom */
		get_ue_golomb(gb); /* max_bits_per_mb_denom */
		get_ue_golomb(gb); /* log2_max_mv_length_horizontal */
		get_ue_golomb(gb); /* log2_max_mv_length_vertical */
		sps->num_reorder_frames = get_ue_golomb(gb);
		get_ue_golomb(gb); /*max_dec_frame_buffering*/

		if (get_bits_left(gb) < 0) {
			sps->num_reorder_frames = 0;
			sps->bitstream_restriction_flag = 0;
		}

		if (sps->num_reorder_frames > 16U
			/* max_dec_frame_buffering || max_dec_frame_buffering > 16 */) {
			sps->num_reorder_frames = 16;
			return AVERROR_INVALIDDATA;
		}
	}

	return 0;
}

static int decode_scaling_list(GetBitContext *gb, uint8_t *factors, int size,
	const uint8_t *jvt_list,
	const uint8_t *fallback_list)
{
	int i, last = 8, next = 8;
	const uint8_t *scan = size == 16 ? ff_zigzag_scan : ff_zigzag_direct;
	if (!get_bits1(gb)) /* matrix not written, we use the predicted one */
		memcpy(factors, fallback_list, size * sizeof(uint8_t));
	else
		for (i = 0; i < size; i++) {
			if (next) {
				int v = get_se_golomb(gb);
				if (v < -128 || v > 127) {
					return AVERROR_INVALIDDATA;
				}
				next = (last + v) & 0xff;
			}
			if (!i && !next) { /* matrix not written, we use the preset one */
				memcpy(factors, jvt_list, size * sizeof(uint8_t));
				break;
			}
			last = factors[scan[i]] = next ? next : last;
		}
	return 0;
}

/* returns non zero if the provided SPS scaling matrix has been filled */
static int decode_scaling_matrices(GetBitContext *gb, const SPS *sps,
	const PPS *pps, int is_sps,
	uint8_t(*scaling_matrix4)[16],
	uint8_t(*scaling_matrix8)[64])
{
	int fallback_sps = !is_sps && sps->scaling_matrix_present;
	const uint8_t *fallback[4] = {
		fallback_sps ? sps->scaling_matrix4[0] : default_scaling4[0],
		fallback_sps ? sps->scaling_matrix4[3] : default_scaling4[1],
		fallback_sps ? sps->scaling_matrix8[0] : default_scaling8[0],
		fallback_sps ? sps->scaling_matrix8[3] : default_scaling8[1]
	};
	int ret = 0;
	if (get_bits1(gb)) {
		ret |= decode_scaling_list(gb, scaling_matrix4[0], 16, default_scaling4[0], fallback[0]);        // Intra, Y
		ret |= decode_scaling_list(gb, scaling_matrix4[1], 16, default_scaling4[0], scaling_matrix4[0]); // Intra, Cr
		ret |= decode_scaling_list(gb, scaling_matrix4[2], 16, default_scaling4[0], scaling_matrix4[1]); // Intra, Cb
		ret |= decode_scaling_list(gb, scaling_matrix4[3], 16, default_scaling4[1], fallback[1]);        // Inter, Y
		ret |= decode_scaling_list(gb, scaling_matrix4[4], 16, default_scaling4[1], scaling_matrix4[3]); // Inter, Cr
		ret |= decode_scaling_list(gb, scaling_matrix4[5], 16, default_scaling4[1], scaling_matrix4[4]); // Inter, Cb
		if (is_sps || pps->transform_8x8_mode) {
			ret |= decode_scaling_list(gb, scaling_matrix8[0], 64, default_scaling8[0], fallback[2]); // Intra, Y
			ret |= decode_scaling_list(gb, scaling_matrix8[3], 64, default_scaling8[1], fallback[3]); // Inter, Y
			if (sps->chroma_format_idc == 3) {
				ret |= decode_scaling_list(gb, scaling_matrix8[1], 64, default_scaling8[0], scaling_matrix8[0]); // Intra, Cr
				ret |= decode_scaling_list(gb, scaling_matrix8[4], 64, default_scaling8[1], scaling_matrix8[3]); // Inter, Cr
				ret |= decode_scaling_list(gb, scaling_matrix8[2], 64, default_scaling8[0], scaling_matrix8[1]); // Intra, Cb
				ret |= decode_scaling_list(gb, scaling_matrix8[5], 64, default_scaling8[1], scaling_matrix8[4]); // Inter, Cb
			}
		}
		if (!ret)
			ret = is_sps;
	}

	return ret;
}

static void remove_sps(H264ParamSets *s, int id)
{
#if 0
	int i;
	if (s->sps_list[id]) {
		/* drop all PPS that depend on this SPS */
		for (i = 0; i < FF_ARRAY_ELEMS(s->pps_list); i++)
			if (s->pps_list[i] && ((PPS*)s->pps_list[i]->data)->sps_id == id)
				remove_pps(s, i);
	}
#endif
	av_buffer_unref(&s->sps_list[id]);
}

int inline ff_h264_decode_seq_parameter_set(GetBitContext *gb, H264ParamSets *ps, int ignore_truncation)
{
	AVBufferRef *sps_buf;
	int profile_idc, level_idc, constraint_set_flags = 0;
	unsigned int sps_id;
	int i, log2_max_frame_num_minus4;
	SPS *sps;
	int ret;

	sps_buf = av_buffer_allocz(sizeof(*sps));
	if (!sps_buf)
		return AVERROR(ENOMEM);
	sps = (SPS*)sps_buf->data;

	sps->data_size = gb->buffer_end - gb->buffer;
	if (sps->data_size > sizeof(sps->data)) {
		sps->data_size = sizeof(sps->data);
	}
	memcpy(sps->data, gb->buffer, sps->data_size);

	profile_idc = get_bits(gb, 8);
	constraint_set_flags |= get_bits1(gb) << 0;   // constraint_set0_flag
	constraint_set_flags |= get_bits1(gb) << 1;   // constraint_set1_flag
	constraint_set_flags |= get_bits1(gb) << 2;   // constraint_set2_flag
	constraint_set_flags |= get_bits1(gb) << 3;   // constraint_set3_flag
	constraint_set_flags |= get_bits1(gb) << 4;   // constraint_set4_flag
	constraint_set_flags |= get_bits1(gb) << 5;   // constraint_set5_flag
	skip_bits(gb, 2);                             // reserved_zero_2bits
	level_idc = get_bits(gb, 8);
	sps_id = get_ue_golomb_31(gb);

	if (sps_id >= MAX_SPS_COUNT) {
		goto fail;
	}

	sps->sps_id = sps_id;
	sps->time_offset_length = 24;
	sps->profile_idc = profile_idc;
	sps->constraint_set_flags = constraint_set_flags;
	sps->level_idc = level_idc;
	sps->full_range = -1;

	memset(sps->scaling_matrix4, 16, sizeof(sps->scaling_matrix4));
	memset(sps->scaling_matrix8, 16, sizeof(sps->scaling_matrix8));
	sps->scaling_matrix_present = 0;
	sps->colorspace = AVCOL_SPC_UNSPECIFIED;

	if (sps->profile_idc == 100 ||  // High profile
		sps->profile_idc == 110 ||  // High10 profile
		sps->profile_idc == 122 ||  // High422 profile
		sps->profile_idc == 244 ||  // High444 Predictive profile
		sps->profile_idc == 44 ||  // Cavlc444 profile
		sps->profile_idc == 83 ||  // Scalable Constrained High profile (SVC)
		sps->profile_idc == 86 ||  // Scalable High Intra profile (SVC)
		sps->profile_idc == 118 ||  // Stereo High profile (MVC)
		sps->profile_idc == 128 ||  // Multiview High profile (MVC)
		sps->profile_idc == 138 ||  // Multiview Depth High profile (MVCD)
		sps->profile_idc == 144) {  // old High444 profile
		sps->chroma_format_idc = get_ue_golomb_31(gb);
		if (sps->chroma_format_idc > 3U) {
			goto fail;
		}
		else if (sps->chroma_format_idc == 3) {
			sps->residual_color_transform_flag = get_bits1(gb);
			if (sps->residual_color_transform_flag) {
				goto fail;
			}
		}
		sps->bit_depth_luma = get_ue_golomb(gb) + 8;
		sps->bit_depth_chroma = get_ue_golomb(gb) + 8;
		if (sps->bit_depth_chroma != sps->bit_depth_luma) {
			goto fail;
		}
		if (sps->bit_depth_luma   < 8 || sps->bit_depth_luma   > 14 ||
			sps->bit_depth_chroma < 8 || sps->bit_depth_chroma > 14) {
			goto fail;
		}
		sps->transform_bypass = get_bits1(gb);
		ret = decode_scaling_matrices(gb, sps, NULL, 1,
			sps->scaling_matrix4, sps->scaling_matrix8);
		if (ret < 0)
			goto fail;
		sps->scaling_matrix_present |= ret;
	}
	else {
		sps->chroma_format_idc = 1;
		sps->bit_depth_luma = 8;
		sps->bit_depth_chroma = 8;
	}

	log2_max_frame_num_minus4 = get_ue_golomb(gb);
	if (log2_max_frame_num_minus4 < MIN_LOG2_MAX_FRAME_NUM - 4 ||
		log2_max_frame_num_minus4 > MAX_LOG2_MAX_FRAME_NUM - 4) {
		goto fail;
	}
	sps->log2_max_frame_num = log2_max_frame_num_minus4 + 4;

	sps->poc_type = get_ue_golomb_31(gb);

	if (sps->poc_type == 0) { // FIXME #define
		unsigned t = get_ue_golomb(gb);
		if (t>12) {
			goto fail;
		}
		sps->log2_max_poc_lsb = t + 4;
	}
	else if (sps->poc_type == 1) { // FIXME #define
		sps->delta_pic_order_always_zero_flag = get_bits1(gb);
		sps->offset_for_non_ref_pic = get_se_golomb(gb);
		sps->offset_for_top_to_bottom_field = get_se_golomb(gb);
		sps->poc_cycle_length = get_ue_golomb(gb);

		if ((unsigned)sps->poc_cycle_length >=
			FF_ARRAY_ELEMS(sps->offset_for_ref_frame)) {
			goto fail;
		}

		for (i = 0; i < sps->poc_cycle_length; i++)
			sps->offset_for_ref_frame[i] = get_se_golomb(gb);
	}
	else if (sps->poc_type != 2) {
		goto fail;
	}

	sps->ref_frame_count = get_ue_golomb_31(gb);
// 	if (avctx->codec_tag == MKTAG('S', 'M', 'V', '2'))
// 		sps->ref_frame_count = FFMAX(2, sps->ref_frame_count);
	if (sps->ref_frame_count > MAX_DELAYED_PIC_COUNT) {
		goto fail;
	}
	sps->gaps_in_frame_num_allowed_flag = get_bits1(gb);
	sps->mb_width = get_ue_golomb(gb) + 1;
	sps->mb_height = get_ue_golomb(gb) + 1;

	sps->frame_mbs_only_flag = get_bits1(gb);

	if (sps->mb_height >= INT_MAX / 2U) {
		goto fail;
	}
	sps->mb_height *= 2 - sps->frame_mbs_only_flag;

	if (!sps->frame_mbs_only_flag)
		sps->mb_aff = get_bits1(gb);
	else
		sps->mb_aff = 0;

	if ((unsigned)sps->mb_width >= INT_MAX / 16 ||
		(unsigned)sps->mb_height >= INT_MAX / 16) {
		goto fail;
	}

	sps->direct_8x8_inference_flag = get_bits1(gb);

// #ifndef ALLOW_INTERLACE
// 	if (sps->mb_aff)
// 		av_log(avctx, AV_LOG_ERROR,
// 			"MBAFF support not included; enable it at compile-time.\n");
// #endif
	sps->crop = get_bits1(gb);
	if (sps->crop) {
		unsigned int crop_left = get_ue_golomb(gb);
		unsigned int crop_right = get_ue_golomb(gb);
		unsigned int crop_top = get_ue_golomb(gb);
		unsigned int crop_bottom = get_ue_golomb(gb);
		int width = 16 * sps->mb_width;
		int height = 16 * sps->mb_height;

// 		if (avctx->flags2 & AV_CODEC_FLAG2_IGNORE_CROP) {
// 			av_log(avctx, AV_LOG_DEBUG, "discarding sps cropping, original "
// 				"values are l:%d r:%d t:%d b:%d\n",
// 				crop_left, crop_right, crop_top, crop_bottom);
// 
// 			sps->crop_left =
// 				sps->crop_right =
// 				sps->crop_top =
// 				sps->crop_bottom = 0;
// 		}
		/*else*/ {
			int vsub = (sps->chroma_format_idc == 1) ? 1 : 0;
			int hsub = (sps->chroma_format_idc == 1 ||
				sps->chroma_format_idc == 2) ? 1 : 0;
			int step_x = 1 << hsub;
			int step_y = (2 - sps->frame_mbs_only_flag) << vsub;

			if (crop_left  > (unsigned)INT_MAX / 4 / step_x ||
				crop_right > (unsigned)INT_MAX / 4 / step_x ||
				crop_top   > (unsigned)INT_MAX / 4 / step_y ||
				crop_bottom> (unsigned)INT_MAX / 4 / step_y ||
				(crop_left + crop_right) * step_x >= width ||
				(crop_top + crop_bottom) * step_y >= height
				) {
				goto fail;
			}

			sps->crop_left = crop_left   * step_x;
			sps->crop_right = crop_right  * step_x;
			sps->crop_top = crop_top    * step_y;
			sps->crop_bottom = crop_bottom * step_y;
		}
	}
	else {
		sps->crop_left =
			sps->crop_right =
			sps->crop_top =
			sps->crop_bottom =
			sps->crop = 0;
	}

	sps->vui_parameters_present_flag = get_bits1(gb);
	if (sps->vui_parameters_present_flag) {
		int ret = decode_vui_parameters(gb, sps);
		if (ret < 0)
			goto fail;
	}

	if (get_bits_left(gb) < 0) {
		if (!ignore_truncation)
			goto fail;
	}

	/* if the maximum delay is not stored in the SPS, derive it based on the
	* level */
	if (!sps->bitstream_restriction_flag &&
		(sps->ref_frame_count /*|| avctx->strict_std_compliance >= FF_COMPLIANCE_STRICT*/)) {
		sps->num_reorder_frames = MAX_DELAYED_PIC_COUNT - 1;
		for (i = 0; i < FF_ARRAY_ELEMS(level_max_dpb_mbs); i++) {
			if (level_max_dpb_mbs[i][0] == sps->level_idc) {
				sps->num_reorder_frames = FFMIN(level_max_dpb_mbs[i][1] / (sps->mb_width * sps->mb_height),
					sps->num_reorder_frames);
				break;
			}
		}
	}

	if (!sps->sar.den)
		sps->sar.den = 1;

	/* check if this is a repeat of an already parsed SPS, then keep the
	* original one.
	* otherwise drop all PPSes that depend on it */
	if (ps->sps_list[sps_id] &&
		!memcmp(ps->sps_list[sps_id]->data, sps_buf->data, sps_buf->size)) {
		av_buffer_unref(&sps_buf);
	}
	else {
		remove_sps(ps, sps_id);
		ps->sps_list[sps_id] = sps_buf;
	}

	return 0;

fail:
	av_buffer_unref(&sps_buf);
	return AVERROR_INVALIDDATA;
}

static int more_rbsp_data_in_pps(const SPS *sps)
{
	int profile_idc = sps->profile_idc;

	if ((profile_idc == 66 || profile_idc == 77 ||
		profile_idc == 88) && (sps->constraint_set_flags & 7)) {
		return 0;
	}

	return 1;
}

static void build_qp_table(PPS *pps, int t, int index, const int depth)
{
	int i;
	const int max_qp = 51 + 6 * (depth - 8);
	for (i = 0; i < max_qp + 1; i++)
		pps->chroma_qp_table[t][i] =
		ff_h264_chroma_qp[depth - 8][av_clip(i + index, 0, max_qp)];
}

static void init_dequant8_coeff_table(PPS *pps, const SPS *sps)
{
	int i, j, q, x;
	const int max_qp = 51 + 6 * (sps->bit_depth_luma - 8);

	for (i = 0; i < 6; i++) {
		pps->dequant8_coeff[i] = pps->dequant8_buffer[i];
		for (j = 0; j < i; j++)
			if (!memcmp(pps->scaling_matrix8[j], pps->scaling_matrix8[i],
				64 * sizeof(uint8_t))) {
				pps->dequant8_coeff[i] = pps->dequant8_buffer[j];
				break;
			}
		if (j < i)
			continue;

		for (q = 0; q < max_qp + 1; q++) {
			int shift = ff_h264_quant_div6[q];
			int idx = ff_h264_quant_rem6[q];
			for (x = 0; x < 64; x++)
				pps->dequant8_coeff[i][q][(x >> 3) | ((x & 7) << 3)] =
				((uint32_t)ff_h264_dequant8_coeff_init[idx][ff_h264_dequant8_coeff_init_scan[((x >> 1) & 12) | (x & 3)]] *
					pps->scaling_matrix8[i][x]) << shift;
		}
	}
}

static void init_dequant4_coeff_table(PPS *pps, const SPS *sps)
{
	int i, j, q, x;
	const int max_qp = 51 + 6 * (sps->bit_depth_luma - 8);
	for (i = 0; i < 6; i++) {
		pps->dequant4_coeff[i] = pps->dequant4_buffer[i];
		for (j = 0; j < i; j++)
			if (!memcmp(pps->scaling_matrix4[j], pps->scaling_matrix4[i],
				16 * sizeof(uint8_t))) {
				pps->dequant4_coeff[i] = pps->dequant4_buffer[j];
				break;
			}
		if (j < i)
			continue;

		for (q = 0; q < max_qp + 1; q++) {
			int shift = ff_h264_quant_div6[q] + 2;
			int idx = ff_h264_quant_rem6[q];
			for (x = 0; x < 16; x++)
				pps->dequant4_coeff[i][q][(x >> 2) | ((x << 2) & 0xF)] =
				((uint32_t)ff_h264_dequant4_coeff_init[idx][(x & 1) + ((x >> 2) & 1)] *
					pps->scaling_matrix4[i][x]) << shift;
		}
	}
}

static void init_dequant_tables(PPS *pps, const SPS *sps)
{
	int i, x;
	init_dequant4_coeff_table(pps, sps);
	memset(pps->dequant8_coeff, 0, sizeof(pps->dequant8_coeff));

	if (pps->transform_8x8_mode)
		init_dequant8_coeff_table(pps, sps);
	if (sps->transform_bypass) {
		for (i = 0; i < 6; i++)
			for (x = 0; x < 16; x++)
				pps->dequant4_coeff[i][0][x] = 1 << 6;
		if (pps->transform_8x8_mode)
			for (i = 0; i < 6; i++)
				for (x = 0; x < 64; x++)
					pps->dequant8_coeff[i][0][x] = 1 << 6;
	}
}
int ff_h264_decode_picture_parameter_set(GetBitContext *gb, H264ParamSets *ps, int bit_length)
{
	AVBufferRef *pps_buf;
	const SPS *sps;
	unsigned int pps_id = get_ue_golomb(gb);
	PPS *pps;
	int qp_bd_offset;
	int bits_left;
	int ret;

	if (pps_id >= MAX_PPS_COUNT) {
		return AVERROR_INVALIDDATA;
	}

	pps_buf = av_buffer_allocz(sizeof(*pps));
	if (!pps_buf)
		return AVERROR(ENOMEM);
	pps = (PPS*)pps_buf->data;

	pps->data_size = gb->buffer_end - gb->buffer;
	if (pps->data_size > sizeof(pps->data)) {
		pps->data_size = sizeof(pps->data);
	}
	memcpy(pps->data, gb->buffer, pps->data_size);

	pps->sps_id = get_ue_golomb_31(gb);
	if ((unsigned)pps->sps_id >= MAX_SPS_COUNT ||
		!ps->sps_list[pps->sps_id]) {
		ret = AVERROR_INVALIDDATA;
		goto fail;
	}
	sps = (const SPS*)ps->sps_list[pps->sps_id]->data;
	if (sps->bit_depth_luma > 14) {
		ret = AVERROR_INVALIDDATA;
		goto fail;
	}
	else if (sps->bit_depth_luma == 11 || sps->bit_depth_luma == 13) {
		ret = AVERROR_PATCHWELCOME;
		goto fail;
	}

	pps->cabac = get_bits1(gb);
	pps->pic_order_present = get_bits1(gb);
	pps->slice_group_count = get_ue_golomb(gb) + 1;
	if (pps->slice_group_count > 1) {
		pps->mb_slice_group_map_type = get_ue_golomb(gb);
	}
	pps->ref_count[0] = get_ue_golomb(gb) + 1;
	pps->ref_count[1] = get_ue_golomb(gb) + 1;
	if (pps->ref_count[0] - 1 > 32 - 1 || pps->ref_count[1] - 1 > 32 - 1) {
		ret = AVERROR_INVALIDDATA;
		goto fail;
	}

	qp_bd_offset = 6 * (sps->bit_depth_luma - 8);

	pps->weighted_pred = get_bits1(gb);
	pps->weighted_bipred_idc = get_bits(gb, 2);
	pps->init_qp = get_se_golomb(gb) + 26U + qp_bd_offset;
	pps->init_qs = get_se_golomb(gb) + 26U + qp_bd_offset;
	pps->chroma_qp_index_offset[0] = get_se_golomb(gb);
	if (pps->chroma_qp_index_offset[0] < -12 || pps->chroma_qp_index_offset[0] > 12) {
		ret = AVERROR_INVALIDDATA;
		goto fail;
	}

	pps->deblocking_filter_parameters_present = get_bits1(gb);
	pps->constrained_intra_pred = get_bits1(gb);
	pps->redundant_pic_cnt_present = get_bits1(gb);

	pps->transform_8x8_mode = 0;
	memcpy(pps->scaling_matrix4, sps->scaling_matrix4,
		sizeof(pps->scaling_matrix4));
	memcpy(pps->scaling_matrix8, sps->scaling_matrix8,
		sizeof(pps->scaling_matrix8));

	bits_left = bit_length - get_bits_count(gb);
	if (bits_left > 0 && more_rbsp_data_in_pps(sps)) {
		pps->transform_8x8_mode = get_bits1(gb);
		ret = decode_scaling_matrices(gb, sps, pps, 0,
			pps->scaling_matrix4, pps->scaling_matrix8);
		if (ret < 0)
			goto fail;
		// second_chroma_qp_index_offset
		pps->chroma_qp_index_offset[1] = get_se_golomb(gb);
		if (pps->chroma_qp_index_offset[1] < -12 || pps->chroma_qp_index_offset[1] > 12) {
			ret = AVERROR_INVALIDDATA;
			goto fail;
		}
	}
	else {
		pps->chroma_qp_index_offset[1] = pps->chroma_qp_index_offset[0];
	}

	build_qp_table(pps, 0, pps->chroma_qp_index_offset[0],
		sps->bit_depth_luma);
	build_qp_table(pps, 1, pps->chroma_qp_index_offset[1],
		sps->bit_depth_luma);

	init_dequant_tables(pps, sps);

	if (pps->chroma_qp_index_offset[0] != pps->chroma_qp_index_offset[1])
		pps->chroma_qp_diff = 1;

	remove_pps(ps, pps_id);
	ps->pps_list[pps_id] = pps_buf;

	return 0;

fail:
	av_buffer_unref(&pps_buf);
	return ret;
}

#endif /* AVCODEC_H264_H */
