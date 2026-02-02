/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

 /* Derived from code by Ethan Lee (Copyright 2009-2024).
  * Released under the Microsoft Public License.
  * See fna.LICENSE for details.

  * Derived from code by the Mono.Xna Team (Copyright 2006).
  * Released under the MIT License.
  * See monoxna.LICENSE for details.
  */

#pragma once

#include "../utils/Macros.h"
#include "../io/FixedByteBuffer.h"
#include "../io/BufferReader.h"

#ifdef AUDIO_DUMMY
typedef struct WaveFileData WaveFileData;
#else
typedef struct WaveFileData
{
	// Sample data
	FixedByteBuffer* mWaveData;
	int32_t mWaveDataLength;

	// WaveFormatEx data
	uint16_t wFormatTag;
	uint16_t nChannels;
	uint32_t nSamplesPerSec;
	uint32_t nAvgBytesPerSec;
	uint16_t nBlockAlign;
	uint16_t wBitsPerSample;
	// ushort cbSize;

	int32_t samplerLoopStart;
	int32_t samplerLoopEnd;
	//
} WaveFileData;
#endif

void ReadFourChar(BufferReader* br, char* sig);
WaveFileData* WaveFileData_FromStream(BufferReader* br);
void WaveFileData_Dispose(WaveFileData* waveFileData);