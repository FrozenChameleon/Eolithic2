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

#include "Game.h"

#include "SDL3/SDL.h"
#include "stdbool.h"
#include "Window.h"
#include "../utils/Stopwatch.h"
#include "../render/Renderer.h"
#include "../input/Input.h"
#include "../utils/Exception.h"
#include "../audio/Music.h"
#include "../audio/SoundEffect.h"
#include "../render/Sheet.h"
#include "../core/GameLoader.h"
#include "../service/Service.h"
#include "ServiceHelper.h"
#include "../utils/Cvars.h"
#include "GameUpdater.h"
#include "../globals/Globals.h"
#include "../utils/Utils.h"
#include "../gamestate/GameStateManager.h"
#include "../gamesave/GameSaveManager.h"
#include "../font/FontMap.h"
#include "../input/ControllerState.h"
#include "../utils/Logger.h"
#include "GameHelper.h"
#include "MenuFunc.h"
#include  "../audio/AudioEngine.h"
#include "../input/MouseState.h"

static const double FIXED_TIME_STEP_TICK = (1.0 / 60.0);
#define MAX_TIME_STEP_FRAMES 4
#define MAX_TIME_STEP ((1.0 / 60.0) * MAX_TIME_STEP_FRAMES)

#define INIT_STATUS_NONE 0
#define INIT_STATUS_FIRST 1
#define INIT_STATUS_SECOND 2
#define INIT_STATUS_COMPLETE 3

static bool _mIsFirstPollEvents = true;
static bool _mIsExitingGame;
static int32_t _mInitStatus;

static bool IsFixedTimeStep(void)
{
	if (GameLoader_IsLoading())
	{
		return false;
	}

	if (Service_PlatformForcesRelyOnVsync() || Cvars_GetAsBool(CVARS_ENGINE_RELY_ON_VSYNC))
	{
		return false;
	}

#ifdef EDITOR_MODE
	if (Globals_IsDebugGameSpeedSet())
	{
		return false;
	}
	if (GameUpdater_IsDebugAutoSpeedOn())
	{
		return false;
	}
#endif

	return Cvars_GetAsBool(CVARS_USER_IS_FIXED_TIMESTEP_ENABLED);
}

