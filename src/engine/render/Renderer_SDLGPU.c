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
#include "../globals/Globals.h"
#include "RenderTTFont.h"
#include "SDL3/SDL.h"
#include "ImagePixelData.h"
#ifdef EDITOR_MODE
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"
#endif

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

static bool IsOffscreenTargetNeeded(void)
{
	if (Globals_IsEditorActive())
	{
		return false;
	}
	else
	{
		return Service_PlatformRequiresOffscreenTargetForRender();
	}
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

	SDL_GPUShader* shader = NULL;
	{
		SDL_GPUShaderCreateInfo shaderInfo;
		Utils_memset(&shaderInfo, 0, sizeof(SDL_GPUShaderCreateInfo));
		shaderInfo.code = (Uint8*)code;
		shaderInfo.code_size = codeSize;
		shaderInfo.entrypoint = entrypoint;
		shaderInfo.format = format;
		shaderInfo.stage = stage;
		shaderInfo.num_samplers = samplerCount;
		shaderInfo.num_uniform_buffers = uniformBufferCount;
		shaderInfo.num_storage_buffers = storageBufferCount;
		shaderInfo.num_storage_textures = storageTextureCount;
		shader = SDL_CreateGPUShader(device, &shaderInfo);
	}
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

	SDL_GPUTransferBuffer* transferBufferToUse = NULL;
	{
		SDL_GPUTransferBufferCreateInfo info;
		Utils_memset(&info, 0, sizeof(SDL_GPUTransferBufferCreateInfo));
		info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
		info.size = textureDataLen;
		transferBufferToUse = SDL_CreateGPUTransferBuffer(_mFixed.DeviceHandle, &info);
	}

	Uint8* textureTransferPtr = (Uint8*)SDL_MapGPUTransferBuffer(_mFixed.DeviceHandle, transferBufferToUse, false);
	SDL_memcpy(textureTransferPtr, textureData, textureDataLen);
	SDL_UnmapGPUTransferBuffer(_mFixed.DeviceHandle, 0);

	{ // Upload the transfer data to the GPU resources
		SDL_GPUTextureTransferInfo info;
		Utils_memset(&info, 0, sizeof(SDL_GPUTextureTransferInfo));
		info.transfer_buffer = transferBufferToUse;
		info.offset = 0;
		SDL_GPUTextureRegion region;
		Utils_memset(&region, 0, sizeof(SDL_GPUTextureRegion));
		region.texture = (SDL_GPUTexture*)tex->mTextureData;
		region.w = textureBounds.Width;
		region.h = textureBounds.Height;
		region.d = 1;
		SDL_UploadToGPUTexture(copyPass, &info, &region, false);
	}

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

	{
		SDL_GPUTextureCreateInfo info = { SDL_GPU_TEXTURETYPE_2D };
		info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
		info.width = width;
		info.height = height;
		info.layer_count_or_depth = 1;
		info.num_levels = 1;
		info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
		tex->mTextureData = SDL_CreateGPUTexture(_mFixed.DeviceHandle, &info);
	}

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
	SDL_GPUColorTargetBlendState blendState;
	Utils_memset(&blendState, 0, sizeof(SDL_GPUColorTargetBlendState));
	blendState.enable_blend = true;
	blendState.color_blend_op = SDL_GPU_BLENDOP_ADD;
	blendState.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
	blendState.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	blendState.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	blendState.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	blendState.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

	SDL_GPUColorTargetDescription colorTargetDescriptions;
	Utils_memset(&colorTargetDescriptions, 0, sizeof(SDL_GPUColorTargetDescription));
	colorTargetDescriptions.format = textureFormat;
	colorTargetDescriptions.blend_state = blendState;

	SDL_GPUGraphicsPipelineTargetInfo targetInfo;
	Utils_memset(&targetInfo, 0, sizeof(SDL_GPUGraphicsPipelineTargetInfo));
	targetInfo.num_color_targets = 1;
	targetInfo.color_target_descriptions = &colorTargetDescriptions;

	SDL_GPUVertexBufferDescription vertexBufferDescription;
	Utils_memset(&vertexBufferDescription, 0, sizeof(SDL_GPUVertexBufferDescription));
	vertexBufferDescription.slot = 0;
	vertexBufferDescription.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	vertexBufferDescription.instance_step_rate = 0;
	vertexBufferDescription.pitch = sizeof(VertexPositionColorTexture);
	
	SDL_GPUVertexAttribute vertexAttributes[3]; //LENGTH = 3
	Utils_memset(vertexAttributes, 0, sizeof(SDL_GPUVertexAttribute) * 3); //LENGTH = 3

	vertexAttributes[0].buffer_slot = 0;
	vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
	vertexAttributes[0].location = 0;
	vertexAttributes[0].offset = 0;

	vertexAttributes[1].buffer_slot = 0;
	vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
	vertexAttributes[1].location = 1;
	vertexAttributes[1].offset = (sizeof(float) * 3);

	vertexAttributes[2].buffer_slot = 0;
	vertexAttributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
	vertexAttributes[2].location = 2;
	vertexAttributes[2].offset = (sizeof(float) * 3) + (sizeof(float) * 4);

	SDL_GPUVertexInputState vertexInputState;
	Utils_memset(&vertexInputState, 0, sizeof(SDL_GPUVertexInputState));
	vertexInputState.num_vertex_buffers = 1;
	vertexInputState.vertex_buffer_descriptions = &vertexBufferDescription;
	vertexInputState.num_vertex_attributes = 3;
	vertexInputState.vertex_attributes = vertexAttributes;

	SDL_GPUGraphicsPipelineCreateInfo createInfo;
	Utils_memset(&createInfo, 0, sizeof(SDL_GPUGraphicsPipelineCreateInfo));
	createInfo.target_info = targetInfo,
		createInfo.vertex_input_state = vertexInputState;
	createInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
	createInfo.vertex_shader = vertexShader;
	createInfo.fragment_shader = fragmentShader;

	SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(_mFixed.DeviceHandle, &createInfo);
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
	_mFixed.DeviceHandle = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL, true, NULL);

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

	Renderer_UpdateVsync();

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
	{
		SDL_GPUSamplerCreateInfo sampler0;
		Utils_memset(&sampler0, 0, sizeof(SDL_GPUSamplerCreateInfo));
		sampler0.min_filter = SDL_GPU_FILTER_NEAREST;
		sampler0.mag_filter = SDL_GPU_FILTER_NEAREST;
		sampler0.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
		sampler0.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		sampler0.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		sampler0.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		_mFixed.Samplers[0] = SDL_CreateGPUSampler(_mFixed.DeviceHandle, &sampler0);
	}
	// PointWrap
	{
		SDL_GPUSamplerCreateInfo sampler1;
		Utils_memset(&sampler1, 0, sizeof(SDL_GPUSamplerCreateInfo));
		sampler1.min_filter = SDL_GPU_FILTER_NEAREST;
		sampler1.mag_filter = SDL_GPU_FILTER_NEAREST;
		sampler1.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
		sampler1.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
		sampler1.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
		sampler1.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
		_mFixed.Samplers[1] = SDL_CreateGPUSampler(_mFixed.DeviceHandle, &sampler1);
	}
	// LinearClamp
	{
		SDL_GPUSamplerCreateInfo sampler2;
		Utils_memset(&sampler2, 0, sizeof(SDL_GPUSamplerCreateInfo));
		sampler2.min_filter = SDL_GPU_FILTER_LINEAR;
		sampler2.mag_filter = SDL_GPU_FILTER_LINEAR;
		sampler2.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
		sampler2.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		sampler2.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		sampler2.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		_mFixed.Samplers[2] = SDL_CreateGPUSampler(_mFixed.DeviceHandle, &sampler2);
	}
	// LinearWrap
	{
		SDL_GPUSamplerCreateInfo sampler3;
		Utils_memset(&sampler3, 0, sizeof(SDL_GPUSamplerCreateInfo));
		sampler3.min_filter = SDL_GPU_FILTER_LINEAR;
		sampler3.mag_filter = SDL_GPU_FILTER_LINEAR;
		sampler3.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
		sampler3.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
		sampler3.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
		sampler3.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
		_mFixed.Samplers[3] = SDL_CreateGPUSampler(_mFixed.DeviceHandle, &sampler3);
	}
	// AnisotropicClamp
	{
		SDL_GPUSamplerCreateInfo sampler4;
		Utils_memset(&sampler4, 0, sizeof(SDL_GPUSamplerCreateInfo));
		sampler4.min_filter = SDL_GPU_FILTER_LINEAR;
		sampler4.mag_filter = SDL_GPU_FILTER_LINEAR;
		sampler4.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
		sampler4.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		sampler4.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		sampler4.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
		sampler4.enable_anisotropy = true;
		sampler4.max_anisotropy = 4;
		_mFixed.Samplers[4] = SDL_CreateGPUSampler(_mFixed.DeviceHandle, &sampler4);
	}
	// AnisotropicWrap
	{
		SDL_GPUSamplerCreateInfo sampler5;
		Utils_memset(&sampler5, 0, sizeof(SDL_GPUSamplerCreateInfo));
		sampler5.min_filter = SDL_GPU_FILTER_LINEAR;
		sampler5.mag_filter = SDL_GPU_FILTER_LINEAR;
		sampler5.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
		sampler5.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
		sampler5.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
		sampler5.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
		sampler5.enable_anisotropy = true;
		sampler5.max_anisotropy = 4;
		_mFixed.Samplers[5] = SDL_CreateGPUSampler(_mFixed.DeviceHandle, &sampler5);
	}

	// Create the GPU resources
	{
		SDL_GPUBufferCreateInfo info;
		Utils_memset(&info, 0, sizeof(SDL_GPUBufferCreateInfo));
		info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
		info.size = sizeof(VertexPositionColorTexture) * MAX_VERTICES;
		_mFixed.VertexBuffer = SDL_CreateGPUBuffer(_mFixed.DeviceHandle, &info);
	}

	{
		SDL_GPUTransferBufferCreateInfo info;
		Utils_memset(&info, 0, sizeof(SDL_GPUTransferBufferCreateInfo));
		info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
		info.size = sizeof(VertexPositionColorTexture) * MAX_VERTICES;
		_mFixed.TransferBufferForVertexBuffer = SDL_CreateGPUTransferBuffer(_mFixed.DeviceHandle, &info);
	}

	{
		SDL_GPUBufferCreateInfo info;
		Utils_memset(&info, 0, sizeof(SDL_GPUBufferCreateInfo));
		info.usage = SDL_GPU_BUFFERUSAGE_INDEX;
		info.size = sizeof(Uint16) * MAX_INDICES;
		_mFixed.IndexBuffer = SDL_CreateGPUBuffer(_mFixed.DeviceHandle, &info);
	}

	//UPLOAD INDEX DATA
	{
		// Set up buffer data
		SDL_GPUTransferBuffer* bufferTransferBuffer = NULL;

		{
			SDL_GPUTransferBufferCreateInfo info;
			Utils_memset(&info, 0, sizeof(SDL_GPUTransferBufferCreateInfo));
			info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
			info.size = (sizeof(Uint16) * MAX_INDICES);
			bufferTransferBuffer = SDL_CreateGPUTransferBuffer(_mFixed.DeviceHandle, &info);
		}

		uint16_t* transferData = (uint16_t*)SDL_MapGPUTransferBuffer(_mFixed.DeviceHandle, bufferTransferBuffer, true);
		SDL_memcpy(transferData, _mFixed.IndexBufferData, sizeof(uint16_t) * MAX_INDICES);

		SDL_UnmapGPUTransferBuffer(_mFixed.DeviceHandle, bufferTransferBuffer);

		SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(_mFixed.DeviceHandle);
		SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

		{
			SDL_GPUTransferBufferLocation location;
			Utils_memset(&location, 0, sizeof(SDL_GPUTransferBufferLocation));
			location.transfer_buffer = bufferTransferBuffer;
			location.offset = 0;
			SDL_GPUBufferRegion region;
			Utils_memset(&region, 0, sizeof(SDL_GPUBufferRegion));
			region.buffer = _mFixed.IndexBuffer;
			region.offset = 0;
			region.size = sizeof(Uint16) * MAX_INDICES;
			SDL_UploadToGPUBuffer(copyPass, &location, &region, false);
		}

		SDL_EndGPUCopyPass(copyPass);
		SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
		SDL_ReleaseGPUTransferBuffer(_mFixed.DeviceHandle, bufferTransferBuffer);
	}
	//

