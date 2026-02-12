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

//SDL_gpu_examples and FNA3D's SDL_GPU driver were the main references while writing this...
 
//Still needs to not fail spectacularly if vertice count goes beyond MAX_VERTICES, as unlikely as that is...

#ifdef RENDER_SDLGPU

#include "Renderer.h"

#include "../utils/Macros.h"
#include "RendererMacros.h"
#include "../service/Service.h"
#include "../core/Game.h"
#include "../utils/Cvars.h"
#include "../render/Texture.h"
#include "../utils/Utils.h"
#include "../leveldata/DrawTile.h"
#include "../leveldata/Tile.h"
#include "../leveldata/AnimTile.h"
#include "../utils/Logger.h"
#include "../render/Sheet.h"
#include "../globals/Align.h"
#include "../math/Math.h"
#include "../math/Rectangle.h"
#include "../math/Matrix.h"
#include "../math/MathHelper.h"
#include "../io/File.h"
#include "RenderTTFont.h"
#include "SDL3/SDL.h"
#include "ImagePixelData.h"

#define MULTI_COLOR_REPLACE_LEN SHADER_PROGRAM_MAX_REPLACE_LENGTH

#define TRANSFER_BUFFER_FOR_TEXTURE_LEN 16777216 /* 16 MiB */

static const char* SHADER_PARAMETER_IS_TOTAL_WHITE_HIT_FLASH = "IsTotalWhiteHitFlash";
static const char* SHADER_PARAMETER_PERFORM_ALPHA_TEST = "PerformAlphaTest";
static const char* SHADER_PARAMETER_MATRIX_TRANSFORM = "MatrixTransform";
static const char* SHADER_PARAMETER_SHADER_TYPE = "ShaderType";
static const char* SHADER_PARAMETER_COLOR_LENGTH = "ColorLength";
static const char* SHADER_PARAMETER_COLOR_TARGET_0 = "ColorTarget0";
static const char* SHADER_PARAMETER_COLOR_REPLACE_0 = "ColorReplace0";
static const char* SHADER_PARAMETER_COLOR_TARGET_1 = "ColorTarget1";
static const char* SHADER_PARAMETER_COLOR_REPLACE_1 = "ColorReplace1";
static const char* SHADER_PARAMETER_COLOR_TARGET_2 = "ColorTarget2";
static const char* SHADER_PARAMETER_COLOR_REPLACE_2 = "ColorReplace2";
static const char* SHADER_PARAMETER_COLOR_TARGET_3 = "ColorTarget3";
static const char* SHADER_PARAMETER_COLOR_REPLACE_3 = "ColorReplace3";
static const char* SHADER_PARAMETER_COLOR_REPLACE_ALPHA = "ColorReplaceAlpha";

static float _mInternalWidth;
static float _mInternalHeight;
static Vector3 _mInternalWorldTranslation;
static float _mInternalWidthRender;
static float _mInternalHeightRender;
static Matrix _mMatrix;
static SDL_Window* _mDeviceWindowHandle;
static SDL_GPUDevice* _mDeviceGPU;
static int32_t _mBatchNumber;
//Setup Render State Stuff
static float _mMultiColorReplaceData[MULTI_COLOR_REPLACE_LEN];
static BlendState _mLastUsedBlendState = BLENDSTATE_INVALID;
//
static Texture _mOffscreenTarget;
static bool _mIsDrawingFromOffscreenTarget;
static bool _mIsDrawingToOffscreenTarget;
static Rectangle _mVirtualBufferBounds;
static Rectangle _mActualBufferBounds;

static SDL_GPUTransferBuffer* _mTransferBufferForTexture;
static uint32_t _mTransferBufferForTextureOffset;

// Vertex Formats
typedef struct PositionTextureVertex
{
	float x, y, z;
	float u, v;
} PositionTextureVertex;

static const char* SamplerNames[] =
{
	"PointClamp",
	"PointWrap",
	"LinearClamp",
	"LinearWrap",
	"AnisotropicClamp",
	"AnisotropicWrap",
};

static SDL_GPUGraphicsPipeline* Pipeline;
static SDL_GPUBuffer* VertexBuffer;
static SDL_GPUBuffer* IndexBuffer;
static SDL_GPUSampler* Samplers[SDL_arraysize(SamplerNames)];
static Texture* _mLastTextureUsed;

static Rectangle GetCurrentBufferBounds(void)
{
	if (_mIsDrawingToOffscreenTarget)
	{
		return _mVirtualBufferBounds;
	}
	else
	{
		return _mActualBufferBounds;
	}
}
static bool IsDepthBufferDisabled(void)
{
	return Service_PlatformDisablesDepthBufferForRender();
}
static bool IsOffscreenTargetNeeded(void)
{
	return Service_PlatformRequiresOffscreenTargetForRender();
}

static SDL_GPUShader* LoadShader(
	SDL_GPUDevice* device,
	const char* shaderFilename,
	Uint32 samplerCount,
	Uint32 uniformBufferCount,
	Uint32 storageBufferCount,
	Uint32 storageTextureCount
) {
	// Auto-detect the shader stage from the file name for convenience
	SDL_GPUShaderStage stage;
	if (SDL_strstr(shaderFilename, ".vert"))
	{
		stage = SDL_GPU_SHADERSTAGE_VERTEX;
	}
	else if (SDL_strstr(shaderFilename, ".frag"))
	{
		stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
	}
	else
	{
		SDL_Log("Invalid shader stage!");
		return NULL;
	}

	char fullPath[256];
	SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
	SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
	const char* entrypoint;

	if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Shaders/Compiled/SPIRV/%s.spv", File_GetBasePath(), shaderFilename);
		format = SDL_GPU_SHADERFORMAT_SPIRV;
		entrypoint = "main";
	}
	else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Shaders/Compiled/MSL/%s.msl", File_GetBasePath(), shaderFilename);
		format = SDL_GPU_SHADERFORMAT_MSL;
		entrypoint = "main0";
	}
	else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Shaders/Compiled/DXIL/%s.dxil", File_GetBasePath(), shaderFilename);
		format = SDL_GPU_SHADERFORMAT_DXIL;
		entrypoint = "main";
	}
	else {
		SDL_Log("%s", "Unrecognized backend shader format!");
		return NULL;
	}

	size_t codeSize;
	void* code = SDL_LoadFile(fullPath, &codeSize);
	if (code == NULL)
	{
		SDL_Log("Failed to load shader from disk! %s", fullPath);
		return NULL;
	}

	SDL_GPUShaderCreateInfo shaderInfo = {
		.code = code,
		.code_size = codeSize,
		.entrypoint = entrypoint,
		.format = format,
		.stage = stage,
		.num_samplers = samplerCount,
		.num_uniform_buffers = uniformBufferCount,
		.num_storage_buffers = storageBufferCount,
		.num_storage_textures = storageTextureCount
	};
	SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);
	if (shader == NULL)
	{
		SDL_Log("Failed to create shader!");
		SDL_free(code);
		return NULL;
	}

	SDL_free(code);
	return shader;
}

