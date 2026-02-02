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

#include "Vector3.h"

#include "MathHelper.h"
#include "Math.h"

void Vector3_InitFromFloat(Vector3* vector, float value)
{
	vector->X = value;
	vector->Y = value;
	vector->Z = value;
}
void Vector3_Init(Vector3* vector, float x, float y, float z)
{
	vector->X = x;
	vector->Y = y;
	vector->Z = z;
}
Vector3 Vector3_Normalize(const Vector3* value)
{
	float factor = 1.0f / (float)Math_sqrt(
		(value->X * value->X) +
		(value->Y * value->Y) +
		(value->Z * value->Z)
	);
	Vector3 temp;
	temp.X = (value->X * factor);
	temp.Y = (value->Y * factor);
	temp.Z = (value->Z * factor);
	return temp;
}
Vector3 Vector3_Cross(const Vector3* vector1, const Vector3* vector2)
{
	Vector3 vector3;
	Vector3_CrossMut(vector1, vector2, &vector3);
	return vector3;
}
void Vector3_CrossMut(const Vector3* vector1, const Vector3* vector2, Vector3* result)
{
	float x = (vector1->Y * vector2->Z - vector2->Y * vector1->Z);
	float y = -(vector1->X * vector2->Z - vector2->X * vector1->Z);
	float z = (vector1->X * vector2->Y - vector2->X * vector1->Y);
	result->X = x;
	result->Y = y;
	result->Z = z;
}
float Vector3_Dot(const Vector3* vector1, const Vector3* vector2)
{
	return (vector1->X * vector2->X + vector1->Y * vector2->Y + vector1->Z * vector2->Z);
}
void Vector3_DotMut(const Vector3* vector1, const Vector3* vector2, float* result)
{
	float dotProduct = ((vector1->X * vector2->X) +
		(vector1->Y * vector2->Y) +
		(vector1->Z * vector2->Z));
	*result = dotProduct;
}
Vector3 Vector3_Sub(const Vector3* vector1, const Vector3* vector2)
{
	Vector3 temp;
	temp.X = (vector1->X - vector2->X);
	temp.Y = (vector1->Y - vector2->Y);
	temp.Z = (vector1->Z - vector2->Z);
	return temp;
}