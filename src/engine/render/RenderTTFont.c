/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "RenderTTFont.h"

#define FONTSTASH_IMPLEMENTATION
#include "../../third_party/fontstash/fontstash.h"

#define FNAFONTSTASH_IMPLEMENTATION
#include "../../third_party/fontstash/fnafontstash.h"

#include "../math/Math.h"
#include "../../third_party/stb_ds.h"

static struct { const char* key; FONScontext* value; }*_mContexts;
static FixedByteBuffer** _mPinnedPointers;

static bool _mHasInit;
static void Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	sh_new_arena(_mContexts);

	_mHasInit = true;
}

static void SetupFontState(FONScontext* fontContext, const FontMapData* replacement)
{
	fonsSetAlign(fontContext, FONS_ALIGN_TOP);
	fonsSetSize(fontContext, (float)(replacement->mFontSize * replacement->mScaleFactor));
	fonsSetSpacing(fontContext, (float)replacement->mCharacterSpacing);
}

static void DrawTheText(FONScontext* fontContext, const char* textToDraw, Vector2 renderPosition, Vector2 linePosition, Color color, float scaleFactor)
{
	TTFontState fontState;
	fontState.mPosition = Vector2_Add(renderPosition, linePosition);
	fontState.mColor = color;
	fontState.mScaleFactor = scaleFactor;
	Renderer_SetupTTFontState(&fontState);
	fonsDrawText(fontContext, 0, 0, textToDraw, NULL);
}

void RenderTTFont_Create(const char* name, FixedByteBuffer* data)
{
	Init();

	FixedByteBuffer* fbbClone = FixedByteBuffer_Clone(data);

	arrput(_mPinnedPointers, fbbClone);

	FONScontext* fontContext = NULL;

	int64_t index = shgeti(_mContexts, name);
	if (index >= 0)
	{
		fontContext = _mContexts[index].value;
	}
	else
	{
		fontContext = fnafonsCreate(1024, 1024, FONS_ZERO_TOPLEFT);
		shput(_mContexts, name, fontContext);
	}

	int32_t font = fonsAddFontMem(fontContext, name, FixedByteBuffer_GetBuffer(fbbClone), (int32_t)FixedByteBuffer_GetLength(fbbClone), 0);

	fonsSetFont(fontContext, font);
}
void RenderTTFont_Draw(const FontMapData* replacement, const char* text, Color color, Vector2 position)
{
	Init();

	int64_t index = shgeti(_mContexts, replacement->mFontName);
	if (index < 0)
	{
		return;
	}

	FONScontext* fontContext = _mContexts[index].value;
	SetupFontState(fontContext, replacement);

	Vector2 renderPosition = Vector2_Add(position, replacement->mOffset);
	Vector2 linePosition = Vector2_Zero;
	float scaleFactor = (float)replacement->mScaleFactor;
	int32_t lineSpacing = replacement->mOverrideBoundsHeight;

	MString* textToDraw = NULL;
	MString_AssignString(&textToDraw, "");
	size_t textSize = Utils_strlen(text);
	for (int32_t i = 0; i < textSize; i += 1)
	{
		if (text[i] == '\n')
		{
			DrawTheText(fontContext, MString_Text(textToDraw), renderPosition, linePosition, color, scaleFactor);
			linePosition.Y += lineSpacing;
			MString_AssignString(&textToDraw, "");
		}
		else
		{
			MString_AddAssignChar(&textToDraw, text[i]);
		}
	}

	if (MString_Length(textToDraw) > 0)
	{
		DrawTheText(fontContext, MString_Text(textToDraw), renderPosition, linePosition, color, scaleFactor);
	}

	MString_Dispose(&textToDraw);
}
Rectangle RenderTTFont_GetBounds(const FontMapData* replacement, const char* text)
{
	Init();

	int64_t index = shgeti(_mContexts, replacement->mFontName);
	if (index < 0)
	{
		return Rectangle_Empty;
	}

	FONScontext* fontContext = _mContexts[index].value;
	SetupFontState(fontContext, replacement);

	float rects[4];
	fonsTextBounds(fontContext, 0, 0, text, NULL, rects);

	float width = rects[2] - rects[0];
	float height = rects[3] - rects[1];

	float correctedWidth = width / replacement->mScaleFactor;
	float correctedHeight = height / replacement->mScaleFactor;

	Rectangle returnBounds;
	returnBounds.X = 0;
	returnBounds.Y = 0;
	returnBounds.Width = (int32_t)correctedWidth;
	returnBounds.Height = (int32_t)correctedHeight;

	if (replacement->mOverrideBoundsHeight > 0)
	{
		returnBounds.Height = replacement->mOverrideBoundsHeight;
	}

	return returnBounds;
}
