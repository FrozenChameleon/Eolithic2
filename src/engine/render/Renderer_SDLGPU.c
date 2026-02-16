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

//#define DISABLE_OFFSCREEN_TARGET

#define MULTI_COLOR_REPLACE_LEN SHADER_PROGRAM_MAX_REPLACE_LENGTH

#define TRANSFER_BUFFER_FOR_TEXTURE_LEN 16777216 /* 16 MiB */

/*
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
static const char* SHADER_PARAMETER_COLOR_REPLACE_ALPHA = "ColorReplaceAlpha";*/


//static float _mMultiColorReplaceData[MULTI_COLOR_REPLACE_LEN];
//static BlendState _mLastUsedBlendState = BLENDSTATE_INVALID;
//static bool _mIsDrawingFroOffscreenTarget;
//static bool _mIsDrawingToOffscreenTarget;
//static Rectangle _mVirtualBufferBounds;
//static Rectangle _mActualBufferBounds;

static const char* SamplerNames[] =
{
	"PointClamp",
	"PointWrap",
	"LinearClamp",
	"LinearWrap",
	"AnisotropicClamp",
	"AnisotropicWrap",
};

/*static Rectangle GetCurrentBufferBounds(void)
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
}*/

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

typedef struct FixedRenderState
{
	SDL_Window* WindowHandle;
	SDL_GPUDevice* DeviceHandle;
	SDL_GPUGraphicsPipeline* PipelineForRTT;
	SDL_GPUGraphicsPipeline* PipelineForSwap;
	SDL_GPUBuffer* VertexBuffer;
	SDL_GPUBuffer* IndexBuffer;
	SDL_GPUSampler* Samplers[SDL_arraysize(SamplerNames)];
	SDL_GPUTransferBuffer* TransferBufferForVertexBuffer;
	VertexPositionColorTexture BatchVertexBuffer[MAX_VERTICES];
	uint16_t* IndexBufferData;
	Texture* OffscreenTarget;
}FixedRenderState;

typedef struct TempRenderState
{
	SDL_GPUCommandBuffer* CommandRender;
	SDL_GPUCommandBuffer* CommandUpload;
	SDL_GPURenderPass* RenderPass;
	SDL_GPUTexture* SwapchainTexture;
	SDL_GPUCopyPass* CopyPass;
	Texture* LastTextureUsed;
	uint32_t CurrentBatchVerticesOffset;
	uint32_t LastRenderVerticesOffset;

}TempRenderState;

static FixedRenderState _mFixed;
static TempRenderState _mTemp;

