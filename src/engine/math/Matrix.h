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

#include "Vector3.h"

typedef struct Matrix
{
	float M11;
	float M12;
	float M13;
	float M14;
	float M21;
	float M22;
	float M23;
	float M24;
	float M31;
	float M32;
	float M33;
	float M34;
	float M41;
	float M42;
	float M43;
	float M44;
} Matrix;

Matrix Matrix_Identity(void);

void Matrix_Init(Matrix* value,
	float m11, float m12, float m13, float m14,
	float m21, float m22, float m23, float m24,
	float m31, float m32, float m33, float m34,
	float m41, float m42, float m43, float m44
);

void Matrix_CreateRotationZMut(float radians, Matrix* result);
Matrix Matrix_CreateRotationZ(float radians);
void Matrix_CreateScaleMut(Vector3 scales, Matrix* result);
Matrix Matrix_CreateScale(Vector3 scales);
void Matrix_CreateTranslationMut(Vector3 position, Matrix* result);
Matrix Matrix_CreateTranslation(Vector3 position);
Matrix Matrix_Multiply(Matrix matrix1, Matrix matrix2);
Matrix Matrix_Mul(const Matrix* matrix1, Matrix matrix2);
Matrix Matrix_CreateOrthographicOffCenter(
	float left,
	float right,
	float bottom,
	float top,
	float zNearPlane,
	float zFarPlane
);