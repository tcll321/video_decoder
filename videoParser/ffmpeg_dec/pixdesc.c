#include "pixfmt.h"

static const char * const color_primaries_names[AVCOL_PRI_NB] = {
	[AVCOL_PRI_RESERVED0] = "reserved",
	[AVCOL_PRI_BT709] = "bt709",
	[AVCOL_PRI_UNSPECIFIED] = "unknown",
	[AVCOL_PRI_RESERVED] = "reserved",
	[AVCOL_PRI_BT470M] = "bt470m",
	[AVCOL_PRI_BT470BG] = "bt470bg",
	[AVCOL_PRI_SMPTE170M] = "smpte170m",
	[AVCOL_PRI_SMPTE240M] = "smpte240m",
	[AVCOL_PRI_FILM] = "film",
	[AVCOL_PRI_BT2020] = "bt2020",
	[AVCOL_PRI_SMPTE428] = "smpte428",
	[AVCOL_PRI_SMPTE431] = "smpte431",
	[AVCOL_PRI_SMPTE432] = "smpte432",
	[AVCOL_PRI_JEDEC_P22] = "jedec-p22",
};

static const char * const color_transfer_names[] = {
	[AVCOL_TRC_RESERVED0] = "reserved",
	[AVCOL_TRC_BT709] = "bt709",
	[AVCOL_TRC_UNSPECIFIED] = "unknown",
	[AVCOL_TRC_RESERVED] = "reserved",
	[AVCOL_TRC_GAMMA22] = "bt470m",
	[AVCOL_TRC_GAMMA28] = "bt470bg",
	[AVCOL_TRC_SMPTE170M] = "smpte170m",
	[AVCOL_TRC_SMPTE240M] = "smpte240m",
	[AVCOL_TRC_LINEAR] = "linear",
	[AVCOL_TRC_LOG] = "log100",
	[AVCOL_TRC_LOG_SQRT] = "log316",
	[AVCOL_TRC_IEC61966_2_4] = "iec61966-2-4",
	[AVCOL_TRC_BT1361_ECG] = "bt1361e",
	[AVCOL_TRC_IEC61966_2_1] = "iec61966-2-1",
	[AVCOL_TRC_BT2020_10] = "bt2020-10",
	[AVCOL_TRC_BT2020_12] = "bt2020-12",
	[AVCOL_TRC_SMPTE2084] = "smpte2084",
	[AVCOL_TRC_SMPTE428] = "smpte428",
	[AVCOL_TRC_ARIB_STD_B67] = "arib-std-b67",
};

static const char * const color_space_names[] = {
	[AVCOL_SPC_RGB] = "gbr",
	[AVCOL_SPC_BT709] = "bt709",
	[AVCOL_SPC_UNSPECIFIED] = "unknown",
	[AVCOL_SPC_RESERVED] = "reserved",
	[AVCOL_SPC_FCC] = "fcc",
	[AVCOL_SPC_BT470BG] = "bt470bg",
	[AVCOL_SPC_SMPTE170M] = "smpte170m",
	[AVCOL_SPC_SMPTE240M] = "smpte240m",
	[AVCOL_SPC_YCGCO] = "ycgco",
	[AVCOL_SPC_BT2020_NCL] = "bt2020nc",
	[AVCOL_SPC_BT2020_CL] = "bt2020c",
	[AVCOL_SPC_SMPTE2085] = "smpte2085",
	[AVCOL_SPC_CHROMA_DERIVED_NCL] = "chroma-derived-nc",
	[AVCOL_SPC_CHROMA_DERIVED_CL] = "chroma-derived-c",
	[AVCOL_SPC_ICTCP] = "ictcp",
};
