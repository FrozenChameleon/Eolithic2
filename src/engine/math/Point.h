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

typedef struct Point
{
	int32_t X;
	int32_t Y;
} Point;

const extern Point Point_Zero;

Point Point_Create(int32_t x, int32_t y);
void Point_Init(Point* point, int32_t x, int32_t y);
bool Point_EqualTo(Point value1, Point value2);
bool Point_NotEqual(Point value1, Point value2);
void Point_AddAssign(Point* value1, Point value2);
void Point_SubAssign (Point* value1, Point value2);
Point Point_Add(Point value1, Point value2);
Point Point_Sub(Point value1, Point value2);
Point Point_Mul(Point value1, Point value2);
Point Point_Div(Point value1, Point value2);

