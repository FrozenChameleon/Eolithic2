/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"

#define ACTIONLIST_LENGTH 30

typedef struct IStringArray IStringArray;

const char* ActionList_GetAction(int32_t index);
IStringArray* ActionList_GetArray(void);

extern const char* ACTIONLIST_GAME_A;
extern const char* ACTIONLIST_GAME_B;
extern const char* ACTIONLIST_GAME_X;
extern const char* ACTIONLIST_GAME_Y;
extern const char* ACTIONLIST_GAME_LB;
extern const char* ACTIONLIST_GAME_LT;
extern const char* ACTIONLIST_GAME_LS;
extern const char* ACTIONLIST_GAME_RB;
extern const char* ACTIONLIST_GAME_RT;
extern const char* ACTIONLIST_GAME_RS;
extern const char* ACTIONLIST_GAME_START;
extern const char* ACTIONLIST_GAME_SELECT;
extern const char* ACTIONLIST_GAME_LS_UP;
extern const char* ACTIONLIST_GAME_LS_RIGHT;
extern const char* ACTIONLIST_GAME_LS_DOWN;
extern const char* ACTIONLIST_GAME_LS_LEFT;
extern const char* ACTIONLIST_GAME_RS_UP;
extern const char* ACTIONLIST_GAME_RS_RIGHT;
extern const char* ACTIONLIST_GAME_RS_DOWN;
extern const char* ACTIONLIST_GAME_RS_LEFT;
extern const char* ACTIONLIST_GAME_DIGITAL_UP;
extern const char* ACTIONLIST_GAME_DIGITAL_RIGHT;
extern const char* ACTIONLIST_GAME_DIGITAL_DOWN;
extern const char* ACTIONLIST_GAME_DIGITAL_LEFT;
extern const char* ACTIONLIST_GAME_MENU_SELECT;
extern const char* ACTIONLIST_GAME_MENU_BACK;
extern const char* ACTIONLIST_GAME_MENU_UP;
extern const char* ACTIONLIST_GAME_MENU_RIGHT;
extern const char* ACTIONLIST_GAME_MENU_DOWN;
extern const char* ACTIONLIST_GAME_MENU_LEFT;