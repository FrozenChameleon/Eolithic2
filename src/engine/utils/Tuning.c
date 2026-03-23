#include "Tuning.h"

#include "../../third_party/stb_ds.h"
#include "../io/DynamicByteBuffer.h"
#include "Utils.h"
#include "../io/BufferReader.h"
#include "Logger.h"
#include "../resources/ResourceMan.h"
#include "../resources/ResourceList.h"
#include "../io/File.h"

//public static readonly string TUNINGS_DIRECTORY = OeFile.Combine("data", "tunings");
#define DIFFICULTY_GLOBAL 777
#define SYSTEM777 "SYSTEM777"
#define GLOBAL777 "GLOBAL777"

static int _mCurrentDifficulty;

void Tuning_Init(Tuning* tuning)
{
	sh_new_arena(tuning->mDictionary);
}

Tuning* Tuning_CreateNewInstance()
{
	return Tuning_CreateNew();
}
void Tuning_Write(Tuning* tuning, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_SetIsWritingText(dbb, true);

	for (int32_t i = 0; i < shlen(tuning->mDictionary); i += 1)
	{
		const char* name = tuning->mDictionary[i].key;
		DynamicByteBuffer_WriteString(dbb, name, Utils_strlen(name));
		DynamicByteBuffer_WriteFloat(dbb, tuning->mDictionary[i].value);
		DynamicByteBuffer_WriteNewline(dbb);
	}
}
void Tuning_Read(Tuning* tuning, BufferReader* br)
{
	BufferReader_SetIsReadingText(br, true);

	while (BufferReader_HasNext(br))
	{
		MString* key = MString_CreateForJustThisFrame();
		BufferReader_ReadMString(&key, br);
		float value = BufferReader_ReadFloat(br);
		shput(tuning->mDictionary, MString_Text(key), value);
	}
}
int Tuning_GetCurrentDifficulty()
{
	return _mCurrentDifficulty;
}
void Tuning_SetCurrentDifficulty(int value)
{
	_mCurrentDifficulty = value;
}
int Tuning_GetHealth(const char* thingName)
{
	return (int)Tuning_GetHelper(thingName, "[INT][HEALTH]");
}
int Tuning_GetScoreOnes(const char* thingName)
{
	return (int)Tuning_GetHelper2(thingName, "[INT][SCORE_ONES]", 0);
}
int Tuning_GetScoreFives(const char* thingName)
{
	return (int)Tuning_GetHelper2(thingName, "[INT][SCORE_FIVES]", 0);
}
int Tuning_GetScoreTens(const char* thingName)
{
	return (int)Tuning_GetHelper2(thingName, "[INT][SCORE_TENS]", 0);
}
int Tuning_GetScoreTwenties(const char* thingName)
{
	return (int)Tuning_GetHelper2(thingName, "[INT][SCORE_TWENTIES]", 0);
}
int Tuning_GetScoreFifties(const char* thingName)
{
	return (int)Tuning_GetHelper2(thingName, "[INT][SCORE_FIFTIES]", 0);
}
int Tuning_GetScoreHundreds(const char* thingName)
{
	return (int)Tuning_GetHelper2(thingName, "[INT][SCORE_HUNDREDS]", 0);
}
int Tuning_GetScoreBase(const char* thingName)
{
	return (int)Tuning_GetHelper(thingName, "[INT][SCORE_BASE]");
}
float Tuning_GetTuning(const char* thingName, const char* dataName)
{
	return Tuning_GetHelper(thingName, dataName);
}
float Tuning_GetSystem777Tuning(const char* dataName)
{
	return Tuning_GetHelper(SYSTEM777, dataName);
}
int Tuning_GetSystem777TuningAsInt(const char* dataName)
{
	return (int)Tuning_GetSystem777Tuning(dataName);
}
float Tuning_GetGlobal777Tuning(const char* dataName)
{
	return Tuning_GetHelper3(DIFFICULTY_GLOBAL, GLOBAL777, dataName, 0);
}
int Tuning_GetGlobal777TuningAsInt(const char* dataName)
{
	return (int)Tuning_GetGlobal777Tuning(dataName);
}
float Tuning_GetGlobalTuning(const char* thingName, const char* dataName)
{
	return Tuning_GetHelper3(DIFFICULTY_GLOBAL, thingName, dataName, 0);
}
int Tuning_GetGlobalTuningAsInt(const char* thingName, const char* dataName)
{
	return (int)Tuning_GetGlobalTuning(thingName, dataName);
}
int Tuning_GetTuningAsInt(const char* thingName, const char* dataName)
{
	return (int)Tuning_GetHelper(thingName, dataName);
}
float Tuning_GetHelper(const char* thingName, const char* dataName)
{
	return Tuning_GetHelper2(thingName, dataName, 0);
}
float Tuning_GetHelper2(const char* thingName, const char* dataName, int defaultValue)
{
	return Tuning_GetHelper3(Tuning_GetCurrentDifficulty(), thingName, dataName, defaultValue);
}
float Tuning_GetHelper3(int difficulty, const char* thingName, const char* dataName, int defaultValue)
{
	ResourceMan* resourceManager = ResourceList_Tuning(difficulty);
	if (ResourceMan_HasResource(resourceManager, thingName))
	{
		Resource* tuningResource = ResourceMan_GetResource(resourceManager, thingName);
		Tuning* tuningData = (Tuning*)Resource_GetData(tuningResource);
		int64_t lookupIndex = shgeti(tuningData->mDictionary, dataName);
		if (lookupIndex >= 0)
		{
			return tuningData->mDictionary[lookupIndex].value;
		}
		else
		{
			shput(tuningData->mDictionary, dataName, defaultValue);
#if EDITOR_MODE
			Resource_Save(tuningResource, true);
#endif
			return defaultValue;
		}
	}
#if EDITOR_MODE
	MString* tuningFileName = MString_CreateForJustThisFrame();
	MString_AssignString(&tuningFileName, thingName);
	MString_AddAssignString(&tuningFileName, ".txt");

	MString* difficultyStr = MString_CreateForJustThisFrame();
	MString_AddAssignInt(&difficultyStr, difficulty);

	MString* newTuningFilePath = MString_CreateForJustThisFrame();
	File_PathCombine4(&newTuningFilePath, "data", "tunings", MString_Text(difficultyStr), MString_Text(tuningFileName));

	Tuning* newTuning = Tuning_CreateNew();
	Resource* newTuningResource = ResourceMan_CreateResource(resourceManager, thingName, MString_Text(newTuningFilePath), true);
	Resource_Save(newTuningResource, true);

	return Tuning_GetHelper3(difficulty, thingName, dataName, defaultValue);
#else
	return 0;
#endif
}
Tuning* Tuning_CreateNew()
{
	Tuning* tuning = (Tuning*)Utils_calloc(1, sizeof(Tuning));
	Tuning_Init(tuning);
	return tuning;
}
void Tuning_CopyTo(Tuning* dst, Tuning* src)
{
	shfree(dst->mDictionary);
	sh_new_arena(dst->mDictionary);
	for (int32_t i = 0; i < shlen(src->mDictionary); i += 1)
	{
		shput(dst->mDictionary, src->mDictionary[i].key, src->mDictionary[i].value);
	}
}
void Tuning_Dispose(Tuning* tuning)
{
	shfree(tuning->mDictionary);
	tuning->mDictionary = NULL;
	free(tuning);
}
Tuning* Tuning_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	Tuning* tuning = Tuning_CreateNew();
	Tuning_Read(tuning, br);
	return tuning;
}
