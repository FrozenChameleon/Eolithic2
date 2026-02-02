/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "MoviePlayer.h"

#include "../audio/Music.h"
#include "../core/Func.h"
#include "../utils/Utils.h"
#include "../gamestate/GameStateManager.h"
#include "MovieOperationPan.h"
#include "MovieOperationShake.h"
#include "../utils/Logger.h"
#include "../render/SpriteBatch.h"
#include "../input/Input.h"
#include "../input/ActionList.h"
#include "../input/InputPlayer.h"
#include "MovieOperationText.h"
#include "../utils/Strings.h"
#include "MovieOperationWait.h"
#include "MovieOperationFadeText.h"
#include "../render/Color.h"
#include "../movie/Movie.h"
#include "../movie/MovieTiming.h"
#include "../resources/ResourceManagerList.h"
#include "../../third_party/stb_ds.h"
#include "MovieGlobals.h"

static const char* KEY_SCREEN = "KEY_SCREEN";

typedef struct NewMovieTextData
{
	char mFont[EE_FILENAME_MAX];
	Vector2 mPosition;
	int32_t mSpeed;
	int32_t mRate;
	int32_t mWait;
	char mColor[EE_FILENAME_MAX];
	int32_t mFadeRampSpeed;
	int32_t mFadeHoldTime;
	bool mIsCentered;
} NewMovieTextData;

typedef struct MovieTextData
{
	char mFont[EE_FILENAME_MAX];
	Vector2 mPosition;
	int32_t mSpeed;
	int32_t mRate;
} MovieTextData;

typedef struct MovieFadeTextData
{
	char mFont[EE_FILENAME_MAX];
	Vector2 mPosition;
	int32_t mRampSpeed;
	int32_t mHoldTime;
} MovieFadeTextData;

typedef struct MoviePlayer
{
	int32_t _mFrameCounter;
	int32_t _mScale;
	int32_t _mSkipPoint;
	int32_t _mCounter;
	bool _mIsStarted;
	bool _mIsEnded;
	bool _mIsBlocking;
	bool _mIsWaiting;
	bool _mIsSkipping;
	bool _mIsComplete;
	bool _mDisableSpeedUp;
	bool _mUseSwappedImages;
	char _mNextSwappedImage[EE_FILENAME_MAX];
	bool _mShowFrameTimer;
	char _mMovieName[EE_FILENAME_MAX];
	int32_t _mCurrentTiming;
	MovieTiming* _mTimingsToUse;
	int32_t* _mTimingsToWrite;
	bool _mUseStrictTiming;
	NewMovieTextData _mTextData;
	NewMovieTextData _mNextTextData;
	int32_t _mReaderLoc;
	IStringArray* _mReader;
	void** arr_operations;
	struct { const char* key; MovieImage* value; }*sh_images;
} MoviePlayer;

static MoviePlayer* _mData;

static void WriteTimings(void)
{
	//WILLNOTDO 05152023
	/*
	if (!OeGlobals.IsDebugFileMode())
	{
	return;
	}

	if (_mUseStrictTiming)
	{
	return;
	}

	OeFile.WriteString(OeFile.Combine("data", "timings", _mMovieName + ".txt"), OeUtils_GetListToCsvString(_mTimingsToWrite));
	*/
}

