/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "Vector2.h"
#include "Point.h"

const extern Vector2 Vectors_MinValue;
const extern Vector2 Vectors_MaxValue;
const extern Vector2 Vectors_NegativeOne;
const extern Vector2 Vectors_Up;
const extern Vector2 Vectors_UpRight;
const extern Vector2 Vectors_Right;
const extern Vector2 Vectors_DownRight;
const extern Vector2 Vectors_Down;
const extern Vector2 Vectors_DownLeft;
const extern Vector2 Vectors_Left;
const extern Vector2 Vectors_UpLeft;

Vector2 Vectors_ToVector2(Point point);

