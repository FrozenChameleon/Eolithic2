#include "UniquePositions.h"

#include "Exception.h"

#define MAX_SIZE 8
#define UP_DEFAULT_VALUE 0xFF

void UniquePositions_Reset(UniquePositions* positions)
{
	for (int i = 0; i < MAX_SIZE; i += 1)
	{
		UniquePositions_Set(positions, i, UP_DEFAULT_VALUE);
	}
}
void UniquePositions_GeneratePositionsNewFirstCannotBePreviousLast(UniquePositions* positions, Random32* random, int length)
{
	UniquePositions_Reset(positions);
	int previousLast = UniquePositions_Get(positions, length - 1);
	do
	{
		UniquePositions_GeneratePositions(positions, random, length);
	} while (positions->Data0 == previousLast);
}
void UniquePositions_GeneratePositionsNewFirstCannotBePreviousFirst(UniquePositions* positions, Random32* random, int length)
{
	UniquePositions_Reset(positions);
	uint8_t previousFirst = positions->Data0;
	do
	{
		UniquePositions_GeneratePositions(positions, random, length);
	} while (positions->Data0 == previousFirst);
}
void UniquePositions_GeneratePositions(UniquePositions* positions, Random32* random, int length)
{
	UniquePositions_Reset(positions);
	for (int i = 0; i < length; i += 1)
	{
		bool good = false;
		while (!good)
		{
			int temp = Random32_NextInt(random, length);
			good = true;
			for (int j = 0; j < length; j += 1)
			{
				if (UniquePositions_Get(positions, j) == temp)
				{
					good = false;
				}
			}
			if (good)
			{
				UniquePositions_Set(positions, i, temp);
			}
		}
	}
}
void UniquePositions_Check(int i)
{
	if (i < 0 || i >(MAX_SIZE - 1))
	{
		Exception_Run("Out of unique positions!", false);
	}
}
int UniquePositions_Get(UniquePositions* positions, int i)
{
	UniquePositions_Check(i);
	switch (i)
	{
	case 0:
		return positions->Data0;
	case 1:
		return positions->Data1;
	case 2:
		return positions->Data2;
	case 3:
		return positions->Data3;
	case 4:
		return positions->Data4;
	case 5:
		return positions->Data5;
	case 6:
		return positions->Data6;
	case 7:
		return positions->Data7;
	}
	return -1;
}
void UniquePositions_Set(UniquePositions* positions, int i, int value)
{
	UniquePositions_Check(i);
	switch (i)
	{
	case 0:
		positions->Data0 = (uint8_t)value;
		break;
	case 1:
		positions->Data1 = (uint8_t)value;
		break;
	case 2:
		positions->Data2 = (uint8_t)value;
		break;
	case 3:
		positions->Data3 = (uint8_t)value;
		break;
	case 4:
		positions->Data4 = (uint8_t)value;
		break;
	case 5:
		positions->Data5 = (uint8_t)value;
		break;
	case 6:
		positions->Data6 = (uint8_t)value;
		break;
	case 7:
		positions->Data7 = (uint8_t)value;
		break;
	}
}
