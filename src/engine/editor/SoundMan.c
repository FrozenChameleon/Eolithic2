#include "SoundMan.h"

#include "imgui.h"
#include "../utils/Macros.h"
#include "../audio/VolumeTool.h"
#include "../audio/SoundEffect.h"
#include "../audio/Music.h"
#include "../utils/IStringArray.h"
#include "../resources/ResourceList.h"
#include "../utils/Utils.h"
#include "../core/Func.h"

static bool _mIsActive;
static char _mStrSoundEffect[EE_FILENAME_MAX];
static char _mStrMusic[EE_FILENAME_MAX];

static void HandleSoundEffect()
{
	ResourceMan* rmSoundEffect = ResourceList_SoundEffect();
	IStringArray* soundEffectFilenames = IStringArray_CreateForJustThisFrame();
	ResourceMan_FillArrayWithAllResourceNames(rmSoundEffect, soundEffectFilenames);

	if (ImGui::BeginListBox("Sound Effects", ImVec2(400, 600)))
	{
		for (int i = 0; i < IStringArray_Length(soundEffectFilenames); i += 1)
		{
			const char* filename = IStringArray_Get(soundEffectFilenames, i);
			bool isSelected = Utils_StringEqualTo(filename, _mStrSoundEffect);
			if (ImGui::Selectable(filename, isSelected))
			{
				Resource* resource = ResourceMan_GetResourceByIndex(rmSoundEffect, i);
				Utils_strlcpy(_mStrSoundEffect, Resource_GetName(resource), EE_FILENAME_MAX);
				Do_PlaySound(_mStrSoundEffect);
			}
		}
		ImGui::EndListBox();
	}

	if (Utils_IsStringEmpty(_mStrSoundEffect))
	{
		return;
	}

	if (ImGui::Button("Play Sound", ImVec2(250, 25)))
	{
		Do_PlaySound(_mStrSoundEffect);
	}
	int currentVolume = VolumeTool_GetVolume(_mStrSoundEffect, false);
	if (ImGui::InputInt("SFX Volume", &currentVolume, 5))
	{
		VolumeTool_SetVolume(_mStrSoundEffect, currentVolume, false);
	}
	if (ImGui::Button("Stop All Sounds", ImVec2(250, 25)))
	{
		Do_StopAllSounds();
	}
	if (ImGui::Button("Reload Sound Effects", ImVec2(250, 25)))
	{
		//TODO OeResourceManagers.SoundEffectManager.Dispose();
	}
	if (ImGui::Button("Save SFX Volume", ImVec2(250, 25)))
	{
		VolumeTool_Save(false);
		Do_PlaySound("editorSave");
	}
}
static void HandleMusic()
{
	ResourceMan* rmMusic = ResourceList_Music();
	IStringArray* musicFilenames = IStringArray_CreateForJustThisFrame();
	ResourceMan_FillArrayWithAllResourceNames(rmMusic, musicFilenames);

	if (ImGui::BeginListBox("Music", ImVec2(400, 600)))
	{
		for (int i = 0; i < IStringArray_Length(musicFilenames); i += 1)
		{
			const char* filename = IStringArray_Get(musicFilenames, i);
			bool isSelected = Utils_StringEqualTo(filename, _mStrMusic);
			if (ImGui::Selectable(filename, isSelected))
			{
				Resource* resource = ResourceMan_GetResourceByIndex(rmMusic, i);
				Utils_strlcpy(_mStrMusic, Resource_GetName(resource), EE_FILENAME_MAX);
				Do_PlayMusic2(_mStrMusic, true, true);
			}
		}
		ImGui::EndListBox();
	}

	if (Utils_IsStringEmpty(_mStrMusic))
	{
		return;
	}

	if (ImGui::Button("Play Music", ImVec2(250, 25)))
	{
		Do_PlayMusic2(_mStrMusic, true, true);
	}
	int tempVolumeMusic = VolumeTool_GetVolume(_mStrMusic, true);
	if (ImGui::InputInt("Music Volume", &tempVolumeMusic, 5))
	{
		VolumeTool_SetVolume(_mStrMusic, tempVolumeMusic, true);
	}
	if (ImGui::Button("Stop Music", ImVec2(250, 25)))
	{
		Do_StopMusic();
	}
	if (ImGui::Button("Reload Music", ImVec2(250, 25)))
	{
		//TODO OeResourceManagers.MusicManager.Dispose();
	}
	if (ImGui::Button("Save Music Volume", ImVec2(250, 25)))
	{
		VolumeTool_Save(true);
		Do_PlaySound("editorSave");
	}
}
static void CreateSoundGuyWindow()
{
	if (!ImGui::Begin(KEY_WINDOW_SOUND_GUY, &_mIsActive))
	{
		ImGui::End();
		return;
	}

	ImGui::Columns(2);

	HandleSoundEffect();

	ImGui::NextColumn();

	HandleMusic();

	ImGui::End();
}

void SoundMan_Update()
{
	if (!_mIsActive)
	{
		return;
	}

	CreateSoundGuyWindow();
}
void SoundMan_Activate()
{
	_mIsActive = true;
}
