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

float MathHelper_ToDegrees(float radians);
float MathHelper_ToRadians(float degrees);
float MathHelper_GetMachineEpsilonFloat(void);
float MathHelper_Clamp(float value, float min, float max);
float MathHelper_Lerp(float value1, float value2, float amount);