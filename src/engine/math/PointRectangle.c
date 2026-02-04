/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "PointRectangle.h"

#include "../math/Math.h"
#include "../math/Points.h"
#include "../math/RectangleTools.h"

bool PointRectangle_IsEmpty(PointRectangle* prect)
{
	if (Point_EqualTo(prect->mPointOne, Point_Zero) && Point_EqualTo(prect->mPointTwo, Point_Zero))
	{
		return true;
	}
	return false;
}
void PointRectangle_CorrectPoints(PointRectangle* prect)
{
	if (prect->mPointTwo.X < prect->mPointOne.X)
	{
		int32_t tempX = prect->mPointOne.X;
		prect->mPointOne.X = prect->mPointTwo.X;
		prect->mPointTwo.X = tempX;
	}
	if (prect->mPointTwo.Y < prect->mPointOne.Y)
	{
		int32_t tempY = prect->mPointOne.Y;
		prect->mPointOne.Y = prect->mPointTwo.Y;
		prect->mPointTwo.Y = tempY;
	}
}
void PointRectangle_Read(PointRectangle* prect, BufferReader* br)
{
	Points_Read(&prect->mPointOne, br);
	Points_Read(&prect->mPointTwo, br);
	Rectangle dummy = { 0 }; //To maintain compatibility
	RectangleTools_Read(&dummy, br);
}
void PointRectangle_Write(PointRectangle* prect, DynamicByteBuffer* dbb)
{
	Points_Write(&prect->mPointOne, dbb);
	Points_Write(&prect->mPointTwo, dbb);
	Rectangle dummy = { 0 }; //To maintain compatibility
	RectangleTools_Write(&dummy, dbb);
}
Rectangle PointRectangle_GetRectangle(PointRectangle* prect)
{
	return PointRectangle_GetRectanglePoint(prect->mPointOne, prect->mPointTwo);
}
Rectangle PointRectangle_GetRectanglePoint(Point pointOne, Point pointTwo)
{
	int32_t diffX = pointTwo.X - pointOne.X;
	int32_t diffY = pointTwo.Y - pointOne.Y;

	int32_t x = diffX < 0 ? pointTwo.X : pointOne.X;
	int32_t y = diffY < 0 ? pointTwo.Y : pointOne.Y;

	int32_t width = Math_abs(diffX);
	int32_t height = Math_abs(diffY);

	Rectangle tempRect = { x, y, width, height };
	return tempRect;
}
void PointRectangle_Add(PointRectangle* prect, int32_t amountX, int32_t amountY)
{
	Point temp = { amountX, amountY };
	Point_AddAssign(&prect->mPointOne, temp);
	Point_AddAssign(&prect->mPointTwo, temp);
}