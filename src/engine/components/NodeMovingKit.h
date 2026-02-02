/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"

typedef struct NodeMovingKit
{
	int32_t mType;
	int32_t mCurrentNode;
	bool mIsOnTarget;
	bool mBackwards;
	bool mIsComplete;
	bool mMoveContacts;
} NodeMovingKit;