static const char* GetCurrentTextColor(void)
{
	const char* color = _mData->_mTextData.mColor;
	if (!Utils_StringEqualTo(_mData->_mNextTextData.mColor, EE_STR_EMPTY))
	{
		color = _mData->_mNextTextData.mColor;
	}
	return color;
}
static MovieFadeTextData GetFadeTextData(IStringArray* arguments)
{
	MovieFadeTextData fadeTextData = { 0 };
	if (IStringArray_Length(arguments) >= 2)
	{
		Utils_strlcpy(fadeTextData.mFont, IStringArray_Get(arguments, 1), EE_FILENAME_MAX);
	}
	if (IStringArray_Length(arguments) >= 4)
	{
		fadeTextData.mPosition = Vector2_Create((float)Utils_ParseInt(IStringArray_Get(arguments, 2)), (float)Utils_ParseInt(IStringArray_Get(arguments, 3)));
	}
	if (IStringArray_Length(arguments) >= 5)
	{
		fadeTextData.mRampSpeed = Utils_ParseInt(IStringArray_Get(arguments, 4));
	}
	if (IStringArray_Length(arguments) >= 6)
	{
		fadeTextData.mHoldTime = Utils_ParseInt(IStringArray_Get(arguments, 5));
	}
	return fadeTextData;
}
static MovieTextData GetTextData(IStringArray* arguments)
{
	MovieTextData textData = { 0 };
	if (IStringArray_Length(arguments) >= 2)
	{
		Utils_strlcpy(textData.mFont, IStringArray_Get(arguments, 1), EE_FILENAME_MAX);
	}
	if (IStringArray_Length(arguments) >= 4)
	{
		textData.mPosition = Vector2_Create((float)Utils_ParseInt(IStringArray_Get(arguments, 2)), (float)Utils_ParseInt(IStringArray_Get(arguments, 3)));
	}
	if (IStringArray_Length(arguments) >= 5)
	{
		textData.mSpeed = Utils_ParseInt(IStringArray_Get(arguments, 4));
	}
	if (IStringArray_Length(arguments) >= 6)
	{
		textData.mRate = Utils_ParseInt(IStringArray_Get(arguments, 5));
	}
	return textData;
}
static MovieOperationText* CreateNewMovieOperationText(bool isMappedText, const char* str, const char* font, Vector2 position,
	int32_t speed, int32_t rate, int32_t wait, const char* color, bool isTextCentered)
{
	MovieOperationText* text = (MovieOperationText*)Utils_CallocArena(1, sizeof(MovieOperationText), UTILS_ALLOCATION_ARENA_MOVIE_PLAYER);
	MovieOperationText_Init(text, isMappedText, str, font, position, speed, rate, wait, color, isTextCentered);
	return text;
}
static MovieOperationFadeText* CreateNewMovieOperationFadeText(const char* str, const char* font, Vector2 position, int32_t rampSpeed, int32_t holdTime, bool isTextCentered)
{
	MovieOperationFadeText* fadeText = (MovieOperationFadeText*)Utils_CallocArena(1, sizeof(MovieOperationFadeText), UTILS_ALLOCATION_ARENA_MOVIE_PLAYER);
	MovieOperationFadeText_Init(fadeText, str, font, position, rampSpeed, holdTime, isTextCentered);
	return fadeText;
}
static MovieOperationPan* CreateNewMovieOperationPan(MovieImage* image, Vector2 speed, int32_t time)
{
	MovieOperationPan* pan = (MovieOperationPan*)Utils_CallocArena(1, sizeof(MovieOperationPan), UTILS_ALLOCATION_ARENA_MOVIE_PLAYER);
	MovieOperationPan_Init(pan, image, speed, time);
	return pan;
}
static MovieOperationShake* CreateNewMovieOperationShake(const char* key, MovieImage* image, int32_t min, int32_t max, int32_t time)
{
	MovieOperationShake* shake = (MovieOperationShake*)Utils_CallocArena(1, sizeof(MovieOperationShake), UTILS_ALLOCATION_ARENA_MOVIE_PLAYER);
	MovieOperationShake_Init(shake, key, image, min, max, time);
	return shake;
}
static MovieOperationWait* CreateNewMovieOperationWait(int32_t timeLimit)
{
	MovieOperationWait* wait = (MovieOperationWait*)Utils_CallocArena(1, sizeof(MovieOperationWait), UTILS_ALLOCATION_ARENA_MOVIE_PLAYER);
	MovieOperationWait_Init(wait, timeLimit);
	return wait;
}
static MovieImage* CreateNewMovieImage(int32_t scale, const char* image)
{
	MovieImage* mi = (MovieImage*)Utils_CallocArena(1, sizeof(MovieImage), UTILS_ALLOCATION_ARENA_MOVIE_PLAYER);
	MovieImage_Init(mi, scale, image);
	return mi;
}
static MovieImage* CreateNewMovieImage2(int32_t scale, const char* baseImage, int32_t frames, int32_t flip)
{
	MovieImage* mi = (MovieImage*)Utils_CallocArena(1, sizeof(MovieImage), UTILS_ALLOCATION_ARENA_MOVIE_PLAYER);
	MovieImage_Init2(mi, scale, baseImage, frames, flip);
	return mi;
}
static void OperationSwapNextImage(IStringArray* arguments)
{
	if (!_mData->_mUseSwappedImages)
	{
		return;
	}

	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	Utils_strlcpy(_mData->_mNextSwappedImage, IStringArray_Get(arguments, 0), EE_FILENAME_MAX);
}
static void OperationSetNextTextColor(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	Utils_strlcpy(_mData->_mNextTextData.mColor, IStringArray_Get(arguments, 0), EE_FILENAME_MAX);
}
static void OperationRemove(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	shdel(_mData->sh_images, IStringArray_Get(arguments, 0));
}
static void OperationGoTo(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	_mData->_mIsSkipping = true;
	_mData->_mSkipPoint = Utils_ParseInt(IStringArray_Get(arguments, 0));
}
static void OperationPan(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 4)
	{
		return;
	}

	const char* key = IStringArray_Get(arguments, 0);
	MovieImage* image = shget(_mData->sh_images, key);
	if (image != NULL)
	{
		Vector2 speed = Vector2_Create(Utils_ParseFloat(IStringArray_Get(arguments, 1)), Utils_ParseFloat(IStringArray_Get(arguments, 2)));
		int32_t time = Utils_ParseInt(IStringArray_Get(arguments, 3));
		MovieOperationPan* pan = CreateNewMovieOperationPan(image, speed, time);
		arrput(_mData->arr_operations, pan);
	}
}
static void OperationClear(void)
{
	{
		IStringArray* removeThese = IStringArray_Create();
		for (int32_t i = 0; i < shlen(_mData->sh_images); i += 1)
		{
			if (!_mData->sh_images[i].value->mIsPermanent)
			{
				IStringArray_Add(removeThese, _mData->sh_images[i].key);
			}
		}

		for (int32_t i = 0; i < IStringArray_Length(removeThese); i += 1)
		{
			shdel(_mData->sh_images, IStringArray_Get(removeThese, i));
		}

		arrsetlen(_mData->arr_operations, 0);

		IStringArray_Dispose(removeThese);
	}
}
static void OperationPlaySound(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	Do_PlaySound(IStringArray_Get(arguments, 0));
}
static void OperationPlayMusic(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) == 1)
	{
		Do_PlayMusic2(IStringArray_Get(arguments, 0), true, false);
	}
	else if (IStringArray_Length(arguments) == 5)
	{
		Do_PlayMusic3(IStringArray_Get(arguments, 0), true, false, Utils_ParseBoolean(IStringArray_Get(arguments, 1)), 
			Utils_ParseInt(IStringArray_Get(arguments, 2)), Utils_ParseBoolean(IStringArray_Get(arguments, 3)),
			Utils_ParseInt(IStringArray_Get(arguments, 4)));
	}
	
}
static void OperationFadeOutMusic(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	Do_FadeOutMusic(Utils_ParseInt(IStringArray_Get(arguments, 0)));
}
static void OperationAddImage(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 5)
	{
		return;
	}
	
	const char* key = IStringArray_Get(arguments, 0);
	const char* strImage = IStringArray_Get(arguments, 1);
	if (_mData->_mUseSwappedImages && (!Utils_StringEqualTo(_mData->_mNextSwappedImage, EE_STR_EMPTY)))
	{
		strImage = _mData->_mNextSwappedImage;
		Utils_strlcpy(_mData->_mNextSwappedImage, EE_STR_EMPTY, EE_FILENAME_MAX);
	}
	MovieImage* image = CreateNewMovieImage(_mData->_mScale, strImage);
	float depth = Utils_ParseFloat(IStringArray_Get(arguments, 2));
	image->mDepth = (int32_t)(depth * 100);
	image->mPosition = Vector2_Create((float)Utils_ParseInt(IStringArray_Get(arguments, 3)), (float)Utils_ParseInt(IStringArray_Get(arguments, 4)));
	if (IStringArray_Length(arguments) == 6)
	{
		image->mIsPermanent = Utils_ParseBoolean(IStringArray_Get(arguments, 5));
	}
	shput(_mData->sh_images, key, image);
}
static void OperationAddScreen(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	int32_t waitTime = 0;
	if (IStringArray_Length(arguments) >= 2)
	{
		waitTime = Utils_ParseInt(IStringArray_Get(arguments, 1));
	}

	const char* image = IStringArray_Get(arguments, 0);
	MovieImage* movieImage = CreateNewMovieImage(_mData->_mScale, image);
	movieImage->mDepth = 50;

	shput(_mData->sh_images, KEY_SCREEN, movieImage);
	if (waitTime > 0)
	{
		_mData->_mIsWaiting = true;
		MovieOperationWait* wait = CreateNewMovieOperationWait(waitTime);
		arrput(_mData->arr_operations, wait);
	}
}
static void OperationNewText(const char* operation, IStringArray* arguments)
{
	if ((IStringArray_Length(arguments) < 1) || (Utils_StringEqualTo(_mData->_mTextData.mFont, EE_STR_EMPTY)))
	{
		return;
	}

	const char* str = IStringArray_Get(arguments, 0);
	bool isMappedText = false;
	if (Utils_StringEqualTo(operation, "newMappedText"))
	{
		isMappedText = true;
	}
	MovieOperationText* text = CreateNewMovieOperationText(isMappedText, str, _mData->_mTextData.mFont, _mData->_mTextData.mPosition,
		_mData->_mTextData.mSpeed, _mData->_mTextData.mRate, _mData->_mTextData.mWait, GetCurrentTextColor(), _mData->_mTextData.mIsCentered);
	arrput(_mData->arr_operations, text);
}
static void OperationNewFadeText(const char* operation, IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	const char* str = IStringArray_Get(arguments, 0);
	if (Utils_StringEqualTo(operation, ("newMappedFadeText")))
	{
		str = Strings_Get(str);
	}
	MovieOperationFadeText* fadeText = CreateNewMovieOperationFadeText(str, _mData->_mTextData.mFont, _mData->_mTextData.mPosition,
		_mData->_mTextData.mFadeRampSpeed, _mData->_mTextData.mFadeHoldTime, _mData->_mTextData.mIsCentered);
	arrput(_mData->arr_operations, fadeText);
}
static void OperationSetTextFont(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	Utils_strlcpy(_mData->_mTextData.mFont, IStringArray_Get(arguments, 0), EE_FILENAME_MAX);
}
static void OperationSetTextPosition(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 2)
	{
		return;
	}

	_mData->_mTextData.mPosition = Vector2_Create((float)Utils_ParseInt(IStringArray_Get(arguments, 0)), (float)Utils_ParseInt(IStringArray_Get(arguments, 1)));
}
static void OperationSetTextSpeed(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	_mData->_mTextData.mSpeed = Utils_ParseInt(IStringArray_Get(arguments, 0));
}
static void OperationSetTextRate(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	_mData->_mTextData.mRate = Utils_ParseInt(IStringArray_Get(arguments, 0));
}
static void OperationSetTextWait(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	_mData->_mTextData.mWait = Utils_ParseInt(IStringArray_Get(arguments, 0));
}
static void OperationSetTextFadeRampSpeed(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	_mData->_mTextData.mFadeRampSpeed = Utils_ParseInt(IStringArray_Get(arguments, 0));
}
static void OperationSetTextFadeHoldTime(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	_mData->_mTextData.mFadeHoldTime = Utils_ParseInt(IStringArray_Get(arguments, 0));
}
static void OperationSetTextColor(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	Utils_strlcpy(_mData->_mTextData.mColor, IStringArray_Get(arguments, 0), EE_FILENAME_MAX);
}
static void OperationRemoveScreen(void)
{
	shdel(_mData->sh_images, KEY_SCREEN);
}
static void OperationAddAnimation(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 7)
	{
		return;
	}

	const char* key = IStringArray_Get(arguments, 0);
	const char* baseImage = IStringArray_Get(arguments, 1);
	if (_mData->_mUseSwappedImages && (!Utils_StringEqualTo(_mData->_mNextSwappedImage, EE_STR_EMPTY)))
	{
		baseImage = _mData->_mNextSwappedImage;
		Utils_strlcpy(_mData->_mNextSwappedImage, EE_STR_EMPTY, EE_FILENAME_MAX);
	}
	int32_t frames = Utils_ParseInt(IStringArray_Get(arguments, 2));
	int32_t flip = Utils_ParseInt(IStringArray_Get(arguments, 3));

	MovieImage* image = CreateNewMovieImage2(_mData->_mScale, baseImage, frames, flip);

	float depth = Utils_ParseFloat(IStringArray_Get(arguments, 4));
	image->mDepth = (int32_t)(depth * 100);
	image->mPosition = Vector2_Create((float)Utils_ParseInt(IStringArray_Get(arguments, 5)), (float)Utils_ParseInt(IStringArray_Get(arguments, 6)));

	if (IStringArray_Length(arguments) >= 8)
	{
		image->mIsPermanent = Utils_ParseBoolean(IStringArray_Get(arguments, 7));
	}

	if (IStringArray_Length(arguments) >= 9)
	{
		MovieImage_SetIsLoopingDisabled(image, Utils_ParseBoolean(IStringArray_Get(arguments, 8)));
	}

	if (IStringArray_Length(arguments) >= 10)
	{
		MovieImage_SetLoopPoint(image, Utils_ParseInt(IStringArray_Get(arguments, 9)));
	}

	shput(_mData->sh_images, key, image);
}
static void OperationWait(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	_mData->_mIsWaiting = true;

	int32_t timeLimit = Utils_ParseInt(IStringArray_Get(arguments, 0));
	MovieOperationWait* wait = CreateNewMovieOperationWait(timeLimit);
	arrput(_mData->arr_operations, wait);
}
static void OperationShake(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 4)
	{
		return;
	}

	const char* strImage = IStringArray_Get(arguments, 0);
	MovieImage* movieImage = shget(_mData->sh_images, strImage);
	if (movieImage != NULL)
	{
		int32_t min = Utils_ParseInt(IStringArray_Get(arguments, 1));
		int32_t max = Utils_ParseInt(IStringArray_Get(arguments, 2));
		int32_t time = Utils_ParseInt(IStringArray_Get(arguments, 3));
		MovieOperationShake* shake = CreateNewMovieOperationShake(strImage, movieImage, min, max, time);
		arrput(_mData->arr_operations, shake);
	}
}
static void OperationText(const char* operation, IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 6)
	{
		return;
	}

	MovieTextData textData = GetTextData(arguments);
	const char* str = IStringArray_Get(arguments, 0);
	bool isMappedText = false;
	if (Utils_StringEqualTo(operation, "mappedText"))
	{
		isMappedText = true;
	}
	int32_t wait = 0;
	if (IStringArray_Length(arguments) >= 7)
	{
		wait = Utils_ParseInt(IStringArray_Get(arguments, 6));
	}
	const char* color = "WHITE";
	if (IStringArray_Length(arguments) >= 8)
	{
		color = IStringArray_Get(arguments, 7);
	}

	MovieOperationText* text = CreateNewMovieOperationText(isMappedText, str, textData.mFont,
		textData.mPosition, textData.mSpeed, textData.mRate, wait, color, false);
	arrput(_mData->arr_operations, text);
}
static void OperationGiveTime(void)
{
	MString* temp = NULL;
	MString_AddAssignInt(&temp, _mData->_mFrameCounter);
	Logger_LogInformation(MString_Text(temp));
	MString_Dispose(&temp);
}
static void OperationFadeText(const char* operation, IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 6)
	{
		return;
	}

	MovieFadeTextData fadeTextData = GetFadeTextData(arguments);
	const char* str = IStringArray_Get(arguments, 0);
	if (Utils_StringEqualTo(operation, "mappedFadeText"))
	{
		str = Strings_Get(str);
	}
	MovieOperationFadeText* fadeText = CreateNewMovieOperationFadeText(str, fadeTextData.mFont, fadeTextData.mPosition, fadeTextData.mRampSpeed, fadeTextData.mHoldTime, false);
	arrput(_mData->arr_operations, fadeText);
}
static void Broadcast(const char* key, const char* value)
{
	for (int32_t i = 0; i < arrlen(_mData->arr_operations); i += 1)
	{
		void* op = _mData->arr_operations[i];
		int32_t type = *((int32_t*)op);
		switch (type)
		{
		case MOVIE_OPERATION_TYPE_SHAKE:
			MovieOperationShake_Broadcast((MovieOperationShake*)op, key, value);
			break;
		}
	}
}
static void OperationStopShake(IStringArray* arguments)
{
	if (IStringArray_Length(arguments) < 1)
	{
		return;
	}

	Broadcast("shake", IStringArray_Get(arguments, 0));
}
static void AddOperation(const char* operation, IStringArray* arguments)
{
	if (Utils_StringEqualTo(operation, ("useStrictTimings")))
	{
		_mData->_mUseStrictTiming = true;
	}
	else if (Utils_StringEqualTo(operation, ("showFrameTimer")))
	{
		_mData->_mShowFrameTimer = true;
	}
	else if (Utils_StringEqualTo(operation, ("swapNextImage")))
	{
		OperationSwapNextImage(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("giveTime")))
	{
		OperationGiveTime();
	}
	else if (Utils_StringEqualTo(operation, ("disableSpeedUp")))
	{
		_mData->_mDisableSpeedUp = true;
	}
	else if (Utils_StringEqualTo(operation, ("enableSpeedUp")))
	{
		_mData->_mDisableSpeedUp = false;
	}
	else if (Utils_StringEqualTo(operation, ("dispose")))
	{
		OperationRemove(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("remove")))
	{
		OperationRemove(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("text")) || Utils_StringEqualTo(operation, "mappedText"))
	{
		OperationText(operation, arguments);
	}
	else if (Utils_StringEqualTo(operation, ("fadeText")) || Utils_StringEqualTo(operation, "mappedFadeText"))
	{
		OperationFadeText(operation, arguments);
	}
	else if (Utils_StringEqualTo(operation, ("goto")))
	{
		OperationGoTo(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("loop")))
	{
		_mData->_mIsEnded = true;
		GameStateManager_SetupLoadMap(Get_LevelFileName());
	}
	else if (Utils_StringEqualTo(operation, ("end")))
	{
		_mData->_mIsEnded = true;
		_mData->_mIsComplete = true;
		WriteTimings();
	}
	else if (Utils_StringEqualTo(operation, ("pan")))
	{
		OperationPan(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("clear")))
	{
		OperationClear();
	}
	else if (Utils_StringEqualTo(operation, ("playSound")))
	{
		OperationPlaySound(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("playMusic")))
	{
		OperationPlayMusic(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("fadeOutMusic")))
	{
		OperationFadeOutMusic(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("stopMusic")))
	{
		Do_StopMusic();
	}
	else if (Utils_StringEqualTo(operation, ("addImage")))
	{
		OperationAddImage(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("addScreen")))
	{
		OperationAddScreen(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("removeScreen")))
	{
		OperationRemoveScreen();
	}
	else if (Utils_StringEqualTo(operation, ("addAnimation")))
	{
		OperationAddAnimation(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("wait")))
	{
		OperationWait(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("block")))
	{
		_mData->_mIsBlocking = true;
	}
	else if (Utils_StringEqualTo(operation, ("stopShake")))
	{
		OperationStopShake(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("shake")))
	{
		OperationShake(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("setTextFont")))
	{
		OperationSetTextFont(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("setTextPosition")))
	{
		OperationSetTextPosition(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("setTextSpeed")))
	{
		OperationSetTextSpeed(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("setTextRate")))
	{
		OperationSetTextRate(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("setTextWait")))
	{
		OperationSetTextWait(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("setTextFadeRampSpeed")))
	{
		OperationSetTextFadeRampSpeed(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("setTextFadeHoldTime")))
	{
		OperationSetTextFadeHoldTime(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("setTextColor")))
	{
		OperationSetTextColor(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("newText")) || Utils_StringEqualTo(operation, ("newMappedText")))
	{
		OperationNewText(operation, arguments);
	}
	else if (Utils_StringEqualTo(operation, ("newFadeText")) || Utils_StringEqualTo(operation, ("newMappedFadeText")))
	{
		OperationNewFadeText(operation, arguments);
	}
	else if (Utils_StringEqualTo(operation, ("setNextTextColor")))
	{
		OperationSetNextTextColor(arguments);
	}
	else if (Utils_StringEqualTo(operation, ("hackIgnoreNextMusicFadeOut")))
	{
		Music_SetHackToIgnoreNextFadeOutMusic(true);
	}
	else if (Utils_StringEqualTo(operation, ("setTextCentered")))
	{
		_mData->_mTextData.mIsCentered = true;
	}
	else if (Utils_StringEqualTo(operation, ("setTextUncentered")))
	{
		_mData->_mTextData.mIsCentered = false;
	}
}

