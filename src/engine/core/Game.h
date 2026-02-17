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

int32_t Game_FirstInit(void);
int32_t Game_SecondInit(void);
int32_t Game_Run(void);
void Game_PollEvents(void);
bool Game_IsExitingGame(void);
void Game_Exit(void);
bool Game_IsActive(void);
void Game_Update(double gameTime);
void Game_Draw(double gameTime);
void Game_Dispose(void);