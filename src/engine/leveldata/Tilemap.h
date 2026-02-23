#pragma once

#include "../utils/Macros.h"
#include "Tile.h"
#include "../math/Rectangle.h"

typedef struct Tilemap
{
	Tile** tiles;
	Rectangle boundary;
	Rectangle boundaryInPixels;
} Tilemap;

Tilemap* Tilemap_CreateCloneOfSubsection(Tilemap* src, Rectangle srcLocation);
Tilemap* Tilemap_Create(Rectangle boundary);
void Tilemap_CopyTo(Tilemap* dst, Rectangle dstLocation, Tilemap* src, Rectangle srcLocation, bool respectCopyCvars);
void Tilemap_Init(Tilemap* tm, Rectangle boundary);
void Tilemap_FillWithNewTiles(Tilemap* tm);
int32_t Tilemap_GetPos1D(Tilemap* tm, int32_t x, int32_t y);
Tile* Tilemap_GetTile(Tilemap* tm, int32_t x, int32_t y);
void Tilemap_SetTile(Tilemap* tm, int32_t x, int32_t y, Tile* tile);
bool Tilemap_IsSafe(Tilemap* tm, int32_t x, int32_t y);