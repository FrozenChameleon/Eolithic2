/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GameLoader.h"

#include "../../third_party/stb_ds.h"
#include "../achievement/AchievementHandler.h"
#include "../core/GameHelper.h"
#include "../core/Game.h"
#include "../utils/Cvars.h"
#include "../utils/Strings.h"
#include "../gamestate/GameStateManager.h"
#include "../audio/SoundEffect.h"
#include "../globals/Globals.h"
#include "../render/Sheet.h"
#include "../render/Animation.h"
#include "../audio/Music.h"
#include "../utils/Utils.h"
#include "../utils/Logger.h"
#include "../audio/VolumeData.h"
#include "../utils/Timer.h"
#include "../render/SpriteBatch.h"
#include "../render/Renderer.h"
#include "../render/DrawTool.h"
#include "../service/Service.h"
#include "../resources/ResourceManagerList.h"

#define LOADER_TICK_TIME (1.0 / 60.0)

enum
{
	FPS_TARGET_FOR_HIGH_FPS = 75
};

enum Step
{
	STEP_LOADING = 0,
	STEP_TEST_FPS = 1,
	STEP_BLINK = 2,
	STEP_FADE_OUT = 3,
	STEP_WAIT_ON_SERVICE = 4
};

typedef bool (*JobFunc)(void);

static int32_t _mFps;
static int32_t _mStep;
static int32_t _mPreloaderGraphicFrame;
static double _mDeltaAccumulator;
static double _mFlipDeltaAccumulator;
static bool _mIsFirstFrame = true;
static bool _mIsLoading = true;
static Timer _mBlinkTimer = { 0, 60 };
static Timer _mFadeOutTimer = { 0, 60 };
static Texture* _mPreloaderTextureFinalToDisplay;
static Texture* _mPreloaderTextureFinal;
static Texture* _mPreloaderTextureFinalB;
static Texture** arr_preloader_textures;
static JobFunc* arr_loading_jobs;

