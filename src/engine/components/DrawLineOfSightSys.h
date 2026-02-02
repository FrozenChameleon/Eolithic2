/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../systems/System.h"
#include "DrawLineOfSight.h"

typedef struct SpriteBatch SpriteBatch;

#ifdef EDITOR_MODE
void DrawLineOfSightSys_DrawRoutine(Entity owner, DrawLineOfSight* data, SpriteBatch* spriteBatch);
#endif
System* DrawLineOfSightSys_CreateSystem(void);
