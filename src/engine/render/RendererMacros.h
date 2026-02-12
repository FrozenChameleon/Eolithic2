/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#ifdef RENDER_SDLGPU
#define MAX_VERTICES 32768
#define MAX_INDICES 49152
#else
#define MAX_VERTICES 8192
#define MAX_INDICES 12288
#endif
#define VERTICES_IN_SPRITE 4