static void DisposeTextures(void)
{
	_mPreloaderTextureFinalToDisplay = NULL;
	_mPreloaderTextureFinal = NULL;
	_mPreloaderTextureFinalB = NULL;
	arrsetlen(arr_preloader_textures, 0);
}
static void UpdateLoadingJob(void)
{
	if (arr_loading_jobs[0]())
	{
		arrdel(arr_loading_jobs, 0);
	}
}
static void FinishLoading(void)
{
#ifdef EDITOR_MODE
	/*GuiThingViewer.Init(); //WILLNOTDO 06142023
	Editor.Init();
	EditorPart.Init();
	Logger.LogInformation("Editor loaded");*/
#endif

	GameStateManager_SetGameState2(GAMESTATEMANAGER_GAME_STATE_NORMAL, true);

	Globals_SetAsGameLoaded();

	_mIsLoading = false;
}
static void QuickStart(void)
{
	while (arrlen(arr_loading_jobs) > 0)
	{
		UpdateLoadingJob();
	}
	FinishLoading();
}
static void FindTextureFiles(void)
{
	int32_t counter = 0;
	bool needToStop = false;
	while(!needToStop)
	{
		if (counter > 100) //Safety.
		{
			return;
		}

		MString* path = NULL;

		{
			MString* preloader = NULL;
			MString_Combine3(&preloader, "preloader_", Utils_IntToStringGlobalBuffer(counter), ".png");
			File_PathCombine2(&path, "data", MString_Text(preloader));
			MString_Dispose(&preloader);
		}

		if (File_Exists(MString_Text(path)))
		{
			Texture* tex = Renderer_GetTextureData(MString_Text(path), File_ReadAll(MString_Text(path)));
			arrput(arr_preloader_textures, tex);
		}
		else
		{
			needToStop = true;
		}

		MString_Dispose(&path);

		counter += 1;
	}
}
static void LoadTextures(void)
{
	FindTextureFiles();

	MString* fileFinalPath = NULL;
	MString* fileFinalBPath = NULL;

	File_PathCombine2(&fileFinalPath, "data", "preloaderFinal.png");
	if (File_Exists(MString_Text(fileFinalPath)))
	{
		_mPreloaderTextureFinal = Renderer_GetTextureData(MString_Text(fileFinalPath), File_ReadAll(MString_Text(fileFinalPath)));
	}
	_mPreloaderTextureFinalToDisplay = _mPreloaderTextureFinal;

	File_PathCombine2(&fileFinalBPath, "data", "preloaderFinalB.png");
	if (File_Exists(MString_Text(fileFinalBPath)))
	{
		_mPreloaderTextureFinalB = Renderer_GetTextureData(MString_Text(fileFinalBPath), File_ReadAll(MString_Text(fileFinalBPath)));
	}

	MString_Dispose(&fileFinalPath);
	MString_Dispose(&fileFinalBPath);
}
static bool LoadStart(void)
{
	GameHelper_CreateGlobalSystems();
	GameHelper_CreateStateSystems();
	/*for (int32_t i = 0; i < GameStateManager_StateSystems.size(); i += 1) //UNUSED
	{
		GameStateManager_StateSystems[i]->InitStringSettingsHere();
	}*/
	GameHelper_OnLoadingStart();
	GameHelper_SetupPlatformTypes();
	Strings_Init();
	ResourceManagerList_Init();
	//Tuning_SetCurrentDifficulty(Cvars_GetAsInt(CVARS_ENGINE_DEFAULT_DIFFICULTY)); //UNUSED
	Logger_LogInformation("Loading started");
	return true;
}
static bool LoadResources(void)
{
	ResourceManagerList_LoadAllFromDat();
	return true;
}
static bool LoadAfterResources(void)
{
	Sheet_BuildSheets();
	Animation_BuildAnimations();
	GameHelper_BuildAchievementList();
	GameHelper_InitPoolsForGame();
	GameHelper_OnLoadingAfterResources();
	return true;
}
static bool LoadGameStates(void)
{
	GameHelper_CreateGameStates();
	Logger_LogInformation("Game states loaded");
	return true;
}
static bool LoadEnd(void)
{
	Logger_LogInformation("Loading complete");
	return true;
}
static void SetupLoadingJobs(void)
{
	arrput(arr_loading_jobs, LoadStart);
	arrput(arr_loading_jobs, LoadResources);
	arrput(arr_loading_jobs, LoadAfterResources);
	arrput(arr_loading_jobs, LoadGameStates);
	arrput(arr_loading_jobs, LoadEnd);
}
static void HandleFps(void)
{
	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "Frame rate was ");
		MString_AddAssignInt(&tempString, _mFps);
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}

	if (!Cvars_GetAsBool(CVARS_USER_IS_FIXED_TIMESTEP_ENABLED) || !Cvars_GetAsBool(CVARS_USER_IS_VSYNC))
	{
		return;
	}

	if (_mFps >= FPS_TARGET_FOR_HIGH_FPS) //broad test
	{
		Logger_LogInformation("High hz detected, turning fixed time step off");
		Cvars_SetAsBool(CVARS_USER_IS_FIXED_TIMESTEP_ENABLED, false);
		Cvars_SaveUserConfig2(false);
	}
}
static void UpdatePreloaderGraphic(double deltaTime)
{
	if (arrlen(arr_preloader_textures) <= 0)
	{
		return;
	}

	_mFlipDeltaAccumulator += deltaTime;
	double flipTime = 1.0 / 4.0;
	if (_mFlipDeltaAccumulator >= flipTime)
	{
		_mPreloaderGraphicFrame += 1;
		if (_mPreloaderGraphicFrame > (arrlen(arr_preloader_textures) - 1))
		{
			_mPreloaderGraphicFrame = 0;
		}
		_mFlipDeltaAccumulator -= flipTime;
	}
}
static void StepLoading(double deltaTime)
{
	UpdatePreloaderGraphic(deltaTime);

	if (arrlen(arr_loading_jobs) > 0)
	{
		UpdateLoadingJob();
	}
	else
	{
		if (Service_IsWaitingOnServiceToFinishLoading(deltaTime))
		{
			_mStep = STEP_WAIT_ON_SERVICE;
		}
		else if (!Globals_NeedToTestFrameRate())
		{
			_mStep = STEP_BLINK;
		}
		else
		{
			Logger_LogInformation("It is the first load, testing frame rate");
			_mStep = STEP_TEST_FPS;
		}
	}
}
static void StepTestFPS(double deltaTime)
{
	UpdatePreloaderGraphic(deltaTime);

	_mDeltaAccumulator += deltaTime;
	if (_mDeltaAccumulator >= 1)
	{
		HandleFps();
		_mDeltaAccumulator = 0;
		_mStep = STEP_BLINK;
	}
	else
	{
		_mFps += 1;
	}
}
static void StepBlink(double deltaTime)
{
	_mDeltaAccumulator += deltaTime;
	if (_mDeltaAccumulator < LOADER_TICK_TIME)
	{
		return;
	}

	if (_mBlinkTimer.mCurrent == 0)
	{
		SoundEffect_PlaySound("preloaderBlink");
	}
	if (_mBlinkTimer.mCurrent == 10)
	{
		if (_mPreloaderTextureFinalB != NULL)
		{
			_mPreloaderTextureFinalToDisplay = _mPreloaderTextureFinalB;
		}
	}
	if (Timer_Update(&_mBlinkTimer))
	{
		_mStep = STEP_FADE_OUT;
	}
	else
	{
		_mDeltaAccumulator -= LOADER_TICK_TIME;
	}
}
static void StepFadeOut(double deltaTime)
{
	_mDeltaAccumulator += deltaTime;
	if (_mDeltaAccumulator < LOADER_TICK_TIME)
	{
		return;
	}

	if (Timer_Update(&_mFadeOutTimer))
	{
		DisposeTextures();
		FinishLoading();
	}
	else
	{
		_mDeltaAccumulator -= LOADER_TICK_TIME;
	}
}

