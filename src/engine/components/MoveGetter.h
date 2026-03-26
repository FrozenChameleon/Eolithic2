#include "../utils/Macros.h"

#include "../systems/SystemSimple.h"
#include "../collections/FixedListEightInt.h"
#include "../math/Point.h"
#include "../math/Random32.h"

typedef struct MoveGetter
{
	int mFixedOrderLoops;
	int mLastMove;
	int mMovesCounter;
	bool mIsFixedOrder;
	const int* mFixedOrderMoves;
	int mFixedOrderMovesLength;
	FixedListEightInt mLastMoves;
	FixedListEightInt mConstraints;
	const char** mMoveNames;
	int mMoveNamesLength;
	int mDebugMyId;
	int mDebugHeight;
	bool mDebugIsShowing;
	bool mDebugDoNotUseMoveForcedByTool;
	char mDebugName[EE_FILENAME_MAX];
	Point mDebugFirstWindowPosition;
} MoveGetter;

void MoveGetter_Setup(MoveGetter* mg, const char* name, int movesToRemember);
void MoveGetter_SetFixedOrderMoves(MoveGetter* mg, const int* moves, int movesLength);
void MoveGetter_SetMoveNames(MoveGetter* mg, const char** moveNames, int moveNamesLength);
void MoveGetter_OverflowCheck(MoveGetter* mg);
int MoveGetter_GetLastMove(MoveGetter* mg);
void MoveGetter_AddConstraint(MoveGetter* mg, int value);

System* MoveGetter_CreateSystem(void);
int MoveGetter_GetMove2(MoveGetter* data, Random32* random);
int MoveGetter_GetMove(MoveGetter* data);
void MoveGetter_InitRoutine(Entity owner, MoveGetter* data);
void MoveGetter_UpdateRoutine(Entity owner, MoveGetter* data);