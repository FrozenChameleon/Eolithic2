/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "ResourceMan.h"

void ResourceList_Init(void);
void ResourceList_LoadAllFromDat(void);
ResourceMan* ResourceList_AnimTile(void);
ResourceMan* ResourceList_Prop(void);
ResourceMan* ResourceList_Texture(void);
ResourceMan* ResourceList_FontTexture(void);
ResourceMan* ResourceList_MovieTexture(void);
ResourceMan* ResourceList_SoundEffect(void);
ResourceMan* ResourceList_Music(void);
ResourceMan* ResourceList_LevelData(void);
ResourceMan* ResourceList_TilesetOffset(void);
ResourceMan* ResourceList_TextureOffset(void);
ResourceMan* ResourceList_Font(void);
ResourceMan* ResourceList_Particle(void);
ResourceMan* ResourceList_MovieTiming(void);
ResourceMan* ResourceList_Movie(void);
ResourceMan* ResourceList_TTFont(void);
ResourceMan* ResourceList_ThingSettings(void);