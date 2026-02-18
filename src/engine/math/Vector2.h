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

typedef struct Vector2
{
	float X;
	float Y;
} Vector2;

const extern Vector2 Vector2_Zero;
const extern Vector2 Vector2_One;

Vector2 Vector2_Create(float x, float y);
Vector2 Vector2_Create2(float value);
void Vector2_Init(Vector2* value, float x, float y);
void Vector2_InitFromFloat(Vector2* value, float value2);
float Vector2_Distance(Vector2 value1, Vector2 value2);
Vector2 Vector2_Lerp(Vector2 value1, Vector2 value2, float amount);
float Vector2_Length(Vector2 value);
void Vector2_Normalize(Vector2* value);
bool Vector2_EqualTo(Vector2 value1, Vector2 value2);
bool Vector2_NotEqual(Vector2 value1, Vector2 value2);
void Vector2_MulAssign(Vector2* value1, Vector2 value2);
void Vector2_AddAssign(Vector2* value1, Vector2 value2);
void Vector2_SubAssign(Vector2* value1, Vector2 value2);
Vector2 Vector2_Add(Vector2 value1, Vector2 value2);
Vector2 Vector2_Sub(Vector2 value1, Vector2 value2);
Vector2 Vector2_Mul(Vector2 value1, Vector2 value2);
Vector2 Vector2_Div(Vector2 value1, Vector2 value2);
Vector2 Vector2_DivInt(Vector2 value1, int32_t value2);
Vector2 Vector2_MulInt(Vector2 value1, int32_t value2);
Vector2 Vector2_MulFloat(Vector2 value1, float value2);
Vector2 Vector2_MulDouble(Vector2 value1, double value2);
Vector2 Vector2_DivFloat(Vector2 value1, float value2);