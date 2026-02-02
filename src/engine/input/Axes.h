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

extern const float AXES_ANALOG_MUTE_DEADZONE;

enum
{
	AXES_AMOUNT_OF_AXIS = 6
};

typedef enum Axis
{
	AXES_ANALOG_LEFT_STICK_X = 0,
	AXES_ANALOG_LEFT_STICK_Y = 1,
	AXES_ANALOG_RIGHT_STICK_X = 2,
	AXES_ANALOG_RIGHT_STICK_Y = 3,
	AXES_ANALOG_LEFT_TRIGGER = 4,
	AXES_ANALOG_RIGHT_TRIGGER = 5
} Axis;