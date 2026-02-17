/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "ResourceManager.h"

void ResourceManagerList_Init(void);
void ResourceManagerList_LoadAllFromDat(void);
ResourceManager* ResourceManagerList_AnimTile(void);
ResourceManager* ResourceManagerList_Prop(void);
ResourceManager* ResourceManagerList_Texture(void);
ResourceManager* ResourceManagerList_FontTexture(void);
ResourceManager* ResourceManagerList_MovieTexture(void);
ResourceManager* ResourceManagerList_SoundEffect(void);
ResourceManager* ResourceManagerList_Music(void);
ResourceManager* ResourceManagerList_LevelData(void);
ResourceManager* ResourceManagerList_TilesetOffset(void);
ResourceManager* ResourceManagerList_TextureOffset(void);
ResourceManager* ResourceManagerList_Font(void);
ResourceManager* ResourceManagerList_Particle(void);
ResourceManager* ResourceManagerList_MovieTiming(void);
ResourceManager* ResourceManagerList_Movie(void);
ResourceManager* ResourceManagerList_TTFont(void);
ResourceManager* ResourceManagerList_ThingSettings(void);