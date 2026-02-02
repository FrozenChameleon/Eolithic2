/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Vector2.h"
#include "../math/Point.h"
#include "../render/Color.h"
#include "../utils/Timer.h"
#include "../render/Animation.h"
#include "Particle.h"

typedef struct Sheet Sheet;
typedef struct ShaderProgram ShaderProgram;
typedef struct MString MString;

typedef struct ParticleInstance
{
	int32_t mCounter;
	int32_t mFlickerStart;
	int32_t mInfluencedDepth;
	float mInfluencedRotation;
	bool mAttachedThingFollowFlipX;
	bool mIsFlickering;
	bool mFlipX;
	bool mFlipY;
	bool mIsComplete;
	char mName[EE_FILENAME_MAX];
	Point mAttachedThingOffset;
	Point mInfluencedDirection;
	Vector2 mCurveMaximum;
	Vector2 mCurrentCurveRate;
	Vector2 mCurveRate;
	Vector2 mOffset;
	Vector2 mLastRenderPosition;
	Vector2 mPosition;
	Vector2 mConstant;
	Color mInfluencedColor;
	Timer mTimerConstantFluctuate;
	Timer mTimerCurveFluctuate;
	Timer mTimerTimeToLive;
	Timer mTimerFlicker;
	Animation mAnimation;
	int32_t mAttachedEntity;
	void* mAttachedThing;
	Particle* mParticle;
	Sheet* mSheet;
	ShaderProgram* mShaderProgram;
} ParticleInstance;

ParticleInstance* ParticleInstance_Dummy(void);
const ParticleInstance* ParticleInstance_Empty(void);