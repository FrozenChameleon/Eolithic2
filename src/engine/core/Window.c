/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Window.h"

#include "../utils/Macros.h"
#include "SDL3/SDL.h"
#include "Game.h"
#include "../utils/Cvars.h"
#include "../utils/Logger.h"
#include "../globals/Globals.h"
#include "../render/Texture.h"
#include "../render/Renderer.h"
#include "../input/ControllerState.h"
#include "../service/Service.h"
#include "../io/File.h"
#include "../utils/Utils.h"
#include "../../third_party/stb_ds.h"
#ifdef RENDER_FNA3D
#include <FNA3D.h>
#endif

static Rectangle* arr_display_modes_bounds;
static bool _mHasWindowInit;
static bool _mHasLoadedIcon;
static bool _mIsWindowActive;
static SDL_Window* _mWindowContext;

static SDL_DisplayID GetCurrentDisplayID(void)
{
	return SDL_GetDisplayForWindow(_mWindowContext);
}

static bool IsFullscreenRightNow(void)
{
#ifdef DEBUG_DEF_FORCE_FULLSCREEN_OFF
	return false;
#elif DEBUG_DEF_FORCE_FULLSCREEN_ON
	return true;
#else
	bool isFullscreen = Window_IsFullscreen();

	if (!_mHasWindowInit && Service_PlatformForcesWindowedModeOnBoot())
	{
		isFullscreen = false;
	}

	if (Service_PlatformForcesFullscreen())
	{
		isFullscreen = true;
	}

	return isFullscreen;
#endif
}
static uint32_t GetWindowFlagForFullscreen(void)
{
	if (IsFullscreenRightNow())
	{
		return SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN;
	}
	else
	{
		return SDL_WINDOW_RESIZABLE;
	}
}

static Rectangle GetProposedWindowBounds(void)
{
	Rectangle returnRect = { 0 };

	Rectangle forcedWindowSize = Service_PlatformGetForcedWindowSize();
	if (!Rectangle_IsEmpty(&forcedWindowSize))
	{
		returnRect.Width = forcedWindowSize.Width;
		returnRect.Height = forcedWindowSize.Height;
		return returnRect;
	}

	int32_t windowSizeWidthFromCvar = Cvars_GetAsInt(CVARS_USER_WINDOW_SIZE_WIDTH);
	int32_t windowSizeHeightFromCvar = Cvars_GetAsInt(CVARS_USER_WINDOW_SIZE_HEIGHT);
	if ((windowSizeWidthFromCvar > 0) && (windowSizeHeightFromCvar > 0)) //Fixed resolutions from CVAR
	{
		returnRect.Width = windowSizeWidthFromCvar;
		returnRect.Height = windowSizeHeightFromCvar;
		return returnRect;
	}
	else //Scaler resolutions from CVAR
	{
		int32_t windowMul = Cvars_GetAsInt("usr_window_size_multiple");
#ifdef DEBUG_DEF_FORCED_WINDOW_MUL
		if (DEBUG_DEF_FORCED_WINDOW_MUL > 0)
		{
			windowMul = DEBUG_DEF_FORCED_WINDOW_MUL;
		}
#endif
		returnRect.Width = Cvars_GetAsInt("internal_render_width") * windowMul;
		returnRect.Height = Cvars_GetAsInt("internal_render_height") * windowMul;
		return returnRect;
	}
}

