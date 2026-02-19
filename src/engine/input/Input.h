/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdbool.h"
#include "stdint.h"
#include "../math/Vector2.h"
#include "../math/Rectangle.h"
#include "../math/Point.h"
#include "../components/Camera.h"
#include "Keys.h"

typedef struct InputPlayer InputPlayer;
typedef struct ControllerState ControllerState;
typedef struct InputAction InputAction;

enum MasterRecordingState
{
	INPUT_MASTER_RECORDING_STATE_OFF = 0,
	INPUT_MASTER_RECORDING_STATE_RECORDING = 1,
	INPUT_MASTER_RECORDING_STATE_PLAYBACK = 2
};

enum ControllerGlyph
{
	INPUT_CONTROLLER_GLYPH_X = 0,
	INPUT_CONTROLLER_GLYPH_P4 = 1,
	INPUT_CONTROLLER_GLYPH_P5 = 2,
	INPUT_CONTROLLER_GLYPH_N = 3
};

enum PlayerNumbers
{
	INPUT_PLAYER_1 = 0,
	INPUT_PLAYER_2 = 1,
	INPUT_PLAYER_3 = 2,
	INPUT_PLAYER_4 = 3
};

enum
{
	INPUT_MAXIMUM_PLAYER_COUNT = 4
};

extern bool INPUT_FLAG_IS_ON_MENU;
extern bool INPUT_FLAG_IS_ON_MULTIPLAYER_SETUP_MENU;
extern bool INPUT_FLAG_IS_PLAYING_MASTER_RECORDING_FAST;

void Input_Init(void);
bool Input_HasInit(void);

void Input_MuteAllInput(void);
bool Input_IsAnythingPressedByAnyPlayer(void);
bool Input_IsPressedByAnyPlayer(const char* action);
bool Input_IsReleasedByAnyPlayer(const char* action);
bool Input_IsTappedByAnyPlayer(const char* action);
int32_t Input_GetLongestTimeHeldByAnyPlayer(const char* action);
int32_t Input_GetAmountOfActivePlayers(void);
void Input_Update(bool doNotUpdateMasterRecording);
bool Input_IsAnyPlayerUsingThisDeviceNumber(int32_t deviceNumber);
bool Input_IsSimultaneousInputForKeyboard(void);
bool Input_IsSimultaneousInputForController(void);
bool Input_IsSinglePlayerInputMode(void);
bool Input_IsMultiplayerInputMode(void);
void Input_BlockMKBInputForFrames(int32_t frames);
InputPlayer* Input_GetPlayerOne(void);
InputPlayer* Input_GetPlayer(int32_t i);
ControllerState* Input_GetController(int32_t number);
InputAction* Input_GetPlayerOneAction(const char* name);
InputAction* Input_GetPlayerAction(int32_t playerNumber, const char* name);
Vector2 Input_GetCameraAdjustedMouse(const Camera* camera);
bool Input_JustScrolledUp(void);
bool Input_JustScrolledDown(void);
float Input_GetDifferenceMouseX(void);
float Input_GetDifferenceMouseY(void);
float Input_GetMouseX(void);
float Input_GetMouseY(void);
Vector2 Input_GetMouse(void);
Vector2 Input_GetScaledMouseForRetroScreen(void);
float Input_GetScaledMouseForRetroScreenX(void);
float Input_GetScaledMouseForRetroScreenY(void);
Vector2 Input_GetCameraAdjustedMouseForRetroScreen(const Camera* camera);
float Input_GetCameraAdjustedMouseForRetroScreenX(const Camera* camera);
float Input_GetCameraAdjustedMouseForRetroScreenY(const Camera* camera);
bool Input_MouseHasChangedPosition(void);
bool Input_IsLeftMousePressed(void);
bool Input_IsLeftMouseTapped(void);
bool Input_IsLeftMouseReleased(void);
bool Input_IsLeftMouseDoubleClicked(void);
bool Input_IsRightMousePressed(void);
bool Input_IsRightMouseTapped(void);
bool Input_IsRightMouseReleased(void);
bool Input_IsMiddleMousePressed(void);
bool Input_IsMiddleMouseTapped(void);
bool Input_IsMiddleMouseReleased(void);
bool Input_IsCtrlPressed(void);
bool Input_IsCtrlPressed2(bool respectForceInvertCtrl);
bool Input_IsTabPressed(void);
bool Input_IsShiftPressed(void);
bool Input_IsShiftTapped(void);
bool Input_IsAltPressed(void);
bool Input_IsDeletePressed(void);
bool Input_IsDeleteTapped(void);
bool Input_IsDeleteReleased(void);
bool Input_IsTildePressed(void);
bool Input_IsTildeReleased(void);
bool Input_IsEscapePressed(void);
bool Input_IsEscapeTapped(void);
bool Input_IsEnterTapped(void);
bool Input_IsModifierPressed(void);
void Input_StopAllRumble(void);
Vector2 Input_ProcessStickDeadzones(Vector2 stick, float radialDeadzone, float axialDeadzone, float outerDeadzone);
void Input_OverrideDeadzones(int32_t radial, int32_t outer, int32_t axial);
int32_t Input_GetRadialDeadzone(void);
int32_t Input_GetOuterDeadzone(void);
int32_t Input_GetAxialDeadzone(void);
float Input_GetRadialDeadzoneAsFloat(void);
float Input_GetOuterDeadzoneAsFloat(void);
float Input_GetAxialDeadzoneAsFloat(void);
float Input_GetAnalogToPressThresholdForSticks(void);
float Input_GetAnalogToPressThresholdForTriggers(void);
bool Input_IsRecordingMasterRecording(void);
bool Input_IsPlayingMasterRecording(void);
bool Input_IsRecordingOrPlayingMasterRecording(void);
void Input_SetMasterRecordingState(int32_t state);
#ifdef EDITOR_MODE
bool Input_IsKeyTapped(Key key);
bool Input_IsKeyReleased(Key key);
bool Input_IsKeyPressed(Key key);
int32_t Input_GetKeyTimeHeld(Key key);
#endif