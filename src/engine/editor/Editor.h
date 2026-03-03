#pragma once

#include "../utils/Macros.h"

#define EDITOR_KEY_WINDOW_TOOL_MAN "Tool Manager"
#define EDITOR_KEY_WINDOW_SETTINGS "Settings"
#define EDITOR_KEY_WINDOW_TOGGLES "Toggles"
#define EDITOR_KEY_WINDOW_TOGGLES_MINI "Mini Toggles"

typedef struct SpriteBatch SpriteBatch;
typedef struct Camera Camera;

void Editor_Init(void);
void Editor_Update();
void Editor_Draw(SpriteBatch* spriteBatch);
void Editor_DrawHud(SpriteBatch* spriteBatch);
Camera* Editor_GetCamera(void);
void Editor_LoadPreviousLevel();
const char* Editor_GetEditorCameraData(int32_t index);
const char* Editor_GetEditorDataName(int32_t index);
const char* Editor_GetEditorCollisionName(int32_t index);