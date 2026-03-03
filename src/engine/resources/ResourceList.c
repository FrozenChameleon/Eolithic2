/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ResourceList.h"

#include "../utils/Utils.h"

#include "../leveldata/AnimTile.h"
#include "../leveldata/Prop.h"
#include "../render/Texture.h"
#include "../audio/SoundEffect.h"
#include "../audio/Music.h"
#include "../leveldata/LevelData.h"
#include "../render/TilesetOffset.h"
#include "../render/TextureOffset.h"
#include "../font/BmFont.h"
#include "../leveldata/Particle.h"
#include "../movie/MovieTiming.h"
#include "../movie/Movie.h"
#include "../font/TTFont.h"
#include "../leveldata/ThingSettings.h"
#include "../io/File.h"

#define MANAGERS_LEN 50

//THIS ORDER DETERMINES LOAD ORDER

#define MANAGERS_INDEX_TEXTURE 0
#define MANAGERS_INDEX_FONT_TEXTURE 1
#define MANAGERS_INDEX_MOVIE_TEXTURE 2
#define MANAGERS_INDEX_ANIM_TILE 3
#define MANAGERS_INDEX_PROP 4
#define MANAGERS_INDEX_SOUND_EFFECT 5
#define MANAGERS_INDEX_MUSIC 6
#define MANAGERS_INDEX_TILESET_OFFSET 7
#define MANAGERS_INDEX_LEVEL_DATA 8
#define MANAGERS_INDEX_TEXTURE_OFFSET 9
#define MANAGERS_INDEX_FONT 10
#define MANAGERS_INDEX_PARTICLE 11
#define MANAGERS_INDEX_MOVIE_TIMING 12
#define MANAGERS_INDEX_MOVIE 13
#define MANAGERS_INDEX_TTFONT 14
#define MANAGERS_INDEX_THING_SETTINGS 15

static bool _mHasInit;
static ResourceMan _mManagers[MANAGERS_LEN];

