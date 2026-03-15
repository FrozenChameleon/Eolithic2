#include "NodeMovingKit.h"

#include "../core/Func.h"
#include "../math/Math.h"

void NodeMovingKit_Setup(NodeMovingKit* nmk, Entity owner, int type, bool moveContacts)
{
	nmk->mType = type;
	nmk->mMoveContacts = moveContacts;
	if (!Is_NodesPresent(owner))
	{
		nmk->mIsComplete = true;
	}
	else
	{
		nmk->mIsComplete = false;
	}
}
bool NodeMovingKit_IsOnTarget(NodeMovingKit* nmk)
{
	return nmk->mIsOnTarget;
}
int32_t NodeMovingKit_GetCurrentNode(NodeMovingKit* nmk)
{
	return nmk->mCurrentNode;
}
bool NodeMovingKit_IsComplete(NodeMovingKit* nmk)
{
	return nmk->mIsComplete;
}
int32_t NodeMovingKit_GetDistanceToNextNodeY(NodeMovingKit* nmk, Entity owner)
{
	if (nmk->mCurrentNode >= Get_AmountOfNodes(owner))
	{
		return -1;
	}

	Point point = Get_Node(owner, nmk->mCurrentNode);

	return (int32_t)Math_fabsf(Get_Y(owner) - point.Y);
}
void NodeMovingKit_HandleNodeMoving(NodeMovingKit* nmk, Entity owner)
{
	NodeMovingKit_HandleNodeMoving2(nmk, owner, 1);
}
void NodeMovingKit_HandleNodeMoving2(NodeMovingKit* nmk, Entity owner, float speed)
{
	nmk->mIsOnTarget = false;

	if (nmk->mIsComplete)
	{
		return;
	}

	if (nmk->mCurrentNode >= 0)
	{
		Point point = Get_Node(owner, nmk->mCurrentNode);

		if (Do_MoveSomewhere(owner, Vector2_Create(point.X, point.Y), speed))
		{
			nmk->mIsOnTarget = true;

			if (!nmk->mBackwards)
			{
				nmk->mCurrentNode += 1;
			}
			else
			{
				nmk->mCurrentNode -= 1;
			}

			if (nmk->mCurrentNode > Get_AmountOfNodes(owner) - 1)
			{
				if (nmk->mType == NODEMOVINGKIT_BACK_AND_FORTH)
				{
					nmk->mBackwards = true;
					nmk->mCurrentNode = Get_AmountOfNodes(owner) - 1;
				}
				else if (nmk->mType == NODEMOVINGKIT_END_TO_BEGIN)
				{
					nmk->mCurrentNode = -1;
				}
				else if (nmk->mType == NODEMOVINGKIT_STOP)
				{
					nmk->mIsComplete = true;
				}
				else if (nmk->mType == NODEMOVINGKIT_END_TO_FIRST_NODE)
				{
					nmk->mCurrentNode = 0;
				}
			}
		}
		if (nmk->mMoveContacts)
		{
			//Do_MoveAllThings(contacts, point.X, point.Y, speed);
		}
	}
	else
	{
		if (Do_MoveSomewhere(owner, Vector2_Create(Get_InitialX(owner), Get_InitialY(owner)), speed))
		{
			nmk->mIsOnTarget = true;

			nmk->mCurrentNode = 0;
			nmk->mBackwards = false;
		}
		if (nmk->mMoveContacts)
		{
			//Do_MoveAllThings(contacts, Get_InitialX(owner), Get_InitialY(owner), speed);
		}
	}
}
