/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../io/BufferReader.h"

#define SHADER_PROGRAM_MAX_REPLACE_LENGTH 24
#define SHADER_PROGRAM_MAX_REPLACE_ENTRY_OFFSET 6

typedef struct ShaderProgram
{
	char mName[EE_FILENAME_MAX];
	uint32_t mIsTotalWhiteHitFlash;
	uint32_t mShaderType;
	uint32_t mMultiColorReplaceLength;
	uint8_t mMultiColorReplace[SHADER_PROGRAM_MAX_REPLACE_LENGTH];
	uint8_t mMultiColorReplaceAlpha;
} ShaderProgram;

ShaderProgram* ShaderProgram_GetShaderFullRed(void);
ShaderProgram* ShaderProgram_GetShaderFullRed2(void);
ShaderProgram* ShaderProgram_GetShaderWhiteHitFlash(void);