/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"

const char* GameUpdater_GetFpsString(void);
float GameUpdater_GetAutoSpeedUpdateSpeed(void);
int32_t GameUpdater_GetFPS(void);
bool GameUpdater_IsInterpolated(void);
void GameUpdater_Update(double delta);
void GameUpdater_DebugReloadMap(void);
void GameUpdater_DebugSaveMap(void);
void GameUpdater_FastReset(void);
void GameUpdater_FastResetPlusMove(void);
void GameUpdater_ToggleEditor(void);
void GameUpdater_ToggleDebugAutoSpeed(void);
void GameUpdater_DebugReloadGraphics(void);
void GameUpdater_CycleDebugShowInfo(void);
uint64_t GameUpdater_GetGlobalTicks(void);
bool GameUpdater_IsDebugAutoSpeedOn(void);
double GameUpdater_GetLastDelta(void);
double GameUpdater_GetDeltaAccumulator(void);
uint64_t GameUpdater_GetSkippedFrames(void);