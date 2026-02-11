#pragma once

#include "../utils/Macros.h"
#include "../math/Rectangle.h"

typedef struct ImagePixelData ImagePixelData;
typedef struct FixedByteBuffer FixedByteBuffer;

uint8_t* ImagePixelData_GetData(ImagePixelData* ipd);
Rectangle ImagePixelData_GetBounds(ImagePixelData* ipd);
ImagePixelData* ImagePixelData_Create(FixedByteBuffer* blob);
void ImagePixelData_Dispose(ImagePixelData* ipd);