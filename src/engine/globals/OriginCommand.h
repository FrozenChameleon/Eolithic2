#pragma once

#include "../math/Vector2.h"

typedef enum OriginCommand
{
	ORIGINCOMMAND_NOTHING = 0,
	ORIGINCOMMAND_TOP = 1,
	ORIGINCOMMAND_TOP_RIGHT = 2,
	ORIGINCOMMAND_RIGHT = 3,
	ORIGINCOMMAND_BOTTOM_RIGHT = 4,
	ORIGINCOMMAND_BOTTOM = 5,
	ORIGINCOMMAND_BOTTOM_LEFT = 6,
	ORIGINCOMMAND_LEFT = 7,
	ORIGINCOMMAND_TOP_LEFT = 8,
	ORIGINCOMMAND_USE_GIVEN_ORIGIN = 9
} OriginCommand;

Vector2 OriginCommand_ToOrigin(OriginCommand originCommand, Vector2 givenOriginForOriginCommand, int sheetWidth, int sheetHeight);
