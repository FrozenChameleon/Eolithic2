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

#include "Vector2.h"

#include "MathHelper.h"
#include "Math.h"

const Vector2 Vector2_Zero = { 0, 0 };
const Vector2 Vector2_One = { 1, 1 };

Vector2 Vector2_Create(float x, float y)
{
	Vector2 vec;
	vec.X = x;
	vec.Y = y;
	return vec;
}
Vector2 Vector2_Create2(float value)
{
	Vector2 vec;
	vec.X = value;
	vec.Y = value;
	return vec;
}
void Vector2_Init(Vector2* value, float x, float y)
{
	value->X = x;
	value->Y = y;
}
void Vector2_InitFromFloat(Vector2* value, float value2)
{
	value->X = value2;
	value->Y = value2;
}
float Vector2_Distance(Vector2 value1, Vector2 value2)
{
	float v1 = value1.X - value2.X;
	float v2 = value1.Y - value2.Y;
	double distance = Math_sqrt((v1 * v1) + (v2 * v2));
	return (float)distance;
}
Vector2 Vector2_Lerp(Vector2 value1, Vector2 value2, float amount)
{
	Vector2 tempVec;
	tempVec.X = MathHelper_Lerp(value1.X, value2.X, amount);
	tempVec.Y = MathHelper_Lerp(value1.Y, value2.Y, amount);
	return tempVec;
}
float Vector2_Length(Vector2 value)
{
	double length = Math_sqrt((value.X * value.X) + (value.Y * value.Y));
	return (float)length;
}
void Vector2_Normalize(Vector2* value)
{
	double length = Math_sqrt((value->X * value->X) + (value->Y * value->Y));
	float val = (1.0f / (float)length);
	value->X *= val;
	value->Y *= val;
}
bool Vector2_EqualTo(Vector2 value1, Vector2 value2)
{
	return ((value1.X == value2.X) && (value1.Y == value2.Y));
}
bool Vector2_NotEqual(Vector2 value1, Vector2 value2)
{
	return !Vector2_EqualTo(value1, value2);
}
void Vector2_MulAssign(Vector2* value1, Vector2 value2)
{
	value1->X *= value2.X;
	value1->Y *= value2.Y;
}
void Vector2_AddAssign(Vector2* value1, Vector2 value2)
{
	value1->X += value2.X;
	value1->Y += value2.Y;
}
void Vector2_SubAssign(Vector2* value1, Vector2 value2)
{
	value1->X -= value2.X;
	value1->Y -= value2.Y;
}
Vector2 Vector2_Add(Vector2 value1, Vector2 value2)
{
	Vector2 temp;
	temp.X = (value1.X + value2.X);
	temp.Y = (value1.Y + value2.Y);
	return temp;
}
Vector2 Vector2_Sub(Vector2 value1, Vector2 value2)
{
	Vector2 temp;
	temp.X = (value1.X - value2.X);
	temp.Y = (value1.Y - value2.Y);
	return temp;
}
Vector2 Vector2_Mul(Vector2 value1, Vector2 value2)
{
	Vector2 temp;
	temp.X = (value1.X * value2.X);
	temp.Y = (value1.Y * value2.Y);
	return temp;
}
Vector2 Vector2_Div(Vector2 value1, Vector2 value2)
{
	Vector2 temp;
	temp.X = (value1.X / value2.X);
	temp.Y = (value1.Y / value2.Y);
	return temp;
}
Vector2 Vector2_DivInt(Vector2 value1, int32_t value2)
{
	Vector2 temp;
	temp.X = (value1.X / value2);
	temp.Y = (value1.Y / value2);
	return temp;
}
Vector2 Vector2_MulInt(Vector2 value1, int32_t value2)
{
	value1.X *= value2;
	value1.Y *= value2;
	return value1;
}
Vector2 Vector2_MulFloat(Vector2 value1, float value2)
{
	value1.X *= value2;
	value1.Y *= value2;
	return value1;
}
Vector2 Vector2_MulDouble(Vector2 value1, double value2)
{
	value1.X = (float)(value1.X * value2);
	value1.Y = (float)(value1.Y * value2);
	return value1;
}
Vector2 Vector2_DivFloat(Vector2 value1, float value2)
{
	value1.X /= value2;
	value1.Y /= value2;
	return value1;
}
