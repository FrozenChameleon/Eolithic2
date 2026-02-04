/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ResourceManagerList.h"

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
static ResourceManager _mManagers[MANAGERS_LEN];

void ResourceManagerList_Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	MString* tempString = NULL;

	{
		ResourceManager* animTileMan = ResourceManagerList_AnimTile();
		ResourceManager_Init(animTileMan);
		Utils_strlcpy(animTileMan->_mResourceType, "AnimTile", EE_FILENAME_MAX);
		Utils_strlcpy(animTileMan->_mDatFileName, "animtile.dat", EE_FILENAME_MAX);
		Utils_strlcpy(animTileMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "animtile");
		Utils_strlcpy(animTileMan->_mDirectory, MString_Text(tempString), EE_PATH_MAX);
		animTileMan->_mFromStream = (ResourceManager_FromStreamFunc)AnimTile_FromStream;
		animTileMan->_mDispose = (ResourceManager_DisposeFunc)AnimTile_Dispose;
		animTileMan->_mRead = (ResourceManager_ReadFunc)AnimTile_Read;
		animTileMan->_mWrite = (ResourceManager_WriteFunc)AnimTile_Write;
		animTileMan->_mReadFromDirectory = true;
		animTileMan->_mIsReadingText = true;
	}

	{
		ResourceManager* propMan = ResourceManagerList_Prop();
		ResourceManager_Init(propMan);
		Utils_strlcpy(propMan->_mResourceType, "Prop", EE_FILENAME_MAX);
		Utils_strlcpy(propMan->_mDatFileName, "props.dat", EE_FILENAME_MAX);
		Utils_strlcpy(propMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "props");
		Utils_strlcpy(propMan->_mDirectory, MString_Text(tempString), EE_PATH_MAX);
		propMan->_mFromStream = (ResourceManager_FromStreamFunc)Prop_FromStream;
		propMan->_mDispose = (ResourceManager_DisposeFunc)Prop_Dispose;
		propMan->_mRead = (ResourceManager_ReadFunc)Prop_Read;
		propMan->_mWrite = (ResourceManager_WriteFunc)Prop_Write;
		propMan->_mReadFromDirectory = true;
		propMan->_mIsReadingText = true;
	}

	{
		ResourceManager* textureMan = ResourceManagerList_Texture();
		ResourceManager_Init(textureMan);
		Utils_strlcpy(textureMan->_mResourceType, "Texture", EE_FILENAME_MAX);
		Utils_strlcpy(textureMan->_mDatFileName, "gfx.dat", EE_FILENAME_MAX);
		Utils_strlcpy(textureMan->_mDefaultResource, "DEBUG_ERROR_777", EE_FILENAME_MAX);
		textureMan->_mFromStream = (ResourceManager_FromStreamFunc)Texture_FromStream;
		textureMan->_mDispose = (ResourceManager_DisposeFunc)Texture_Dispose;
	}

	{
		ResourceManager* fontTextureMan = ResourceManagerList_FontTexture();
		ResourceManager_Init(fontTextureMan);
		Utils_strlcpy(fontTextureMan->_mResourceType, "Font Texture", EE_FILENAME_MAX);
		Utils_strlcpy(fontTextureMan->_mDatFileName, "fontsgfx.dat", EE_FILENAME_MAX);
		Utils_strlcpy(fontTextureMan->_mFileExtension, ".png", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "fonts");
		Utils_strlcpy(fontTextureMan->_mDirectory, MString_Text(tempString), EE_PATH_MAX);
		fontTextureMan->_mFromStream = (ResourceManager_FromStreamFunc)Texture_FromStream;
		fontTextureMan->_mDispose = (ResourceManager_DisposeFunc)Texture_Dispose;
		fontTextureMan->_mReadFromDirectory = true;
	}

	{
		ResourceManager* movieTextureMan = ResourceManagerList_MovieTexture();
		ResourceManager_Init(movieTextureMan);
		Utils_strlcpy(movieTextureMan->_mResourceType, "Movie Texture", EE_FILENAME_MAX);
		Utils_strlcpy(movieTextureMan->_mDatFileName, "moviegfx.dat", EE_FILENAME_MAX);
		movieTextureMan->_mFromStream = (ResourceManager_FromStreamFunc)Texture_FromStream;
		movieTextureMan->_mDispose = (ResourceManager_DisposeFunc)Texture_Dispose;
	}

	{
		ResourceManager* soundEffectMan = ResourceManagerList_SoundEffect();
		ResourceManager_Init(soundEffectMan);
		Utils_strlcpy(soundEffectMan->_mResourceType, "Sound Effect", EE_FILENAME_MAX);
		Utils_strlcpy(soundEffectMan->_mDatFileName, "sfx.dat", EE_FILENAME_MAX);
		Utils_strlcpy(soundEffectMan->_mDefaultResource, "editorSave", EE_FILENAME_MAX);
		Utils_strlcpy(soundEffectMan->_mFileExtension, ".wav", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "sfx");
		Utils_strlcpy(soundEffectMan->_mDirectory, MString_Text(tempString), EE_PATH_MAX);
		soundEffectMan->_mFromStream = (ResourceManager_FromStreamFunc)SoundEffect_FromStream;
		soundEffectMan->_mDispose = (ResourceManager_DisposeFunc)SoundEffect_Dispose;
		soundEffectMan->_mReadFromDirectory = true;
	}

	{
		ResourceManager* musicMan = ResourceManagerList_Music();
		ResourceManager_Init(musicMan);
		Utils_strlcpy(musicMan->_mResourceType, "Music", EE_FILENAME_MAX);
		Utils_strlcpy(musicMan->_mDatFileName, "music.dat", EE_FILENAME_MAX);
		Utils_strlcpy(musicMan->_mDefaultResource, "default", EE_FILENAME_MAX);
		Utils_strlcpy(musicMan->_mFileExtension, ".wav", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "music");
		Utils_strlcpy(musicMan->_mDirectory, MString_Text(tempString), EE_PATH_MAX);
		musicMan->_mFromStream = (ResourceManager_FromStreamFunc)Music_FromStream;
		musicMan->_mDispose = (ResourceManager_DisposeFunc)Music_Dispose;
		musicMan->_mReadFromDirectory = true;
	}

	{
		ResourceManager* levelDataMan = ResourceManagerList_LevelData();
		ResourceManager_Init(levelDataMan);
		Utils_strlcpy(levelDataMan->_mResourceType, "Level Data", EE_FILENAME_MAX);
		Utils_strlcpy(levelDataMan->_mDatFileName, "lvl.dat", EE_FILENAME_MAX);
		Utils_strlcpy(levelDataMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "lvl");
		Utils_strlcpy(levelDataMan->_mDirectory, MString_Text(tempString), EE_PATH_MAX);
		levelDataMan->_mFromStream = (ResourceManager_FromStreamFunc)LevelData_FromStream;
		levelDataMan->_mDispose = (ResourceManager_DisposeFunc)LevelData_Dispose;
		levelDataMan->_mWrite = (ResourceManager_WriteFunc)LevelData_Write;
		levelDataMan->_mReadFromDirectory = true;
		levelDataMan->_mIsReadingText = true;
	}

	{
		ResourceManager* tilesetOffsetMan = ResourceManagerList_TilesetOffset();
		ResourceManager_Init(tilesetOffsetMan);
		Utils_strlcpy(tilesetOffsetMan->_mResourceType, "Tileset Offset", EE_FILENAME_MAX);
		Utils_strlcpy(tilesetOffsetMan->_mDatFileName, "offsetstile.dat", EE_FILENAME_MAX);
		Utils_strlcpy(tilesetOffsetMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine3(&tempString, "data", "gfx", "generated_tilesets");
		Utils_strlcpy(tilesetOffsetMan->_mDirectory, MString_Text(tempString), EE_PATH_MAX);
		tilesetOffsetMan->_mFromStream = (ResourceManager_FromStreamFunc)TilesetOffset_FromStream;
		tilesetOffsetMan->_mDispose = (ResourceManager_DisposeFunc)TilesetOffset_Dispose;
		tilesetOffsetMan->_mReadFromDirectory = true;
	}

	{
		ResourceManager* textureOffsetMan = ResourceManagerList_TextureOffset();
		ResourceManager_Init(textureOffsetMan);
		Utils_strlcpy(textureOffsetMan->_mResourceType, "Texture Offset", EE_FILENAME_MAX);
		Utils_strlcpy(textureOffsetMan->_mDatFileName, "offsetstex.dat", EE_FILENAME_MAX);
		Utils_strlcpy(textureOffsetMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine3(&tempString, "data", "gfx", "generated_game");
		Utils_strlcpy(textureOffsetMan->_mDirectory, MString_Text(tempString), EE_PATH_MAX);
		textureOffsetMan->_mFromStream = (ResourceManager_FromStreamFunc)TextureOffset_FromStream;
		textureOffsetMan->_mDispose = (ResourceManager_DisposeFunc)TextureOffset_Dispose;
		textureOffsetMan->_mReadFromDirectory = true;
	}

	{
		ResourceManager* fontMan = ResourceManagerList_Font();
		ResourceManager_Init(fontMan);
		Utils_strlcpy(fontMan->_mResourceType, "Font", EE_FILENAME_MAX);
		Utils_strlcpy(fontMan->_mDatFileName, "fonts.dat", EE_FILENAME_MAX);
		Utils_strlcpy(fontMan->_mFileExtension, ".fnt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "fonts");
		Utils_strlcpy(fontMan->_mDirectory, MString_Text(tempString), EE_FILENAME_MAX);
		fontMan->_mFromStream = (ResourceManager_FromStreamFunc)BmFont_FromStream;
		fontMan->_mDispose = (ResourceManager_DisposeFunc)BmFont_Dispose;
		fontMan->_mReadFromDirectory = true;
	}

	{
		ResourceManager* particleMan = ResourceManagerList_Particle();
		ResourceManager_Init(particleMan);
		Utils_strlcpy(particleMan->_mResourceType, "Particle", EE_FILENAME_MAX);
		Utils_strlcpy(particleMan->_mDatFileName, "particles.dat", EE_FILENAME_MAX);
		Utils_strlcpy(particleMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "particles");
		Utils_strlcpy(particleMan->_mDirectory, MString_Text(tempString), EE_FILENAME_MAX);
		particleMan->_mFromStream = (ResourceManager_FromStreamFunc)Particle_FromStream;
		particleMan->_mDispose = (ResourceManager_DisposeFunc)Particle_Dispose;
		particleMan->_mRead = (ResourceManager_ReadFunc)Particle_Read;
		particleMan->_mWrite = (ResourceManager_WriteFunc)Particle_Write;
		particleMan->_mReadFromDirectory = true;
		particleMan->_mIsReadingText = true;
	}

	{
		ResourceManager* movieTimingMan = ResourceManagerList_MovieTiming();
		ResourceManager_Init(movieTimingMan);
		Utils_strlcpy(movieTimingMan->_mResourceType, "Movie Timings", EE_FILENAME_MAX);
		Utils_strlcpy(movieTimingMan->_mDatFileName, "timings.dat", EE_FILENAME_MAX);
		Utils_strlcpy(movieTimingMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "timings");
		Utils_strlcpy(movieTimingMan->_mDirectory, MString_Text(tempString), EE_FILENAME_MAX);
		movieTimingMan->_mFromStream = (ResourceManager_FromStreamFunc)MovieTiming_FromStream;
		movieTimingMan->_mDispose = (ResourceManager_DisposeFunc)MovieTiming_Dispose;
		movieTimingMan->_mReadFromDirectory = true;
	}

	{
		ResourceManager* movieMan = ResourceManagerList_Movie();
		ResourceManager_Init(movieMan);
		Utils_strlcpy(movieMan->_mResourceType, "Movies", EE_FILENAME_MAX);
		Utils_strlcpy(movieMan->_mDatFileName, "movies.dat", EE_FILENAME_MAX);
		Utils_strlcpy(movieMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "movies");
		Utils_strlcpy(movieMan->_mDirectory, MString_Text(tempString), EE_FILENAME_MAX);
		movieMan->_mFromStream = (ResourceManager_FromStreamFunc)Movie_FromStream;
		movieMan->_mDispose = (ResourceManager_DisposeFunc)Movie_Dispose;
		movieMan->_mReadFromDirectory = true;
	}

	{
		ResourceManager* ttFontMan = ResourceManagerList_TTFont();
		ResourceManager_Init(ttFontMan);
		Utils_strlcpy(ttFontMan->_mResourceType, "TTFont", EE_FILENAME_MAX);
		Utils_strlcpy(ttFontMan->_mDatFileName, "ttfonts.dat", EE_FILENAME_MAX);
		Utils_strlcpy(ttFontMan->_mFileExtension, ".ttf", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "ttfonts");
		Utils_strlcpy(ttFontMan->_mDirectory, MString_Text(tempString), EE_FILENAME_MAX);
		ttFontMan->_mFromStream = (ResourceManager_FromStreamFunc)TTFont_FromStream;
		ttFontMan->_mDispose = (ResourceManager_DisposeFunc)TTFont_Dispose;
		ttFontMan->_mReadFromDirectory = true;
	}

	{
		ResourceManager* thingSettingsMan = ResourceManagerList_ThingSettings();
		ResourceManager_Init(thingSettingsMan);
		Utils_strlcpy(thingSettingsMan->_mResourceType, "Thing Settings", EE_FILENAME_MAX);
		Utils_strlcpy(thingSettingsMan->_mDatFileName, "things.dat", EE_FILENAME_MAX);
		Utils_strlcpy(thingSettingsMan->_mFileExtension, ".txt", EE_FILENAME_MAX);
		File_PathCombine2(&tempString, "data", "things");
		Utils_strlcpy(thingSettingsMan->_mDirectory, MString_Text(tempString), EE_FILENAME_MAX);
		thingSettingsMan->_mFromStream = (ResourceManager_FromStreamFunc)ThingSettings_FromStream;
		thingSettingsMan->_mDispose = (ResourceManager_DisposeFunc)ThingSettings_Dispose;
		thingSettingsMan->_mWrite = (ResourceManager_WriteFunc)ThingSettings_Write;
		thingSettingsMan->_mReadFromDirectory = true;
		thingSettingsMan->_mIsReadingText = true;
	}

	MString_Dispose(&tempString);

	_mHasInit = true;
}
void ResourceManagerList_LoadAllFromDat(void)
{
	for (int32_t i = 0; i < MANAGERS_LEN; i += 1)
	{
		if (_mManagers[i]._mHasInit)
		{
			ResourceManager_LoadAllFromDat(&_mManagers[i]);
		}
	}
}
ResourceManager* ResourceManagerList_AnimTile(void)
{
	return &_mManagers[MANAGERS_INDEX_ANIM_TILE];
}
ResourceManager* ResourceManagerList_Prop(void)
{
	return &_mManagers[MANAGERS_INDEX_PROP];
}
ResourceManager* ResourceManagerList_Texture(void)
{
	return &_mManagers[MANAGERS_INDEX_TEXTURE];
}
ResourceManager* ResourceManagerList_FontTexture(void)
{
	return &_mManagers[MANAGERS_INDEX_FONT_TEXTURE];
}
ResourceManager* ResourceManagerList_MovieTexture(void)
{
	return &_mManagers[MANAGERS_INDEX_MOVIE_TEXTURE];
}
ResourceManager* ResourceManagerList_SoundEffect(void)
{
	return &_mManagers[MANAGERS_INDEX_SOUND_EFFECT];
}
ResourceManager* ResourceManagerList_Music(void)
{
	return &_mManagers[MANAGERS_INDEX_MUSIC];
}
ResourceManager* ResourceManagerList_LevelData(void)
{
	return &_mManagers[MANAGERS_INDEX_LEVEL_DATA];
}
ResourceManager* ResourceManagerList_TilesetOffset(void)
{
	return &_mManagers[MANAGERS_INDEX_TILESET_OFFSET];
}
ResourceManager* ResourceManagerList_TextureOffset(void)
{
	return &_mManagers[MANAGERS_INDEX_TEXTURE_OFFSET];
}
ResourceManager* ResourceManagerList_Font(void)
{
	return &_mManagers[MANAGERS_INDEX_FONT];
}
ResourceManager* ResourceManagerList_Particle(void)
{
	return &_mManagers[MANAGERS_INDEX_PARTICLE];
}
ResourceManager* ResourceManagerList_MovieTiming(void)
{
	return &_mManagers[MANAGERS_INDEX_MOVIE_TIMING];
}
ResourceManager* ResourceManagerList_Movie(void)
{
	return &_mManagers[MANAGERS_INDEX_MOVIE];
}
ResourceManager* ResourceManagerList_TTFont(void)
{
	return &_mManagers[MANAGERS_INDEX_TTFONT];
}
ResourceManager* ResourceManagerList_ThingSettings(void)
{
	return &_mManagers[MANAGERS_INDEX_THING_SETTINGS];
}
