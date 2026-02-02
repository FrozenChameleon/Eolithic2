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

enum
{
	BUTTONS_AMOUNT_OF_BUTTONS = 14,
};

typedef enum Button
{
	BUTTONS_DPadUp = 1,
	BUTTONS_DPadDown = 2,
	BUTTONS_DPadLeft = 4,
	BUTTONS_DPadRight = 8,
	BUTTONS_Start = 16,
	BUTTONS_Back = 32,
	BUTTONS_LeftStick = 64,
	BUTTONS_RightStick = 128,
	BUTTONS_LeftShoulder = 256,
	BUTTONS_RightShoulder = 512,
	BUTTONS_A = 4096,
	BUTTONS_B = 8192,
	BUTTONS_X = 16384,
	BUTTONS_Y = 32768,
} Button;
