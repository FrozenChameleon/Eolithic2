/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Vector2.h"
#include "../render/DrawRectangle.h"

typedef struct CollisionEngine
{
	int32_t mDefaultBodyPushAmount;
	bool mIsNullDividePlus;
	Vector2 mFixedGravity;
	int32_t* mCollisionGridPristine;
	int32_t* mCollisionGrid;
	Rectangle mCollisionGridSize;
} CollisionEngine;