void ResourceList_Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	MString* tempString = NULL;

	{
		ResourceMan* animTileMan = ResourceList_AnimTile();
		ResourceMan_Init(animTileMan);
		Utils_strlcpy(animTileMan->_mResourceType, "AnimTile", EE_FILENAME_MAX);
		Utils_strlcpy(animTileMan->_mDatFileName, "animtile.dat", EE_FILENAME_MAX);
		Utils_strlcpy(animTileMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "animtile");
		Utils_strlcpy(animTileMan->_mDirectories[0], MString_Text(tempString), EE_PATH_MAX);
		animTileMan->_mFromStream = (ResourceMan_FromStreamFunc)AnimTile_FromStream;
		animTileMan->_mDispose = (ResourceMan_DisposeFunc)AnimTile_Dispose;
		animTileMan->_mRead = (ResourceMan_ReadFunc)AnimTile_Read;
		animTileMan->_mWrite = (ResourceMan_WriteFunc)AnimTile_Write;
		animTileMan->_mCreateNew = (ResourceMan_CreateNewFunc)AnimTile_CreateNew;
		animTileMan->_mCopyTo = (ResourceMan_CopyToFunc)AnimTile_CopyTo;
		animTileMan->_mReadFromDirectory = true;
		animTileMan->_mIsReadingText = true;
	}

	{
		ResourceMan* propMan = ResourceList_Prop();
		ResourceMan_Init(propMan);
		Utils_strlcpy(propMan->_mResourceType, "Prop", EE_FILENAME_MAX);
		Utils_strlcpy(propMan->_mDatFileName, "props.dat", EE_FILENAME_MAX);
		Utils_strlcpy(propMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "props");
		Utils_strlcpy(propMan->_mDirectories[0], MString_Text(tempString), EE_PATH_MAX);
		propMan->_mFromStream = (ResourceMan_FromStreamFunc)Prop_FromStream;
		propMan->_mDispose = (ResourceMan_DisposeFunc)Prop_Dispose;
		propMan->_mRead = (ResourceMan_ReadFunc)Prop_Read;
		propMan->_mWrite = (ResourceMan_WriteFunc)Prop_Write;
		propMan->_mCreateNew = (ResourceMan_CreateNewFunc)Prop_CreateNew;
		propMan->_mCopyTo = (ResourceMan_CopyToFunc)Prop_CopyTo;
		propMan->_mReadFromDirectory = true;
		propMan->_mIsReadingText = true;
	}

	/*
	  public static readonly string[] DEBUG_TEXTURE_DIRECTORIES = { OeFile.Combine("data", "gfx", "game"), OeFile.Combine("data", "gfx", "tilesets"), OeFile.Combine("data", "gfx", "special") };
		public static readonly string[] USER_TEXTURE_DIRECTORIES = { OeFile.Combine("data", "gfx", "generated_game"), OeFile.Combine("data", "gfx", "generated_tilesets"), OeFile.Combine("data", "gfx", "special") };
		/*
		*/

	{
		ResourceMan* textureMan = ResourceList_Texture();
		ResourceMan_Init(textureMan);
		Utils_strlcpy(textureMan->_mResourceType, "Texture", EE_FILENAME_MAX);
		Utils_strlcpy(textureMan->_mDatFileName, "gfx.dat", EE_FILENAME_MAX);
		Utils_strlcpy(textureMan->_mDefaultResource, "DEBUG_ERROR_777", EE_FILENAME_MAX);
		Utils_strlcpy(textureMan->_mFileExtension, ".png", EE_FILENAME_MAX);

		File_PathCombine3(&tempString, "data", "gfx", "game");
		Utils_strlcpy(textureMan->_mDirectories[0], MString_Text(tempString), EE_PATH_MAX);

		File_PathCombine3(&tempString, "data", "gfx", "tilesets");
		Utils_strlcpy(textureMan->_mDirectories[1], MString_Text(tempString), EE_PATH_MAX);

		File_PathCombine3(&tempString, "data", "gfx", "special");
		Utils_strlcpy(textureMan->_mDirectories[2], MString_Text(tempString), EE_PATH_MAX);

		textureMan->_mFromStream = (ResourceMan_FromStreamFunc)Texture_FromStream;
		//textureMan->_mDispose = (ResourceMan_DisposeFunc)Texture_Dispose;
		textureMan->_mReadFromDirectory = true;
		textureMan->_mDelayLoading = true;
	}

	{
		ResourceMan* fontTextureMan = ResourceList_FontTexture();
		ResourceMan_Init(fontTextureMan);
		Utils_strlcpy(fontTextureMan->_mResourceType, "Font Texture", EE_FILENAME_MAX);
		Utils_strlcpy(fontTextureMan->_mDatFileName, "fontsgfx.dat", EE_FILENAME_MAX);
		Utils_strlcpy(fontTextureMan->_mFileExtension, ".png", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "fonts");
		Utils_strlcpy(fontTextureMan->_mDirectories[0], MString_Text(tempString), EE_PATH_MAX);
		fontTextureMan->_mFromStream = (ResourceMan_FromStreamFunc)Texture_FromStream;
		//fontTextureMan->_mDispose = (ResourceMan_DisposeFunc)Texture_Dispose;
		fontTextureMan->_mReadFromDirectory = true;
	}

	{
		ResourceMan* movieTextureMan = ResourceList_MovieTexture();
		ResourceMan_Init(movieTextureMan);
		Utils_strlcpy(movieTextureMan->_mResourceType, "Movie Texture", EE_FILENAME_MAX);
		Utils_strlcpy(movieTextureMan->_mDatFileName, "moviegfx.dat", EE_FILENAME_MAX);
		Utils_strlcpy(movieTextureMan->_mFileExtension, ".png", EE_FILENAME_MAX);
		File_PathCombine3(&tempString, "data", "gfx", "movies");
		Utils_strlcpy(movieTextureMan->_mDirectories[0], MString_Text(tempString), EE_PATH_MAX);
		movieTextureMan->_mFromStream = (ResourceMan_FromStreamFunc)Texture_FromStream;
		//movieTextureMan->_mDispose = (ResourceMan_DisposeFunc)Texture_Dispose;
		movieTextureMan->_mReadFromDirectory = true;
		movieTextureMan->_mDelayLoading = true;
	}

	{
		ResourceMan* soundEffectMan = ResourceList_SoundEffect();
		ResourceMan_Init(soundEffectMan);
		Utils_strlcpy(soundEffectMan->_mResourceType, "Sound Effect", EE_FILENAME_MAX);
		Utils_strlcpy(soundEffectMan->_mDatFileName, "sfx.dat", EE_FILENAME_MAX);
		Utils_strlcpy(soundEffectMan->_mDefaultResource, "editorSave", EE_FILENAME_MAX);
		Utils_strlcpy(soundEffectMan->_mFileExtension, ".wav", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "sfx");
		Utils_strlcpy(soundEffectMan->_mDirectories[0], MString_Text(tempString), EE_PATH_MAX);
		soundEffectMan->_mFromStream = (ResourceMan_FromStreamFunc)SoundEffect_FromStream;
		soundEffectMan->_mDispose = (ResourceMan_DisposeFunc)SoundEffect_Dispose;
		soundEffectMan->_mReadFromDirectory = true;
		soundEffectMan->_mDelayLoading = true;
	}

	{
		ResourceMan* musicMan = ResourceList_Music();
		ResourceMan_Init(musicMan);
		Utils_strlcpy(musicMan->_mResourceType, "Music", EE_FILENAME_MAX);
		Utils_strlcpy(musicMan->_mDatFileName, "music.dat", EE_FILENAME_MAX);
		Utils_strlcpy(musicMan->_mDefaultResource, "default", EE_FILENAME_MAX);
		Utils_strlcpy(musicMan->_mFileExtension, ".wav", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "music");
		Utils_strlcpy(musicMan->_mDirectories[0], MString_Text(tempString), EE_PATH_MAX);
		musicMan->_mFromStream = (ResourceMan_FromStreamFunc)Music_FromStream;
		musicMan->_mDispose = (ResourceMan_DisposeFunc)Music_Dispose;
		musicMan->_mReadFromDirectory = true;
		musicMan->_mDelayLoading = true;
	}

	{
		ResourceMan* levelDataMan = ResourceList_LevelData();
		ResourceMan_Init(levelDataMan);
		Utils_strlcpy(levelDataMan->_mResourceType, "Level Data", EE_FILENAME_MAX);
		Utils_strlcpy(levelDataMan->_mDatFileName, "lvl.dat", EE_FILENAME_MAX);
		Utils_strlcpy(levelDataMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "lvl");
		Utils_strlcpy(levelDataMan->_mDirectories[0], MString_Text(tempString), EE_PATH_MAX);
		levelDataMan->_mFromStream = (ResourceMan_FromStreamFunc)LevelData_FromStream;
		levelDataMan->_mDispose = (ResourceMan_DisposeFunc)LevelData_Dispose;
		levelDataMan->_mWrite = (ResourceMan_WriteFunc)LevelData_Write;
		levelDataMan->_mCreateNew = (ResourceMan_CreateNewFunc)LevelData_CreateNew;
		levelDataMan->_mReadFromDirectory = true;
		levelDataMan->_mIsReadingText = true;
	}

	{
		ResourceMan* tilesetOffsetMan = ResourceList_TilesetOffset();
		ResourceMan_Init(tilesetOffsetMan);
		Utils_strlcpy(tilesetOffsetMan->_mResourceType, "Tileset Offset", EE_FILENAME_MAX);
		Utils_strlcpy(tilesetOffsetMan->_mDatFileName, "offsetstile.dat", EE_FILENAME_MAX);
		Utils_strlcpy(tilesetOffsetMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine3(&tempString, "data", "gfx", "generated_tilesets");
		Utils_strlcpy(tilesetOffsetMan->_mDirectories[0], MString_Text(tempString), EE_PATH_MAX);
		tilesetOffsetMan->_mFromStream = (ResourceMan_FromStreamFunc)TilesetOffset_FromStream;
		tilesetOffsetMan->_mDispose = (ResourceMan_DisposeFunc)TilesetOffset_Dispose;
		tilesetOffsetMan->_mReadFromDirectory = true;
	}

	{
		ResourceMan* textureOffsetMan = ResourceList_TextureOffset();
		ResourceMan_Init(textureOffsetMan);
		Utils_strlcpy(textureOffsetMan->_mResourceType, "Texture Offset", EE_FILENAME_MAX);
		Utils_strlcpy(textureOffsetMan->_mDatFileName, "offsetstex.dat", EE_FILENAME_MAX);
		Utils_strlcpy(textureOffsetMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine3(&tempString, "data", "gfx", "generated_game");
		Utils_strlcpy(textureOffsetMan->_mDirectories[0], MString_Text(tempString), EE_PATH_MAX);
		textureOffsetMan->_mFromStream = (ResourceMan_FromStreamFunc)TextureOffset_FromStream;
		textureOffsetMan->_mDispose = (ResourceMan_DisposeFunc)TextureOffset_Dispose;
		textureOffsetMan->_mReadFromDirectory = true;
	}

	{
		ResourceMan* fontMan = ResourceList_Font();
		ResourceMan_Init(fontMan);
		Utils_strlcpy(fontMan->_mResourceType, "Font", EE_FILENAME_MAX);
		Utils_strlcpy(fontMan->_mDatFileName, "fonts.dat", EE_FILENAME_MAX);
		Utils_strlcpy(fontMan->_mFileExtension, ".fnt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "fonts");
		Utils_strlcpy(fontMan->_mDirectories[0], MString_Text(tempString), EE_FILENAME_MAX);
		fontMan->_mFromStream = (ResourceMan_FromStreamFunc)BmFont_FromStream;
		fontMan->_mDispose = (ResourceMan_DisposeFunc)BmFont_Dispose;
		fontMan->_mReadFromDirectory = true;
	}

	{
		ResourceMan* particleMan = ResourceList_Particle();
		ResourceMan_Init(particleMan);
		Utils_strlcpy(particleMan->_mResourceType, "Particle", EE_FILENAME_MAX);
		Utils_strlcpy(particleMan->_mDatFileName, "particles.dat", EE_FILENAME_MAX);
		Utils_strlcpy(particleMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "particles");
		Utils_strlcpy(particleMan->_mDirectories[0], MString_Text(tempString), EE_FILENAME_MAX);
		particleMan->_mFromStream = (ResourceMan_FromStreamFunc)Particle_FromStream;
		particleMan->_mDispose = (ResourceMan_DisposeFunc)Particle_Dispose;
		particleMan->_mRead = (ResourceMan_ReadFunc)Particle_Read;
		particleMan->_mWrite = (ResourceMan_WriteFunc)Particle_Write;
		particleMan->_mCreateNew = (ResourceMan_CreateNewFunc)Particle_CreateNew;
		particleMan->_mCopyTo = (ResourceMan_CopyToFunc)Particle_CopyTo;
		particleMan->_mReadFromDirectory = true;
		particleMan->_mIsReadingText = true;
	}

	{
		ResourceMan* movieTimingMan = ResourceList_MovieTiming();
		ResourceMan_Init(movieTimingMan);
		Utils_strlcpy(movieTimingMan->_mResourceType, "Movie Timings", EE_FILENAME_MAX);
		Utils_strlcpy(movieTimingMan->_mDatFileName, "timings.dat", EE_FILENAME_MAX);
		Utils_strlcpy(movieTimingMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "timings");
		Utils_strlcpy(movieTimingMan->_mDirectories[0], MString_Text(tempString), EE_FILENAME_MAX);
		movieTimingMan->_mFromStream = (ResourceMan_FromStreamFunc)MovieTiming_FromStream;
		movieTimingMan->_mDispose = (ResourceMan_DisposeFunc)MovieTiming_Dispose;
		movieTimingMan->_mReadFromDirectory = true;
	}

	{
		ResourceMan* movieMan = ResourceList_Movie();
		ResourceMan_Init(movieMan);
		Utils_strlcpy(movieMan->_mResourceType, "Movies", EE_FILENAME_MAX);
		Utils_strlcpy(movieMan->_mDatFileName, "movies.dat", EE_FILENAME_MAX);
		Utils_strlcpy(movieMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "movies");
		Utils_strlcpy(movieMan->_mDirectories[0], MString_Text(tempString), EE_FILENAME_MAX);
		movieMan->_mFromStream = (ResourceMan_FromStreamFunc)Movie_FromStream;
		movieMan->_mDispose = (ResourceMan_DisposeFunc)Movie_Dispose;
		movieMan->_mReadFromDirectory = true;
	}

	{
		ResourceMan* ttFontMan = ResourceList_TTFont();
		ResourceMan_Init(ttFontMan);
		Utils_strlcpy(ttFontMan->_mResourceType, "TTFont", EE_FILENAME_MAX);
		Utils_strlcpy(ttFontMan->_mDatFileName, "ttfonts.dat", EE_FILENAME_MAX);
		Utils_strlcpy(ttFontMan->_mFileExtension, ".ttf", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "ttfonts");
		Utils_strlcpy(ttFontMan->_mDirectories[0], MString_Text(tempString), EE_FILENAME_MAX);
		ttFontMan->_mFromStream = (ResourceMan_FromStreamFunc)TTFont_FromStream;
		ttFontMan->_mDispose = (ResourceMan_DisposeFunc)TTFont_Dispose;
		ttFontMan->_mReadFromDirectory = true;
	}

	{
		ResourceMan* thingSettingsMan = ResourceList_ThingSettings();
		ResourceMan_Init(thingSettingsMan);
		Utils_strlcpy(thingSettingsMan->_mResourceType, "Thing Settings", EE_FILENAME_MAX);
		Utils_strlcpy(thingSettingsMan->_mDatFileName, "things.dat", EE_FILENAME_MAX);
		Utils_strlcpy(thingSettingsMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "things");
		Utils_strlcpy(thingSettingsMan->_mDirectories[0], MString_Text(tempString), EE_FILENAME_MAX);
		thingSettingsMan->_mFromStream = (ResourceMan_FromStreamFunc)ThingSettings_FromStream;
		thingSettingsMan->_mDispose = (ResourceMan_DisposeFunc)ThingSettings_Dispose;
		thingSettingsMan->_mWrite = (ResourceMan_WriteFunc)ThingSettings_Write;
		thingSettingsMan->_mReadFromDirectory = true;
		thingSettingsMan->_mIsReadingText = true;
	}

	MString_Dispose(&tempString);

	_mHasInit = true;
}
void ResourceList_LoadAllFromDat(void)
{
	for (int32_t i = 0; i < MANAGERS_LEN; i += 1)
	{
		if (_mManagers[i]._mHasInit)
		{
			ResourceMan_LoadAllFromDat(&_mManagers[i]);
		}
	}
}
ResourceMan* ResourceList_AnimTile(void)
{
	return &_mManagers[MANAGERS_INDEX_ANIM_TILE];
}
ResourceMan* ResourceList_Prop(void)
{
	return &_mManagers[MANAGERS_INDEX_PROP];
}
ResourceMan* ResourceList_Texture(void)
{
	return &_mManagers[MANAGERS_INDEX_TEXTURE];
}
ResourceMan* ResourceList_FontTexture(void)
{
	return &_mManagers[MANAGERS_INDEX_FONT_TEXTURE];
}
ResourceMan* ResourceList_MovieTexture(void)
{
	return &_mManagers[MANAGERS_INDEX_MOVIE_TEXTURE];
}
ResourceMan* ResourceList_SoundEffect(void)
{
	return &_mManagers[MANAGERS_INDEX_SOUND_EFFECT];
}
ResourceMan* ResourceList_Music(void)
{
	return &_mManagers[MANAGERS_INDEX_MUSIC];
}
ResourceMan* ResourceList_LevelData(void)
{
	return &_mManagers[MANAGERS_INDEX_LEVEL_DATA];
}
ResourceMan* ResourceList_TilesetOffset(void)
{
	return &_mManagers[MANAGERS_INDEX_TILESET_OFFSET];
}
ResourceMan* ResourceList_TextureOffset(void)
{
	return &_mManagers[MANAGERS_INDEX_TEXTURE_OFFSET];
}
ResourceMan* ResourceList_Font(void)
{
	return &_mManagers[MANAGERS_INDEX_FONT];
}
ResourceMan* ResourceList_Particle(void)
{
	return &_mManagers[MANAGERS_INDEX_PARTICLE];
}
ResourceMan* ResourceList_MovieTiming(void)
{
	return &_mManagers[MANAGERS_INDEX_MOVIE_TIMING];
}
ResourceMan* ResourceList_Movie(void)
{
	return &_mManagers[MANAGERS_INDEX_MOVIE];
}
ResourceMan* ResourceList_TTFont(void)
{
	return &_mManagers[MANAGERS_INDEX_TTFONT];
}
ResourceMan* ResourceList_ThingSettings(void)
{
	return &_mManagers[MANAGERS_INDEX_THING_SETTINGS];
}
