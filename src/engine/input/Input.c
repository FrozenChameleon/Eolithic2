/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Input.h"

#include "InputPlayer.h"
#include "MouseState.h"
#include "KeyboardState.h"
#include "InputAction.h"
#include "ControllerState.h"
#include "ControllerStates.h"
#include "../math/Math.h"
#include "InputBindings.h"
#include "../core/Game.h"
#include "../service/Service.h"
#include "../components/Camera.h"
#include "../utils/Cvars.h"
#include "../globals/Globals.h"
#include "../render/Renderer.h"
#include "../utils/Logger.h"

typedef struct DoubleClickData
{
	int32_t mTimeSinceLastTap;
	bool mIsTapped;
	bool mIsDoubleClicked;
} DoubleClickData;

typedef struct OverrideDeadzoneData
{
	bool mIsOverriding;
	int32_t mRadialDeadzone;
	int32_t mOuterDeadzone;
	int32_t mAxialDeadzone;
} OverrideDeadzoneData;

bool INPUT_FLAG_IS_ON_MENU = false;
bool INPUT_FLAG_IS_ON_MULTIPLAYER_SETUP_MENU = false;
bool INPUT_FLAG_IS_PLAYING_MASTER_RECORDING_FAST = false;

static bool _mHasInit;
static InputPlayer _mPlayers[INPUT_MAXIMUM_PLAYER_COUNT];
static DoubleClickData _mLeftDoubleClick;
static OverrideDeadzoneData _mOverrideDeadzoneData;
static int32_t _mBlockMKBInputForFrames;
static int32_t _mMasterRecordingState;

static void HandleDoubleClickData(DoubleClickData* data, bool tapValue)
{
	data->mIsDoubleClicked = false;
	data->mIsTapped = tapValue;
	if (data->mIsTapped)
	{
		if (data->mTimeSinceLastTap < INPUTACTION_DOUBLE_TAP_LENGTH)
		{
			data->mIsDoubleClicked = true;
		}
		data->mTimeSinceLastTap = 0;
	}
	else
	{
		data->mTimeSinceLastTap += 1;
	}
}
static bool IsStickBeyondOuterDeadzone(Vector2 stick, float outerDeadzone)
{
	return (Vector2_Length(stick) > (1 - outerDeadzone));
}
static Vector2 ProcessStickRadialDeadzone(Vector2 stick, float radialDeadzone)
{
	if (Vector2_Length(stick) < radialDeadzone)
	{
		return Vector2_Zero;
	}
	else
	{
		return stick;
	}
}
static Vector2 ProcessStickAxialDeadzone(Vector2 stick, float axialDeadzone)
{
	if (Math_fabsf(stick.X) < axialDeadzone)
	{
		stick.X = 0;
	}
	if (Math_fabsf(stick.Y) < axialDeadzone)
	{
		stick.Y = 0;
	}
	return stick;
}

void Input_Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	InputPlayer_Init(&_mPlayers[0], 0);
	InputPlayer_Init(&_mPlayers[1], 1);
	InputPlayer_Init(&_mPlayers[2], 2);
	InputPlayer_Init(&_mPlayers[3], 3);

	MouseState_Init();
	KeyboardState_Init();
	ControllerStates_Init();

	InputBindings_Init();

	InputBindings_SyncAllBindingsToAllPlayers();

	_mHasInit = true;
}
bool Input_HasInit(void)
{
	return _mHasInit;
}

