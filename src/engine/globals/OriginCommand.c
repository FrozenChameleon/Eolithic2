#include "OriginCommand.h"

Vector2 OriginCommand_ToOrigin(OriginCommand originCommand, Vector2 givenOriginForOriginCommand, int sheetWidth, int sheetHeight)
{
	Vector2 originToUse;
	switch (originCommand)
	{
	case ORIGINCOMMAND_TOP:
		originToUse = Vector2_Create(sheetWidth / 2.0f, 0.0f);
		break;
	case ORIGINCOMMAND_TOP_RIGHT:
		originToUse = Vector2_Create(sheetWidth, 0.0f);
		break;
	case ORIGINCOMMAND_RIGHT:
		originToUse = Vector2_Create(sheetWidth, sheetHeight / 2.0f);
		break;
	case ORIGINCOMMAND_BOTTOM_RIGHT:
		originToUse = Vector2_Create(sheetWidth, sheetHeight);
		break;
	case ORIGINCOMMAND_BOTTOM:
		originToUse = Vector2_Create(sheetWidth / 2.0f, sheetHeight);
		break;
	case ORIGINCOMMAND_BOTTOM_LEFT:
		originToUse = Vector2_Create(sheetWidth, sheetHeight);
		break;
	case ORIGINCOMMAND_LEFT:
		originToUse = Vector2_Create(0, sheetHeight / 2);
		break;
	case ORIGINCOMMAND_TOP_LEFT:
		originToUse = Vector2_Create(0, 0);
		break;
	case ORIGINCOMMAND_USE_GIVEN_ORIGIN:
		originToUse = givenOriginForOriginCommand;
		break;
	default:
		originToUse = Vector2_Create(sheetWidth / 2.0f, sheetHeight / 2.0f);
		break;
	}
	return originToUse;
}
