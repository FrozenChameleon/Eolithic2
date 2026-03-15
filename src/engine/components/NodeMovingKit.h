/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"

#define NODEMOVINGKIT_BACK_AND_FORTH 0
#define NODEMOVINGKIT_END_TO_BEGIN 1
#define NODEMOVINGKIT_STOP 2
#define NODEMOVINGKIT_END_TO_FIRST_NODE 3

typedef struct NodeMovingKit
{
	int32_t mType;
	int mCurrentNode;
	bool mIsOnTarget;
	bool mBackwards;
	bool mIsComplete;
	bool mMoveContacts;
} NodeMovingKit;

void NodeMovingKit_Setup(NodeMovingKit* nmk, Entity owner, int type, bool moveContacts);
bool NodeMovingKit_IsOnTarget(NodeMovingKit* nmk);
int32_t NodeMovingKit_GetCurrentNode(NodeMovingKit* nmk);
bool NodeMovingKit_IsComplete(NodeMovingKit* nmk);
int32_t NodeMovingKit_GetDistanceToNextNodeY(NodeMovingKit* nmk, Entity owner);
void NodeMovingKit_HandleNodeMoving(NodeMovingKit* nmk, Entity owner);
void NodeMovingKit_HandleNodeMoving2(NodeMovingKit* nmk, Entity owner, float speed);