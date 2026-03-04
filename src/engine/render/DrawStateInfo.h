/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Vector2.h"

typedef struct ShaderProgram ShaderProgram;

typedef struct DrawStateInfo
{
	char mState[EE_FILENAME_MAX];
	char mCurrentPhase[EE_FILENAME_MAX];
	int32_t mDepth;
	float mRotation;
	Vector2 mNudge;
	ShaderProgram* mShader;
} DrawStateInfo;

