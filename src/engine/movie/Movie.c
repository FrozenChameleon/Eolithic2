/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Movie.h"

#include "../io/File.h"
#include "../utils/Utils.h"

Movie* Movie_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	Movie* movie = (Movie*)Utils_calloc(1, sizeof(Movie));
	movie->mMovieData = File_ReadAllToStrings(br);
	return movie;
}
void Movie_Dispose(Movie* movie)
{
	Utils_free(movie);
}