static int32_t GetOperationType(void* op)
{
	return *((int32_t*)op);;
}
static void DoOperationSpeedUp(void* op)
{
	int32_t type = GetOperationType(op);
	switch (type)
	{
	case MOVIE_OPERATION_TYPE_WAIT:
		MovieOperationWait_SpeedUp((MovieOperationWait*)op);
		break;
	case MOVIE_OPERATION_TYPE_SHAKE:
		MovieOperationShake_SpeedUp((MovieOperationShake*)op);
		break;
	case MOVIE_OPERATION_TYPE_FADE_TEXT:
		MovieOperationFadeText_SpeedUp((MovieOperationFadeText*)op);
		break;
	case MOVIE_OPERATION_TYPE_TEXT:
		MovieOperationText_SpeedUp((MovieOperationText*)op);
		break;
	case MOVIE_OPERATION_TYPE_PAN:
		MovieOperationPan_SpeedUp((MovieOperationPan*)op);
		break;
	}
}
static void DoOperationUpdate(void* op)
{
	int32_t type = GetOperationType(op);
	switch (type)
	{
	case MOVIE_OPERATION_TYPE_WAIT:
		MovieOperationWait_Update((MovieOperationWait*)op);
		break;
	case MOVIE_OPERATION_TYPE_SHAKE:
		MovieOperationShake_Update((MovieOperationShake*)op);
		break;
	case MOVIE_OPERATION_TYPE_FADE_TEXT:
		MovieOperationFadeText_Update((MovieOperationFadeText*)op);
		break;
	case MOVIE_OPERATION_TYPE_TEXT:
		MovieOperationText_Update((MovieOperationText*)op);
		break;
	case MOVIE_OPERATION_TYPE_PAN:
		MovieOperationPan_Update((MovieOperationPan*)op);
		break;
	}
}
static bool OperationDoesNotBlock(void* op)
{
	int32_t type = GetOperationType(op);
	switch (type)
	{
	case MOVIE_OPERATION_TYPE_SHAKE:
		return MovieOperationShake_DoesNotBlock((MovieOperationShake*)op);
	}
	return false;
}
static bool OperationForcesWait(void* op)
{
	int32_t type = GetOperationType(op);
	switch (type)
	{
	case MOVIE_OPERATION_TYPE_WAIT:
		return MovieOperationWait_ForcesWait((MovieOperationWait*)op);
	}
	return false;
}

