#pragma once

#include "Macros.h"
#include "../math/Random32.h"

typedef struct UniquePositions
{
	uint8_t Data0;
	uint8_t Data1;
	uint8_t Data2;
	uint8_t Data3;
	uint8_t Data4;
	uint8_t Data5;
	uint8_t Data6;
	uint8_t Data7;
} UniquePositions;

void UniquePositions_Reset(UniquePositions* positions);
void UniquePositions_GeneratePositionsNewFirstCannotBePreviousLast(UniquePositions* positions, Random32* random, int length);
void UniquePositions_GeneratePositionsNewFirstCannotBePreviousFirst(UniquePositions* positions, Random32* random, int length);
void UniquePositions_GeneratePositions(UniquePositions* positions, Random32* random, int length);
void UniquePositions_Check(int i);
int UniquePositions_Get(UniquePositions* positions, int i);
void UniquePositions_Set(UniquePositions* positions, int i, int value);
