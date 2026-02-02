#pragma once

//
// Copyright (c) 2009-2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
#ifndef FNAFONTSTASH_H
#define FNAFONTSTASH_H

#include "assert.h"
#include "../../engine/render/Texture.h"
#include "../../engine/render/Renderer.h"

FONScontext* fnafonsCreate(int width, int height, int flags);
void fnafonsDelete(FONScontext* ctx);

unsigned int fnafonsRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

#endif

#ifdef FNAFONTSTASH_IMPLEMENTATION

struct FNAFONScontext {
	Texture* tex;
	int width, height;
};

typedef struct FNAFONScontext FNAFONScontext;

static int _mFontCounter = 0;

static int fnafons__renderCreate(void* userPtr, int width, int height)
{
	FNAFONScontext* fna = (FNAFONScontext*)userPtr;
	if (fna->tex != NULL)
	{
		assert(0);
	}

	MString* path = NULL;
	MString_AssignString(&path, "fontdata");
	MString_AddAssignInt(&path, _mFontCounter);
	fna->tex = Renderer_GetNewTextureData(MString_Text(path), width, height, true);
	MString_Dispose(&path);
	fna->width = width;
	fna->height = height;
	_mFontCounter += 1;
	/*
	GLFONScontext* gl = (GLFONScontext*)userPtr;
	// Create may be called multiple times, delete existing texture.
	if (gl->tex != 0) {
		glDeleteTextures(1, &gl->tex);
		gl->tex = 0;
	}
	glGenTextures(1, &gl->tex);
	if (!gl->tex) return 0;
	gl->width = width;
	gl->height = height;
	glBindTexture(GL_TEXTURE_2D, gl->tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, gl->width, gl->height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	return 1;
	*/
	return 1;
	//return FONS_INVALID;
}

static int fnafons__renderResize(void* userPtr, int width, int height)
{
	// Reuse create to resize too.
	return fnafons__renderCreate(userPtr, width, height);
}

static void fnafons__renderUpdate(void* userPtr, int* rect, const unsigned char* data)
{
	FNAFONScontext* gl = (FNAFONScontext*)userPtr;

	int texWidth = gl->width;

	int x = rect[0];
	int y = rect[1];
	int w = rect[2] - x;
	int h = rect[3] - y;

	int bytes = 4; //RGBA

	int dataLength = w * h * bytes;

	uint8_t* chunk = (uint8_t*)Utils_calloc(dataLength, sizeof(uint8_t));
	int counter = 0;
	for (int j = y; j < (y + h); j += 1)
	{
		for (int i = x; i < (x + w); i += 1)
		{
			int offset = i + (j * texWidth);
			uint8_t alpha = data[offset];
			uint8_t color;
			if (alpha == 0)
			{
				color = 0x00;
			}
			else
			{
				color = 0xFF;
			}
			chunk[counter] = color; //TODO FIX THIS
			counter += 1;
			chunk[counter] = color;
			counter += 1;
			chunk[counter] = color;
			counter += 1;
			chunk[counter] = alpha;
			counter += 1;
		}
	}
	//for (int i = (x * bytes)
	//	for (int i = 0; i < dataLength; i += 1)
	//	{
	//		chunk[i] = x + y * w;
	//	}
	Renderer_UpdateTextureData(gl->tex, x, y, w, h, 0, chunk, dataLength);
	Utils_free(chunk);
	/*
	if (gl->tex == 0) return;
	glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
	glBindTexture(GL_TEXTURE_2D, gl->tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, gl->width);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, rect[0]);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, rect[1]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, rect[0], rect[1], w, h, GL_ALPHA, GL_UNSIGNED_BYTE, data);
	glPopClientAttrib();
	*/
}

static void fnafons__renderDraw(void* userPtr, const float* verts, const float* tcoords, const unsigned int* colors, int nverts)
{
	FNAFONScontext* gl = (FNAFONScontext*)userPtr;

	Renderer_DrawTtText(gl->tex, verts, tcoords, colors, nverts);

	/*
	GLFONScontext* gl = (GLFONScontext*)userPtr;
	if (gl->tex == 0) return;
	glBindTexture(GL_TEXTURE_2D, gl->tex);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(float) * 2, verts);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 2, tcoords);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(unsigned int), colors);

	glDrawArrays(GL_TRIANGLES, 0, nverts);

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	*/
}

static void fnafons__renderDelete(void* userPtr)
{
	//int test = 0;
	/*
	FNAFONScontext* gl = (FNAFONScontext*)userPtr;
	if (gl->tex != 0)
		glDeleteTextures(1, &gl->tex);
	gl->tex = 0;
	free(gl);*/
}


FONScontext* fnafonsCreate(int width, int height, int flags)
{
	FONSparams params;
	FNAFONScontext* fna;

	fna = (FNAFONScontext*)malloc(sizeof(FNAFONScontext));
	if (fna == NULL) goto error;
	memset(fna, 0, sizeof(FNAFONScontext));

	memset(&params, 0, sizeof(params));
	params.width = width;
	params.height = height;
	params.flags = (unsigned char)flags;
	params.renderCreate = fnafons__renderCreate;
	params.renderResize = fnafons__renderResize;
	params.renderUpdate = fnafons__renderUpdate;
	params.renderDraw = fnafons__renderDraw;
	params.renderDelete = fnafons__renderDelete;
	params.userPtr = fna;

	return fonsCreateInternal(&params);

error:
	if (fna != NULL)
	{
		free(fna);
	}
	return NULL;
}

void fnafonsDelete(FONScontext* ctx)
{
	fonsDeleteInternal(ctx);
}

unsigned int fnafonsRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return (r) | (g << 8) | (b << 16) | (a << 24);
}

#endif