int32_t Game_FirstInit(void)
{
	if (_mInitStatus >= INIT_STATUS_FIRST)
	{
		return 0;
	}

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD))
	{
		return Exception_Run("Unable to init SDL!", false);
	}

	_mInitStatus = INIT_STATUS_FIRST;

	return 0;
}
int32_t Game_SecondInit(void)
{
	if (_mInitStatus >= INIT_STATUS_SECOND)
	{
		return 0;
	}

	int32_t initStatus = 0;
	initStatus = Window_Init();
	if (initStatus < 0)
	{
		return Exception_Run("Unable to init window!", false);
	}

	initStatus = Renderer_Init(Window_GetWindowContext());
	if (initStatus < 0)
	{
		return Exception_Run("Unable to init platform renderer!", false);
	}
	Renderer_InitSpriteBatch();
	Renderer_ApplyChanges();

	initStatus = AudioEngine_Init();
	if (initStatus < 0)
	{
		return Exception_Run("Unable to init platform renderer!", false);
	}

	Window_LoadIcon();

	/*OeServiceHelper::Init(); //UNUSED
	OeFunc.Init();
	OeRecordingTool.Init();
	OeTilesetOffset.Init();*/
	
	FontMap_Init();
	Input_Init();
	GameStateManager_Ctor();
	GameSaveManager_Init();
	GameHelper_Initialize();
	MenuFunc_Init();
	Music_Init();
	SoundEffect_Init();

	_mInitStatus = INIT_STATUS_SECOND;

	return initStatus;
}
int32_t Game_Run(void)
{
	uint64_t oldTicks = Stopwatch_GetTicks();
	bool isDone = false;
	double deltaLeftover = 0;
	while (!isDone)
	{
		bool isFixedTimeStep = IsFixedTimeStep();
		double delta = deltaLeftover;
		deltaLeftover = 0;
		do
		{
			uint64_t newTicks = Stopwatch_GetTicks();
			delta += Stopwatch_GetElapsedSeconds(oldTicks, newTicks);
			oldTicks = newTicks;
		} while ((delta < FIXED_TIME_STEP_TICK) && isFixedTimeStep);
		if (isFixedTimeStep)
		{
			deltaLeftover = (delta - FIXED_TIME_STEP_TICK);
			delta = FIXED_TIME_STEP_TICK;
		}
		if (delta > MAX_TIME_STEP)
		{
			delta = MAX_TIME_STEP;
		}
		if (deltaLeftover > MAX_TIME_STEP)
		{
			deltaLeftover = MAX_TIME_STEP;
		}
		Game_PollEvents();
		Game_Update(delta);
		if (!Service_SuppressDrawing())
		{
			Game_Draw(delta);
		}
		if (!isDone)
		{
			isDone = Game_IsExitingGame();
		}
	}
	Game_Dispose();
	return 0;
}
void Game_PollEvents(void) //Derived from FNA
{
	SDL_Event e;
	while (SDL_PollEvent(&e) == 1)
	{
		if (e.type == SDL_EVENT_MOUSE_WHEEL)
		{
			//Logger_LogInformationSilently("Event: mouse wheel");

			// 120 units per notch. Because reasons.
			MouseState_INTERNAL_SetScrollWheelValue((int32_t)(e.wheel.y * 120));
		}
		if (e.type == SDL_EVENT_GAMEPAD_ADDED)
		{
			Logger_LogInformationSilently("Event: gamepad added");

			ControllerState_AddControllerInstance(e.cdevice.which);
		}
		else if (e.type == SDL_EVENT_GAMEPAD_REMOVED)
		{
			Logger_LogInformationSilently("Event: gamepad removed");

			ControllerState_RemoveControllerInstance(e.cdevice.which);
		}
		else if (e.type == SDL_EVENT_WINDOW_FOCUS_GAINED) // Various Window Events...
		{
			Logger_LogInformationSilently("Event: window focus gained");

			Window_SetActive(true);

			/*const char* videoDriver = SDL_GetCurrentVideoDriver(); //UNUSED FOR NOW
			if (Utils_StringEqualTo(videoDriver, "x11"))
			{
				// If we alt-tab away, we lose the 'fullscreen desktop' flag on some WMs
				SDL_SetWindowFullscreen((SDL_Window*)(OeWindow_GetWindowContext()),
						OeWindow_IsFullscreen() ? SDL_WindowFlags_SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
			}*/

			// Disable the screensaver when we're back.
			SDL_DisableScreenSaver();
		}
		else if (e.type == SDL_EVENT_WINDOW_FOCUS_LOST)
		{
			Logger_LogInformationSilently("Event: window focus lost");

			Window_SetActive(false);

			/*const char* videoDriver = SDL_GetCurrentVideoDriver(); //UNUSED FOR NOW
			if (Utils_StringEqualTo(videoDriver, "x11"))
			{
				SDL_SetWindowFullscreen((SDL_Window*)(OeWindow_GetWindowContext()), 0);
			}*/

			// Give the screensaver back, we're not that important now.
			SDL_EnableScreenSaver();
		}
		else if (e.type == SDL_EVENT_WINDOW_RESIZED)
		{
			Logger_LogInformationSilently("Event: window resized");

			Renderer_ApplyChanges();
		}
		else if (e.type == SDL_EVENT_WINDOW_MOUSE_ENTER)
		{
			//Logger_LogInformationSilently("Event: mouse enter");

			SDL_DisableScreenSaver();
		}
		else if (e.type == SDL_EVENT_WINDOW_MOUSE_LEAVE)
		{
			//Logger_LogInformationSilently("Event: mouse leave");

			SDL_EnableScreenSaver();
		}
		else if (e.type == SDL_EVENT_QUIT)
		{
			Logger_LogInformationSilently("Event: quit");

			_mIsExitingGame = true;
		}
	}

	_mIsFirstPollEvents = false;
}
bool Game_IsExitingGame(void)
{
	return _mIsExitingGame;
}
void Game_Exit(void)
{
	_mIsExitingGame = true;
}
bool Game_IsActive(void)
{
	return Window_IsActive();
}
void Game_Update(double gameTime)
{
	Game_UpdateHelper(gameTime);
}
void Game_Draw(double gameTime)
{
#ifdef EDITOR_MODE
	if (Globals_DebugIsRenderDisabled())
	{
		return;
	}
#endif

	Renderer_Render(gameTime);
}
void Game_UpdateHelper(double delta)
{
	ServiceHelper_Update(delta);

	if (GameLoader_IsLoading())
	{
		GameLoader_Update(delta);
		Renderer_SetupRenderState();
	}

	if (!GameLoader_IsLoading())
	{
		GameUpdater_Update(delta);
	}
}
void Game_Dispose(void)
{
	Window_Dispose();
	SDL_Quit();
}
