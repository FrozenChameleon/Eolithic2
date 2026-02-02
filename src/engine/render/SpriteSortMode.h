/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

typedef enum SpriteSortMode
{
	SPRITESORTMODE_DEFERRED = 0,
	SPRITESORTMODE_IMMEDIATE = 1,
	SPRITESORTMODE_TEXTURE = 2,
	SPRITESORTMODE_BACKTOFRONT = 3,
	SPRITESORTMODE_FRONTTOBACK = 4
} SpriteSortMode;

