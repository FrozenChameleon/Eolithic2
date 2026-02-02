/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"

void GameSaveManager_Init(void);
bool GameSaveManager_HasInit(void);
void* GameSaveManager_GetCurrentSaveData(void);
void GameSaveManager_ResetSaveData(void);
void GameSaveManager_Save(void);
void GameSaveManager_Load(void);
bool GameSaveManager_HasLoaded(void);
void GameSaveManager_ResetWasSaveDataJustLoaded(void);
bool GameSaveManager_WasSaveDataJustLoaded(void);
