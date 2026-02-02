/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#define STB_DS_IMPLEMENTATION
#include "../third_party/stb_ds.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#include "core/GameRunner.h"

#include "io/DynamicByteBuffer.h"
#include "service/Service.h"

int main(int argc, char* args[])
{
	return GameRunner_Run(argc, args);
}