typedef struct FragMultiplyUniform
{
	float r, g, b, a;
} FragMultiplyUniform;

static uint16_t* _mIndexBufferData;

static SDL_GPUTransferBuffer* _mTransferBufferForVertexBuffer;
static SDL_GPUCommandBuffer* _mRenderCommandBuffer;
static SDL_GPUCommandBuffer* _mUploadCommandBuffer;
static SDL_GPURenderPass* _mRenderPass;
static SDL_GPUTexture* _mSwapchainTexture;
static SDL_GPUCopyPass* _mCopyPass;

static PositionTextureVertex _mBatchVertexBuffer[MAX_VERTICES];
static uint32_t _mCurrentBatchVerticesOffset;
static uint32_t _mLastRenderVerticesOffset;

// Matrix Math
typedef struct Matrix4x4
{
	float m11, m12, m13, m14;
	float m21, m22, m23, m24;
	float m31, m32, m33, m34;
	float m41, m42, m43, m44;
} Matrix4x4;

static Vector3 Vector3_Normalize(Vector3 vec)
{
	float magnitude = SDL_sqrtf((vec.X * vec.X) + (vec.Y * vec.Y) + (vec.Z * vec.Z));
	return (Vector3) {
		vec.X / magnitude,
			vec.Y / magnitude,
			vec.Z / magnitude
	};
}

static float Vector3_Dot(Vector3 vecA, Vector3 vecB)
{
	return (vecA.X * vecB.X) + (vecA.Y * vecB.Y) + (vecA.Z * vecB.Z);
}

static Vector3 Vector3_Cross(Vector3 vecA, Vector3 vecB)
{
	return (Vector3) {
		vecA.Y* vecB.Z - vecB.Y * vecA.Z,
			-(vecA.X * vecB.Z - vecB.X * vecA.Z),
			vecA.X* vecB.Y - vecB.X * vecA.Y
	};
}
static Matrix4x4 Matrix4x4_Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2)
{
	Matrix4x4 result;

	result.m11 = (
		(matrix1.m11 * matrix2.m11) +
		(matrix1.m12 * matrix2.m21) +
		(matrix1.m13 * matrix2.m31) +
		(matrix1.m14 * matrix2.m41)
		);
	result.m12 = (
		(matrix1.m11 * matrix2.m12) +
		(matrix1.m12 * matrix2.m22) +
		(matrix1.m13 * matrix2.m32) +
		(matrix1.m14 * matrix2.m42)
		);
	result.m13 = (
		(matrix1.m11 * matrix2.m13) +
		(matrix1.m12 * matrix2.m23) +
		(matrix1.m13 * matrix2.m33) +
		(matrix1.m14 * matrix2.m43)
		);
	result.m14 = (
		(matrix1.m11 * matrix2.m14) +
		(matrix1.m12 * matrix2.m24) +
		(matrix1.m13 * matrix2.m34) +
		(matrix1.m14 * matrix2.m44)
		);
	result.m21 = (
		(matrix1.m21 * matrix2.m11) +
		(matrix1.m22 * matrix2.m21) +
		(matrix1.m23 * matrix2.m31) +
		(matrix1.m24 * matrix2.m41)
		);
	result.m22 = (
		(matrix1.m21 * matrix2.m12) +
		(matrix1.m22 * matrix2.m22) +
		(matrix1.m23 * matrix2.m32) +
		(matrix1.m24 * matrix2.m42)
		);
	result.m23 = (
		(matrix1.m21 * matrix2.m13) +
		(matrix1.m22 * matrix2.m23) +
		(matrix1.m23 * matrix2.m33) +
		(matrix1.m24 * matrix2.m43)
		);
	result.m24 = (
		(matrix1.m21 * matrix2.m14) +
		(matrix1.m22 * matrix2.m24) +
		(matrix1.m23 * matrix2.m34) +
		(matrix1.m24 * matrix2.m44)
		);
	result.m31 = (
		(matrix1.m31 * matrix2.m11) +
		(matrix1.m32 * matrix2.m21) +
		(matrix1.m33 * matrix2.m31) +
		(matrix1.m34 * matrix2.m41)
		);
	result.m32 = (
		(matrix1.m31 * matrix2.m12) +
		(matrix1.m32 * matrix2.m22) +
		(matrix1.m33 * matrix2.m32) +
		(matrix1.m34 * matrix2.m42)
		);
	result.m33 = (
		(matrix1.m31 * matrix2.m13) +
		(matrix1.m32 * matrix2.m23) +
		(matrix1.m33 * matrix2.m33) +
		(matrix1.m34 * matrix2.m43)
		);
	result.m34 = (
		(matrix1.m31 * matrix2.m14) +
		(matrix1.m32 * matrix2.m24) +
		(matrix1.m33 * matrix2.m34) +
		(matrix1.m34 * matrix2.m44)
		);
	result.m41 = (
		(matrix1.m41 * matrix2.m11) +
		(matrix1.m42 * matrix2.m21) +
		(matrix1.m43 * matrix2.m31) +
		(matrix1.m44 * matrix2.m41)
		);
	result.m42 = (
		(matrix1.m41 * matrix2.m12) +
		(matrix1.m42 * matrix2.m22) +
		(matrix1.m43 * matrix2.m32) +
		(matrix1.m44 * matrix2.m42)
		);
	result.m43 = (
		(matrix1.m41 * matrix2.m13) +
		(matrix1.m42 * matrix2.m23) +
		(matrix1.m43 * matrix2.m33) +
		(matrix1.m44 * matrix2.m43)
		);
	result.m44 = (
		(matrix1.m41 * matrix2.m14) +
		(matrix1.m42 * matrix2.m24) +
		(matrix1.m43 * matrix2.m34) +
		(matrix1.m44 * matrix2.m44)
		);

	return result;
}

