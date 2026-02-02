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

#include "Matrix.h"

#include "MathHelper.h"
#include "Math.h"

static Matrix _mIdentity = 
{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

Matrix Matrix_Identity(void)
{
	return _mIdentity;
}

void Matrix_Init(Matrix* value,
	float m11, float m12, float m13, float m14,
	float m21, float m22, float m23, float m24,
	float m31, float m32, float m33, float m34,
	float m41, float m42, float m43, float m44
)
{
	value->M11 = m11;
	value->M12 = m12;
	value->M13 = m13;
	value->M14 = m14;
	value->M21 = m21;
	value->M22 = m22;
	value->M23 = m23;
	value->M24 = m24;
	value->M31 = m31;
	value->M32 = m32;
	value->M33 = m33;
	value->M34 = m34;
	value->M41 = m41;
	value->M42 = m42;
	value->M43 = m43;
	value->M44 = m44;
}

void Matrix_CreateRotationZMut(float radians, Matrix* result)
{
	*result = Matrix_Identity();

	float val1 = (float)Math_cos(radians);
	float val2 = (float)Math_sin(radians);

	result->M11 = val1;
	result->M12 = val2;
	result->M21 = -val2;
	result->M22 = val1;
}
Matrix Matrix_CreateRotationZ(float radians)
{
	Matrix result;
	Matrix_CreateRotationZMut(radians, &result);
	return result;
}
void Matrix_CreateScaleMut(Vector3 scales, Matrix* result)
{
	result->M11 = scales.X;
	result->M12 = 0;
	result->M13 = 0;
	result->M14 = 0;
	result->M21 = 0;
	result->M22 = scales.Y;
	result->M23 = 0;
	result->M24 = 0;
	result->M31 = 0;
	result->M32 = 0;
	result->M33 = scales.Z;
	result->M34 = 0;
	result->M41 = 0;
	result->M42 = 0;
	result->M43 = 0;
	result->M44 = 1;
}
Matrix Matrix_CreateScale(Vector3 scales)
{
	Matrix result;
	Matrix_CreateScaleMut(scales, &result);
	return result;
}
void Matrix_CreateTranslationMut(Vector3 position, Matrix* result)
{
	result->M11 = 1;
	result->M12 = 0;
	result->M13 = 0;
	result->M14 = 0;
	result->M21 = 0;
	result->M22 = 1;
	result->M23 = 0;
	result->M24 = 0;
	result->M31 = 0;
	result->M32 = 0;
	result->M33 = 1;
	result->M34 = 0;
	result->M41 = position.X;
	result->M42 = position.Y;
	result->M43 = position.Z;
	result->M44 = 1;
}
Matrix Matrix_CreateTranslation(Vector3 position)
{
	Matrix result;
	Matrix_CreateTranslationMut(position, &result);
	return result;
}
Matrix Matrix_Multiply(Matrix matrix1, Matrix matrix2)
{
	float m11 = (
		(matrix1.M11 * matrix2.M11) +
		(matrix1.M12 * matrix2.M21) +
		(matrix1.M13 * matrix2.M31) +
		(matrix1.M14 * matrix2.M41)
		);
	float m12 = (
		(matrix1.M11 * matrix2.M12) +
		(matrix1.M12 * matrix2.M22) +
		(matrix1.M13 * matrix2.M32) +
		(matrix1.M14 * matrix2.M42)
		);
	float m13 = (
		(matrix1.M11 * matrix2.M13) +
		(matrix1.M12 * matrix2.M23) +
		(matrix1.M13 * matrix2.M33) +
		(matrix1.M14 * matrix2.M43)
		);
	float m14 = (
		(matrix1.M11 * matrix2.M14) +
		(matrix1.M12 * matrix2.M24) +
		(matrix1.M13 * matrix2.M34) +
		(matrix1.M14 * matrix2.M44)
		);
	float m21 = (
		(matrix1.M21 * matrix2.M11) +
		(matrix1.M22 * matrix2.M21) +
		(matrix1.M23 * matrix2.M31) +
		(matrix1.M24 * matrix2.M41)
		);
	float m22 = (
		(matrix1.M21 * matrix2.M12) +
		(matrix1.M22 * matrix2.M22) +
		(matrix1.M23 * matrix2.M32) +
		(matrix1.M24 * matrix2.M42)
		);
	float m23 = (
		(matrix1.M21 * matrix2.M13) +
		(matrix1.M22 * matrix2.M23) +
		(matrix1.M23 * matrix2.M33) +
		(matrix1.M24 * matrix2.M43)
		);
	float m24 = (
		(matrix1.M21 * matrix2.M14) +
		(matrix1.M22 * matrix2.M24) +
		(matrix1.M23 * matrix2.M34) +
		(matrix1.M24 * matrix2.M44)
		);
	float m31 = (
		(matrix1.M31 * matrix2.M11) +
		(matrix1.M32 * matrix2.M21) +
		(matrix1.M33 * matrix2.M31) +
		(matrix1.M34 * matrix2.M41)
		);
	float m32 = (
		(matrix1.M31 * matrix2.M12) +
		(matrix1.M32 * matrix2.M22) +
		(matrix1.M33 * matrix2.M32) +
		(matrix1.M34 * matrix2.M42)
		);
	float m33 = (
		(matrix1.M31 * matrix2.M13) +
		(matrix1.M32 * matrix2.M23) +
		(matrix1.M33 * matrix2.M33) +
		(matrix1.M34 * matrix2.M43)
		);
	float m34 = (
		(matrix1.M31 * matrix2.M14) +
		(matrix1.M32 * matrix2.M24) +
		(matrix1.M33 * matrix2.M34) +
		(matrix1.M34 * matrix2.M44)
		);
	float m41 = (
		(matrix1.M41 * matrix2.M11) +
		(matrix1.M42 * matrix2.M21) +
		(matrix1.M43 * matrix2.M31) +
		(matrix1.M44 * matrix2.M41)
		);
	float m42 = (
		(matrix1.M41 * matrix2.M12) +
		(matrix1.M42 * matrix2.M22) +
		(matrix1.M43 * matrix2.M32) +
		(matrix1.M44 * matrix2.M42)
		);
	float m43 = (
		(matrix1.M41 * matrix2.M13) +
		(matrix1.M42 * matrix2.M23) +
		(matrix1.M43 * matrix2.M33) +
		(matrix1.M44 * matrix2.M43)
		);
	float m44 = (
		(matrix1.M41 * matrix2.M14) +
		(matrix1.M42 * matrix2.M24) +
		(matrix1.M43 * matrix2.M34) +
		(matrix1.M44 * matrix2.M44)
		);
	matrix1.M11 = m11;
	matrix1.M12 = m12;
	matrix1.M13 = m13;
	matrix1.M14 = m14;
	matrix1.M21 = m21;
	matrix1.M22 = m22;
	matrix1.M23 = m23;
	matrix1.M24 = m24;
	matrix1.M31 = m31;
	matrix1.M32 = m32;
	matrix1.M33 = m33;
	matrix1.M34 = m34;
	matrix1.M41 = m41;
	matrix1.M42 = m42;
	matrix1.M43 = m43;
	matrix1.M44 = m44;
	return matrix1;
}
Matrix Matrix_Mul(const Matrix* matrix1, Matrix matrix2)
{
	return Matrix_Multiply(*matrix1, matrix2);
}