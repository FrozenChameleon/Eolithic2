#pragma once

typedef struct SpriteBatch SpriteBatch;
typedef struct Camera Camera;

void Editor_Init(void);
void Editor_Update(void);
void Editor_Draw(SpriteBatch* spriteBatch);
void Editor_DrawHud(SpriteBatch* spriteBatch);
Camera* Editor_GetCamera(void);