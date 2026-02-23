#pragma once

#include "../utils/Macros.h"
#include "../math/Point.h"
#include "../math/Rectangle.h"

typedef struct Texture Texture;
typedef struct SpriteBatch SpriteBatch;

bool WindowTilePicker_CreateWindow(Texture* texture);
bool WindowTilePicker_FindPoint(int width, int height, float scale);
Point WindowTilePicker_GetTexturePosition();
void WindowTilePicker_SetTexturePosition(Point point);
Rectangle WindowTilePicker_GetSelectedBoundaryInPixels();
Rectangle WindowTilePicker_GetSelectedBoundary();
void WindowTilePicker_DrawHud(SpriteBatch* spriteBatch);