/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Line.h"

#include "../utils/Macros.h"
#include "../math/Vectors.h"
#include "../math/PointRectangle.h"
#include "../math/Points.h"
#include "../render/DrawTool.h"
#include "../render/Color.h"
#include "../globals/Align.h"
#include "../utils/Utils.h"
#include "../math/Math.h"
#include "../io/BufferReader.h"
#include "../render/SpriteBatch.h"

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE
#define HALF_TILE_SIZE GLOBAL_DEF_HALF_TILE_SIZE

enum
{
	LINE_DELAY = 16,
	LINE_THICKNESS = 4
};

Point Line_GetMiddle(Point begin, Point end)
{
	Point diff = Point_Sub(end, begin);
	return Point_Create(begin.X + (diff.X / 2), begin.Y + (diff.Y / 2));
}
void Line_DrawLineCamera(Line* line, SpriteBatch* spriteBatch, const char* font, Color color, int32_t cameraWidth, int32_t cameraHeight)
{
	Point beginPoint = Line_GetRealBegin(line);
	Point endPoint = Line_GetRealEnd(line);

	int32_t halfCameraWidth = cameraWidth / 2;
	int32_t halfCameraHeight = cameraHeight / 2;

	Point upperLeft = Point_Create(beginPoint.X - halfCameraWidth, beginPoint.Y - halfCameraHeight);
	Point upperRight = Point_Create(endPoint.X + halfCameraWidth, beginPoint.Y - halfCameraHeight);
	Point lowerRight = Point_Create(endPoint.X + halfCameraWidth, endPoint.Y + halfCameraHeight);
	Point lowerLeft = Point_Create(beginPoint.X - halfCameraWidth, endPoint.Y + halfCameraHeight);

	int32_t depth = 200;
	int32_t lineDelay = 4;
	int32_t lineThickness = 4;
	DrawTool_DrawLine(spriteBatch, color, depth, lineDelay, 0, lineThickness, upperLeft, upperRight);
	DrawTool_DrawLine(spriteBatch, color, depth, lineDelay, 0, lineThickness, upperRight, lowerRight);
	DrawTool_DrawLine(spriteBatch, color, depth, lineDelay, 0, lineThickness, lowerRight, lowerLeft);
	DrawTool_DrawLine(spriteBatch, color, depth, lineDelay, 0, lineThickness, lowerLeft, upperLeft);
}
void Line_DrawLine(Line* line, SpriteBatch* spriteBatch, const char* font, Color color, int32_t lineNumber, int32_t offset)
{
	Point beginPoint = Line_GetRealBegin(line);
	Point endPoint = Line_GetRealEnd(line);
	Line_DrawLineText(line, spriteBatch, font, lineNumber, Vectors_ToVector2(beginPoint));
	DrawTool_DrawRectangle2(spriteBatch, COLOR_COMETSTRIKER_RED, 100, Rectangle_Create(beginPoint.X, beginPoint.Y, HALF_TILE_SIZE, HALF_TILE_SIZE), 0, true);
	int32_t lineDelay = 16;
	int32_t lineThickness = 4;
	DrawTool_DrawLine(spriteBatch, color, 99, lineDelay, offset, lineThickness, beginPoint, endPoint);
	DrawTool_DrawRectangle2(spriteBatch, COLOR_COMETSTRIKER_BLUE, 100, Rectangle_Create(endPoint.X, endPoint.Y, HALF_TILE_SIZE, HALF_TILE_SIZE), 0, true);
}
void Line_DrawLineText(Line* line, SpriteBatch* spriteBatch, const char* font, int32_t lineNumber, Vector2 pos)
{
	int32_t align = ALIGN_CENTER;
	Vector2 textPos = Vector2_Create(pos.X, pos.Y - HALF_TILE_SIZE);
	int32_t depth = 100;
	Color textColor = COLOR_RED;
	SpriteBatch_DrawString2(spriteBatch, font, Utils_IntToStringGlobalBuffer(lineNumber), textColor, depth, textPos, align, align);
	if (line->mEnterDownOnly)
	{
		textPos.Y -= HALF_TILE_SIZE;
		SpriteBatch_DrawString2(spriteBatch, font, "Enter Down Only", textColor, depth, textPos, align, align);
	}
	if (line->mEnterLeftOnly)
	{
		textPos.Y -= HALF_TILE_SIZE;
		SpriteBatch_DrawString2(spriteBatch, font, "Enter Left Only", textColor, depth, textPos, align, align);
	}
	if (line->mEnterUpOnly)
	{
		textPos.Y -= HALF_TILE_SIZE;
		SpriteBatch_DrawString2(spriteBatch, font, "Enter Up Only", textColor, depth, textPos, align, align);
	}
	if (line->mEnterRightOnly)
	{
		textPos.Y -= HALF_TILE_SIZE;
		SpriteBatch_DrawString2(spriteBatch, font, "Enter Right Only", textColor, depth, textPos, align, align);
	}
	if (line->mOneWayUp)
	{
		textPos.Y -= HALF_TILE_SIZE;
		SpriteBatch_DrawString2(spriteBatch, font, "One Way Up", textColor, depth, textPos, align, align);
	}
	if (line->mOneWayRight)
	{
		textPos.Y -= HALF_TILE_SIZE;
		SpriteBatch_DrawString2(spriteBatch, font, "One Way Right", textColor, depth, textPos, align, align);
	}
	if (line->mOneWayDown)
	{
		textPos.Y -= HALF_TILE_SIZE;
		SpriteBatch_DrawString2(spriteBatch, font, "One Way Down", textColor, depth, textPos, align, align);
	}
	if (line->mOneWayLeft)
	{
		textPos.Y -= HALF_TILE_SIZE;
		SpriteBatch_DrawString2(spriteBatch, font, "One Way Left", textColor, depth, textPos, align, align);
	}
	if (line->mIsGap)
	{
		textPos.Y -= HALF_TILE_SIZE;
		SpriteBatch_DrawString2(spriteBatch, font, "Is Gap", textColor, depth, textPos, align, align);
	}
	if (line->mIsFirstLine)
	{
		textPos.Y -= HALF_TILE_SIZE;
		SpriteBatch_DrawString2(spriteBatch, font, "Is First Line", textColor, depth, textPos, align, align);
	}
	if (line->mIsLastLine)
	{
		textPos.Y -= HALF_TILE_SIZE;
		SpriteBatch_DrawString2(spriteBatch, font, "Is Last Line", textColor, depth, textPos, align, align);
	}
	if (line->mOverrideSpeedFromPlayer != 0)
	{
		textPos.Y -= HALF_TILE_SIZE;
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Override Speed From Player: ", Utils_IntToStringGlobalBuffer(line->mOverrideSpeedFromPlayer));
		SpriteBatch_DrawString2(spriteBatch, font, MString_Text(tempString), textColor, depth, textPos, align, align);
		MString_Dispose(&tempString);
	}
	if (line->mOverrideSpeedFromMinecart != 0)
	{
		textPos.Y -= HALF_TILE_SIZE;
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Override Speed From Minecart: ", Utils_IntToStringGlobalBuffer(line->mOverrideSpeedFromMinecart));
		SpriteBatch_DrawString2(spriteBatch, font, MString_Text(tempString), textColor, depth, textPos, align, align);
		MString_Dispose(&tempString);
	}
}
double Line_GetDistance(Line* line)
{
	return Math_GetDistanceEuclideanPoint(line->mBegin, line->mEnd);
}
bool Line_DoLinesConnect(Line* line1, Line* line2)
{
	if (!Point_EqualTo(Line_GetJunction(line1, line2), Points_NegativeOne))
	{
		return true;
	}
	else
	{
		return false;
	}
}
Point Line_GetJunction(Line* line1, Line* line2)
{
	if (Point_EqualTo(line1->mBegin, line2->mBegin))
	{
		return line1->mBegin;
	}
	else if (Point_EqualTo(line1->mBegin, line2->mEnd))
	{
		return line1->mBegin;
	}
	else if (Point_EqualTo(line1->mEnd, line2->mBegin))
	{
		return line1->mEnd;
	}
	else if (Point_EqualTo(line1->mEnd, line2->mEnd))
	{
		return line1->mEnd;
	}
	return Points_NegativeOne;
}
void Line_CorrectPoints(Line* line)
{
	if (!Line_IsVertical(line))
	{
		if (line->mBegin.X > line->mEnd.X)
		{
			Point temp = line->mBegin;
			line->mBegin = line->mEnd;
			line->mEnd = temp;
		}
	}
	else
	{
		if (line->mBegin.Y > line->mEnd.Y)
		{
			Point temp = line->mBegin;
			line->mBegin = line->mEnd;
			line->mEnd = temp;
		}
	}
}
bool Line_IsVertical(Line* line)
{
	return Math_GetDistanceInt(line->mBegin.X, line->mEnd.X) < Math_GetDistanceInt(line->mBegin.Y, line->mEnd.Y);
}
int32_t Line_GetReal(int32_t point)
{
	return (point * TILE_SIZE) + HALF_TILE_SIZE;
}
Point Line_GetRealPoint(Point point)
{
	Point tileSize = Points_TileSize();
	Point halfTileSize = Points_HalfTileSize();
	Point returnPoint = { 0 };
	returnPoint.X = (point.X * tileSize.X) + halfTileSize.X;
	returnPoint.Y = (point.Y * tileSize.Y) + halfTileSize.Y;
	return returnPoint;
}
Point Line_GetRealBegin(Line* line)
{
	return Line_GetRealPoint(line->mBegin);
}
Point Line_GetRealEnd(Line* line)
{
	return Line_GetRealPoint(line->mEnd);
}
void Line_Read(int32_t version, Line* line, BufferReader* br)
{
	Points_Read(&line->mBegin, br);
	Points_Read(&line->mEnd, br);
	line->mEnterDownOnly = BufferReader_ReadBoolean(br);
	line->mEnterLeftOnly = BufferReader_ReadBoolean(br);
	line->mEnterUpOnly = BufferReader_ReadBoolean(br);
	line->mEnterRightOnly = BufferReader_ReadBoolean(br);
	line->mIsGap = BufferReader_ReadBoolean(br);
	line->mIsFirstLine = BufferReader_ReadBoolean(br);
	line->mIsLastLine = BufferReader_ReadBoolean(br);
	line->mOneWayUp = BufferReader_ReadBoolean(br);
	line->mOneWayRight = BufferReader_ReadBoolean(br);
	line->mOneWayDown = BufferReader_ReadBoolean(br);
	line->mOneWayLeft = BufferReader_ReadBoolean(br);
	line->mOverrideSpeedFromPlayer = BufferReader_ReadI32(br);
	line->mOverrideSpeedFromMinecart = BufferReader_ReadI32(br);
}
void Line_Write(Line* line, DynamicByteBuffer* dbb)
{
	Points_Write(&line->mBegin, dbb);
	Points_Write(&line->mEnd, dbb);
	DynamicByteBuffer_WriteBoolean(dbb, line->mEnterDownOnly);
	DynamicByteBuffer_WriteBoolean(dbb, line->mEnterLeftOnly);
	DynamicByteBuffer_WriteBoolean(dbb, line->mEnterUpOnly);
	DynamicByteBuffer_WriteBoolean(dbb, line->mEnterRightOnly);
	DynamicByteBuffer_WriteBoolean(dbb, line->mIsGap);
	DynamicByteBuffer_WriteBoolean(dbb, line->mIsFirstLine);
	DynamicByteBuffer_WriteBoolean(dbb, line->mIsLastLine);
	DynamicByteBuffer_WriteBoolean(dbb, line->mOneWayUp);
	DynamicByteBuffer_WriteBoolean(dbb, line->mOneWayRight);
	DynamicByteBuffer_WriteBoolean(dbb, line->mOneWayDown);
	DynamicByteBuffer_WriteBoolean(dbb, line->mOneWayLeft);
	DynamicByteBuffer_WriteI32(dbb, line->mOverrideSpeedFromPlayer);
	DynamicByteBuffer_WriteI32(dbb, line->mOverrideSpeedFromMinecart);
}
Rectangle Line_GetTouchBounds(Line* line, int32_t inflation)
{
	Rectangle rect = PointRectangle_GetRectanglePoint(Line_GetRealBegin(line), Line_GetRealEnd(line));
	Rectangle_Inflate(&rect, inflation, inflation);
	return rect;
}