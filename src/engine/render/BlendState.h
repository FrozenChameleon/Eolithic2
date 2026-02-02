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

typedef enum BlendState
{
	BLENDSTATE_NONPREMULTIPLIED = 0,
	BLENDSTATE_ADDITIVE = 1,
	BLENDSTATE_ALPHABLEND = 2,
	BLENDSTATE_INVALID = -1
} BlendState;