/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

typedef struct BmFontDataCommon
{
	int32_t LineHeight;
	int32_t Base;
	int32_t ScaleW;
	int32_t ScaleH;
	int32_t Pages;
	int32_t Packed;
	int32_t AlphaChnl;
	int32_t RedChnl;
	int32_t GreenChnl;
	int32_t BlueChnl;
} BmFontDataCommon;