static Matrix4x4 Matrix4x4_CreateRotationZ(float radians)
{
	return (Matrix4x4) {
		SDL_cosf(radians), SDL_sinf(radians), 0, 0,
			-SDL_sinf(radians), SDL_cosf(radians), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	};
}

static Matrix4x4 Matrix4x4_CreateTranslation(float x, float y, float z)
{
	return (Matrix4x4) {
		1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			x, y, z, 1
	};
}

static Matrix4x4 Matrix4x4_CreateOrthographicOffCenter(
	float left,
	float right,
	float bottom,
	float top,
	float zNearPlane,
	float zFarPlane
) {
	return (Matrix4x4) {
		2.0f / (right - left), 0, 0, 0,
			0, 2.0f / (top - bottom), 0, 0,
			0, 0, 1.0f / (zNearPlane - zFarPlane), 0,
			(left + right) / (left - right), (top + bottom) / (bottom - top), zNearPlane / (zNearPlane - zFarPlane), 1
	};
}

static Matrix4x4 Matrix4x4_CreatePerspectiveFieldOfView(
	float fieldOfView,
	float aspectRatio,
	float nearPlaneDistance,
	float farPlaneDistance
) {
	float num = 1.0f / ((float)SDL_tanf(fieldOfView * 0.5f));
	return (Matrix4x4) {
		num / aspectRatio, 0, 0, 0,
			0, num, 0, 0,
			0, 0, farPlaneDistance / (nearPlaneDistance - farPlaneDistance), -1,
			0, 0, (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance), 0
	};
}

static Matrix4x4 Matrix4x4_CreateLookAt(
	Vector3 cameraPosition,
	Vector3 cameraTarget,
	Vector3 cameraUpVector
) {
	Vector3 targetToPosition = {
		cameraPosition.X - cameraTarget.X,
		cameraPosition.Y - cameraTarget.Y,
		cameraPosition.Z - cameraTarget.Z
	};
	Vector3 vectorA = Vector3_Normalize(targetToPosition);
	Vector3 vectorB = Vector3_Normalize(Vector3_Cross(cameraUpVector, vectorA));
	Vector3 vectorC = Vector3_Cross(vectorA, vectorB);

	return (Matrix4x4) {
		vectorB.X, vectorC.X, vectorA.X, 0,
			vectorB.Y, vectorC.Y, vectorA.Y, 0,
			vectorB.Z, vectorC.Z, vectorA.Z, 0,
			-Vector3_Dot(vectorB, cameraPosition), -Vector3_Dot(vectorC, cameraPosition), -Vector3_Dot(vectorA, cameraPosition), 1
	};
}

static void SubmitCommandBufferForCopy(void)
{
	if (_mCopyPass != NULL)
	{
		SDL_EndGPUCopyPass(_mCopyPass);
		_mCopyPass = NULL;
	}

	if (_mUploadCommandBuffer != NULL)
	{
		SDL_SubmitGPUCommandBuffer(_mUploadCommandBuffer);
		_mUploadCommandBuffer = NULL;
	}

	_mUploadCommandBuffer = SDL_AcquireGPUCommandBuffer(_mDeviceGPU);
	if (_mUploadCommandBuffer == NULL)
	{
		SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
		return;
	}

	_mCopyPass = SDL_BeginGPUCopyPass(_mUploadCommandBuffer);
	if (_mCopyPass == NULL)
	{
		SDL_Log("SDL_BeginGPUCopyPass failed: %s", SDL_GetError());
		return;
	}
}