void MoviePlayer_Init(bool useSwappedImages, int32_t scale, const char* movieName)
{
	if (_mData == NULL)
	{
		_mData = (MoviePlayer*)Utils_calloc(1, sizeof(MoviePlayer));
	}
	else
	{
		shfree(_mData->sh_images);
		arrfree(_mData->arr_operations);
	}

	Utils_FreeArena(UTILS_ALLOCATION_ARENA_MOVIE_PLAYER);

	Utils_memset(_mData, 0, sizeof(MoviePlayer));

	sh_new_arena(_mData->sh_images);

	Utils_strlcpy(_mData->_mMovieName, movieName, EE_FILENAME_MAX);

	_mData->_mUseSwappedImages = useSwappedImages;

	Utils_strlcpy(_mData->_mTextData.mFont, "game", EE_FILENAME_MAX);
	Utils_strlcpy(_mData->_mTextData.mColor, "WHITE", EE_FILENAME_MAX);

	_mData->_mScale = scale;

	ResourceManager* movieMan = ResourceManagerList_Movie();
	if (!ResourceManager_HasResource(movieMan, movieName))
	{
		_mData->_mIsComplete = true;
	}
	else
	{
		_mData->_mReader = ((Movie*)ResourceManager_GetResourceData(movieMan, movieName))->mMovieData;
		ResourceManager* movieTimingMan = ResourceManagerList_MovieTiming();
		if (ResourceManager_HasResource(movieTimingMan, movieName))
		{
			_mData->_mTimingsToUse = (MovieTiming*)ResourceManager_GetResourceData(movieTimingMan, movieName);
		}
	}
}

