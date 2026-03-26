#include "MoveGetter.h"
#include "../utils/Utils.h"
#include "../core/Func.h"
#include "../utils/Logger.h"

static bool DEBUG_IS_FORCED_MOVE;
static int DEBUG_FORCED_MOVE;
static int DEBUG_ID_COUNTER;

void MoveGetter_Setup(MoveGetter* mg, const char* name, int movesToRemember)
{
	Utils_strlcpy(mg->mDebugName, name, EE_FILENAME_MAX);
	FixedListEightInt_Clear(&mg->mLastMoves);
	for (int i = 0; i < movesToRemember; i += 1)
	{
		FixedListEightInt_Add(&mg->mLastMoves, -1);
	}
}
void MoveGetter_SetFixedOrderMoves(MoveGetter* mg, const int* moves, int movesLength)
{
	mg->mIsFixedOrder = true;
	mg->mFixedOrderMoves = moves;
	mg->mFixedOrderMovesLength = movesLength;
}
void MoveGetter_SetMoveNames(MoveGetter* mg, const char** moveNames, int moveNamesLength)
{
	mg->mMoveNames = moveNames;
	mg->mMoveNamesLength = moveNamesLength;
}
void MoveGetter_OverflowCheck(MoveGetter* mg)
{
	if (FixedListEightInt_Count(&mg->mLastMoves) < mg->mMoveNamesLength)
	{
		return;
	}

	bool reset = true;
	for (int i = 0; i < FixedListEightInt_Count(&mg->mLastMoves); i += 1)
	{
		if (FixedListEightInt_Get(&mg->mLastMoves, i) == -1)
		{
			reset = false;
		}
	}
	if (reset)
	{
		mg->mMovesCounter = 0;
		FixedListEightInt_SetAll(&mg->mLastMoves, -1);
	}
}
int MoveGetter_GetLastMove(MoveGetter* mg)
{
	return FixedListEightInt_Get(&mg->mLastMoves, 0);
}
void MoveGetter_AddConstraint(MoveGetter* mg, int value)
{
	FixedListEightInt_Add(&mg->mConstraints, value);
}

