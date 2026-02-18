/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdbool.h"

typedef struct SpriteBatch SpriteBatch;

void GameLoader_Update(double deltaTime);
void GameLoader_Draw(SpriteBatch* spriteBatch);
bool GameLoader_IsLoading(void);