void MoviePlayer_DisableSpeedUp(void)
{
	_mData->_mDisableSpeedUp = true;
}
void MoviePlayer_SetComplete(void)
{
	_mData->_mIsComplete = true;
	arrsetlen(_mData->arr_operations, 0);
}
bool MoviePlayer_IsComplete(void)
{
	return _mData->_mIsComplete;
}
void MoviePlayer_Update(void)
{
	MoviePlayer_Update2(false);
}
void MoviePlayer_Update2(bool doNotAllowMovieSkip)
{
	if (!doNotAllowMovieSkip)
	{
		if (Input_IsTappedByAnyPlayer(ACTIONLIST_GAME_START) || Input_IsEscapeTapped())
		{
			MoviePlayer_SetComplete();
			return;
		}
	}

	bool hasLoggedTime = false;
	bool waitBecauseOfStrictTimings = false;
	if (_mData->_mUseStrictTiming)
	{
		if (_mData->_mTimingsToUse->len == 0)
		{
			Logger_LogInformation("No timings to use, cannot use strict timing");
			_mData->_mUseStrictTiming = false;
		}
	}
	if (_mData->_mUseStrictTiming)
	{
		int32_t targetTime = _mData->_mTimingsToUse->timings[_mData->_mCurrentTiming];
		if (_mData->_mFrameCounter != targetTime)
		{
			waitBecauseOfStrictTimings = true;
		}
		if (_mData->_mFrameCounter > targetTime)
		{
			Logger_LogInformation("Frame counter over target time, cannot continue movie");
			if (_mData->_mFrameCounter > (targetTime + (60 * 5)))
			{
				_mData->_mIsComplete = true;
			}
		}
	}
	while (!_mData->_mIsWaiting && !_mData->_mIsBlocking && !_mData->_mIsEnded && !waitBecauseOfStrictTimings)
	{
		if (!hasLoggedTime)
		{
			//_mData->_mTimingsToWrite.push_back(_mFrameCounter); //UNUSED, USED FOR WRITING TIMINGS
			_mData->_mCurrentTiming += 1;
			hasLoggedTime = true;
		}

		const char* line = IStringArray_Get(_mData->_mReader, _mData->_mReaderLoc);
		_mData->_mReaderLoc += 1;

		bool skip = false;
		if (_mData->_mIsSkipping)
		{
			if (_mData->_mSkipPoint != _mData->_mCounter)
			{
				skip = true;
			}
			else
			{
				_mData->_mIsSkipping = false;
			}
		}

		if ((!Utils_StringEqualTo(line, EE_STR_EMPTY)) && !skip)
		{
			if (!Utils_StringContains(line, "//"))
			{
				if (_mData->_mIsStarted)
				{
					if (Utils_StringContains(line, "(") && Utils_StringContains(line, ")"))
					{
						size_t lineSize = Utils_strlen(line);
						int32_t firstIndex = Utils_StringIndexOf('(', line, lineSize, false);
						//int32_t secondIndex = OeUtils_StringIndexOf(line, ')');
						{
							MString* operation = NULL;
							MString* argumentString = NULL;
							IStringArray* arguments = NULL;
							MString_AssignSubString(&operation, line, 0, firstIndex);
							MString_AssignSubString(&argumentString, line, firstIndex + 1, (int32_t)(lineSize - firstIndex - 2));
							arguments = IStringArray_Create();
							Utils_GetSplitCSV(MString_Text(argumentString), arguments);
							AddOperation(MString_Text(operation), arguments);
							MString_Dispose(&operation);
							MString_Dispose(&argumentString);
							IStringArray_Dispose(arguments);
						}
					}
				}

				if (Utils_StringEqualTo(line, "begin()"))
				{
					_mData->_mIsStarted = true;
				}
			}
		}

		_mData->_mCounter += 1;
	}

	if (!_mData->_mIsEnded)
	{
		for (int32_t i = 0; i < shlen(_mData->sh_images); i += 1)
		{
			MovieImage_Update(_mData->sh_images[i].value);
		}

		bool speedUp = false;
		if (Input_IsTappedByAnyPlayer(ACTIONLIST_GAME_MENU_SELECT) && !_mData->_mDisableSpeedUp)
		{
			speedUp = true;
		}

		bool allOperationsComplete = true;
		bool allWaitingOperationsComplete = true;
		for (int32_t i = 0; i < arrlen(_mData->arr_operations); i += 1)
		{
			void* op = _mData->arr_operations[i];

			bool isComplete = *(((uint8_t*)op) + sizeof(int32_t));

			if (!isComplete)
			{
				if (speedUp)
				{
					DoOperationSpeedUp(op);
				}

				DoOperationUpdate(op);

				if (!OperationDoesNotBlock(op))
				{
					if (OperationForcesWait(op))
					{
						allWaitingOperationsComplete = false;
					}
					allOperationsComplete = false;
				}
			}
		}

		if (allWaitingOperationsComplete)
		{
			_mData->_mIsWaiting = false;
		}
		if (allOperationsComplete)
		{
			_mData->_mIsBlocking = false;
		}
	}

	_mData->_mFrameCounter += 1;
}
void MoviePlayer_DrawHud(SpriteBatch* spriteBatch)
{
	for (int32_t i = 0; i < arrlen(_mData->arr_operations); i += 1)
	{
		void* op = _mData->arr_operations[i];
		int32_t type = GetOperationType(op);
		switch (type)
		{
		case MOVIE_OPERATION_TYPE_FADE_TEXT:
			MovieOperationFadeText_DrawHud((MovieOperationFadeText*)op, spriteBatch);
			break;
		case MOVIE_OPERATION_TYPE_TEXT:
			MovieOperationText_DrawHud((MovieOperationText*)op, spriteBatch);
			break;
		}
	}

	for (int32_t i = 0; i < shlen(_mData->sh_images); i += 1)
	{
		MovieImage_DrawHud(_mData->sh_images[i].value, spriteBatch);
	}
#ifdef EDITOR_MODE
	int32_t targetY = 150;
	SpriteBatch_DrawString(spriteBatch, "game", Utils_IntToStringGlobalBuffer(_mData->_mFrameCounter), COLOR_YELLOW, 200, Vector2_Create(0, (float)targetY));
	SpriteBatch_DrawString(spriteBatch, "game", Utils_IntToStringGlobalBuffer(_mData->_mFrameCounter / 60), COLOR_YELLOW, 200, Vector2_Create(0, (float)(targetY + 16)));
	if (_mData->_mUseStrictTiming)
	{
		SpriteBatch_DrawString(spriteBatch, "game", "STRICT", COLOR_YELLOW, 200, Vector2_Create(0, (float)(targetY + 32)));
	}
#endif
}
