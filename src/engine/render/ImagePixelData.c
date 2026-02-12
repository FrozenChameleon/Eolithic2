#include "ImagePixelData.h"

#include "../utils/Utils.h"
#include "SDL3/SDL.h"
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "../../third_party/stb_image.h"
#include "../io/FixedByteBuffer.h"

static uint64_t _mRefs;

typedef struct ImagePixelData
{
	uint8_t* mData;
	Rectangle mBounds;
} ImagePixelData;

uint8_t* ImagePixelData_GetData(ImagePixelData* ipd)
{
	return ipd->mData;
}
Rectangle ImagePixelData_GetBounds(ImagePixelData* ipd)
{
	return ipd->mBounds;
}
int32_t ImagePixelData_ImageRead(void* context, char* data, int32_t size)
{
	SDL_IOStream* rwop = (SDL_IOStream*)(context);
	return (int32_t)SDL_ReadIO(rwop, data, size);
}
void ImagePixelData_ImageSkip(void* context, int32_t n)
{
	SDL_IOStream* rwop = (SDL_IOStream*)(context);
	SDL_SeekIO(rwop, n, SDL_IO_SEEK_CUR);
}
int32_t ImagePixelData_ImageEndOfFile(void* context)
{
	SDL_IOStream* rwop = (SDL_IOStream*)(context);
	int64_t size = SDL_GetIOSize(rwop);
	int64_t loc = SDL_TellIO(rwop);
	return size == loc ? 1 : 0;
}
ImagePixelData* ImagePixelData_Create(FixedByteBuffer* blob)
{
	_mRefs += 1;

	SDL_IOStream* rwops = SDL_IOFromMem(FixedByteBuffer_GetBuffer(blob), FixedByteBuffer_GetLength(blob));

	stbi_io_callbacks cb;
	cb.eof = ImagePixelData_ImageEndOfFile;
	cb.read = ImagePixelData_ImageRead;
	cb.skip = ImagePixelData_ImageSkip;

	int32_t imageWidth;
	int32_t imageHeight;
	int32_t imageFormat;
	uint8_t* imageData;

	imageData = stbi_load_from_callbacks(&cb, rwops, &imageWidth, &imageHeight, &imageFormat, STBI_rgb_alpha);
	SDL_CloseIO(rwops);

	ImagePixelData* pixelImageData = Utils_calloc(1, sizeof(ImagePixelData));
	pixelImageData->mData = imageData;
	pixelImageData->mBounds.Width = imageWidth;
	pixelImageData->mBounds.Height = imageHeight;
	return pixelImageData;
}
void ImagePixelData_Dispose(ImagePixelData* ipd)
{
	_mRefs -= 1;
	stbi_image_free(ipd->mData);
	Utils_free(ipd);
}