#ifdef EDITOR_MODE
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForSDLGPU(_mFixed.WindowHandle);
	ImGui_ImplSDLGPU3_InitInfo init_info = {};
	init_info.Device = _mFixed.DeviceHandle;
	init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(_mFixed.DeviceHandle, _mFixed.WindowHandle);
	init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;                      // Only used in multi-viewports mode.
	init_info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;  // Only used in multi-viewports mode.
	init_info.PresentMode = SDL_GPU_PRESENTMODE_VSYNC;
	ImGui_ImplSDLGPU3_Init(&init_info);
#endif

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

	if (!SDL_WaitAndAcquireGPUSwapchainTexture(_mTemp.CommandRender, _mFixed.WindowHandle, &_mTemp.SwapchainTexture, NULL, NULL))
	{
		SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
		return;
	}

	SDL_GPUTexture* textureToRenderTo = NULL;
	SDL_GPUGraphicsPipeline* pipelineToUse = NULL;
	if (IsOffscreenTargetNeeded())
	{
		textureToRenderTo = (SDL_GPUTexture*)_mFixed.OffscreenTarget->mTextureData;
		pipelineToUse = _mFixed.PipelineForRTT;
	}
	else
	{
		textureToRenderTo = _mTemp.SwapchainTexture;
		pipelineToUse = _mFixed.PipelineForSwap;
	}

	{
		SDL_FColor clearColor;
		Utils_memset(&clearColor, 0, sizeof(SDL_FColor));
		clearColor.r = 0.0f;
		clearColor.g = 0.0f;
		clearColor.b = 0.0f;
		clearColor.a = 1.0f;

		SDL_GPUColorTargetInfo colorTargetInfo;
		Utils_memset(&colorTargetInfo, 0, sizeof(SDL_GPUColorTargetInfo));
		colorTargetInfo.texture = textureToRenderTo;
		colorTargetInfo.clear_color = clearColor;
		colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
		colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
		_mTemp.RenderPass = SDL_BeginGPURenderPass(_mTemp.CommandRender, &colorTargetInfo, 1, NULL);
	}

	SDL_BindGPUGraphicsPipeline(_mTemp.RenderPass, pipelineToUse);

	{
		SDL_GPUBufferBinding bufferBinding;
		Utils_memset(&bufferBinding, 0, sizeof(SDL_GPUBufferBinding));
		bufferBinding.buffer = _mFixed.IndexBuffer;
		bufferBinding.offset = 0;
		SDL_BindGPUIndexBuffer(_mTemp.RenderPass, &bufferBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
	}
}
void Renderer_BeforeCommit(void)
{
	{ //Push current camera positions...
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
	}

	{ //Bind index buffer
		SDL_GPUBufferBinding bufferBinding;
		Utils_memset(&bufferBinding, 0, sizeof(SDL_GPUBufferBinding));
		bufferBinding.buffer = _mFixed.IndexBuffer;
		bufferBinding.offset = 0;
		SDL_BindGPUIndexBuffer(_mTemp.RenderPass, &bufferBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
	}
}
static void SubmitRender(void)
{
	if ((_mTemp.CurrentBatchVerticesOffset == 0) ||
		(_mTemp.LastTextureUsed == NULL) ||
		(_mTemp.LastRenderVerticesOffset == _mTemp.CurrentBatchVerticesOffset))
	{
		return;
	}

	{
		SDL_GPUTextureSamplerBinding samplerBinding;
		Utils_memset(&samplerBinding, 0, sizeof(SDL_GPUTextureSamplerBinding));
		samplerBinding.texture = (SDL_GPUTexture*)_mTemp.LastTextureUsed->mTextureData;
		samplerBinding.sampler = _mFixed.Samplers[0];
		SDL_BindGPUFragmentSamplers(_mTemp.RenderPass, 0, &samplerBinding, 1);
	}

	{
		SDL_GPUBufferBinding bufferBinding;
		Utils_memset(&bufferBinding, 0, sizeof(SDL_GPUBufferBinding));
		bufferBinding.buffer = _mFixed.VertexBuffer;
		bufferBinding.offset = 0;
		SDL_BindGPUVertexBuffers(_mTemp.RenderPass, 0, &bufferBinding, 1);
	}

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
	VertexPositionColorTexture* transferData = (VertexPositionColorTexture*)SDL_MapGPUTransferBuffer(_mFixed.DeviceHandle,
		_mFixed.TransferBufferForVertexBuffer, false);
	SDL_memcpy(transferData, _mFixed.BatchVertexBuffer, sizeof(VertexPositionColorTexture) * _mTemp.CurrentBatchVerticesOffset);

	SDL_UnmapGPUTransferBuffer(_mFixed.DeviceHandle, _mFixed.TransferBufferForVertexBuffer);

	SDL_GPUTransferBufferLocation location;
	Utils_memset(&location, 0, sizeof(SDL_GPUTransferBufferLocation));
	location.transfer_buffer = _mFixed.TransferBufferForVertexBuffer;
	location.offset = 0;

	SDL_GPUBufferRegion region;
	Utils_memset(&region, 0, sizeof(SDL_GPUBufferRegion));
	region.buffer = _mFixed.VertexBuffer;
	region.offset = 0;
	region.size = sizeof(VertexPositionColorTexture) * _mTemp.CurrentBatchVerticesOffset;

	SDL_UploadToGPUBuffer(_mTemp.CopyPass, &location, &region, false);
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

	_mFixed.BatchVertexBuffer[_mTemp.CurrentBatchVerticesOffset + 0] =
	{
		sprite->Position0.X,  sprite->Position0.Y, sprite->Position0.Z,
		sprite->Color0.R, sprite->Color0.G, sprite->Color0.B, sprite->Color0.A,
		sprite->TextureCoordinate0.U, sprite->TextureCoordinate0.V
	};

	_mFixed.BatchVertexBuffer[_mTemp.CurrentBatchVerticesOffset + 1] =
	{
		sprite->Position1.X,  sprite->Position1.Y, sprite->Position1.Z,
		sprite->Color1.R, sprite->Color1.G, sprite->Color1.B, sprite->Color1.A,
		sprite->TextureCoordinate1.U, sprite->TextureCoordinate1.V
	};

	_mFixed.BatchVertexBuffer[_mTemp.CurrentBatchVerticesOffset + 2] =
	{
		sprite->Position2.X,  sprite->Position2.Y, sprite->Position2.Z,
		sprite->Color2.R, sprite->Color2.G, sprite->Color2.B, sprite->Color2.A,
		sprite->TextureCoordinate2.U, sprite->TextureCoordinate2.V
	};

	_mFixed.BatchVertexBuffer[_mTemp.CurrentBatchVerticesOffset + 3] =
	{
		sprite->Position3.X,  sprite->Position3.Y, sprite->Position3.Z,
		sprite->Color3.R, sprite->Color3.G, sprite->Color3.B, sprite->Color3.A,
		sprite->TextureCoordinate3.U, sprite->TextureCoordinate3.V
	};

	_mTemp.CurrentBatchVerticesOffset += VERTICES_IN_SPRITE;
}
void Renderer_AfterCommit(void)
{
}
void Renderer_AfterRender(void)
{
	FlushBatch();

	SDL_EndGPURenderPass(_mTemp.RenderPass);

	if (IsOffscreenTargetNeeded())
	{
		Rectangle source = _mFixed.OffscreenTarget->mBounds;
		Rectangle dest = Renderer_GetScreenBounds();

		SDL_GPUBlitInfo blitInfo;
		Utils_memset(&blitInfo, 0, sizeof(SDL_GPUBlitInfo));
		blitInfo.source.texture = (SDL_GPUTexture*)_mFixed.OffscreenTarget->mTextureData;
		blitInfo.source.x = source.X;
		blitInfo.source.y = source.Y;
		blitInfo.source.w = source.Width;
		blitInfo.source.h = source.Height;
		blitInfo.destination.texture = _mTemp.SwapchainTexture;
		blitInfo.destination.x = dest.X;
		blitInfo.destination.y = dest.Y;
		blitInfo.destination.w = dest.Width;
		blitInfo.destination.h = dest.Height;
		blitInfo.load_op = SDL_GPU_LOADOP_CLEAR;
		if (Rectangle_EqualTo(&source, &dest))
		{
			blitInfo.filter = SDL_GPU_FILTER_NEAREST;
		}
		else
		{
			if (Cvars_GetAsInt(CVARS_USER_IS_LINEAR_FILTER_ALLOWED))
			{
				blitInfo.filter = SDL_GPU_FILTER_LINEAR;
			}
			else
			{
				blitInfo.filter = SDL_GPU_FILTER_NEAREST;
			}
		}
		SDL_BlitGPUTexture(_mTemp.CommandRender, &blitInfo);
	}

	SDL_EndGPUCopyPass(_mTemp.CopyPass);

	SDL_SubmitGPUCommandBuffer(_mTemp.CommandUpload);

#ifdef EDITOR_MODE
	// Rendering
// (Your code clears your framebuffer, renders your other stuff etc.)
	ImGui::Render();

	ImDrawData* draw_data = ImGui::GetDrawData();
	ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, _mTemp.CommandRender);

	// Setup and start a render pass
	SDL_GPUColorTargetInfo target_info = {};
	target_info.texture = _mTemp.SwapchainTexture;
	target_info.load_op = SDL_GPU_LOADOP_LOAD;
	target_info.store_op = SDL_GPU_STOREOP_STORE;
	target_info.mip_level = 0;
	target_info.layer_or_depth_plane = 0;
	target_info.cycle = false;
	SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(_mTemp.CommandRender, &target_info, 1, nullptr);

	// Render ImGui
	ImGui_ImplSDLGPU3_RenderDrawData(draw_data, _mTemp.CommandRender, render_pass);

	SDL_EndGPURenderPass(render_pass);
#endif

	SDL_SubmitGPUCommandBuffer(_mTemp.CommandRender);

	Utils_memset(&_mTemp, 0, sizeof(TempRenderState));
}
void Renderer_FlushBatch(void)
{
}
void Renderer_EnableDepthBufferWrite(void)
{
	//TODO?
}
void Renderer_DisableDepthBufferWrite(void)
{
	//TODO?
}
void Renderer_UpdateVsync(void)
{
	bool useVsync = Renderer_IsVsync();
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

	SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;

	if (!useVsync)
	{
		if (SDL_WindowSupportsGPUPresentMode(_mFixed.DeviceHandle, _mFixed.WindowHandle, SDL_GPU_PRESENTMODE_IMMEDIATE))
		{
			presentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;
		}
		else if (SDL_WindowSupportsGPUPresentMode(_mFixed.DeviceHandle, _mFixed.WindowHandle, SDL_GPU_PRESENTMODE_MAILBOX))
		{
			presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
		}
	}

	SDL_SetGPUSwapchainParameters(_mFixed.DeviceHandle, _mFixed.WindowHandle, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode);
}
void Renderer_ApplyChanges(void)
{
	Renderer_ResetBackBuffer();
	Renderer_UpdateViewport();
	Renderer_UpdateScissor();
}
void Renderer_UpdateViewport(void)
{
}
void Renderer_UpdateScissor(void)
{
}
void Renderer_ResetBackBuffer(void)
{
	Rectangle wantedBackBufferBounds = Renderer_GetWantedBackBufferBounds();

	Rectangle virtualBufferBounds = Rectangle_Empty;
	Rectangle actualBufferBounds = Rectangle_Empty;

	virtualBufferBounds.Width = wantedBackBufferBounds.Width;
	virtualBufferBounds.Height = wantedBackBufferBounds.Height;

	if (IsOffscreenTargetNeeded())
	{
		actualBufferBounds = Renderer_GetDrawableSize();
	}
	else
	{
		actualBufferBounds = virtualBufferBounds;
	}

	Logger_LogInformation("Back Buffer has been reset");
	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "Actual buffer bounds: ");
		MString_AddAssignInt(&tempString, actualBufferBounds.Width);
		MString_AddAssignString(&tempString, "x");
		MString_AddAssignInt(&tempString, actualBufferBounds.Height);
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}

	if (IsOffscreenTargetNeeded())
	{
		{
			MString* tempString = NULL;
			MString_AssignString(&tempString, "Virtual buffer bounds: ");
			MString_AddAssignInt(&tempString, virtualBufferBounds.Width);
			MString_AddAssignString(&tempString, "x");
			MString_AddAssignInt(&tempString, virtualBufferBounds.Height);
			Logger_LogInformation(MString_Text(tempString));
			MString_Dispose(&tempString);
		}

		_mFixed.OffscreenTarget = (Texture*)Utils_calloc(1, sizeof(Texture));
		MString_AssignString(&_mFixed.OffscreenTarget->mPath, "");
		_mFixed.OffscreenTarget->mBounds.Width = virtualBufferBounds.Width;
		_mFixed.OffscreenTarget->mBounds.Height = virtualBufferBounds.Height;

		SDL_GPUTextureCreateInfo info;
		Utils_memset(&info, 0, sizeof(SDL_GPUTextureCreateInfo));
		info.type = SDL_GPU_TEXTURETYPE_2D;
		info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
		info.width = virtualBufferBounds.Width;
		info.height = virtualBufferBounds.Height;
		info.layer_count_or_depth = 1;
		info.num_levels = 1;
		info.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
		SDL_PropertiesID temp = SDL_CreateProperties();
		SDL_SetFloatProperty(temp, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_R_FLOAT, 0.0f);
		SDL_SetFloatProperty(temp, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_G_FLOAT, 0.0f);
		SDL_SetFloatProperty(temp, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_B_FLOAT, 0.0f);
		SDL_SetFloatProperty(temp, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_A_FLOAT, 1.0f);
		info.props = temp;
		_mFixed.OffscreenTarget->mTextureData = SDL_CreateGPUTexture(_mFixed.DeviceHandle, &info);
	}
}
Rectangle Renderer_GetDrawableSize(void)
{
	Rectangle drawableSize = Rectangle_Empty;
	SDL_GetWindowSizeInPixels(_mFixed.WindowHandle, &drawableSize.Width, &drawableSize.Height);
	return drawableSize;
}

#endif

typedef int compiler_warning_compliance;