void Renderer_DrawTtText(Texture* texture, const float* verts, const float* tcoords, const unsigned int* colors, int32_t nverts)
{
	/*NextBatch(false);

	const TTFontState* fontState = Renderer_GetTTFontState();

	float scaleFactor = fontState->mScaleFactor;

	Vector2 scaler = { 1.0f / scaleFactor, 1.0f / scaleFactor };
	Vector2 currentCameraPosition = Renderer_INTERNAL_GetCurrentCameraPosition();
	Vector2 offsetPosition = Vector2_Sub(currentCameraPosition, fontState->mPosition);
	Vector2 cameraPos = Vector2_MulFloat(offsetPosition, scaleFactor);
	SetupRenderState(cameraPos, scaler, BLENDSTATE_NONPREMULTIPLIED, (FNA3D_Texture*)(texture->mTextureData), Renderer_INTERNAL_GetCurrentShaderProgram());

	Renderer_SetupVerticesForTTFont(_mVertexBuffer.mVertices, fontState->mColor, 0, verts, tcoords, colors, nverts);

	VertexBufferInstance_SetData(&_mVertexBuffer, 0, nverts);

	FNA3D_ApplyVertexBufferBindings(_mDeviceContext, &_mVertexBuffer.mVertexBufferBinding, 1, 1, 0);

	FNA3D_DrawPrimitives(_mDeviceContext, FNA3D_PRIMITIVETYPE_TRIANGLELIST, 0, nverts / 3);*/
}
static inline uint32_t SDLGPU_INTERNAL_RoundToAlignment(
	uint32_t value,
	uint32_t alignment
) {
	return alignment * ((value + alignment - 1) / alignment);
}
Texture* Renderer_GetTextureData(const char* path, FixedByteBuffer* blob)
{
	if(_mTransferBufferForTexture == NULL)
	{
		_mTransferBufferForTexture = SDL_CreateGPUTransferBuffer(
			_mDeviceGPU,
			&(SDL_GPUTransferBufferCreateInfo) {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
				.size = TRANSFER_BUFFER_FOR_TEXTURE_LEN
		}
		);

		SubmitCommandBufferForCopy();
	}

	ImagePixelData* ipd = ImagePixelData_Create(blob);

	uint8_t* textureData = ImagePixelData_GetData(ipd);
	Rectangle textureBounds = ImagePixelData_GetBounds(ipd);

	SDL_GPUTexture* gpuTex = SDL_CreateGPUTexture(_mDeviceGPU, &(SDL_GPUTextureCreateInfo){
		.type = SDL_GPU_TEXTURETYPE_2D,
			.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
			.width = textureBounds.Width,
			.height = textureBounds.Height,
			.layer_count_or_depth = 1,
			.num_levels = 1,
			.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER
	});

	uint32_t textureDataLen = (textureBounds.Width * textureBounds.Height * 4);

	bool isUsingTemporaryTransferBuffer = false;
	SDL_GPUTransferBuffer* transferBufferToUse;
	uint32_t transferBufferOffsetToUse;
	bool forceNewTextures = false;
	if (forceNewTextures || (textureDataLen >= TRANSFER_BUFFER_FOR_TEXTURE_LEN))
	{
		isUsingTemporaryTransferBuffer = true;
		transferBufferToUse = SDL_CreateGPUTransferBuffer(
			_mDeviceGPU,
			&(SDL_GPUTransferBufferCreateInfo) {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
				.size = textureDataLen
		}
		);
		transferBufferOffsetToUse = 0;
	}
	else
	{
		if ((_mTransferBufferForTextureOffset + textureDataLen) >= TRANSFER_BUFFER_FOR_TEXTURE_LEN)
		{
			SubmitCommandBufferForCopy();
			_mTransferBufferForTextureOffset = 0;
		}
		transferBufferToUse = _mTransferBufferForTexture;
		transferBufferOffsetToUse = _mTransferBufferForTextureOffset;
	}

	Uint8* textureTransferPtr = SDL_MapGPUTransferBuffer(
		_mDeviceGPU,
		transferBufferToUse,
		false
	);
	SDL_memcpy(textureTransferPtr + transferBufferOffsetToUse, textureData, textureDataLen);
	SDL_UnmapGPUTransferBuffer(_mDeviceGPU, _mTransferBufferForTexture);

	// Upload the transfer data to the GPU resources
	SDL_UploadToGPUTexture(
		_mCopyPass,
		&(SDL_GPUTextureTransferInfo) {
		.transfer_buffer = transferBufferToUse,
			.offset = transferBufferOffsetToUse,
	},
		& (SDL_GPUTextureRegion) {
		.texture = gpuTex,
			.w = textureBounds.Width,
			.h = textureBounds.Height,
			.d = 1
	},
		false
	);

	if (isUsingTemporaryTransferBuffer)
	{
		SDL_ReleaseGPUTransferBuffer(_mDeviceGPU, transferBufferToUse);
	}
	else
	{
		_mTransferBufferForTextureOffset += textureDataLen;
		_mTransferBufferForTextureOffset = SDLGPU_INTERNAL_RoundToAlignment(
			_mTransferBufferForTextureOffset,
			SDL_GPUTextureFormatTexelBlockSize(SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM));
	}
	
	Texture* tex = Renderer_GetNewTextureData(path, textureBounds.Width, textureBounds.Height, false);
	tex->mTextureData = gpuTex;
	return tex;
}
Texture* Renderer_GetNewTextureData(const char* path, int32_t width, int32_t height, bool clearTexture)
{
	Texture* tex = (Texture*)Utils_calloc(1, sizeof(Texture));
	MString_AssignString(&tex->mPath, path);
	tex->mBounds.Width = width;
	tex->mBounds.Height = height;
	return tex;
	/*InvalidateNextSetupRenderState();

	FNA3D_Texture* texture = FNA3D_CreateTexture2D(_mDeviceContext, FNA3D_SURFACEFORMAT_COLOR, width, height, 1, 0);

	if (clearTexture)
	{
		int32_t dataLength = width * height * 4;
		uint8_t* clearData = (uint8_t*)Utils_calloc(dataLength, sizeof(uint8_t));
		FNA3D_SetTextureData2D(_mDeviceContext, texture, 0, 0, width, height, 0, clearData, dataLength);
		Utils_free(clearData);
	}

	Texture* tex = (Texture*)Utils_calloc(1, sizeof(Texture));
	MString_AssignString(&tex->mPath, path);
	tex->mBounds.Width = width;
	tex->mBounds.Height = height;
	tex->mTextureData = texture;

	return tex;*/
}
void Renderer_UpdateTextureData(Texture* texture, int32_t x, int32_t y, int32_t w, int32_t h, int32_t level, void* data, int32_t dataLength)
{
	/*InvalidateNextSetupRenderState();

	FNA3D_SetTextureData2D(_mDeviceContext, (FNA3D_Texture*)(texture->mTextureData), x, y, w, h, level, data, dataLength);*/
}
int32_t Renderer_Init(void* deviceWindowHandle)
{
	_mIndexBufferData = Renderer_GetDefaultIndexBufferData();

	_mInternalWidth = (float)Utils_GetInternalWidth();
	_mInternalHeight = (float)Utils_GetInternalHeight();
	_mInternalWidthRender = (float)Utils_GetInternalRenderWidth();
	_mInternalHeightRender = (float)Utils_GetInternalRenderHeight();
	_mInternalWorldTranslation.X = (_mInternalWidth / 2.0f);
	_mInternalWorldTranslation.Y = (_mInternalHeight / 2.0f);
	_mInternalWorldTranslation.Z = 0;

	_mDeviceWindowHandle = (SDL_Window*)deviceWindowHandle;

	_mDeviceGPU = SDL_CreateGPUDevice(
		SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
		true,
		NULL);

	if (_mDeviceGPU == NULL)
	{
		SDL_Log("GPUCreateDevice failed");
		return -1;
	}

	if (!SDL_ClaimWindowForGPUDevice(_mDeviceGPU, _mDeviceWindowHandle))
	{
		SDL_Log("GPUClaimWindow failed");
		return -1;
	}

	SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;
	if (SDL_WindowSupportsGPUPresentMode(
		_mDeviceGPU,
		_mDeviceWindowHandle,
		SDL_GPU_PRESENTMODE_IMMEDIATE
	)) {
		presentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;
	}
	else if (SDL_WindowSupportsGPUPresentMode(
		_mDeviceGPU,
		_mDeviceWindowHandle,
		SDL_GPU_PRESENTMODE_MAILBOX
	)) {
		presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
	}

	SDL_SetGPUSwapchainParameters(
		_mDeviceGPU,
		_mDeviceWindowHandle,
		SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
		presentMode
	);

	// Create the shaders
	SDL_GPUShader* vertexShader = LoadShader(_mDeviceGPU, "TexturedQuadWithMatrix.vert", 0, 1, 0, 0);
	if (vertexShader == NULL)
	{
		SDL_Log("Failed to create vertex shader!");
		return -1;
	}

	SDL_GPUShader* fragmentShader = LoadShader(_mDeviceGPU, "TexturedQuadWithMultiplyColor.frag", 1, 1, 0, 0);
	if (fragmentShader == NULL)
	{
		SDL_Log("Failed to create fragment shader!");
		return -1;
	}

	// Create the pipeline
	SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
		.target_info = {
			.num_color_targets = 1,
			.color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
				.format = SDL_GetGPUSwapchainTextureFormat(_mDeviceGPU, _mDeviceWindowHandle),
				.blend_state = {
						.enable_blend = true,
						.color_blend_op = SDL_GPU_BLENDOP_ADD,
						.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
						.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
						.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
						.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
						.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
					}
			}},
		},
		.vertex_input_state = (SDL_GPUVertexInputState){
			.num_vertex_buffers = 1,
			.vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{
				.slot = 0,
				.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
				.instance_step_rate = 0,
				.pitch = sizeof(PositionTextureVertex)
			}},
			.num_vertex_attributes = 2,
			.vertex_attributes = (SDL_GPUVertexAttribute[]){{
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
				.location = 0,
				.offset = 0
			}, {
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
				.location = 1,
				.offset = sizeof(float) * 3
			}}
		},
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.vertex_shader = vertexShader,
		.fragment_shader = fragmentShader
	};

	Pipeline = SDL_CreateGPUGraphicsPipeline(_mDeviceGPU, &pipelineCreateInfo);
	if (Pipeline == NULL)
	{
		SDL_Log("Failed to create pipeline!");
		return -1;
	}

	SDL_ReleaseGPUShader(_mDeviceGPU, vertexShader);
	SDL_ReleaseGPUShader(_mDeviceGPU, fragmentShader);

	// PointClamp
	Samplers[0] = SDL_CreateGPUSampler(_mDeviceGPU, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_NEAREST,
			.mag_filter = SDL_GPU_FILTER_NEAREST,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
	});
	// PointWrap
	Samplers[1] = SDL_CreateGPUSampler(_mDeviceGPU, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_NEAREST,
			.mag_filter = SDL_GPU_FILTER_NEAREST,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
	});
	// LinearClamp
	Samplers[2] = SDL_CreateGPUSampler(_mDeviceGPU, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_LINEAR,
			.mag_filter = SDL_GPU_FILTER_LINEAR,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
	});
	// LinearWrap
	Samplers[3] = SDL_CreateGPUSampler(_mDeviceGPU, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_LINEAR,
			.mag_filter = SDL_GPU_FILTER_LINEAR,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
	});
	// AnisotropicClamp
	Samplers[4] = SDL_CreateGPUSampler(_mDeviceGPU, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_LINEAR,
			.mag_filter = SDL_GPU_FILTER_LINEAR,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.enable_anisotropy = true,
			.max_anisotropy = 4
	});
	// AnisotropicWrap
	Samplers[5] = SDL_CreateGPUSampler(_mDeviceGPU, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_LINEAR,
			.mag_filter = SDL_GPU_FILTER_LINEAR,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
			.enable_anisotropy = true,
			.max_anisotropy = 4
	});

	// Create the GPU resources
	VertexBuffer = SDL_CreateGPUBuffer(
		_mDeviceGPU,
		&(SDL_GPUBufferCreateInfo) {
		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
			.size = sizeof(PositionTextureVertex) * MAX_VERTICES
	}
	);

	_mTransferBufferForVertexBuffer = SDL_CreateGPUTransferBuffer(
		_mDeviceGPU,
		&(SDL_GPUTransferBufferCreateInfo) {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = sizeof(PositionTextureVertex) * MAX_VERTICES
	}
	);

	IndexBuffer = SDL_CreateGPUBuffer(
		_mDeviceGPU,
		&(SDL_GPUBufferCreateInfo) {
		.usage = SDL_GPU_BUFFERUSAGE_INDEX,
			.size = sizeof(Uint16) * MAX_INDICES
	}
	);

	//UPLOAD INDEX DATA
	{
		// Set up buffer data
		SDL_GPUTransferBuffer* bufferTransferBuffer = SDL_CreateGPUTransferBuffer(
			_mDeviceGPU,
			&(SDL_GPUTransferBufferCreateInfo) {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
				.size = (sizeof(Uint16) * MAX_INDICES)
		}
		);

		uint16_t* transferData = SDL_MapGPUTransferBuffer(
			_mDeviceGPU,
			bufferTransferBuffer,
			true
		);
		SDL_memcpy(transferData, _mIndexBufferData, sizeof(uint16_t)* MAX_INDICES);

		SDL_UnmapGPUTransferBuffer(_mDeviceGPU, bufferTransferBuffer);

		SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(_mDeviceGPU);
		SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

		SDL_UploadToGPUBuffer(
			copyPass,
			&(SDL_GPUTransferBufferLocation) {
			.transfer_buffer = bufferTransferBuffer,
				.offset = 0
		},
			& (SDL_GPUBufferRegion) {
			.buffer = IndexBuffer,
				.offset = 0,
				.size = sizeof(Uint16) * MAX_INDICES
		},
			false
		);

		SDL_EndGPUCopyPass(copyPass);
		SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
		SDL_ReleaseGPUTransferBuffer(_mDeviceGPU, bufferTransferBuffer);
	}

	// Finally, print instructions!
	SDL_Log("Press Left/Right to switch between sampler states");
	SDL_Log("Setting sampler state to: %s", SamplerNames[0]);

	return 0;

	/*FNA3D_HookLogFunctions(Renderer_LogInfo, Renderer_LogWarning, Renderer_LogError);

	//Blend States
	// 
	//NonPremultiplied
	_mBlendControlTypeNonPremultiplied.alphaBlendFunction = FNA3D_BLENDFUNCTION_ADD;
	_mBlendControlTypeNonPremultiplied.alphaDestinationBlend = FNA3D_BLEND_INVERSESOURCEALPHA;
	_mBlendControlTypeNonPremultiplied.alphaSourceBlend = FNA3D_BLEND_SOURCEALPHA;
	_mBlendControlTypeNonPremultiplied.blendFactor = _mWhiteBlendFactor;
	_mBlendControlTypeNonPremultiplied.colorBlendFunction = FNA3D_BLENDFUNCTION_ADD;
	_mBlendControlTypeNonPremultiplied.colorDestinationBlend = FNA3D_BLEND_INVERSESOURCEALPHA;
	_mBlendControlTypeNonPremultiplied.colorSourceBlend = FNA3D_BLEND_SOURCEALPHA;
	_mBlendControlTypeNonPremultiplied.colorWriteEnable = FNA3D_COLORWRITECHANNELS_ALL;
	_mBlendControlTypeNonPremultiplied.colorWriteEnable1 = FNA3D_COLORWRITECHANNELS_ALL;
	_mBlendControlTypeNonPremultiplied.colorWriteEnable2 = FNA3D_COLORWRITECHANNELS_ALL;
	_mBlendControlTypeNonPremultiplied.colorWriteEnable3 = FNA3D_COLORWRITECHANNELS_ALL;
	_mBlendControlTypeNonPremultiplied.multiSampleMask = -1;

	//Additive
	_mBlendControlTypeAdditive.alphaBlendFunction = FNA3D_BLENDFUNCTION_ADD;
	_mBlendControlTypeAdditive.alphaDestinationBlend = FNA3D_BLEND_ONE;
	_mBlendControlTypeAdditive.alphaSourceBlend = FNA3D_BLEND_SOURCEALPHA;
	_mBlendControlTypeAdditive.blendFactor = _mWhiteBlendFactor;
	_mBlendControlTypeAdditive.colorBlendFunction = FNA3D_BLENDFUNCTION_ADD;
	_mBlendControlTypeAdditive.colorDestinationBlend = FNA3D_BLEND_ONE;
	_mBlendControlTypeAdditive.colorSourceBlend = FNA3D_BLEND_SOURCEALPHA;
	_mBlendControlTypeAdditive.colorWriteEnable = FNA3D_COLORWRITECHANNELS_ALL;
	_mBlendControlTypeAdditive.colorWriteEnable1 = FNA3D_COLORWRITECHANNELS_ALL;
	_mBlendControlTypeAdditive.colorWriteEnable2 = FNA3D_COLORWRITECHANNELS_ALL;
	_mBlendControlTypeAdditive.colorWriteEnable3 = FNA3D_COLORWRITECHANNELS_ALL;
	_mBlendControlTypeAdditive.multiSampleMask = -1;

	//Alpha Blend
	_mBlendControlTypeAlphaBlend.alphaBlendFunction = FNA3D_BLENDFUNCTION_ADD;
	_mBlendControlTypeAlphaBlend.alphaDestinationBlend = FNA3D_BLEND_INVERSESOURCEALPHA;
	_mBlendControlTypeAlphaBlend.alphaSourceBlend = FNA3D_BLEND_ONE;
	_mBlendControlTypeAlphaBlend.blendFactor = _mWhiteBlendFactor;
	_mBlendControlTypeAlphaBlend.colorBlendFunction = FNA3D_BLENDFUNCTION_ADD;
	_mBlendControlTypeAlphaBlend.colorDestinationBlend = FNA3D_BLEND_INVERSESOURCEALPHA;
	_mBlendControlTypeAlphaBlend.colorSourceBlend = FNA3D_BLEND_ONE;
	_mBlendControlTypeAlphaBlend.colorWriteEnable = FNA3D_COLORWRITECHANNELS_ALL;
	_mBlendControlTypeAlphaBlend.colorWriteEnable1 = FNA3D_COLORWRITECHANNELS_ALL;
	_mBlendControlTypeAlphaBlend.colorWriteEnable2 = FNA3D_COLORWRITECHANNELS_ALL;
	_mBlendControlTypeAlphaBlend.colorWriteEnable3 = FNA3D_COLORWRITECHANNELS_ALL;
	_mBlendControlTypeAlphaBlend.multiSampleMask = -1;

	//Sampler State
	_mSamplerState.addressU = FNA3D_TEXTUREADDRESSMODE_WRAP;
	_mSamplerState.addressV = FNA3D_TEXTUREADDRESSMODE_WRAP;
	_mSamplerState.addressW = FNA3D_TEXTUREADDRESSMODE_WRAP;
	_mSamplerState.filter = FNA3D_TEXTUREFILTER_POINT;
	_mSamplerState.maxAnisotropy = 0;
	_mSamplerState.maxMipLevel = 0;
	_mSamplerState.mipMapLevelOfDetailBias = 0;

	//Vertex Elements
	_mVertexElements[0].offset = 0;
	_mVertexElements[0].usageIndex = 0;
	_mVertexElements[0].vertexElementFormat = FNA3D_VERTEXELEMENTFORMAT_VECTOR3;
	_mVertexElements[0].vertexElementUsage = FNA3D_VERTEXELEMENTUSAGE_POSITION;

	_mVertexElements[1].offset = sizeof(float) * 3;
	_mVertexElements[1].usageIndex = 0;
	_mVertexElements[1].vertexElementFormat = FNA3D_VERTEXELEMENTFORMAT_VECTOR4;
	_mVertexElements[1].vertexElementUsage = FNA3D_VERTEXELEMENTUSAGE_COLOR;

	_mVertexElements[2].offset = sizeof(float) * 7;
	_mVertexElements[2].usageIndex = 0;
	_mVertexElements[2].vertexElementFormat = FNA3D_VERTEXELEMENTFORMAT_VECTOR2;
	_mVertexElements[2].vertexElementUsage = FNA3D_VERTEXELEMENTUSAGE_TEXTURECOORDINATE;

	if (IsDepthBufferDisabled())
	{
		_mDeviceParams.depthStencilFormat = FNA3D_DEPTHFORMAT_NONE;
	}
	else
	{
		_mDeviceParams.depthStencilFormat = FNA3D_DEPTHFORMAT_D24;
	}
	_mDeviceParams.deviceWindowHandle = deviceWindowHandle;
	_mDeviceParams.backBufferFormat = FNA3D_SURFACEFORMAT_COLOR;
	Rectangle wantedBackBufferBounds = Renderer_GetWantedBackBufferBounds();
	_mDeviceParams.backBufferWidth = wantedBackBufferBounds.Width;
	_mDeviceParams.backBufferHeight = wantedBackBufferBounds.Height;
	Renderer_UpdateVsync();
	_mDeviceContext = FNA3D_CreateDevice(&_mDeviceParams, 0);
	if (_mDeviceContext == NULL)
	{
		return -1;
	}

	Renderer_UpdateViewport();

	Renderer_UpdateScissor();

	CreateIndexBuffer();

	VertexBufferInstance_Init(&_mVertexBuffer);

	FNA3D_SetBlendState(_mDeviceContext, &_mBlendControlTypeNonPremultiplied);

	if (IsDepthBufferDisabled())
	{
		_mDepthStencilState.depthBufferEnable = 0;
	}
	else
	{
		_mDepthStencilState.depthBufferEnable = 1;
	}
	_mDepthStencilState.depthBufferWriteEnable = 0;
	_mDepthStencilState.depthBufferFunction = FNA3D_COMPAREFUNCTION_LESSEQUAL;
	_mDepthStencilState.stencilEnable = 0;
	FNA3D_SetDepthStencilState(_mDeviceContext, &_mDepthStencilState);

	_mRasterizerState.cullMode = FNA3D_CULLMODE_NONE;
	_mRasterizerState.fillMode = FNA3D_FILLMODE_SOLID;
	_mRasterizerState.depthBias = 0;
	_mRasterizerState.multiSampleAntiAlias = 0;
	_mRasterizerState.scissorTestEnable = 0;
	_mRasterizerState.slopeScaleDepthBias = 0;
	FNA3D_ApplyRasterizerState(_mDeviceContext, &_mRasterizerState);

	// load effect
	LoadShader(&_mEffect, "SHADER.fxc");

	return 0;*/
}
void Renderer_BeforeRender(void)
{
	_mLastTextureUsed = NULL;

	_mRenderCommandBuffer = SDL_AcquireGPUCommandBuffer(_mDeviceGPU);
	if (_mRenderCommandBuffer == NULL)
	{
		SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
		return;
	}

	if (!SDL_WaitAndAcquireGPUSwapchainTexture(_mRenderCommandBuffer, _mDeviceWindowHandle, &_mSwapchainTexture, NULL, NULL)) {
		SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
		return;
	}

	if (_mSwapchainTexture != NULL)
	{
		SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
		colorTargetInfo.texture = _mSwapchainTexture;
		colorTargetInfo.clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 1.0f };
		colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
		colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

		_mRenderPass = SDL_BeginGPURenderPass(_mRenderCommandBuffer, &colorTargetInfo, 1, NULL);
		SDL_BindGPUGraphicsPipeline(_mRenderPass, Pipeline);
		SDL_BindGPUIndexBuffer(_mRenderPass, &(SDL_GPUBufferBinding){.buffer = IndexBuffer, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_16BIT);
	}
}
void Renderer_BeforeCommit(void)
{
	Vector2 cameraPos = Renderer_INTERNAL_GetCurrentCameraPosition();
	cameraPos.X -= 640;
	cameraPos.Y -= 360;
	Matrix4x4 cameraMatrix = Matrix4x4_CreateOrthographicOffCenter(
		cameraPos.X,
		cameraPos.X + 1280,
		cameraPos.Y + 720,
		cameraPos.Y,
		0,
		-1
	);
	SDL_PushGPUVertexUniformData(_mRenderCommandBuffer, 0, &cameraMatrix, sizeof(Matrix4x4));
	SDL_PushGPUFragmentUniformData(_mRenderCommandBuffer, 0, &(FragMultiplyUniform){ 1.0f, 1.0f, 1.0f, 1.0f }, sizeof(FragMultiplyUniform));
	SDL_BindGPUIndexBuffer(_mRenderPass, &(SDL_GPUBufferBinding){.buffer = IndexBuffer, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_16BIT);
}
static void SubmitVertices(void)
{
	if ((_mCurrentBatchVerticesOffset == 0) || (_mLastTextureUsed == NULL))
	{
		return;
	}

	// Set up buffer data
	PositionTextureVertex* transferData = SDL_MapGPUTransferBuffer(
		_mDeviceGPU,
		_mTransferBufferForVertexBuffer,
		false
	);
	SDL_memcpy(transferData, _mBatchVertexBuffer, sizeof(PositionTextureVertex) * _mCurrentBatchVerticesOffset);

	SDL_UnmapGPUTransferBuffer(_mDeviceGPU, _mTransferBufferForVertexBuffer);

	SDL_UploadToGPUBuffer(
		_mCopyPass,
		&(SDL_GPUTransferBufferLocation) {
		.transfer_buffer = _mTransferBufferForVertexBuffer,
			.offset = 0
	},
		& (SDL_GPUBufferRegion) {
		.buffer = VertexBuffer,
			.offset = 0,
			.size = sizeof(PositionTextureVertex) * _mCurrentBatchVerticesOffset
	},
		false
	);
}
static void SubmitRender(void)
{
	if ((_mCurrentBatchVerticesOffset == 0) || (_mLastTextureUsed == NULL) || (_mLastRenderVerticesOffset == _mCurrentBatchVerticesOffset))
	{
		return;
	}

	SDL_BindGPUFragmentSamplers(_mRenderPass, 0, &(SDL_GPUTextureSamplerBinding){.texture = _mLastTextureUsed->mTextureData, .sampler = Samplers[0] }, 1);
	SDL_BindGPUVertexBuffers(_mRenderPass, 0, &(SDL_GPUBufferBinding){.buffer = VertexBuffer, .offset = 0 }, 1);

	uint32_t currentLength = (_mCurrentBatchVerticesOffset - _mLastRenderVerticesOffset);
	uint32_t currentSprites = currentLength / 4;

	uint32_t offsetLength = _mLastRenderVerticesOffset;
	uint32_t offsetSprites = offsetLength / 4;

	SDL_DrawGPUIndexedPrimitives(_mRenderPass, currentSprites * 6, 1, offsetSprites * 6, 0, 0);

	_mLastRenderVerticesOffset = _mCurrentBatchVerticesOffset;
}
static void FlushBatch(void)
{
	if ((_mCurrentBatchVerticesOffset == 0) || (_mLastTextureUsed == NULL))
	{
		_mLastRenderVerticesOffset = 0;
		_mCurrentBatchVerticesOffset = 0;
		return;
	}

	SubmitRender();

	SubmitVertices();

	_mLastRenderVerticesOffset = 0;
	_mCurrentBatchVerticesOffset = 0;
}
void Renderer_DrawVertexPositionColorTexture4(Texture* texture, const VertexPositionColorTexture4* sprite)
{
	bool forceAll = false;
	if (((_mCurrentBatchVerticesOffset + VERTICES_IN_SPRITE) >= MAX_VERTICES))
	{
		FlushBatch();
	}
	else if (forceAll || (_mLastTextureUsed != texture))
	{
		SubmitRender();
		_mLastTextureUsed = texture;
	}

	_mBatchVertexBuffer[_mCurrentBatchVerticesOffset + 0] = (PositionTextureVertex){ sprite->Position0.X,  sprite->Position0.Y, sprite->Position0.Z,
		sprite->TextureCoordinate0.U, sprite->TextureCoordinate0.V };

	_mBatchVertexBuffer[_mCurrentBatchVerticesOffset + 1] = (PositionTextureVertex){ sprite->Position1.X,  sprite->Position1.Y, sprite->Position1.Z,
		sprite->TextureCoordinate1.U, sprite->TextureCoordinate1.V };

	_mBatchVertexBuffer[_mCurrentBatchVerticesOffset + 2] = (PositionTextureVertex){ sprite->Position2.X,  sprite->Position2.Y, sprite->Position2.Z,
		sprite->TextureCoordinate2.U, sprite->TextureCoordinate2.V };

	_mBatchVertexBuffer[_mCurrentBatchVerticesOffset + 3] = (PositionTextureVertex){ sprite->Position3.X,  sprite->Position3.Y, sprite->Position3.Z,
		sprite->TextureCoordinate3.U, sprite->TextureCoordinate3.V };

	_mCurrentBatchVerticesOffset += VERTICES_IN_SPRITE;
}
void Renderer_AfterCommit(void)
{
}
void Renderer_AfterRender(void)
{
	FlushBatch();

	SubmitCommandBufferForCopy();

	SDL_EndGPURenderPass(_mRenderPass);
	_mRenderPass = NULL;

	SDL_SubmitGPUCommandBuffer(_mRenderCommandBuffer);
	_mRenderCommandBuffer = NULL;
}
void Renderer_FlushBatch(void)
{
	//NextBatch(false);
}
void Renderer_EnableDepthBufferWrite(void)
{
	/*if (IsDepthBufferDisabled())
	{
		return;
	}

	_mDepthStencilState.depthBufferWriteEnable = 1;*/
}
void Renderer_DisableDepthBufferWrite(void)
{
	/*if (IsDepthBufferDisabled())
	{
		return;
	}

	_mDepthStencilState.depthBufferWriteEnable = 0;*/
}
void Renderer_UpdateVsync(void)
{
	/*bool useVsync = Renderer_IsVsync();
	if (Service_PlatformForcesVsync())
	{
		useVsync = true;
	}

#ifdef DEBUG_DEF_FORCE_VSYNC_OFF
	useVsync = false;
#endif

#ifdef DEBUG_DEF_FORCE_VSYNC_ON
	useVsync = true;
#endif

	if (useVsync)
	{
		_mDeviceParams.presentationInterval = FNA3D_PRESENTINTERVAL_ONE;
	}
	else
	{
		_mDeviceParams.presentationInterval = FNA3D_PRESENTINTERVAL_IMMEDIATE;
	}*/
}
void Renderer_ApplyChanges(void)
{
	//Renderer_ResetBackBuffer();
	//Renderer_UpdateViewport();
	//Renderer_UpdateScissor();
}
void Renderer_UpdateViewport(void)
{
	/*Rectangle backBufferBounds = GetCurrentBufferBounds();
	FNA3D_Viewport viewport = { 0 };
	viewport.w = backBufferBounds.Width;
	viewport.h = backBufferBounds.Height;
	viewport.maxDepth = 1;
	FNA3D_SetViewport(_mDeviceContext, &viewport);*/
}
void Renderer_UpdateScissor(void)
{
	/*Rectangle backBufferBounds = GetCurrentBufferBounds();
	FNA3D_Rect scissor = { 0 };
	scissor.x = backBufferBounds.X;
	scissor.y = backBufferBounds.Y;
	scissor.w = backBufferBounds.Width;
	scissor.h = backBufferBounds.Height;
	FNA3D_SetScissorRect(_mDeviceContext, &scissor);*/
}
void Renderer_ResetBackBuffer(void)
{
	/*Rectangle wantedBackBufferBounds = Renderer_GetWantedBackBufferBounds();

	_mVirtualBufferBounds.Width = wantedBackBufferBounds.Width;
	_mVirtualBufferBounds.Height = wantedBackBufferBounds.Height;

	if (IsOffscreenTargetNeeded())
	{
		_mActualBufferBounds = Renderer_GetDrawableSize();
	}
	else
	{
		_mActualBufferBounds = _mVirtualBufferBounds;
	}

	_mDeviceParams.backBufferWidth = _mActualBufferBounds.Width;
	_mDeviceParams.backBufferHeight = _mActualBufferBounds.Height;

	Logger_LogInformation("Back Buffer has been reset");
	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "Actual buffer bounds: ");
		MString_AddAssignInt(&tempString, _mActualBufferBounds.Width);
		MString_AddAssignString(&tempString, "x");
		MString_AddAssignInt(&tempString, _mActualBufferBounds.Height);
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}

	FNA3D_ResetBackbuffer(_mDeviceContext, &_mDeviceParams);

	if (IsOffscreenTargetNeeded())
	{
		{
			MString* tempString = NULL;
			MString_AssignString(&tempString, "Virtual buffer bounds: ");
			MString_AddAssignInt(&tempString, _mVirtualBufferBounds.Width);
			MString_AddAssignString(&tempString, "x");
			MString_AddAssignInt(&tempString, _mVirtualBufferBounds.Height);
			Logger_LogInformation(MString_Text(tempString));
			MString_Dispose(&tempString);
		}

		if (_mOffscreenTarget.mTextureData != NULL)
		{
			FNA3D_AddDisposeTexture(_mDeviceContext, (FNA3D_Texture*)(_mOffscreenTarget.mTextureData));
			_mOffscreenTarget.mTextureData = NULL;
		}

		_mOffscreenTargetBinding.twod.width = _mVirtualBufferBounds.Width;
		_mOffscreenTargetBinding.twod.height = _mVirtualBufferBounds.Height;

		_mOffscreenTarget.mTextureData = FNA3D_CreateTexture2D(_mDeviceContext, FNA3D_SURFACEFORMAT_COLOR, _mVirtualBufferBounds.Width, _mVirtualBufferBounds.Height, 1, 1);
		_mOffscreenTarget.mBounds.Width = _mVirtualBufferBounds.Width;
		_mOffscreenTarget.mBounds.Height = _mVirtualBufferBounds.Height;
	}*/
}
Rectangle Renderer_GetDrawableSize(void)
{
	return Rectangle_Empty;
	/*Rectangle drawableSize;
	drawableSize.X = 0;
	drawableSize.Y = 0;
	SDL_GetWindowSizeInPixels((SDL_Window*)_mDeviceWindowHandle, &drawableSize.Width, &drawableSize.Height);
	//FNA3D_GetDrawableSize(_mDeviceWindowHandle, &drawableSize.Width, &drawableSize.Height);
	return drawableSize;*/
}

#endif

typedef int compiler_warning_compliance;