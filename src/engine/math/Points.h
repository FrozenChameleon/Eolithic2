/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "Point.h"
#include "Vector2.h"

typedef struct BufferReader BufferReader;

enum EightWay
{
	POINTS_EIGHTWAY_UP = 0,
	POINTS_EIGHTWAY_UPRIGHT = 1,
	POINTS_EIGHTWAY_RIGHT = 2,
	POINTS_EIGHTWAY_DOWNRIGHT = 3,
	POINTS_EIGHTWAY_DOWN = 4,
	POINTS_EIGHTWAY_DOWNLEFT = 5,
	POINTS_EIGHTWAY_LEFT = 6,
	POINTS_EIGHTWAY_UPLEFT = 7,
};

enum FourWay
{
	POINTS_FOURWAY_UP = 0,
	POINTS_FOURWAY_RIGHT = 1,
	POINTS_FOURWAY_DOWN = 2,
	POINTS_FOURWAY_LEFT = 3,
};

extern const Point Points_MinValue;
extern const Point Points_MaxValue;
extern const Point Points_NegativeOne;
extern const Point Points_Up;
extern const Point Points_UpRight;
extern const Point Points_Right;
extern const Point Points_DownRight;
extern const Point Points_Down;
extern const Point Points_DownLeft;
extern const Point Points_Left;
extern const Point Points_UpLeft;

Point Points_TileSize(void);
Point Points_HalfTileSize(void);
Point Points_ToPointFromInt(int32_t value);
Point Points_ToPointFromVector2(Vector2 value);
Point Points_GetEightWay(int32_t direction);
Point Points_GetFourWay(int32_t direction);
void Points_Rotate(Point* point, int32_t degree);
double Points_GetAngle(Point point);
void Points_ReverseX(Point* point);
void Points_ReverseY(Point* point);
void Points_Reverse(Point* point);
float Points_GetDegree(Point point);
int32_t Points_GetFourWayInt(Point point);
float Points_GetDiagCorrectedX(Point point);
float Points_GetDiagCorrectedY(Point point);
void Points_SetDirectionFromFourWayInt(Point* point, int32_t direction);
void Points_SetDirectionFromEightWayInt(Point* point, int32_t direction);
void Points_RotateClockwise(Point* point, int32_t amount);
void Points_RotateCounterClockwise(Point* point, int32_t amount);
void Points_SetUp(Point* point);
void Points_SetUpRight(Point* point);
void Points_SetRight(Point* point);
void Points_SetDownRight(Point* point);
void Points_SetDown(Point* point);
void Points_SetDownLeft(Point* point);
void Points_SetLeft(Point* point);
void Points_SetUpLeft(Point* point);
bool Points_IsDiagonal(Point point);
bool Points_IsUp(Point point);
bool Points_IsUpRight(Point point);
bool Points_IsRight(Point point);
bool Points_IsDownRight(Point point);
bool Points_IsDown(Point point);
bool Points_IsDownLeft(Point point);
bool Points_IsLeft(Point point);
bool Points_IsUpLeft(Point point);
int32_t Points_GetEightWayInt(Point point);
void Points_SetFromDegree(Point* point, int32_t degree);
void Points_Read(Point* point, BufferReader* reader);