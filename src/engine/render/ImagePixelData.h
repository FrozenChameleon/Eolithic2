#pragma once

#include "../utils/Macros.h"
#include "../math/Rectangle.h"

typedef struct ImagePixelData ImagePixelData;
typedef struct FixedByteBuffer FixedByteBuffer;

uint8_t* ImagePixelData_GetData(ImagePixelData* ipd);
Rectangle ImagePixelData_GetBounds(ImagePixelData* ipd);
int32_t ImagePixelData_ImageRead(void* context, char* data, int32_t size);
void ImagePixelData_ImageSkip(void* context, int32_t n);
int32_t ImagePixelData_ImageEndOfFile(void* context);
ImagePixelData* ImagePixelData_Create(FixedByteBuffer* blob);
void ImagePixelData_Dispose(ImagePixelData* ipd);