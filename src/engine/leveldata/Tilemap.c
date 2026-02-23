#include "Tilemap.h"

#include "../utils/Utils.h"
#include "../utils/Logger.h"

static bool BoundaryCheck(Rectangle boundary)
{
	if ((boundary.Width <= 0) || (boundary.Height <= 0))
	{
		Logger_LogWarning("Invalid dimensions for tilemap boundary");
		return false;
	}
	return true;
}

Tilemap* Tilemap_CreateCloneOfSubsection(Tilemap* src, Rectangle srcLocation)
{
	Rectangle dstBoundary = Rectangle_Create(0, 0, srcLocation.Width, srcLocation.Height);
	if (!BoundaryCheck(dstBoundary))
	{
		return NULL;
	}

	Tilemap* dstClone = Tilemap_Create(dstBoundary);
	for (int j = 0; j < dstBoundary.Height; j += 1)
	{
		for (int i = 0; i < dstBoundary.Width; i += 1)
		{
			Tile* sourceTile = Tilemap_GetTile(src, srcLocation.X + i, srcLocation.Y + j);
			Tilemap_SetTile(dstClone, i, j, Tile_Clone(sourceTile));
		}
	}
	return dstClone;
}

Tilemap* Tilemap_Create(Rectangle boundary)
{
	if (!BoundaryCheck(boundary))
	{
		return NULL;
	}

	Tilemap* tm = (Tilemap*)Utils_calloc(1, sizeof(Tilemap));
	Tilemap_Init(tm, boundary);
	return tm;
}
void Tilemap_CopyTo(Tilemap* dst, Rectangle dstLocation, Tilemap* src, Rectangle srcLocation, bool respectCopyCvars)
{
	if ((dstLocation.Width != srcLocation.Width) || (dstLocation.Height != srcLocation.Height))
	{
		Logger_LogWarning("Cannot do Tilemap_CopyTo with these locations!");
		return;
	}

	for (int j = 0; j < srcLocation.Height; j += 1)
	{
		for (int i = 0; i < srcLocation.Width; i += 1)
		{
			Tile* srcTile = Tilemap_GetTile(src, srcLocation.X + i, srcLocation.Y + j);
			Tile* dstTile = Tilemap_GetTile(dst, dstLocation.X + i, dstLocation.Y + j);
			if ((srcTile == NULL) || (dstTile == NULL))
			{
				Logger_LogWarning("Tilemap_CopyTo getting NULL tiles!");
				continue;
			}
			Tile_CopyTo(dstTile, srcTile, respectCopyCvars);
		}
	}
}
void Tilemap_Init(Tilemap* tm, Rectangle boundary)
{
	if (!BoundaryCheck(boundary))
	{
		return;
	}

	Utils_memset(tm, 0, sizeof(Tilemap));
	tm->boundary = boundary;
	tm->boundaryInPixels = Rectangle_Create(0, 0, boundary.Width * TILE_SIZE, boundary.Height * TILE_SIZE);
	tm->tiles = (Tile**)Utils_calloc(boundary.Width * boundary.Height, sizeof(Tile*)); //TODO MEMORY LEAKS
}
void Tilemap_FillWithNewTiles(Tilemap* tm)
{
	for (int j = 0; j < tm->boundary.Height; j += 1)
	{
		for (int i = 0; i < tm->boundary.Width; i += 1)
		{
			Tilemap_SetTile(tm, i, j, Tile_Create());
		}
	}
}
int32_t Tilemap_GetPos1D(Tilemap* tm, int32_t x, int32_t y)
{
	return x + (y * tm->boundary.Width); //Utils_Get1DArrayPosFor2DArray(x, y, tm->boundary.Width);
}
Tile* Tilemap_GetTile(Tilemap* tm, int32_t x, int32_t y)
{
	if (!Tilemap_IsSafe(tm, x, y))
	{
		return NULL;
	}

	return tm->tiles[Tilemap_GetPos1D(tm, x, y)];
}
void Tilemap_SetTile(Tilemap* tm, int32_t x, int32_t y, Tile* tile)
{
	if (!Tilemap_IsSafe(tm, x, y))
	{
		return;
	}
	else if (tile == NULL)
	{
		Logger_LogWarning("Tilemap_SetTile given NULL tile!");
		return;
	}

	tm->tiles[Tilemap_GetPos1D(tm, x, y)] = tile;
}
bool Tilemap_IsSafe(Tilemap* tm, int32_t x, int32_t y)
{
	if ((x < 0) || (y < 0) || (x >= tm->boundary.Width) || (y >= tm->boundary.Height))
	{
		return false;
	}
	return true;
}
