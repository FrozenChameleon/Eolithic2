#pragma once

#include "../utils/Macros.h"

typedef struct SpriteBatch SpriteBatch;
typedef struct Camera Camera;

void Editor_Init(void);
void Editor_Update(double deltaTime);
void Editor_Draw(SpriteBatch* spriteBatch);
void Editor_DrawHud(SpriteBatch* spriteBatch);
Camera* Editor_GetCamera(void);
const char* Editor_GetEditorCollisionName(int32_t index);