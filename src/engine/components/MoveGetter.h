/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../math/Point.h"

#define MOVE_GETTER_MAX_LEN 20

typedef struct MoveGetter
{
	int32_t mLastMove;
	int32_t mMovesCounter;
	bool mIsFixedOrder;
	int32_t mFixedOrderMoves[MOVE_GETTER_MAX_LEN];
	int32_t mLastMoves[MOVE_GETTER_MAX_LEN];
	const char* mMoves[MOVE_GETTER_MAX_LEN][EE_FILENAME_MAX];
	int32_t mConstraints[MOVE_GETTER_MAX_LEN];
	int32_t mDebugMyId;
	int32_t mDebugHeight;
	bool mDebugIsShowing;
	bool mDebugDoNotUseMoveForcedByTool;
	const char* mDebugName;
	Point mDebugFirstWindowPosition;
} MoveGetter;