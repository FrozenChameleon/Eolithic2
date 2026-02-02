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

typedef struct Vector3
{
	float X;
	float Y;
	float Z;
} Vector3;

void Vector3_InitFromFloat(Vector3* vector, float value);
void Vector3_Init(Vector3* vector, float x, float y, float z);
Vector3 Vector3_Normalize(const Vector3* value);
Vector3 Vector3_Cross(const Vector3* vector1, const Vector3* vector2);
void Vector3_CrossMut(const Vector3* vector1, const Vector3* vector2, Vector3* result);
float Vector3_Dot(const Vector3* vector1, const Vector3* vector2);
void Vector3_DotMut(const Vector3* vector1, const Vector3* vector2, float* result);
Vector3 Vector3_Sub(const Vector3* vector1, const Vector3* vector2);