System* MoveGetter_CreateSystem(void)
{
	SystemSimple* ss = SystemSimple_Create(C_MoveGetter);
	ss->_mInitRoutine = (SystemSimple_InitFunc)MoveGetter_InitRoutine;
	ss->_mUpdateRoutine = (SystemSimple_UpdateFunc)MoveGetter_UpdateRoutine;
	return SystemSimple_CreateSystem(ss);
}
int MoveGetter_GetMove2(MoveGetter* data, Random32* random)
{
	if (data->mMoveNamesLength <= 1)
	{
		return 0;
	}

	if (DEBUG_IS_FORCED_MOVE && !data->mDebugDoNotUseMoveForcedByTool)
	{
		return DEBUG_FORCED_MOVE;
	}

	if (data->mIsFixedOrder)
	{
		FixedListEightInt_Clear(&data->mConstraints);
		int fixedMoveToReturn = data->mFixedOrderMoves[data->mMovesCounter];
		data->mLastMove = fixedMoveToReturn;
		data->mMovesCounter += 1;
		if (data->mMovesCounter >= data->mFixedOrderMovesLength)
		{
			data->mFixedOrderLoops += 1;
			data->mMovesCounter = 0;
		}
		return fixedMoveToReturn;
	}

	int currentMove = -1;
	bool good;
	int counter = 0;
	do //get the new one
	{
		if (counter == 100)
		{
			Logger_Log(LOGGER_INFORMATION, data->mDebugName);
			Logger_Log(LOGGER_INFORMATION, "Ran out of moves, resetting queue!");
			FixedListEightInt_SetAll(&data->mLastMoves, -1);
		}
		if (counter >= 1000)
		{
			break;
		}
		currentMove = Random32_NextInt(random, data->mMoveNamesLength);
		good = true;
		for (int i = 0; i < FixedListEightInt_Count(&data->mLastMoves); i += 1)
		{
			if (FixedListEightInt_Get(&data->mLastMoves, i) == currentMove)
			{
				good = false;
			}
		}
		for (int i = 0; i < FixedListEightInt_Count(&data->mConstraints); i += 1)
		{
			if (currentMove == FixedListEightInt_Get(&data->mConstraints, i))
			{
				good = false;
			}
		}
		if (currentMove == data->mLastMove)
		{
			good = false;
		}
		counter += 1;
	} while (!good);

	for (int i = (FixedListEightInt_Count(&data->mLastMoves) - 1); i > 0; i -= 1) //push the old ones
	{
		FixedListEightInt_Set(&data->mLastMoves, i, FixedListEightInt_Get(&data->mLastMoves, i - 1));
		FixedListEightInt_Set(&data->mLastMoves, i - 1, -1);
	}
	FixedListEightInt_Set(&data->mLastMoves, 0, currentMove);

	data->mMovesCounter += 1;

	FixedListEightInt_Clear(&data->mConstraints);

	MoveGetter_OverflowCheck(data);

	data->mLastMove = currentMove;

	return currentMove;
}
int MoveGetter_GetMove(MoveGetter* data)
{
	return MoveGetter_GetMove2(data, Get_SharedRandom());
}
void MoveGetter_InitRoutine(Entity owner, MoveGetter* data)
{
	Utils_memset(&data->mConstraints, 0, sizeof(FixedListEightInt));
	data->mDebugMyId = DEBUG_ID_COUNTER;
	DEBUG_ID_COUNTER += 1;
	data->mLastMove = -1;
	FixedListEightInt_Clear(&data->mConstraints);
}
void MoveGetter_UpdateRoutine(Entity owner, MoveGetter* data)
{
#if EDITOR
	if (OeInput.IsKeyTapped(Keys.F7))
	{
		data->mDebugIsShowing = !data->mDebugIsShowing;
	}

	if (!data->mDebugIsShowing)
	{
		return;
	}

	OeGui.SetNextWindowAlwaysOpen();
	if (!OeGui.StartWindow("Move Getter: " + data->mDebugName, 500, data->mDebugHeight))
	{
		return;
	}

	data->mDebugHeight = 0;
	int diff = 32;
	string name = "NULL";
	if (data.GetLastMove() != -1)
	{
		name = data->mMoveNames[data.GetLastMove()];
	}
	string lastMoves = "";
	int amount = 0;
	for (int i = 0; i < data->mLastMoves.Count; i += 1)
	{
		if (data->mLastMoves.Get(i) != -1)
		{
			amount += 1;
		}
	}
	if (data->mLastMove != -1)
	{
		lastMoves += "[L=";
		lastMoves += data->mLastMove == -1 ? "-1" : data->mMoveNames[data->mLastMove];
		lastMoves += "]";
		lastMoves += "/[M=";
	}
	for (int i = 0; i < data->mLastMoves.Count; i += 1)
	{
		int moveLoc = data->mLastMoves.Get(i);
		lastMoves += moveLoc == -1 ? "-1" : data->mMoveNames[moveLoc];
		if (i != (data->mLastMoves.Count - 1))
		{
			lastMoves += ",";
		}
	}
	lastMoves += "]";
	OeGui.Text(lastMoves);
	for (int i = 0; i < data->mMoveNames.Length; i += 1)
	{
		data->mDebugHeight += diff;
		bool value = DEBUG_FORCED_MOVE == i;
		if (OeGui.CheckBox(data->mMoveNames[i], ref value))
		{
			if (value)
			{
				DEBUG_IS_FORCED_MOVE = true;
				DEBUG_FORCED_MOVE = i;
			}
			else
			{
				DEBUG_IS_FORCED_MOVE = false;
				DEBUG_FORCED_MOVE = -1;
			}
		}
	}
	data->mDebugHeight += diff;
	OeGui.EndWindow();
#endif
}
