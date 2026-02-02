/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Vector2.h"
#include "MovieImage.h"

void MoviePlayer_Init(bool useSwappedImages, int32_t scale, const char* movieName);

void MoviePlayer_DisableSpeedUp(void);
void MoviePlayer_SetComplete(void);
bool MoviePlayer_IsComplete(void);
void MoviePlayer_Update(void);
void MoviePlayer_Update2(bool doNotAllowMovieSkip);
void MoviePlayer_DrawHud(SpriteBatch* spriteBatch);