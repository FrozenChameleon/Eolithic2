/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#define MOVIE_OPERATION_HEADER 	int32_t mType; bool mIsComplete;

typedef enum MovieOperationType
{
	MOVIE_OPERATION_TYPE_INVALID = 0,
	MOVIE_OPERATION_TYPE_WAIT = 1,
	MOVIE_OPERATION_TYPE_SHAKE = 2,
	MOVIE_OPERATION_TYPE_FADE_TEXT = 3,
	MOVIE_OPERATION_TYPE_TEXT = 4,
	MOVIE_OPERATION_TYPE_PAN = 5,
} MovieOperationType;

