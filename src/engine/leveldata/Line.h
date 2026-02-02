/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Point.h"
#include "../math/Vector2.h"
#include "../render/Color.h"
#include "../math/Rectangle.h"
#include "stdint.h"
#include "stdbool.h"

typedef struct SpriteBatch SpriteBatch;
typedef struct BufferReader BufferReader;

typedef struct Line
{
	Point mBegin;
	Point mEnd;
	bool mEnterDownOnly;
	bool mEnterLeftOnly;
	bool mEnterUpOnly;
	bool mEnterRightOnly;
	bool mIsGap;
	bool mIsFirstLine;
	bool mIsLastLine;
	bool mOneWayUp;
	bool mOneWayRight;
	bool mOneWayDown;
	bool mOneWayLeft;
	int32_t mOverrideSpeedFromPlayer;
	int32_t mOverrideSpeedFromMinecart;
} Line;

Point Line_GetMiddle(Point begin, Point end);
void Line_DrawLineCamera(Line* line, SpriteBatch* spriteBatch, const char* font, Color color, int32_t cameraWidth, int32_t cameraHeight);
void Line_DrawLine(Line* line, SpriteBatch* spriteBatch, const char* font, Color color, int32_t lineNumber, int32_t offset);
void Line_DrawLineText(Line* line, SpriteBatch* spriteBatch, const char* font, int32_t lineNumber, Vector2 pos);
double Line_GetDistance(Line* line);
bool Line_DoLinesConnect(Line* line1, Line* line2);
Point Line_GetJunction(Line* line1, Line* line2);
void Line_CorrectPoints(Line* line);
bool Line_IsVertical(Line* line);
int32_t Line_GetReal(int32_t point);
Point Line_GetRealPoint(Point point);
Point Line_GetRealBegin(Line* line);
Point Line_GetRealEnd(Line* line);
//void Write(Line* line, std::shared_ptr<OeIniWriter> writer); //UNUSED
void Line_Read(int32_t version, Line* line, BufferReader* reader);
Rectangle Line_GetTouchBounds(Line* line, int32_t inflation);
