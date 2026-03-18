/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

 /* Derived from code by Ethan Lee (Copyright 2009-2024).
  * Released under the Microsoft Public License.
  * See fna.LICENSE for details.

  * Derived from code by the Mono.Xna Team (Copyright 2006).
  * Released under the MIT License.
  * See monoxna.LICENSE for details.
  */

#pragma once

#include "stdint.h"
#include "stdbool.h"
#include "Point.h"

typedef struct Rectangle
{
	int32_t X;
	int32_t Y;
	int32_t Width;
	int32_t Height;
} Rectangle;

const extern Rectangle Rectangle_Empty;

Rectangle Rectangle_Create(int32_t x, int32_t y, int32_t width, int32_t height);
void Rectangle_Init(Rectangle* value, int32_t x, int32_t y, int32_t width, int32_t height);
bool Rectangle_EqualTo(Rectangle value1, Rectangle value2);
bool Rectangle_NotEqual(Rectangle value1, Rectangle value2);
void Rectangle_Inflate(Rectangle* value, int32_t horizontalValue, int32_t verticalValue);
Point Rectangle_Center(Rectangle value);
int32_t Rectangle_Left(Rectangle value);
int32_t Rectangle_Right(Rectangle value);
int32_t Rectangle_Top(Rectangle value);
int32_t Rectangle_Bottom(Rectangle value);
bool Rectangle_Intersects(Rectangle value1, Rectangle value2);
bool Rectangle_Contains(Rectangle value, int32_t x, int32_t y);
bool Rectangle_ContainsPoint(Rectangle value1, Point value2);
bool Rectangle_ContainsRectangle(Rectangle value1, Rectangle value2);
bool Rectangle_IsEmpty(Rectangle value);
Rectangle Rectangle_Intersect(Rectangle value1, Rectangle value2);