void Input_MuteAllInput(void)
{
	ControllerStates_MuteInput();
	KeyboardState_MuteInput();
	MouseState_MuteInput();
}
bool Input_IsAnythingPressedByAnyPlayer(void)
{
	int32_t amountOfActiveInput = Input_GetAmountOfActivePlayers();
	for (int32_t i = 0; i < amountOfActiveInput; i += 1)
	{
		if (InputPlayer_IsPlayerPressingAnything(&_mPlayers[i]))
		{
			return true;
		}
	}
	if (Input_IsLeftMousePressed())
	{
		return true;
	}
	if (Input_IsMiddleMousePressed())
	{
		return true;
	}
	if (Input_IsRightMousePressed())
	{
		return true;
	}
	return false;
}
bool Input_IsPressedByAnyPlayer(const char* action)
{
	int32_t amountOfActiveInput = Input_GetAmountOfActivePlayers();
	for (int32_t i = 0; i < amountOfActiveInput; i += 1)
	{
		if (Input_GetPlayerAction(i, action)->mIsPressed)
		{
			return true;
		}
	}
	return false;
}
bool Input_IsReleasedByAnyPlayer(const char* action)
{
	int32_t amountOfActiveInput = Input_GetAmountOfActivePlayers();
	for (int32_t i = 0; i < amountOfActiveInput; i += 1)
	{
		if (Input_GetPlayerAction(i, action)->mIsReleased)
		{
			return true;
		}
	}
	return false;
}
bool Input_IsTappedByAnyPlayer(const char* action)
{
	int32_t amountOfActiveInput = Input_GetAmountOfActivePlayers();
	for (int32_t i = 0; i < amountOfActiveInput; i += 1)
	{
		if (Input_GetPlayerAction(i, action)->mIsTapped)
		{
			return true;
		}
	}
	return false;
}
int32_t Input_GetLongestTimeHeldByAnyPlayer(const char* action)
{
	int32_t amountOfActiveInput = Input_GetAmountOfActivePlayers();
	int32_t longestTimeHeld = 0;
	for (int32_t i = 0; i < amountOfActiveInput; i += 1)
	{
		int32_t timeHeld = Input_GetPlayerAction(i, action)->mTimeHeld;
		if (Input_GetPlayerAction(i, action)->mTimeHeld > longestTimeHeld)
		{
			longestTimeHeld = timeHeld;
		}
	}
	return longestTimeHeld;
}
int32_t Input_GetAmountOfActivePlayers(void)
{
	int32_t amount = 1;
	for (int32_t i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
	{
		if (i != 0)
		{
			if (InputPlayer_IsInputDeviceSet(&_mPlayers[i]))
			{
				amount += 1;
			}
		}
	}
	return amount;
}
void Input_Update(bool doNotUpdateMasterRecording)
{
	if (!_mHasInit)
	{
		Logger_LogError("Need to init input before update!");
		return;
	}

	ControllerState_BeforePollingUpdate();

	MouseState_Poll();
	KeyboardState_Poll();
	ControllerStates_Poll();

	if (!doNotUpdateMasterRecording)
	{
		if (Input_IsPlayingMasterRecording())
		{
			//ControllerStates_Read(); //Overwrite polled controller input with master recording if reading it //UNUSED
		}

		if (Input_IsRecordingMasterRecording())
		{
			//ControllerStates_Write(); //UNUSED
		}
	}

	if (_mBlockMKBInputForFrames > 0)
	{
		MouseState_ClearOnlyButtonData();
		KeyboardState_ClearEverything();
		_mBlockMKBInputForFrames -= 1;
	}

	if (Input_IsSinglePlayerInputMode())
	{
		InputPlayer_DetectInputDevice(&_mPlayers[INPUT_PLAYER_1], false);
	}

	for (int32_t i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
	{
		InputPlayer_Update(&_mPlayers[i]);
	}

	HandleDoubleClickData(&_mLeftDoubleClick, Input_IsLeftMouseTapped());
}
bool Input_IsAnyPlayerUsingThisDeviceNumber(int32_t deviceNumber)
{
	for (int32_t i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
	{
		if (InputPlayer_GetDeviceNumber(&_mPlayers[i]) == deviceNumber)
		{
			return true;
		}
	}
	return false;
}
bool Input_IsSimultaneousInputForKeyboard(void)
{
	if (!Input_IsSinglePlayerInputMode())
	{
		return false;
	}

	return true;
}
bool Input_IsSimultaneousInputForController(void)
{
	if (!Input_IsSinglePlayerInputMode())
	{
		return false;
	}

	if (Service_PlatformDisablesSimultaneousInputForController() && !Input_IsRecordingOrPlayingMasterRecording())
	{
		return false;
	}

	return true;
}
bool Input_IsSinglePlayerInputMode(void)
{
	if (INPUT_FLAG_IS_ON_MULTIPLAYER_SETUP_MENU)
	{
		return false;
	}

	if (Input_GetAmountOfActivePlayers() > 1)
	{
		return false;
	}

	return true;
}
bool Input_IsMultiplayerInputMode(void)
{
	return !Input_IsSinglePlayerInputMode();
}
void Input_BlockMKBInputForFrames(int32_t frames)
{
	_mBlockMKBInputForFrames = frames;
}
InputPlayer* Input_GetPlayerOne(void)
{
	return &_mPlayers[0];
}
InputPlayer* Input_GetPlayer(int32_t i)
{
	return &_mPlayers[i];
}
ControllerState* Input_GetController(int32_t number)
{
	return ControllerStates_GetController(number);
}
InputAction* Input_GetPlayerOneAction(const char* name)
{
	return Input_GetPlayerAction(0, name);
}
InputAction* Input_GetPlayerAction(int32_t playerNumber, const char* name)
{
	return InputPlayer_GetAction(&_mPlayers[playerNumber], name);
}
Vector2 Input_GetCameraAdjustedMouse(const Camera* camera)
{
	Vector2 temp;
	temp.X = (Input_GetMouseX() * camera->mWorldZoom) + Camera_GetLeftFloat(camera);
	temp.Y = (Input_GetMouseY() * camera->mWorldZoom) + Camera_GetTopFloat(camera);
	return temp;
}
bool Input_JustScrolledUp(void)
{
	return MouseState_JustScrolledUp();
}
bool Input_JustScrolledDown(void)
{
	return MouseState_JustScrolledDown();
}
float Input_GetDifferenceMouseX(void)
{
	return MouseState_GetDifferenceMouseX();
}
float Input_GetDifferenceMouseY(void)
{
	return MouseState_GetDifferenceMouseY();
}
float Input_GetMouseX(void)
{
	return MouseState_GetMouseX();
}
float Input_GetMouseY(void)
{
	return MouseState_GetMouseY();
}
Vector2 Input_GetMouse(void)
{
	Vector2 temp;
	temp.X = Input_GetMouseX();
	temp.Y = Input_GetMouseY();
	return temp;
}
Vector2 Input_GetScaledMouseForRetroScreen(void)
{
	float mouseX = Input_GetMouseX();
	float mouseY = Input_GetMouseY();
	Point screenOffset = Renderer_GetScreenOffset();
	Vector2 retroScreenScale = Renderer_GetScreenScale();
	return Vector2_Create((mouseX - screenOffset.X) * retroScreenScale.X,
		(mouseY - screenOffset.Y) * retroScreenScale.Y);
}
float Input_GetScaledMouseForRetroScreenX(void)
{
	return Input_GetScaledMouseForRetroScreen().X;
}
float Input_GetScaledMouseForRetroScreenY(void)
{
	return Input_GetScaledMouseForRetroScreen().X;
}
Vector2 Input_GetCameraAdjustedMouseForRetroScreen(const Camera* camera)
{
	Vector2 temp;
	temp.X = (Input_GetScaledMouseForRetroScreenX() * camera->mWorldZoom) + Camera_GetLeftFloat(camera);
	temp.Y = (Input_GetScaledMouseForRetroScreenY() * camera->mWorldZoom) + Camera_GetTopFloat(camera);
	return temp;
}
float Input_GetCameraAdjustedMouseForRetroScreenX(const Camera* camera)
{
	Vector2 cameraAdjustedMouseForRetroScreen = Input_GetCameraAdjustedMouseForRetroScreen(camera);
	return cameraAdjustedMouseForRetroScreen.X;
}
float Input_GetCameraAdjustedMouseForRetroScreenY(const Camera* camera)
{
	Vector2 cameraAdjustedMouseForRetroScreen = Input_GetCameraAdjustedMouseForRetroScreen(camera);
	return cameraAdjustedMouseForRetroScreen.Y;
}
bool Input_MouseHasChangedPosition(void)
{
	if ((Input_GetDifferenceMouseX() == 0) && (Input_GetDifferenceMouseY() == 0))
	{
		return false;
	}
	return true;
}
bool Input_IsLeftMousePressed(void)
{
	return MouseState_IsButtonPressed(MOUSEBUTTONS_MOUSE_LEFTBUTTON);
}
bool Input_IsLeftMouseTapped(void)
{
	return MouseState_IsButtonTapped(MOUSEBUTTONS_MOUSE_LEFTBUTTON);
}
bool Input_IsLeftMouseReleased(void)
{
	return MouseState_IsButtonReleased(MOUSEBUTTONS_MOUSE_LEFTBUTTON);
}
bool Input_IsLeftMouseDoubleClicked(void)
{
	return _mLeftDoubleClick.mIsDoubleClicked;
}
bool Input_IsRightMousePressed(void)
{
	return MouseState_IsButtonPressed(MOUSEBUTTONS_MOUSE_RIGHTBUTTON);
}
bool Input_IsRightMouseTapped(void)
{
	return MouseState_IsButtonTapped(MOUSEBUTTONS_MOUSE_RIGHTBUTTON);
}
bool Input_IsRightMouseReleased(void)
{
	return MouseState_IsButtonReleased(MOUSEBUTTONS_MOUSE_RIGHTBUTTON);
}
bool Input_IsMiddleMousePressed(void)
{
	return MouseState_IsButtonPressed(MOUSEBUTTONS_MOUSE_MIDDLEBUTTON);
}
bool Input_IsMiddleMouseTapped(void)
{
	return MouseState_IsButtonTapped(MOUSEBUTTONS_MOUSE_MIDDLEBUTTON);
}
bool Input_IsMiddleMouseReleased(void)
{
	return MouseState_IsButtonReleased(MOUSEBUTTONS_MOUSE_MIDDLEBUTTON);
}
bool Input_IsCtrlPressed(void)
{
	return KeyboardState_IsKeyPressed(KEYS_LEFTCONTROL) || KeyboardState_IsKeyPressed(KEYS_RIGHTCONTROL);
}
bool Input_IsTabPressed(void)
{
	return KeyboardState_IsKeyPressed(KEYS_TAB);
}
bool Input_IsShiftPressed(void)
{
	return KeyboardState_IsKeyPressed(KEYS_LEFTSHIFT) || KeyboardState_IsKeyPressed(KEYS_RIGHTSHIFT);
}
bool Input_IsShiftTapped(void)
{
	return KeyboardState_IsKeyTapped(KEYS_LEFTSHIFT) || KeyboardState_IsKeyTapped(KEYS_RIGHTSHIFT);
}
bool Input_IsAltPressed(void)
{
	return KeyboardState_IsKeyPressed(KEYS_LEFTALT) || KeyboardState_IsKeyPressed(KEYS_RIGHTALT);
}
bool Input_IsDeletePressed(void)
{
	return KeyboardState_IsKeyPressed(KEYS_DELETE);
}
bool Input_IsDeleteTapped(void)
{
	return KeyboardState_IsKeyTapped(KEYS_DELETE);
}
bool Input_IsDeleteReleased(void)
{
	return KeyboardState_IsKeyReleased(KEYS_DELETE);
}
bool Input_IsTildePressed(void)
{
	return KeyboardState_IsKeyPressed(KEYS_OEMTILDE);
}
bool Input_IsTildeReleased(void)
{
	return KeyboardState_IsKeyReleased(KEYS_OEMTILDE);
}
bool Input_IsEscapePressed(void)
{
	return KeyboardState_IsKeyPressed(KEYS_ESCAPE);
}
bool Input_IsEscapeTapped(void)
{
	return KeyboardState_IsKeyTapped(KEYS_ESCAPE);
}
bool Input_IsEnterTapped(void)
{
	return KeyboardState_IsKeyTapped(KEYS_ENTER);
}
bool Input_IsModifierPressed(void)
{
	return Input_IsCtrlPressed() || Input_IsShiftPressed() || Input_IsAltPressed();
}
void Input_StopAllRumble(void)
{
	for (int32_t i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
	{
		InputPlayer_Vibrate(&_mPlayers[i], 999999, 2, 0, 0);
	}
}
Vector2 Input_ProcessStickDeadzones(Vector2 stick, float radialDeadzone, float axialDeadzone, float outerDeadzone)
{
	stick = ProcessStickRadialDeadzone(stick, radialDeadzone);
	stick = ProcessStickAxialDeadzone(stick, axialDeadzone);
	if (Vector2_EqualTo(stick, Vector2_Zero))
	{
		return Vector2_Zero;
	}

	if (IsStickBeyondOuterDeadzone(stick, outerDeadzone))
	{
		Vector2_Normalize(&stick);
		return stick;
	}

	Vector2 direction = stick;
	Vector2_Normalize(&direction);
	float stickLength = Vector2_Length(stick);
	float range = 1 - radialDeadzone - outerDeadzone;
	stick = Vector2_MulFloat(direction, ((stickLength - radialDeadzone) / range));

	if (IsStickBeyondOuterDeadzone(stick, 0))
	{
		Vector2_Normalize(&stick);
	}

	return stick;
}
void Input_OverrideDeadzones(int32_t radial, int32_t outer, int32_t axial)
{
	_mOverrideDeadzoneData.mIsOverriding = true;
	_mOverrideDeadzoneData.mRadialDeadzone = radial;
	_mOverrideDeadzoneData.mOuterDeadzone = outer;
	_mOverrideDeadzoneData.mAxialDeadzone = axial;
}
int32_t Input_GetRadialDeadzone(void)
{
	if (_mOverrideDeadzoneData.mIsOverriding)
	{
		return _mOverrideDeadzoneData.mRadialDeadzone;
	}
	return Cvars_GetAsInt(CVARS_ENGINE_CONTROLLER_RADIAL_DEADZONE);
}
int32_t Input_GetOuterDeadzone(void)
{
	if (_mOverrideDeadzoneData.mIsOverriding)
	{
		return _mOverrideDeadzoneData.mOuterDeadzone;
	}
	return Cvars_GetAsInt(CVARS_ENGINE_CONTROLLER_OUTER_DEADZONE);
}
int32_t Input_GetAxialDeadzone(void)
{
	if (_mOverrideDeadzoneData.mIsOverriding)
	{
		return _mOverrideDeadzoneData.mAxialDeadzone;
	}
	return Cvars_GetAsInt(CVARS_ENGINE_CONTROLLER_AXIAL_DEADZONE);
}
float Input_GetRadialDeadzoneAsFloat(void)
{
	return (Input_GetRadialDeadzone() / 100.0f);
}
float Input_GetOuterDeadzoneAsFloat(void)
{
	return (Input_GetOuterDeadzone() / 100.0f);
}
float Input_GetAxialDeadzoneAsFloat(void)
{
	return (Input_GetAxialDeadzone() / 100.0f);
}
float Input_GetAnalogToPressThresholdForSticks(void)
{
	return Cvars_GetAsInt(CVARS_ENGINE_CONTROLLER_ANALOG_TO_PRESS_THRESHOLD_FOR_STICKS) / 100.0f;
}
float Input_GetAnalogToPressThresholdForTriggers(void)
{
	return Cvars_GetAsInt(CVARS_ENGINE_CONTROLLER_ANALOG_TO_PRESS_THRESHOLD_FOR_TRIGGERS) / 100.0f;
}
bool Input_IsRecordingMasterRecording(void)
{
	return _mMasterRecordingState == INPUT_MASTER_RECORDING_STATE_RECORDING;
}
bool Input_IsPlayingMasterRecording(void)
{
	return _mMasterRecordingState == INPUT_MASTER_RECORDING_STATE_PLAYBACK;
}
bool Input_IsRecordingOrPlayingMasterRecording(void)
{
	return Input_IsRecordingMasterRecording() || Input_IsPlayingMasterRecording();
}
void Input_SetMasterRecordingState(int32_t state)
{
	if (state != INPUT_MASTER_RECORDING_STATE_OFF)
	{
		Globals_DisableSavingAndLoadingAndAchievementsAndLeaderboards();
	}

	_mMasterRecordingState = state;
}
#ifdef EDITOR_MODE
bool Input_IsKeyTapped(Key key)
{
	return KeyboardState_IsKeyTapped(key);
}
bool Input_IsKeyReleased(Key key)
{
	return KeyboardState_IsKeyReleased(key);
}
bool Input_IsKeyPressed(Key key)
{
	return KeyboardState_IsKeyPressed(key);
}
int32_t Input_GetKeyTimeHeld(Key key)
{
	return KeyboardState_GetTimeHeld(key);
}
#endif