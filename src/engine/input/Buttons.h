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

#include "stdint.h"
#include "stdbool.h"

#define BUTTONS_AMOUNT_OF_BUTTONS 14

typedef enum Button
{
	BUTTONS_DPADUP = 1,
	BUTTONS_DPADDOWN = 2,
	BUTTONS_DPADLEFT = 4,
	BUTTONS_DPADRIGHT = 8,
	BUTTONS_START = 16,
	BUTTONS_BACK = 32,
	BUTTONS_LEFTSTICK = 64,
	BUTTONS_RIGHTSTICK = 128,
	BUTTONS_LEFTSHOULDER = 256,
	BUTTONS_RIGHTSHOULDER = 512,
	BUTTONS_A = 4096,
	BUTTONS_B = 8192,
	BUTTONS_X = 16384,
	BUTTONS_Y = 32768,
} Button;

Button Buttons_GetButton(int32_t index);
const Button* Buttons_GetArray(void);
int32_t Buttons_GetArrayLength(void);