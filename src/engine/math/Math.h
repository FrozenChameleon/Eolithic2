/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"
#include "Point.h"
#include "Vector2.h"

extern const double MATH_TICK_15HZ;
extern const double MATH_TICK_30HZ;
extern const double MATH_TICK_60HZ;
extern const float MATH_DIAGONAL_LIMITER_MUL;
extern const double MATH_PI;
extern const double MATH_HALFPI;
extern const double MATH_TWOPI;

double Math_cos(double x);
double Math_sin(double x);
double Math_sqrt(double x);
int32_t Math_SignumInt(int32_t value);
int32_t Math_SignumFloat(float value);
int32_t Math_SignumDouble(double value);
float Math_ToDegrees(float value);
float Math_ToRadians(float value);
int32_t Math_abs(int32_t value);
float Math_fabsf(float value);
double Math_fabs(double value);
float Math_ceilf(float value);
double Math_ceil(double value);
float Math_floorf(float value);
double Math_floor(double value);
double Math_atan(double x);
double Math_atan2(double y, double x);
int32_t Math_MinInt(int32_t valueA, int32_t valueB);
float Math_MinFloat(float valueA, float valueB);
double Math_MinDouble(double valueA, double valueB);
int32_t Math_MaxInt(int32_t valueA, int32_t valueB);
float Math_MaxFloat(float valueA, float valueB);
double Math_MaxDouble(double valueA, double valueB);
int32_t Math_GetDistanceInt(int32_t x1, int32_t x2);
float Math_GetDistanceFloat(float x1, float x2);
double Math_GetDistanceDouble(double x1, double x2);
double Math_GetDistanceEuclideanPoint(Point begin, Point end);
double Math_GetDistanceEuclideanVector2(Vector2 begin, Vector2 end);
double Math_GetDistanceEuclideanFloat(float x1, float y1, float x2, float y2);
double Math_pow(double x, double y);
int32_t Math_ClampInt(int32_t value, int32_t lowerBound, int32_t upperBound);
float Math_ClampFloat(float value, float lowerBound, float upperBound);
float Math_GetSlope(float x1, float y1, float x2, float y2);
float Math_GetIntercept(float x, float y, float slope);
float Math_GetSlopeInterceptXPoint(Point begin, Point end, float y);
float Math_GetSlopeInterceptXVector2(Vector2 begin, Vector2 end, float y);
float Math_GetSlopeInterceptX(float x1, float y1, float x2, float y2, float y);
float Math_GetSlopeInterceptYPoint(Point begin, Point end, float x);
float Math_GetSlopeInterceptYVector2(Vector2 begin, Vector2 end, float x);
float Math_GetSlopeInterceptY(float x1, float y1, float x2, float y2, float x);
double Math_GetLineDistance(float x1, float y1, float x2, float y2);
double Math_GetAngle(float x1, float y1, float x2, float y2);
double Math_GetVectorFromRadianAngleX(double radianAngle);
double Math_GetVectorFromRadianAngleY(double radianAngle);
double Math_log10(double value);
