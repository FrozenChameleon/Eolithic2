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

void Vector3_Init(Vector3* vector, float x, float y, float z);
void Vector3_Init2(Vector3* vector, float value);
Vector3 Vector3_Normalize(Vector3 value);
Vector3 Vector3_Cross(Vector3 vector1, Vector3 vector2);
float Vector3_Dot(Vector3 vector1, Vector3 vector2);
Vector3 Vector3_Sub(Vector3 vector1, Vector3 vector2);