#include "ParticleMan.h"

#include "../leveldata/Particle.h"
#include "../leveldata/ParticleInstance.h"
#include "../leveldata/ParticleInstanceSys.h"
#include "../render/SpriteBatch.h"
#include "../utils/Utils.h"
#include "../../third_party/stb_ds.h"
#include "../render/DrawTool.h"
#include "../math/Rectangle.h"
#include "imgui.h"
#include "EditorGlobals.h"
#include "../math/Math.h"
#include "../utils/Logger.h"
#include "../io/File.h"
#include "../resources/ResourceList.h"
#include "../input/Input.h"
#include "../core/Func.h"
#include "../render/Renderer.h"

#define PREVIEW_WIDTH 1280
#define PREVIEW_HEIGHT 720

typedef struct PreviewParticles
{
	ParticleInstance* arr_particles;
	int mCounterInterval;
	bool mStopped;
} PreviewParticles;

static bool _mIsActive;
static char _mTempFileName[EE_FILENAME_MAX];
static Particle _mParticleData;
static PreviewParticles _mPreview;

static void PreviewParticles_Clear()
{
	arrsetlen(_mPreview.arr_particles, 0);
}
static void PreviewParticles_Init()
{
	Utils_memset(&_mPreview, 0, sizeof(PreviewParticles));
	_mPreview.mStopped = true;
}
static void PreviewParticles_Update()
{
	if (_mPreview.mStopped)
	{
		return;
	}

	_mPreview.mCounterInterval += 1;

	if (_mPreview.mCounterInterval >= _mParticleData.mPreviewInterval)
	{
		if (!Utils_StringEqualTo(_mParticleData.mTextureName, EE_STR_EMPTY))
		{
			for (int i = 0; i < _mParticleData.mPreviewAmount; i += 1)
			{
				ParticleInstance instance = { 0 };
				ParticleInstanceSys_Setup(&instance, _mTempFileName, &_mParticleData, PREVIEW_WIDTH / 2, PREVIEW_HEIGHT / 2);
				arrput(_mPreview.arr_particles, instance);
			}
			_mPreview.mCounterInterval = 0;
		}
	}

	for (int i = 0; i < arrlen(_mPreview.arr_particles); i += 1)
	{
		if (!_mPreview.arr_particles[i].mIsComplete)
		{
			ParticleInstanceSys_UpdateLastRenderPosition(&_mPreview.arr_particles[i]);
			ParticleInstanceSys_UpdateRoutine(&_mPreview.arr_particles[i]);
		}
	}
}
static void PreviewParticles_Stop()
{
	PreviewParticles_Clear();
	_mPreview.mStopped = true;
}
static void PreviewParticles_StartReset()
{
	PreviewParticles_Clear();

	_mPreview.mCounterInterval = _mParticleData.mPreviewInterval;

	_mPreview.mStopped = false;
}
static void PreviewParticles_DrawHud(SpriteBatch* spriteBatch)
{
	DrawTool_DrawRectangle2(spriteBatch, COLOR_CORNFLOWER_BLUE, 100, Rectangle_Create(0, 0, PREVIEW_WIDTH, PREVIEW_HEIGHT), 0, false);

	for (int i = 0; i < arrlen(_mPreview.arr_particles); i += 1)
	{
		if (!_mPreview.arr_particles[i].mIsComplete)
		{
			_mPreview.arr_particles[i].mInfluencedDepth = 100;
			ParticleInstanceSys_DrawRoutine(&_mPreview.arr_particles[i], spriteBatch);
		}
	}
}

