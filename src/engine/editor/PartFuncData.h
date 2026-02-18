#pragma once

#include "../utils/Macros.h"

typedef struct SpriteBatch SpriteBatch;

typedef void (*EditorActionDeltaTimeFunc)(double deltaTime);
typedef void (*EditorActionFunc)();
typedef void (*EditorDrawFunc)(SpriteBatch* spriteBatch);

typedef struct PartFuncData
{
	char mDebugName[EE_FILENAME_MAX];
	EditorActionFunc CreateWindows;
	EditorActionDeltaTimeFunc Update;
	EditorDrawFunc Draw;
	EditorDrawFunc DrawHud;
	EditorActionFunc JustLoadedMap;
	EditorActionFunc JustChangedToThisPart;
	EditorActionFunc Save;
} PartFuncData;