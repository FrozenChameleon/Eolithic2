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

#ifndef AUDIO_DUMMY

#include "WaveFileData.h"

#include "../utils/Utils.h"
#include "../utils/Exception.h"

#define LENGTH_OF_SIG 5

void ReadFourChar(BufferReader* br, char* sig)
{
	sig[0] = 'X';
	sig[1] = 'X';
	sig[2] = 'X';
	sig[3] = 'X';
	sig[4] = '\0';

	for (int32_t i = 0; i < 4; i += 1)
	{
		sig[i] = BufferReader_ReadU8(br);
	}
}

WaveFileData* WaveFileData_FromStream(BufferReader* br)
{
	char signature[LENGTH_OF_SIG];
	ReadFourChar(br, signature);
	if (!Utils_StringEqualTo(signature, "RIFF"))
	{
		Exception_Run("Specified stream is not a wave file.", false);
		return NULL;
	}
	
	BufferReader_ReadU32(br); // Riff Chunk Size

	char wformat[LENGTH_OF_SIG];
	ReadFourChar(br, wformat);
	if (!Utils_StringEqualTo(wformat, "WAVE"))
	{
		Exception_Run("Specified stream is not a wave file.", false);
		return NULL;
	}

	// WAVE Header
	char format_signature[LENGTH_OF_SIG];
	ReadFourChar(br, format_signature);
	while (!Utils_StringEqualTo(format_signature, "fmt "))
	{
		int32_t bytesToRead = BufferReader_ReadI32(br);
		BufferReader_Seek(br, bytesToRead, BUFFER_READER_SEEK_FROM_CURRENT);
		ReadFourChar(br, format_signature);
	}

	int32_t format_chunk_size = BufferReader_ReadI32(br);

	WaveFileData* waveData = (WaveFileData*)Utils_calloc(1, sizeof(WaveFileData));
	waveData->wFormatTag = BufferReader_ReadU16(br);
	waveData->nChannels = BufferReader_ReadU16(br);
	waveData->nSamplesPerSec = BufferReader_ReadU32(br);
	waveData->nAvgBytesPerSec = BufferReader_ReadU32(br);
	waveData->nBlockAlign = BufferReader_ReadU16(br);
	waveData->wBitsPerSample = BufferReader_ReadU16(br);

	// Reads residual bytes
	if (format_chunk_size > 16)
	{
		BufferReader_Seek(br, format_chunk_size - 16, BUFFER_READER_SEEK_FROM_CURRENT);
	}

	// data Signature
	char data_signature[LENGTH_OF_SIG];
	ReadFourChar(br, data_signature);
	while (!Utils_StringEqualTo(data_signature, "data")) //ToLowerInvariant... normally
	{
		int32_t bytesToRead = BufferReader_ReadI32(br);
		BufferReader_Seek(br, bytesToRead, BUFFER_READER_SEEK_FROM_CURRENT);
		ReadFourChar(br, data_signature);
	}
	if (!Utils_StringEqualTo(data_signature, "data"))
	{
		Exception_Run("Specified wave file is not supported.", false);
		return NULL;
	}

	waveData->mWaveDataLength = BufferReader_ReadI32(br);
	waveData->mWaveData = BufferReader_ReadBytes(br, waveData->mWaveDataLength);

	// Scan for other chunks
	while (BufferReader_HasNext(br))
	{
		//char chunkIDChars[4];
		char chunk_signature[LENGTH_OF_SIG];
		ReadFourChar(br, chunk_signature);
		int32_t chunkDataSize = BufferReader_ReadI32(br);
		if (Utils_StringEqualTo(chunk_signature, "smpl")) // "smpl", Sampler Chunk Found
		{
			BufferReader_ReadU32(br); // Manufacturer
			BufferReader_ReadU32(br); // Product
			BufferReader_ReadU32(br); // Sample Period
			BufferReader_ReadU32(br); // MIDI Unity Note
			BufferReader_ReadU32(br); // MIDI Pitch Fraction
			BufferReader_ReadU32(br); // SMPTE Format
			BufferReader_ReadU32(br); // SMPTE Offset
			uint32_t numSampleLoops = BufferReader_ReadU32(br);
			int32_t samplerData = BufferReader_ReadI32(br);

			for (int32_t i = 0; i < ((int32_t)numSampleLoops); i += 1)
			{
				BufferReader_ReadU32(br); // Cue Point ID
				BufferReader_ReadU32(br); // Type
				int32_t start = BufferReader_ReadI32(br);
				int32_t end = BufferReader_ReadI32(br);
				BufferReader_ReadU32(br); // Fraction
				BufferReader_ReadU32(br); // Play Count

				if (i == 0) // Grab loopStart and loopEnd from first sample loop
				{
					waveData->samplerLoopStart = start;
					waveData->samplerLoopEnd = end;
				}
			}

			if (samplerData != 0) // Read Sampler Data if it exists
			{
				BufferReader_Seek(br, samplerData, BUFFER_READER_SEEK_FROM_CURRENT);
			}
		}
		else // Read unwanted chunk data and try again
		{
			BufferReader_Seek(br, chunkDataSize, BUFFER_READER_SEEK_FROM_CURRENT);
		}
	}
	// End scan

	return waveData;
}
void WaveFileData_Dispose(WaveFileData* waveFileData)
{
	if (waveFileData->mWaveData != NULL)
	{
		FixedByteBuffer_Dispose(waveFileData->mWaveData);
		waveFileData->mWaveData = NULL;
	}

	Utils_free(waveFileData);
}

#endif

typedef int compiler_warning_compliance;