int32_t Window_Init(void)
{
	if (_mHasWindowInit)
	{
		Logger_LogWarning("Window has already init");
		return 0;
	}

	_mIsWindowActive = true;

	const char* gameName = Cvars_GetAsString("name");

	uint32_t flags = GetWindowFlagForFullscreen();
	flags = flags | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;
#ifdef RENDER_FNA3D
	//SDL_SetHint("FNA3D_FORCE_DRIVER", "OpenGL");
	//SDL_SetHint("FNA3D_OPENGL_FORCE_CORE_PROFILE", "1");
	flags = flags | FNA3D_PrepareWindowAttributes();
#endif

	Rectangle proposedWindowBounds = GetProposedWindowBounds();
	_mWindowContext = SDL_CreateWindow(gameName, proposedWindowBounds.Width, proposedWindowBounds.Height, flags);

	_mHasWindowInit = true;

	if (_mWindowContext == NULL)
	{
		Logger_LogError("Unable to init window!");
		return -1;
	}

	return 0;
}
bool Window_IsFullscreen(void)
{
	return Cvars_GetAsBool(CVARS_USER_IS_FULLSCREEN);
}
void Window_UpdateFullscreen(void)
{
	Rectangle proposedWindowBounds = GetProposedWindowBounds();
	SDL_SetWindowSize(_mWindowContext, proposedWindowBounds.Width, proposedWindowBounds.Height);
	SDL_SetWindowFullscreen(_mWindowContext, IsFullscreenRightNow());
}
void Window_SetWindowPositionToCentered(void)
{
	SDL_SetWindowPosition(_mWindowContext, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}
void* Window_GetWindowContext(void)
{
	return _mWindowContext;
}
void Window_Dispose(void)
{
	SDL_DestroyWindow(_mWindowContext);
	_mWindowContext = NULL;
}
bool Window_IsTheMouseVisible(void)
{
	return SDL_CursorVisible();
}
void Window_SetTheMouseVisible(bool isVisible)
{
	if (isVisible)
	{
		SDL_ShowCursor();
	}
	else
	{
		SDL_HideCursor();
	}
}
Rectangle Window_GetWindowSize(void)
{
	Rectangle rect = Rectangle_Empty;
	SDL_GetWindowSize(_mWindowContext, &rect.Width, &rect.Height);
	return rect;
}
Rectangle Window_GetDisplayBounds(void)
{
	Rectangle returnRect = { 0 };
	SDL_Rect displayBounds = { 0 };
	int32_t success = SDL_GetDisplayBounds(GetCurrentDisplayID(), &displayBounds);
	if (success < 0)
	{
		Logger_LogInformation("Unable to get display bounds");
		returnRect.Width = 1280;
		returnRect.Height = 720;
		return returnRect;
	}
	returnRect.X = displayBounds.x;
	returnRect.Y = displayBounds.y;
	returnRect.Width = displayBounds.w;
	returnRect.Height = displayBounds.h;
	return returnRect;
}
Rectangle* Window_GetAllDisplayModeBounds(int32_t* length)
{
	if (arr_display_modes_bounds != NULL)
	{
		*length = (int32_t)arrlen(arr_display_modes_bounds);
		return arr_display_modes_bounds;
	}
	
	int32_t count;
	SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(GetCurrentDisplayID(), &count);
	if (modes == NULL)
	{
		Logger_LogInformation("Unable to get num display modes");
		return NULL;
	}

	for (int32_t i = 0; i < count; i += 1)
	{
		SDL_DisplayMode* displayMode = modes[i];
		arrput(arr_display_modes_bounds, Rectangle_Create(0, 0, displayMode->w, displayMode->h));
	}

	*length = (int32_t)arrlen(arr_display_modes_bounds);
	return arr_display_modes_bounds;
}
bool Window_IsActive(void)
{
	return _mIsWindowActive;
}
void Window_SetActive(bool value)
{
	_mIsWindowActive = value;
}
void Window_LoadIcon(void)
{
	if (_mHasLoadedIcon)
	{
		Logger_LogWarning("Window icon has already been loaded");
		return;
	}

	_mHasLoadedIcon = true;

	if (!Service_PlatformShouldLoadWindowIcon())
	{
		Logger_LogWarning("Platform does not load window icon");
		return;
	}

	const char* gameName = Cvars_GetAsString(CVARS_ENGINE_NAME);
	SDL_Surface* icon = NULL;

	{
		MString* sharedPath = NULL;
		File_PathCombine2(&sharedPath, File_GetBasePath(), gameName);
		MString_AddAssignString(&sharedPath, ".bmp");
		icon = SDL_LoadBMP(MString_Text(sharedPath));
		MString_Dispose(&sharedPath);
	}

	if (icon == NULL)
	{
		Logger_LogWarning("Failed to load window icon");
		return;
	}

	if (!SDL_SetWindowIcon(_mWindowContext, icon))
	{
		Logger_LogWarning("Unable to load window icon");
	}
	else
	{
		Logger_LogWarning("Successfully loaded window icon");
	}

	SDL_DestroySurface(icon);
}