void GameLoader_Update(double deltaTime)
{
	if (!_mIsLoading)
	{
		return;
	}

	if (_mIsFirstFrame)
	{
		if (!Cvars_GetAsBool(CVARS_ENGINE_QUICKSTART))
		{
			LoadTextures();
		}
		SetupLoadingJobs();
		_mIsFirstFrame = false;
	}

	if (Cvars_GetAsBool(CVARS_ENGINE_QUICKSTART))
	{
		QuickStart();
		return;
	}
	else
	{
		switch (_mStep)
		{
		case STEP_LOADING:
			StepLoading(deltaTime);
			break;
		case STEP_TEST_FPS:
			StepTestFPS(deltaTime);
			break;
		case STEP_BLINK:
			StepBlink(deltaTime);
			break;
		case STEP_FADE_OUT:
			StepFadeOut(deltaTime);
			break;
		case STEP_WAIT_ON_SERVICE:
			if (!Service_IsWaitingOnServiceToFinishLoading(deltaTime))
			{
				_mStep = STEP_BLINK;
			}
			break;
		}
	}

	SoundEffect_Tick();
	Music_Tick();
}
static void RunShaderStutterFix(SpriteBatch* spriteBatch, Texture* tex, Vector2 offset)
{
	//Running this to preempt various pipeline states 
	Rectangle rect = { 0, 0, 32, 32 };
	for (int32_t i = 0; i < 2; i += 1)
	{
		RenderCommandSheet* instance = SpriteBatch_Draw(spriteBatch, tex, Color_White, 0, NULL, offset, rect, Vector2_One, 0, false, false, Vector2_Zero);
		switch (i)
		{
		case 0:
			instance->mBlendState = BLENDSTATE_ADDITIVE;
			break;
		case 1:
			instance->mBlendState = BLENDSTATE_ALPHABLEND;
			break;
		}
	}
}
void GameLoader_Draw(SpriteBatch* spriteBatch)
{
	if (!_mIsLoading)
	{
		return;
	}

	if (arrlen(arr_preloader_textures) <= 0)
	{
		return;
	}

	Texture* tex = arr_preloader_textures[_mPreloaderGraphicFrame];
	if (_mStep == STEP_BLINK || _mStep == STEP_FADE_OUT)
	{
		if (_mPreloaderTextureFinalToDisplay != NULL)
		{
			tex = _mPreloaderTextureFinalToDisplay;
		}
	}
	else if (_mStep == STEP_WAIT_ON_SERVICE)
	{
		tex = _mPreloaderTextureFinalB;
	}

	int32_t internalWidth = Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_WIDTH);
	int32_t internalHeight = Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_HEIGHT);
	Vector2 offset = { (float)(-internalWidth / 2), (float)(-internalHeight / 2) };

	if (_mStep == STEP_LOADING)
	{
		if (Service_PlatformNeedsShaderStutterFix())
		{
			RunShaderStutterFix(spriteBatch, tex, offset);
		}
	}

	Rectangle destRect = { (int32_t)(offset.X), (int32_t)(offset.Y), internalWidth, internalHeight };

	SpriteBatch_DrawRectangle(spriteBatch, tex, COLOR_WHITE, 100, NULL, destRect, tex->mBounds, 0, false, false, Vector2_Zero);

	if (_mStep == STEP_FADE_OUT)
	{
		float percent = Timer_GetPercentage(&_mFadeOutTimer);
		uint8_t alpha = (uint8_t)(0xFF * percent);
		Color tempColor = { 0, 0, 0, alpha };
		DrawTool_DrawRectangle2(spriteBatch, tempColor, 100, destRect, 0, false);
	}
}
bool GameLoader_IsLoading(void)
{
	return _mIsLoading;
}
