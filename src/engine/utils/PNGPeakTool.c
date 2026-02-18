#include "PNGPeakTool.h"

#include "../io/BufferReader.h"
#include "../utils/Utils.h"
#include "../utils/Logger.h"

Rectangle PNGPeakTool_GetBounds(const char* path)
{
	Rectangle bounds = Rectangle_Empty;

	BufferReader* br = BufferReader_CreateFromPath(path);
	BufferReader_Seek(br, 8, BUFFER_READER_SEEK_FROM_CURRENT);
	BufferReader_ReadI32(br); //chunkSize

    char chunkType[5];
	chunkType[0] = BufferReader_ReadU8(br);
    chunkType[1] = BufferReader_ReadU8(br);
    chunkType[2] = BufferReader_ReadU8(br);
    chunkType[3] = BufferReader_ReadU8(br);
	chunkType[4] = '\0';

    if (!Utils_StringEqualTo(chunkType, "IHDR"))
    {
		Logger_LogWarning("Not valid PNG file.");
		BufferReader_Dispose(br);
		return bounds;
    }

    {
        uint8_t width[4];
        width[3] = BufferReader_ReadU8(br);
        width[2] = BufferReader_ReadU8(br);
        width[1] = BufferReader_ReadU8(br);
        width[0] = BufferReader_ReadU8(br);
        Utils_memcpy(&bounds.Width, &width, sizeof(int32_t));
    }

    {
        uint8_t height[4];
        height[3] = BufferReader_ReadU8(br);
        height[2] = BufferReader_ReadU8(br);
        height[1] = BufferReader_ReadU8(br);
        height[0] = BufferReader_ReadU8(br);
        Utils_memcpy(&bounds.Height, &height, sizeof(int32_t));
    }

    return bounds;
}
