#pragma once

#include "../utils/Macros.h"
#include "../math/Rectangle.h"
#include "../math/Point.h"
#include "../resources/ResourceList.h"

typedef struct SpriteBatch SpriteBatch;

bool WindowAnimTilePicker_CreateWindow();
int32_t WindowAnimTilePicker_GetRowAmount();
void WindowAnimTilePicker_DrawHud(SpriteBatch* spriteBatch);
void WindowAnimTilePicker_SetAnimTile(const char* animTile);
Resource* WindowAnimTilePicker_GetAnimTileResource();