static void UpdateOffscreenTexture(void)
{
	Rectangle internalBounds = Utils_GetInternalRectangle();

	_mFixed.OffscreenTarget = (Texture*)Utils_calloc(1, sizeof(Texture));
	MString_AssignString(&_mFixed.OffscreenTarget->mPath, "");
	_mFixed.OffscreenTarget->mBounds.Width = internalBounds.Width;
	_mFixed.OffscreenTarget->mBounds.Height = internalBounds.Height;

	SDL_GPUTextureCreateInfo info = { 0 };
	info.type = SDL_GPU_TEXTURETYPE_2D;
	info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
	info.width = internalBounds.Width;
	info.height = internalBounds.Height;
	info.layer_count_or_depth = 1;
	info.num_levels = 1;
	info.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
	_mFixed.OffscreenTarget->mTextureData = SDL_CreateGPUTexture(_mFixed.DeviceHandle, &info);
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

	Renderer_SetupVerticesForTTFont(_VertexBuffer.mVertices, fontState->mColor, 0, verts, tcoords, colors, nverts);

	VertexBufferInstance_SetData(&_VertexBuffer, 0, nverts);

	FNA3D_ApplyVertexBufferBindings(_mDeviceContext, &_VertexBuffer.VertexBufferBinding, 1, 1, 0);

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
	ImagePixelData* ipd = ImagePixelData_Create(blob);

	uint8_t* textureData = ImagePixelData_GetData(ipd);
	Rectangle textureBounds = ImagePixelData_GetBounds(ipd);
	Texture* tex = Renderer_GetNewTextureData(path, textureBounds.Width, textureBounds.Height, false);

	uint32_t textureDataLen = (textureBounds.Width * textureBounds.Height * 4);

	SDL_GPUCommandBuffer* commandUploadTexture = SDL_AcquireGPUCommandBuffer(_mFixed.DeviceHandle);
	if (commandUploadTexture == NULL)
	{
		SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
		return NULL;
	}

	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandUploadTexture);
	if (copyPass == NULL)
	{
		SDL_Log("SDL_BeginGPUCopyPass failed: %s", SDL_GetError());
		return NULL;
	}

	SDL_GPUTransferBuffer* transferBufferToUse = SDL_CreateGPUTransferBuffer(
		_mFixed.DeviceHandle,
		&(SDL_GPUTransferBufferCreateInfo) {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = textureDataLen
	}
	);

	Uint8* textureTransferPtr = SDL_MapGPUTransferBuffer(
		_mFixed.DeviceHandle,
		transferBufferToUse,
		false
	);
	SDL_memcpy(textureTransferPtr, textureData, textureDataLen);
	SDL_UnmapGPUTransferBuffer(_mFixed.DeviceHandle, 0);

	// Upload the transfer data to the GPU resources
	SDL_UploadToGPUTexture(
		copyPass,
		&(SDL_GPUTextureTransferInfo) {
		.transfer_buffer = transferBufferToUse,
			.offset = 0,
	},
		& (SDL_GPUTextureRegion) {
		.texture = tex->mTextureData,
			.w = textureBounds.Width,
			.h = textureBounds.Height,
			.d = 1
	},
		false
	);

	SDL_EndGPUCopyPass(copyPass);
	SDL_SubmitGPUCommandBuffer(commandUploadTexture);

	SDL_ReleaseGPUTransferBuffer(_mFixed.DeviceHandle, transferBufferToUse);

	return tex;
}
Texture* Renderer_GetNewTextureData(const char* path, int32_t width, int32_t height, bool clearTexture)
{
	Texture* tex = (Texture*)Utils_calloc(1, sizeof(Texture));
	MString_AssignString(&tex->mPath, path);
	tex->mBounds.Width = width;
	tex->mBounds.Height = height;
	SDL_GPUTexture* textureData = SDL_CreateGPUTexture(_mFixed.DeviceHandle, &(SDL_GPUTextureCreateInfo){
		.type = SDL_GPU_TEXTURETYPE_2D,
			.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
			.width = width,
			.height = height,
			.layer_count_or_depth = 1,
			.num_levels = 1,
			.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER
	});
	tex->mTextureData = textureData;
	return tex;
}
void Renderer_UpdateTextureData(Texture* texture, int32_t x, int32_t y, int32_t w, int32_t h, int32_t level, void* data, int32_t dataLength)
{
	/*InvalidateNextSetupRenderState();

	FNA3D_SetTextureData2D(_mDeviceContext, (FNA3D_Texture*)(texture->mTextureData), x, y, w, h, level, data, dataLength);*/
}
static SDL_GPUGraphicsPipeline* CreatePipeline(SDL_GPUShader* vertexShader, SDL_GPUShader* fragmentShader, SDL_GPUTextureFormat textureFormat)
{
	// Create the pipeline
	SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
		.target_info = {
			.num_color_targets = 1,
			.color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
				.format = textureFormat,
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
				.pitch = sizeof(VertexPositionColorTexture)
			}},
			.num_vertex_attributes = 3,
			.vertex_attributes = (SDL_GPUVertexAttribute[]){{
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
				.location = 0,
				.offset = 0
			},
			{
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
				.location = 1,
				.offset = (sizeof(float) * 3)
			},
			{
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
				.location = 2,
				.offset = (sizeof(float) * 3) + (sizeof(float) * 4)
			}}
		},
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.vertex_shader = vertexShader,
		.fragment_shader = fragmentShader
	};

	SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(_mFixed.DeviceHandle, &pipelineCreateInfo);
	if (pipeline == NULL)
	{
		SDL_Log("Failed to create pipeline!");
	}
	return pipeline;
}
int32_t Renderer_Init(void* deviceWindowHandle)
{
	_mFixed.IndexBufferData = Renderer_GetDefaultIndexBufferData();

	_mFixed.WindowHandle = (SDL_Window*)deviceWindowHandle;

	_mFixed.DeviceHandle = SDL_CreateGPUDevice(
		SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
		true,
		NULL);

	if (_mFixed.DeviceHandle == NULL)
	{
		SDL_Log("GPUCreateDevice failed");
		return -1;
	}

	if (!SDL_ClaimWindowForGPUDevice(_mFixed.DeviceHandle, _mFixed.WindowHandle))
	{
		SDL_Log("GPUClaimWindow failed");
		return -1;
	}

	SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;
	if (SDL_WindowSupportsGPUPresentMode(
		_mFixed.DeviceHandle,
		_mFixed.WindowHandle,
		SDL_GPU_PRESENTMODE_IMMEDIATE
	)) {
		presentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;
	}
	else if (SDL_WindowSupportsGPUPresentMode(
		_mFixed.DeviceHandle,
		_mFixed.WindowHandle,
		SDL_GPU_PRESENTMODE_MAILBOX
	)) {
		presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
	}

	SDL_SetGPUSwapchainParameters(
		_mFixed.DeviceHandle,
		_mFixed.WindowHandle,
		SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
		presentMode
	);

	// Create the shaders
	SDL_GPUShader* vertexShader = LoadShader(_mFixed.DeviceHandle, "TexturedQuadColorWithMatrix2.vert", 0, 1, 0, 0);
	if (vertexShader == NULL)
	{
		SDL_Log("Failed to create vertex shader!");
		return -1;
	}

	SDL_GPUShader* fragmentShader = LoadShader(_mFixed.DeviceHandle, "TexturedQuadColor.frag", 1, 1, 0, 0);
	if (fragmentShader == NULL)
	{
		SDL_Log("Failed to create fragment shader!");
		return -1;
	}

	_mFixed.PipelineForSwap = CreatePipeline(vertexShader, fragmentShader, SDL_GetGPUSwapchainTextureFormat(_mFixed.DeviceHandle, _mFixed.WindowHandle));
	_mFixed.PipelineForRTT = CreatePipeline(vertexShader, fragmentShader, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM);

	SDL_ReleaseGPUShader(_mFixed.DeviceHandle, vertexShader);
	SDL_ReleaseGPUShader(_mFixed.DeviceHandle, fragmentShader);

	// PointClamp
	_mFixed.Samplers[0] = SDL_CreateGPUSampler(_mFixed.DeviceHandle, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_NEAREST,
			.mag_filter = SDL_GPU_FILTER_NEAREST,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
	});
	// PointWrap
	_mFixed.Samplers[1] = SDL_CreateGPUSampler(_mFixed.DeviceHandle, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_NEAREST,
			.mag_filter = SDL_GPU_FILTER_NEAREST,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
	});
	// LinearClamp
	_mFixed.Samplers[2] = SDL_CreateGPUSampler(_mFixed.DeviceHandle, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_LINEAR,
			.mag_filter = SDL_GPU_FILTER_LINEAR,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
	});
	// LinearWrap
	_mFixed.Samplers[3] = SDL_CreateGPUSampler(_mFixed.DeviceHandle, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_LINEAR,
			.mag_filter = SDL_GPU_FILTER_LINEAR,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
	});
	// AnisotropicClamp
	_mFixed.Samplers[4] = SDL_CreateGPUSampler(_mFixed.DeviceHandle, &(SDL_GPUSamplerCreateInfo){
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
	_mFixed.Samplers[5] = SDL_CreateGPUSampler(_mFixed.DeviceHandle, &(SDL_GPUSamplerCreateInfo){
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
	_mFixed.VertexBuffer = SDL_CreateGPUBuffer(
		_mFixed.DeviceHandle,
		&(SDL_GPUBufferCreateInfo) {
		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
			.size = sizeof(VertexPositionColorTexture) * MAX_VERTICES
	}
	);

	_mFixed.TransferBufferForVertexBuffer = SDL_CreateGPUTransferBuffer(
		_mFixed.DeviceHandle,
		&(SDL_GPUTransferBufferCreateInfo) {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = sizeof(VertexPositionColorTexture) * MAX_VERTICES
	}
	);

	_mFixed.IndexBuffer = SDL_CreateGPUBuffer(
		_mFixed.DeviceHandle,
		&(SDL_GPUBufferCreateInfo) {
		.usage = SDL_GPU_BUFFERUSAGE_INDEX,
			.size = sizeof(Uint16) * MAX_INDICES
	}
	);

	//UPLOAD INDEX DATA
	{
		// Set up buffer data
		SDL_GPUTransferBuffer* bufferTransferBuffer = SDL_CreateGPUTransferBuffer(
			_mFixed.DeviceHandle,
			&(SDL_GPUTransferBufferCreateInfo) {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
				.size = (sizeof(Uint16) * MAX_INDICES)
		}
		);

		uint16_t* transferData = SDL_MapGPUTransferBuffer(
			_mFixed.DeviceHandle,
			bufferTransferBuffer,
			true
		);
		SDL_memcpy(transferData, _mFixed.IndexBufferData, sizeof(uint16_t)* MAX_INDICES);

		SDL_UnmapGPUTransferBuffer(_mFixed.DeviceHandle, bufferTransferBuffer);

		SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(_mFixed.DeviceHandle);
		SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

		SDL_UploadToGPUBuffer(
			copyPass,
			&(SDL_GPUTransferBufferLocation) {
			.transfer_buffer = bufferTransferBuffer,
				.offset = 0
		},
			& (SDL_GPUBufferRegion) {
			.buffer = _mFixed.IndexBuffer,
				.offset = 0,
				.size = sizeof(Uint16) * MAX_INDICES
		},
			false
		);

		SDL_EndGPUCopyPass(copyPass);
		SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
		SDL_ReleaseGPUTransferBuffer(_mFixed.DeviceHandle, bufferTransferBuffer);
	}

	UpdateOffscreenTexture();

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
	_Samplerstate.addressU = FNA3D_TEXTUREADDRESSMODE_WRAP;
	_Samplerstate.addressV = FNA3D_TEXTUREADDRESSMODE_WRAP;
	_Samplerstate.addressW = FNA3D_TEXTUREADDRESSMODE_WRAP;
	_Samplerstate.filter = FNA3D_TEXTUREFILTER_POINT;
	_Samplerstate.maxAnisotropy = 0;
	_Samplerstate.maxMipLevel = 0;
	_Samplerstate.mipMapLevelOfDetailBias = 0;

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

	VertexBufferInstance_Init(&_VertexBuffer);

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
	Utils_memset(&_mTemp, 0, sizeof(TempRenderState));

	Renderer_UpdateDisplayDimensions();

	_mTemp.LastTextureUsed = NULL;

	_mTemp.CommandUpload = SDL_AcquireGPUCommandBuffer(_mFixed.DeviceHandle);
	if (_mTemp.CommandUpload == NULL)
	{
		SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
		return;
	}

	_mTemp.CopyPass = SDL_BeginGPUCopyPass(_mTemp.CommandUpload);
	if (_mTemp.CopyPass == NULL)
	{
		SDL_Log("SDL_BeginGPUCopyPass failed: %s", SDL_GetError());
		return;
	}

	_mTemp.CommandRender = SDL_AcquireGPUCommandBuffer(_mFixed.DeviceHandle);
	if (_mTemp.CommandRender == NULL)
	{
		SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
		return;
	}

	if (!SDL_WaitAndAcquireGPUSwapchainTexture(_mTemp.CommandRender, _mFixed.WindowHandle, &_mTemp.SwapchainTexture, NULL, NULL)) {
		SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
		return;
	}

	SDL_GPUTexture* textureToRenderTo = NULL;
	SDL_GPUGraphicsPipeline* pipelineToUse = NULL;
#ifdef DISABLE_OFFSCREEN_TARGET
	textureToRenderTo = _mTemp.SwapchainTexture;
	pipelineToUse = _mFixed.PipelineForSwap;
#else
	textureToRenderTo = _mFixed.OffscreenTarget->mTextureData;
	pipelineToUse = _mFixed.PipelineForRTT;
#endif
	SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
	colorTargetInfo.texture = textureToRenderTo;
	colorTargetInfo.clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 1.0f };
	colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
	colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

	_mTemp.RenderPass = SDL_BeginGPURenderPass(_mTemp.CommandRender, &colorTargetInfo, 1, NULL);
	SDL_BindGPUGraphicsPipeline(_mTemp.RenderPass, pipelineToUse);
	SDL_BindGPUIndexBuffer(_mTemp.RenderPass, &(SDL_GPUBufferBinding){.buffer = _mFixed.IndexBuffer, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_16BIT);
}
void Renderer_BeforeCommit(void)
{
	Vector2 cameraPos = Renderer_INTERNAL_GetCurrentCameraPosition();
	cameraPos.X -= 640;
	cameraPos.Y -= 360;
	Matrix cameraMatrix = Matrix_CreateOrthographicOffCenter(
		cameraPos.X,
		cameraPos.X + 1280,
		cameraPos.Y + 720,
		cameraPos.Y,
		0,
		-1
	);
	SDL_PushGPUVertexUniformData(_mTemp.CommandRender, 0, &cameraMatrix, sizeof(Matrix));
	SDL_BindGPUIndexBuffer(_mTemp.RenderPass, &(SDL_GPUBufferBinding){.buffer = _mFixed.IndexBuffer, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_16BIT);
}
static void SubmitRender(void)
{
	if ((_mTemp.CurrentBatchVerticesOffset == 0) ||
		(_mTemp.LastTextureUsed == NULL) ||
		(_mTemp.LastRenderVerticesOffset == _mTemp.CurrentBatchVerticesOffset))
	{
		return;
	}

	SDL_BindGPUFragmentSamplers(_mTemp.RenderPass, 0,
		&(SDL_GPUTextureSamplerBinding){.texture = _mTemp.LastTextureUsed->mTextureData, .sampler = _mFixed.Samplers[0] }, 1);
	SDL_BindGPUVertexBuffers(_mTemp.RenderPass, 0,
		&(SDL_GPUBufferBinding){.buffer = _mFixed.VertexBuffer, .offset = 0 }, 1);

	uint32_t currentLength = (_mTemp.CurrentBatchVerticesOffset - _mTemp.LastRenderVerticesOffset);
	uint32_t currentSprites = currentLength / 4;

	uint32_t offsetLength = _mTemp.LastRenderVerticesOffset;
	uint32_t offsetSprites = offsetLength / 4;

	SDL_DrawGPUIndexedPrimitives(_mTemp.RenderPass, currentSprites * 6, 1, offsetSprites * 6, 0, 0);

	_mTemp.LastRenderVerticesOffset = _mTemp.CurrentBatchVerticesOffset;
}
static void SubmitVertices(void)
{
	if ((_mTemp.CurrentBatchVerticesOffset == 0))
	{
		return;
	}

	// Set up buffer data
	VertexPositionColorTexture* transferData = SDL_MapGPUTransferBuffer(
		_mFixed.DeviceHandle,
		_mFixed.TransferBufferForVertexBuffer,
		false
	);
	SDL_memcpy(transferData, _mFixed.BatchVertexBuffer, sizeof(VertexPositionColorTexture) * _mTemp.CurrentBatchVerticesOffset);

	SDL_UnmapGPUTransferBuffer(_mFixed.DeviceHandle, _mFixed.TransferBufferForVertexBuffer);

	SDL_UploadToGPUBuffer(
		_mTemp.CopyPass,
		&(SDL_GPUTransferBufferLocation) {
		.transfer_buffer = _mFixed.TransferBufferForVertexBuffer,
			.offset = 0
	},
		& (SDL_GPUBufferRegion) {
		.buffer = _mFixed.VertexBuffer,
			.offset = 0,
			.size = sizeof(VertexPositionColorTexture) * _mTemp.CurrentBatchVerticesOffset
	},
		false
	);
}
static void FlushBatch(void)
{
	SubmitRender();

	SubmitVertices();

	_mTemp.LastRenderVerticesOffset = 0;
	_mTemp.CurrentBatchVerticesOffset = 0;
	_mTemp.LastTextureUsed = NULL;
}
void Renderer_DrawVertexPositionColorTexture4(Texture* texture, const VertexPositionColorTexture4* sprite)
{
	if ((texture == NULL) || (texture->mTextureData == NULL))
	{
		Logger_LogWarning("Attempted to render NULL texture or texture data!");
		return;
	}

	if ((_mTemp.CurrentBatchVerticesOffset + VERTICES_IN_SPRITE) >= MAX_VERTICES) //Just hard limiting vertexes for now anyway... //TODO
	{
		Logger_LogWarning("Reached vertex buffer limit!");
		return;
	}

	if (_mTemp.LastTextureUsed == NULL)
	{
		_mTemp.LastTextureUsed = texture;
	}

	if ((_mTemp.LastTextureUsed != texture))
	{
		SubmitRender();
		_mTemp.LastTextureUsed = texture;
	}

	_mFixed.BatchVertexBuffer[_mTemp.CurrentBatchVerticesOffset + 0] = (VertexPositionColorTexture){ sprite->Position0.X,  sprite->Position0.Y, sprite->Position0.Z,
		sprite->Color0.R, sprite->Color0.G, sprite->Color0.B, sprite->Color0.A,
		sprite->TextureCoordinate0.U, sprite->TextureCoordinate0.V };

	_mFixed.BatchVertexBuffer[_mTemp.CurrentBatchVerticesOffset + 1] = (VertexPositionColorTexture){ sprite->Position1.X,  sprite->Position1.Y, sprite->Position1.Z,
		sprite->Color1.R, sprite->Color1.G, sprite->Color1.B, sprite->Color1.A,
		sprite->TextureCoordinate1.U, sprite->TextureCoordinate1.V };

	_mFixed.BatchVertexBuffer[_mTemp.CurrentBatchVerticesOffset + 2] = (VertexPositionColorTexture){ sprite->Position2.X,  sprite->Position2.Y, sprite->Position2.Z,
		sprite->Color2.R, sprite->Color2.G, sprite->Color2.B, sprite->Color2.A,
		sprite->TextureCoordinate2.U, sprite->TextureCoordinate2.V };

	_mFixed.BatchVertexBuffer[_mTemp.CurrentBatchVerticesOffset + 3] = (VertexPositionColorTexture){ sprite->Position3.X,  sprite->Position3.Y, sprite->Position3.Z,
		sprite->Color3.R, sprite->Color3.G, sprite->Color3.B, sprite->Color3.A,
		sprite->TextureCoordinate3.U, sprite->TextureCoordinate3.V };

	_mTemp.CurrentBatchVerticesOffset += VERTICES_IN_SPRITE;
}
void Renderer_AfterCommit(void)
{
}
void Renderer_AfterRender(void)
{
	FlushBatch();

	SDL_EndGPURenderPass(_mTemp.RenderPass);

#ifndef DISABLE_OFFSCREEN_TARGET
	SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
	colorTargetInfo.texture = _mTemp.SwapchainTexture;
	colorTargetInfo.clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 1.0f };
	colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
	colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

	_mTemp.RenderPass = SDL_BeginGPURenderPass(_mTemp.CommandRender, &colorTargetInfo, 1, NULL);

	SDL_BindGPUGraphicsPipeline(_mTemp.RenderPass, _mFixed.PipelineForSwap);
	SDL_BindGPUIndexBuffer(_mTemp.RenderPass, &(SDL_GPUBufferBinding){.buffer = _mFixed.IndexBuffer, .offset = 0 }, SDL_GPU_INDEXELEMENTSIZE_16BIT);
	Vector2 cameraPos = Vector2_Zero;
	cameraPos.X += 0;
	cameraPos.Y += 0;
	Matrix cameraMatrix = Matrix_CreateOrthographicOffCenter(
		cameraPos.X,
		cameraPos.X + 1280,
		cameraPos.Y + 720,
		cameraPos.Y,
		0,
		-1
	);
	SDL_PushGPUVertexUniformData(_mTemp.CommandRender, 0, &cameraMatrix, sizeof(Matrix));

	Rectangle screenBounds = Renderer_GetScreenBounds();
	Renderer_Draw(_mFixed.OffscreenTarget, screenBounds, Color_White);
	FlushBatch();

	SDL_EndGPURenderPass(_mTemp.RenderPass);
#endif

	SDL_EndGPUCopyPass(_mTemp.CopyPass);

	SDL_SubmitGPUCommandBuffer(_mTemp.CommandUpload);

	SDL_SubmitGPUCommandBuffer(_mTemp.CommandRender);

	Utils_memset(&_mTemp, 0, sizeof(TempRenderState));
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

		if (_OffscreenTarget.mTextureData != NULL)
		{
			FNA3D_AddDisposeTexture(_mDeviceContext, (FNA3D_Texture*)(_OffscreenTarget.mTextureData));
			_OffscreenTarget.mTextureData = NULL;
		}

		_OffscreenTargetBinding.twod.width = _mVirtualBufferBounds.Width;
		_OffscreenTargetBinding.twod.height = _mVirtualBufferBounds.Height;

		_OffscreenTarget.mTextureData = FNA3D_CreateTexture2D(_mDeviceContext, FNA3D_SURFACEFORMAT_COLOR, _mVirtualBufferBounds.Width, _mVirtualBufferBounds.Height, 1, 1);
		_OffscreenTarget.mBounds.Width = _mVirtualBufferBounds.Width;
		_OffscreenTarget.mBounds.Height = _mVirtualBufferBounds.Height;
	}*/
}
Rectangle Renderer_GetDrawableSize(void)
{
	Rectangle drawableSize;
	drawableSize.X = 0;
	drawableSize.Y = 0;
	SDL_GetWindowSizeInPixels(_mFixed.WindowHandle, &drawableSize.Width, &drawableSize.Height);
	return drawableSize;
}

#endif

typedef int compiler_warning_compliance;