/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Math.h"

#include "SDL3/SDL.h"
#include "MathHelper.h"
#include "../utils/Logger.h"

#define VALUE_OF_PI 3.14159265358979323846

const double MATH_TICK_15HZ = (1.0 / 15.0);
const double MATH_TICK_30HZ = (1.0 / 30.0);
const double MATH_TICK_60HZ = (1.0 / 60.0);
const float MATH_DIAGONAL_LIMITER_MUL = 0.7071f;
const double MATH_PI = VALUE_OF_PI;
const double MATH_HALFPI = (VALUE_OF_PI / 2.0);
const double MATH_TWOPI = (VALUE_OF_PI * 2.0);

double Math_cos(double x)
{
	return SDL_cos(x);
}
double Math_sin(double x)
{
	return SDL_sin(x);
}
double Math_sqrt(double x)
{
	return SDL_sqrt(x);
}
float Math_sqrtf(float x)
{
	return SDL_sqrtf(x);
}
int32_t Math_SignumInt(int32_t value)
{
	if (value > 0)
	{
		return 1;
	}
	else if (value < 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
int32_t Math_SignumFloat(float value)
{
	if (value > 0)
	{
		return 1;
	}
	else if (value < 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
int32_t Math_SignumDouble(double value)
{
	if (value > 0)
	{
		return 1;
	}
	else if (value < 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
float Math_ToDegrees(float value)
{
	return MathHelper_ToDegrees(value);
}
float Math_ToRadians(float value)
{
	return MathHelper_ToRadians(value);
}
int32_t Math_abs(int32_t value)
{
	return SDL_abs(value);
}
float Math_fabsf(float value)
{
	return SDL_fabsf(value);
}
double Math_fabs(double value)
{
	return SDL_fabs(value);
}
float Math_ceilf(float value)
{
	return SDL_ceilf(value);
}
double Math_ceil(double value)
{
	return SDL_ceil(value);
}
float Math_floorf(float value)
{
	return SDL_floorf(value);
}
double Math_floor(double value)
{
	return SDL_floor(value);
}
double Math_round(double value)
{
	return SDL_round(value);
}
double Math_atan(double x)
{
	return SDL_atan(x);
}
double Math_atan2(double y, double x)
{
	return SDL_atan2(y, x);
}
int32_t Math_MinInt(int32_t valueA, int32_t valueB)
{
	if (valueA > valueB)
	{
		return valueB;
	}
	return valueA;
}
float Math_MinFloat(float valueA, float valueB)
{
	if (valueA > valueB)
	{
		return valueB;
	}
	return valueA;
}
double Math_MinDouble(double valueA, double valueB)
{
	if (valueA > valueB)
	{
		return valueB;
	}
	return valueA;
}
int32_t Math_MaxInt(int32_t valueA, int32_t valueB)
{
	if (valueA < valueB)
	{
		return valueB;
	}
	return valueA;
}
float Math_MaxFloat(float valueA, float valueB)
{
	if (valueA < valueB)
	{
		return valueB;
	}
	return valueA;
}
double Math_MaxDouble(double valueA, double valueB)
{
	if (valueA < valueB)
	{
		return valueB;
	}
	return valueA;
}
int32_t Math_GetDistanceInt(int32_t x1, int32_t x2)
{
	return Math_abs(x1 - x2);
}
float Math_GetDistanceFloat(float x1, float x2)
{
	return Math_fabsf(x1 - x2);
}
double Math_GetDistanceDouble(double x1, double x2)
{
	return Math_fabs(x1 - x2);
}
double Math_GetDistanceEuclideanPoint(Point begin, Point end)
{
	return Math_GetDistanceEuclideanFloat((float)begin.X, (float)begin.Y, (float)end.X, (float)end.Y);
}
double Math_GetDistanceEuclideanVector2(Vector2 begin, Vector2 end)
{
	return Math_GetDistanceEuclideanFloat(begin.X, begin.Y, end.X, end.Y);
}
double Math_GetDistanceEuclideanFloat(float x1, float y1, float x2, float y2)
{
	float x_dis = Math_fabsf(x1 - x2);
	float y_dis = Math_fabsf(y1 - y2);
	return Math_sqrt((x_dis * x_dis) + (y_dis * y_dis));
}
double Math_pow(double x, double y)
{
	return SDL_pow(x, y);
}
int32_t Math_ClampInt(int32_t value, int32_t lowerBound, int32_t upperBound)
{
	value = Math_MinInt(value, upperBound);
	value = Math_MaxInt(value, lowerBound);
	return value;
}
float Math_ClampFloat(float value, float lowerBound, float upperBound)
{
	value = Math_MinFloat(value, upperBound);
	value = Math_MaxFloat(value, lowerBound);
	return value;
}
float Math_GetSlope(float x1, float y1, float x2, float y2)
{
	float rise = y1 - y2;
	float run = x1 - x2;
	if (run == 0) //Undefined slope
	{
		Logger_LogError("Undefined slope!");
	}
	float m = rise / run;
	return m;
}
float Math_GetIntercept(float x, float y, float slope)
{
	float b = (slope * x) - y;
	return -b;
}
float Math_GetSlopeInterceptXPoint(Point begin, Point end, float y)
{
	return Math_GetSlopeInterceptX((float)begin.X, (float)begin.Y, (float)end.X, (float)end.Y, y);
}
float Math_GetSlopeInterceptXVector2(Vector2 begin, Vector2 end, float y)
{
	return Math_GetSlopeInterceptX(begin.X, begin.Y, end.X, end.Y, y);
}
float Math_GetSlopeInterceptX(float x1, float y1, float x2, float y2, float y)
{
	float m = Math_GetSlope(x1, y1, x2, y2);
	float b = Math_GetIntercept(x1, y1, m);
	return (y / m) - (b / m);
}
float Math_GetSlopeInterceptYPoint(Point begin, Point end, float x)
{
	return Math_GetSlopeInterceptY((float)begin.X, (float)begin.Y, (float)end.X, (float)end.Y, x);
}
float Math_GetSlopeInterceptYVector2(Vector2 begin, Vector2 end, float x)
{
	return Math_GetSlopeInterceptY(begin.X, begin.Y, end.X, end.Y, x);
}
float Math_GetSlopeInterceptY(float x1, float y1, float x2, float y2, float x)
{
	float m = Math_GetSlope(x1, y1, x2, y2);
	float b = Math_GetIntercept(x1, y1, m);
	return (m * x) + b;
}
double Math_GetLineDistance(float x1, float y1, float x2, float y2)
{
	return Math_sqrt(Math_pow(x2 - x1, 2) + Math_pow(y2 - y1, 2));
}
double Math_GetAngle(float x1, float y1, float x2, float y2)
{
	return Math_atan2(y2 - y1, x2 - x1);
}
double Math_GetVectorFromRadianAngleX(double radianAngle)
{
	return Math_cos(radianAngle);
}
double Math_GetVectorFromRadianAngleY(double radianAngle)
{
	return Math_sin(radianAngle);
}
double Math_log10(double value)
{
	return log10(value);
}