static void ParticleKingSave()
{
	if (!ImGui::IsWindowFocused())
	{
		return;
	}

	ResourceMan_CopyToResourceDataAndThenSaveAsText(ResourceList_Particle(), _mTempFileName, NULL, &_mParticleData);
}
static void SetupPreview()
{
	ImGui::InputInt("Interval", &_mParticleData.mPreviewInterval);
	ImGui::InputInt("Amount", &_mParticleData.mPreviewAmount);

	if (ImGui::Button("Start / Reset", ImVec2(125, 25)))
	{
		PreviewParticles_StartReset();
	}

	if (ImGui::Button("Stop", ImVec2(125, 25)))
	{
		PreviewParticles_Stop();
	}
}
static void SetupCurve()
{
	ImGui::Text("Curve");

	ImGui::InputFloat("Total Min X##Curve", &_mParticleData.mCurveTotalMinimum.X);
	ImGui::InputFloat("Total Max X##Curve", &_mParticleData.mCurveTotalMaximum.X);

	ImGui::InputFloat("Min Increase Rate X##Curve", &_mParticleData.mCurveMinimumIncreaseRate.X);
	ImGui::InputFloat("Max Increase Rate X##Curve", &_mParticleData.mCurveMaximumIncreaseRate.X);

	ImGui::InputFloat("Total Min Y##Curve", &_mParticleData.mCurveTotalMinimum.Y);
	ImGui::InputFloat("Total Max Y##Curve", &_mParticleData.mCurveTotalMaximum.Y);

	ImGui::InputFloat("Min Increase Rate Y##Curve", &_mParticleData.mCurveMinimumIncreaseRate.Y);
	ImGui::InputFloat("Max Increase Rate Y##Curve", &_mParticleData.mCurveMaximumIncreaseRate.Y);

	ImGui::Checkbox("-Const X?    ##Curve", &_mParticleData.mCurveIsOppositeX);
	ImGui::SameLine();
	ImGui::Checkbox("-Const Y?##Curve", &_mParticleData.mCurveIsOppositeY);
	ImGui::Checkbox("Rand. Sign X?##Curve", &_mParticleData.mCurveIsNegativeX);
	ImGui::SameLine();
	ImGui::Checkbox("Rand. Sign Y?##Curve", &_mParticleData.mCurveIsNegativeY);

	ImGui::Text("Curve Fluctuate");

	ImGui::Checkbox("Fluctuates?##CurveFlucatuate", &_mParticleData.mCurveIsFluctuating);
	ImGui::Checkbox("Reset Rate?  ##CurveFlucatuate", &_mParticleData.mCurveFluctuateResetRate);
	ImGui::SameLine();
	ImGui::Checkbox("Reset Total?##CurveFlucatuate", &_mParticleData.mCurveFluctuateResetTotal);
	ImGui::Checkbox("Keep Speed?##CurveFlucatuate", &_mParticleData.mCurveFluctuateKeepSpeed);

	ImGui::InputInt("Intervl Min##CurveFlucatuate", &_mParticleData.mCurveFluctuateIntervalMin);
	ImGui::InputInt("Intervl Max##CurveFlucatuate", &_mParticleData.mCurveFluctuateIntervalMax);
}
static void SetupConstant()
{
	ImGui::Text("Constant");
	ImGui::InputFloat("Min X##Constant", &_mParticleData.mConstantMinimum.X);
	ImGui::InputFloat("Max X##Constant", &_mParticleData.mConstantMaximum.X);
	ImGui::InputFloat("Min Y##Constant", &_mParticleData.mConstantMinimum.Y);
	ImGui::InputFloat("Max Y##Constant", &_mParticleData.mConstantMaximum.Y);
	ImGui::Checkbox("Rand. Sign X?##Constant", &_mParticleData.mConstantIsNegativeX);
	ImGui::SameLine();
	ImGui::Checkbox("Rand. Sign Y?##Constant", &_mParticleData.mConstantIsNegativeY);
	ImGui::Text("Constant Fluctuate");
	ImGui::Checkbox("Fluctuates?##ConstantFluctuate", &_mParticleData.mConstantIsFluctuating);
	ImGui::InputInt("Intervl Min##ConstantFluctuate", &_mParticleData.mConstantFluctuateIntervalMin);
	ImGui::InputInt("Intervl Max##ConstantFluctuate", &_mParticleData.mConstantFluctuateIntervalMax);
}
static void SetupOffset()
{
	ImGui::Text("Offset");

	ImGui::InputFloat("Min X##Offset", &_mParticleData.mOffsetMinimum.X);
	ImGui::InputFloat("Max X##Offset", &_mParticleData.mOffsetMaximum.X);
	ImGui::InputFloat("Min Y##Offset", &_mParticleData.mOffsetMinimum.Y);
	ImGui::InputFloat("Max Y##Offset", &_mParticleData.mOffsetMaximum.Y);

	ImGui::Checkbox("Rand. Sign X?##Offset", &_mParticleData.mOffsetIsNegativeX);
	ImGui::SameLine();
	ImGui::Checkbox("Rand. Sign Y?##Offset", &_mParticleData.mOffsetIsNegativeY);
}
static void SetupFlicker()
{
	int flickerX = 480;
	int flickerY = 180;

	ImGui::Checkbox("Flicker?", &_mParticleData.mFlickerIsFlicker);

	int minStartY = flickerY + 40;
	int maxStartY = minStartY + 40;
	int startX = flickerX + 75;
	int speedX = startX + 75;

	ImGui::Text("Flicker");

	//Start
	ImGui::InputInt("Min Start##Flicker", &_mParticleData.mFlickerMinimumStart);
	ImGui::InputInt("Min Speed##Flicker", &_mParticleData.mFlickerMinimumSpeed);
	ImGui::InputInt("Max Start##Flicker", &_mParticleData.mFlickerMaximumStart);
	ImGui::InputInt("Max Speed##Flicker", &_mParticleData.mFlickerMaximumSpeed);
	//
}
static void SetupMisc()
{
	if (ImGui::InputText("Name", _mTempFileName, EE_FILENAME_MAX))
	{
		Utils_StringToUpper(_mTempFileName, EE_FILENAME_MAX);
	}

	if (ImGui::Button("Save (Ctrl-S)", ImVec2(125, 25)))
	{
		ParticleKingSave();
	}
	if (EditorGlobals_IsKeyCtrlPressedAndKeySTapped())
	{
		ParticleKingSave();
	}

	if (ImGui::Button("Setup New", ImVec2(100, 25)))
	{
		Logger_Log(LOGGER_INFORMATION, "New particle setup");
		Utils_memset(&_mParticleData, 0, sizeof(Particle));
		Utils_strlcpy(_mTempFileName, EE_STR_NOT_SET, EE_FILENAME_MAX);
	}

	IStringArray* listOfSheetNames = IStringArray_CreateForJustThisFrame();
	if (ImGui::BeginCombo("Texture", _mParticleData.mTextureName))
	{
		for (int i = 0; i < IStringArray_Length(listOfSheetNames); i += 1)
		{
			const char* sheetName = IStringArray_Get(listOfSheetNames, i);
			bool isSelected = Utils_StringEqualTo(sheetName, _mParticleData.mTextureName);
			if (ImGui::Selectable(sheetName, isSelected))
			{
				Utils_strlcpy(_mParticleData.mTextureName, sheetName, EE_FILENAME_MAX);
				PreviewParticles_Clear();
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Checkbox("Is Animation?", &_mParticleData.mTextureIsAnimation))
	{
		PreviewParticles_Clear();
	}

	if (ImGui::InputInt("Flip Speed", &_mParticleData.mTextureFlipSpeed))
	{
		PreviewParticles_Clear();
	}

	ImGui::InputInt("Loop Point", &_mParticleData.mTextureLoopPoint);

	ImGui::Checkbox("Animation Stop?", &_mParticleData.mTextureDoesAnimationStop);

	ImGui::InputInt("Depth", &_mParticleData.mTextureDepth);

	if (ImGui::InputInt("Scaler", &_mParticleData.mScaler))
	{
		_mParticleData.mScaler = Math_MaxInt(_mParticleData.mScaler, 1);
	}

	ImGui::Checkbox("Is Blend State Additive?", &_mParticleData.mIsBlendStateAdditive);

	ImGui::InputInt("Extra Passes", &_mParticleData.mExtraPasses);
	if (ImGui::InputInt("TTL Min", &_mParticleData.mMiscTTLMin))
	{
		PreviewParticles_Clear();
	}
	if (ImGui::InputInt("TTL Max", &_mParticleData.mMiscTTLMax))
	{
		PreviewParticles_Clear();
	}

	Sheet* currentSheet = Sheet_GetSheet(_mParticleData.mTextureName);
	Texture* currentTexture = Sheet_GetTexture(currentSheet);
	ImGui::Image((ImTextureID)(intptr_t)currentTexture->mTextureData, ImVec2(currentTexture->mBounds.Width, currentTexture->mBounds.Height));
}

static void CreateParticleKing()
{
	//ImGui::SetNextWindowSize(ImVec2(1600, 800));
	if (!ImGui::Begin(KEY_WINDOW_PARTICLE_KING, &_mIsActive))
	{
		ImGui::End();
		return;
	}

	ImGui::Columns(3);

	ResourceMan* rmParticle = ResourceList_Particle();
	IStringArray* filenames = IStringArray_CreateForJustThisFrame();
	ResourceMan_FillArrayWithAllResourceNames(rmParticle, filenames);
	if (ImGui::BeginListBox("##ParticlesList"))
	{
		for (int i = 0; i < IStringArray_Length(filenames); i += 1)
		{
			const char* filename = IStringArray_Get(filenames, i);
			bool isSelected = Utils_StringEqualTo(filename, _mTempFileName);
			if (ImGui::Selectable(filename, isSelected))
			{
				PreviewParticles_Stop();
				Resource* resource = ResourceMan_GetResourceByIndex(rmParticle, i);
				Utils_strlcpy(_mTempFileName, filename, EE_FILENAME_MAX);
				Particle* resourceData = (Particle*)Resource_GetData(resource);
				_mParticleData = *resourceData;
			}
	
		}
		ImGui::EndListBox();
	}

	SetupMisc();

	ImGui::NextColumn();

	SetupOffset();

	SetupFlicker();

	SetupConstant();

	SetupCurve();

	ImGui::NextColumn();

	SetupPreview();

	PreviewParticles_Update();

	Texture* tex = Renderer_RenderToTexture(PREVIEW_WIDTH, PREVIEW_HEIGHT, COLOR_CORNFLOWER_BLUE, PreviewParticles_DrawHud);
	ImGui::Image((ImTextureID)(intptr_t)tex->mTextureData, ImVec2(PREVIEW_WIDTH, PREVIEW_HEIGHT));

	ImGui::End();
}

void ParticleMan_Update()
{
	if (!_mIsActive)
	{
		return;
	}

	CreateParticleKing();
}
void ParticleMan_Activate()
{
	_mIsActive = true;
}
