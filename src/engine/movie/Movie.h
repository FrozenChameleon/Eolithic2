/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

typedef struct IStringArray IStringArray;
typedef struct BufferReader BufferReader;

typedef struct Movie
{
	IStringArray* mMovieData;
} Movie;

Movie* Movie_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void Movie_Dispose(Movie* movie);