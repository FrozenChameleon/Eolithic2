/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ShaderProgram.h"
#include "../utils/Utils.h"
#include "Renderer.h"

static ShaderProgram _mShaderProgramWhiteHitFlash;
static ShaderProgram _mShaderProgramFullRed;
static ShaderProgram _mShaderProgramFullRed2;

ShaderProgram* ShaderProgram_GetShaderFullRed(void)
{
	_mShaderProgramFullRed.mShaderType = RENDERER_SHADER_TYPE_WHITE_HIT_FLASH;
	return &_mShaderProgramFullRed;
}
ShaderProgram* ShaderProgram_GetShaderFullRed2(void)
{
	_mShaderProgramFullRed2.mShaderType = RENDERER_SHADER_TYPE_WHITE_HIT_FLASH;
	_mShaderProgramFullRed2.mIsTotalWhiteHitFlash = true;
	return &_mShaderProgramFullRed2;
}
ShaderProgram* ShaderProgram_GetShaderWhiteHitFlash(void)
{
	_mShaderProgramWhiteHitFlash.mShaderType = RENDERER_SHADER_TYPE_WHITE_HIT_FLASH;
	return &_mShaderProgramWhiteHitFlash;
}
