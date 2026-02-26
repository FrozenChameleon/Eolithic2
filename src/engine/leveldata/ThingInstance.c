/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ThingInstance.h"

#include "../utils/Macros.h"
#include "../utils/Utils.h"
#include "../io/BufferReader.h"
#include "../render/SpriteBatch.h"
#include "../math/Points.h"
#include "../utils/Logger.h"
#include "../utils/Cvars.h"
#include "../core/Game.h"
#include "../core/GameHelper.h"
#include "../resources/ResourceManagerList.h"
#include "../../third_party/stb_ds.h"
#include "../render/DrawTool.h"
#include "ThingSettings.h"
#include "../render/Sheet.h"
#include "../render/Color.h"
#include "../globals/Align.h"

//static std_vector<OeStringPair> _mPairs;

static void BuildGlobalThingSettings(void)
{
	/*_mPairs = {}; //UNUSED
	_mPairs.push_back(OeStringPair(ThingInstance_SETTING_DTN_OFFSET_X, "0"));
	_mPairs.push_back(OeStringPair(ThingInstance_SETTING_DTN_OFFSET_Y, "0"));
	if (OeCvars_GetAsInt(OeCvars_ENGINE_NUMBER_OF_DIFFICULTIES) > 1)
	{
		_mPairs.push_back(OeStringPair("BLN_DIFFICULTY_EASY", "T"));
		_mPairs.push_back(OeStringPair("BLN_DIFFICULTY_NORMAL", "T"));
		if (OeCvars_GetAsInt(OeCvars_ENGINE_NUMBER_OF_DIFFICULTIES) > 2)
		{
			_mPairs.push_back(OeStringPair("BLN_DIFFICULTY_HARD", "T"));
		}
		if (OeCvars_GetAsInt(OeCvars_ENGINE_NUMBER_OF_DIFFICULTIES) > 3)
		{
			_mPairs.push_back(OeStringPair("BLN_DIFFICULTY_VERY_HARD", "T"));
		}
	}
	OeGameHelper_AddDefaultThingSettings(_mPairs);*/
}
/*
static std_vector<OeStringPair>& GetGlobalThingSettings() //UNUSED
{
	if (_mPairs.size() == 0)
	{
		BuildGlobalThingSettings();
	}
	return _mPairs;
}
*/

void ThingInstance_Init(ThingInstance* ti)
{
	Utils_memset(ti, 0, sizeof(ThingInstance));
	Utils_strlcpy(ti->mName, EE_STR_EMPTY, EE_FILENAME_MAX);
}

StringPair ThingInstance_GetSetting(ThingInstance* ti, const char* key)
{
	int64_t len = arrlen(ti->arr_settings);
	for (int32_t i = 0; i < len; i += 1)
	{
		StringPair pair = ti->arr_settings[i];
		if (Utils_StringEqualTo(pair.mKey, key))
		{
			return pair;
		}
	}
	return StringPair_Empty;
}
void ThingInstance_SetSetting(ThingInstance* ti, const char* key, const char* value)
{
	/*for (int32_t i = 0; i < mSettings.size(); i += 1) //UNUSED
	{
		OeStringPair* pair = &mSettings[i];
		if (pair->mKey == key)
		{
			pair->mValue = value;
		}
	}*/
}
void ThingInstance_Write(ThingInstance* ti, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteString(dbb, ti->mName, EE_FILENAME_MAX);

	DynamicByteBuffer_WriteNewline(dbb);

	DynamicByteBuffer_WriteI32(dbb, (int32_t)arrlen(ti->arr_nodes));
	for (int32_t i = 0; i < arrlen(ti->arr_nodes); i += 1)
	{
		DynamicByteBuffer_WriteNewline(dbb);
		Points_Write(&ti->arr_nodes[i], dbb);
	}

	DynamicByteBuffer_WriteNewline(dbb);

	DynamicByteBuffer_WriteI32(dbb, (int32_t)arrlen(ti->arr_settings));
	for (int32_t i = 0; i < arrlen(ti->arr_settings); i += 1)
	{
		DynamicByteBuffer_WriteNewline(dbb);
		StringPair_Write(&ti->arr_settings[i], dbb);
	}
}
void ThingInstance_Read(int32_t version, ThingInstance* ti, BufferReader* reader)
{
	//BufferReader_ReadI32(reader);//UNUSED

	BufferReader_ReadString(reader, ti->mName, EE_FILENAME_MAX);

	//BufferReader_ReadFloat(reader);//UNUSED
	//BufferReader_ReadFloat(reader);//UNUSED

	int32_t sizeNode = BufferReader_ReadI32(reader);
	for (int32_t i = 0; i < sizeNode; i += 1)
	{
		Point point = { 0 };
		Points_Read(&point, reader);
		arrput(ti->arr_nodes, point);
	}

	int32_t sizeSettings = BufferReader_ReadI32(reader);
	for (int32_t i = 0; i < sizeSettings; i += 1)
	{
		StringPair pair = { 0 };
		StringPair_Read(&pair, reader);
		arrput(ti->arr_settings, pair);
	}
}
void ThingInstance_SetupSettings(ThingInstance* instance, bool ignoreWarnings)
{
	//WILLNOTDO 05152023
	/*
	OeThingSettings* settings = instance->GetThingSettings(); //UNUSED

	if (settings == nullptr)
	{
		return;
	}

	if (!settings->mHasAI)
	{
		return;
	}

	OeDictionary<int32_t, Type> controllerComponentTypeMap = OeGameHelper_GetControllerComponentTypeMap();
	if (!controllerComponentTypeMap.Keys.Contains(settings.mRoutineId))
	{
		return;
	}

	Type controllerComponentType = controllerComponentTypeMap[settings.mRoutineId];

	OeListStruct<OeStringPair> arrayOfStringPairs;
	if (!OeFunc.Is_InitialStringSettingsMapPresent(controllerComponentType))
	{
		arrayOfStringPairs = null;
	}
	else
	{
		arrayOfStringPairs = OeFunc.Get_InitialStringSettingsFromMap(controllerComponentType);
	}
	if (arrayOfStringPairs != null)
	{
		AddMissingPairs(GetGlobalThingSettings(), arrayOfStringPairs, ignoreWarnings);
		RemoveUnrelatedPairs(instance, arrayOfStringPairs, instance.mSettings, ignoreWarnings);
		AddMissingPairs(arrayOfStringPairs, instance.mSettings, ignoreWarnings);
	}
	else
	{
		AddMissingPairs(GetGlobalThingSettings(), instance.mSettings, ignoreWarnings);
	}
	*/
}
void ThingInstance_RemoveUnrelatedPairs(ThingInstance* instance, StringPair* arr_from, StringPair* arr_to, bool ignoreWarnings)
{
	/*for (int32_t i = 0; i < instance->mSettings.size(); i += 1) //UNUSED
	{
		OeStringPair& pair = arrayTo[i];
		if (!Contains(pair, arrayFrom))
		{
			if (!ignoreWarnings)
			{
				OeLogger_LogWarning("Removed unrelated string pair for " + pair.mKey);
			}
			arrayTo.erase(arrayTo.begin() + i);
			i -= 1;
		}
	}*/
}
void ThingInstance_AddMissingPairs(StringPair* arr_from, StringPair* arr_to, bool ignoreWarnings)
{
	/*for (int32_t i = 0; i < arrayFrom.size(); i += 1) //UNUSED
	{
		const OeStringPair& pair = arrayFrom[i];
		if (!Contains(pair, arrayTo))
		{
			if (!ignoreWarnings)
			{
				OeLogger_LogWarning("Missing string pair for " + pair.mKey);
			}
			arrayTo.insert(arrayTo.begin() + i, pair);
		}
	}*/
}
bool ThingInstance_Contains(StringPair* pair, StringPair* arr_string_pairs)
{
	/*for (int32_t i = 0; i < arrayOfStringPairs.size(); i += 1) //UNUSED
	{
		const const char*& keyOne = arrayOfStringPairs[i].mKey;
		const const char*& keyTwo = pair.mKey;
		if (keyOne == keyTwo)
		{
			return true;
		}
	}*/

	return false;
}
ThingInstance* ThingInstance_GetClone(ThingInstance* ti)
{
	return NULL;
	/*ThingInstance copy = ThingInstance(); //UNUSED

	copy.mName = mName;

	for (int32_t i = 0; i < mNodes.size(); i += 1)
	{
		copy.mNodes.push_back(mNodes[i]);
	}

	for (int32_t i = 0; i < mSettings.size(); i += 1)
	{
		copy.mSettings.push_back(mSettings[i]);
	}

	return copy;*/
}
bool ThingInstance_IsEqualTo(ThingInstance* ti, ThingInstance* instance)
{
	return false;

	//WILLNOTDO 05152023 //UNUSED
	/*
	if (instance->mName != mName)
	{
		return false;
	}

	if (mNodes.size() != instance->mNodes.size())
	{
		return false;
	}
	for (int32_t i = 0; i < mNodes.size(); i += 1)
	{
		if (mNodes[i] != instance->mNodes[i])
		{
			return false;
		}
	}

	if (!OeUtils_IsArrayEqualTo(mSettings, instance.mSettings))
	{
		return false;
	}

	return true;
	*/
}
ThingSettings* ThingInstance_GetThingSettings(ThingInstance* ti)
{
	ResourceManager* rmThings = ResourceManagerList_ThingSettings();
	return (ThingSettings*)ResourceManager_GetResourceData(rmThings, ti->mName);
}
void ThingInstance_Draw(ThingInstance* ti, SpriteBatch* spriteBatch, Color color, Point position, bool isSelected)
{
	ThingInstance_Draw2(ti, spriteBatch, color, 100, position, isSelected);
}
void ThingInstance_Draw2(ThingInstance* ti, SpriteBatch* spriteBatch, Color color, int32_t depth, Point position, bool isSelected)
{
	DrawTool_DrawRectangle2(spriteBatch, color, depth - 1, 
		Rectangle_Create(position.X, position.Y, ThingInstance_GetWidth(ti), ThingInstance_GetHeight(ti)), 0, false);

	ThingSettings* settings = ThingInstance_GetThingSettings(ti);
	const char* sheetName;
	if (settings == NULL)
	{
		sheetName = EE_STR_NOT_SET;
	}
	else
	{
		sheetName = settings->mPreviewSheet;
	}

	Point center = Point_Add(position, Points_HalfTileSize());

	Sheet_Draw3(Sheet_GetSheet(sheetName), spriteBatch, COLOR_WHITE, depth, true, true, NULL, Vector2_Create(center.X, center.Y), Vector2_One, 0);

	if (!isSelected)
	{
		return;
	}

	for (int32_t i = 0; i < arrlen(ti->arr_nodes); i += 1)
	{
		Point nodeOne = ti->arr_nodes[i];

		int32_t nodeOnePositionX = position.X + nodeOne.X * TILE_SIZE + HALF_TILE_SIZE;
		int32_t nodeOnePositionY = position.Y + nodeOne.Y * TILE_SIZE + HALF_TILE_SIZE;

		DrawTool_DrawRectangle2(spriteBatch, COLOR_PINK, depth + 5, Rectangle_Create(nodeOnePositionX, nodeOnePositionY, TILE_SIZE, TILE_SIZE), 0, true);
		SpriteBatch_DrawString2(spriteBatch, "editor", Utils_IntToStringGlobalBuffer(i), COLOR_BLUE, 200,
			Vector2_Create(nodeOnePositionX, nodeOnePositionY), ALIGN_CENTER, ALIGN_CENTER);

		int32_t size = 1;
		if (i == 0)
		{
			DrawTool_DrawLine3(spriteBatch, COLOR_RED, 100, 4, 0, size, nodeOnePositionX, nodeOnePositionY, center.X, center.Y);
		}
		else if (i > 0)
		{
			Point nodeTwo = ti->arr_nodes[i - 1];
			int32_t nodeTwoPositionX = position.X + nodeTwo.X * TILE_SIZE + HALF_TILE_SIZE;
			int32_t nodeTwoPositionY = position.Y + nodeTwo.Y * TILE_SIZE + HALF_TILE_SIZE;
			DrawTool_DrawLine3(spriteBatch, COLOR_BLUE, 100, 4, 0, size, nodeOnePositionX, nodeOnePositionY, nodeTwoPositionX, nodeTwoPositionY);
		}
		if ((i == (arrlen(ti->arr_nodes) - 1)) && (arrlen(ti->arr_nodes) > 1))
		{
			DrawTool_DrawLine3(spriteBatch, COLOR_YELLOW, 100, 4, 0, size, nodeOnePositionX, nodeOnePositionY, center.X, center.Y);
		}
	}
}
const char* ThingInstance_GetName(ThingInstance* ti)
{
	return ti->mName;
}
Rectangle ThingInstance_GetRectangle(ThingInstance* ti, float positionX, float positionY)
{
	return Rectangle_Create(ThingInstance_GetLeft(ti, positionX), ThingInstance_GetTop(ti, positionY),
		ThingInstance_GetWidth(ti), ThingInstance_GetHeight(ti));;
}
int32_t ThingInstance_GetLeft(ThingInstance* ti, float positionX)
{
	float value = positionX + HALF_TILE_SIZE - ThingInstance_GetWidth(ti) / 2;
	return (int32_t)value;
}
int32_t ThingInstance_GetTop(ThingInstance* ti, float positionY)
{
	float value = positionY + HALF_TILE_SIZE - ThingInstance_GetHeight(ti) / 2;
	return (int32_t)value;
}
int32_t ThingInstance_GetWidth(ThingInstance* ti)
{
	return TILE_SIZE;
}
int32_t ThingInstance_GetHeight(ThingInstance* ti)
{
	return TILE_SIZE;
}
ThingInstance ThingInstance_CreateClone(ThingInstance* ti)
{
	ThingInstance clone = { 0 };
	ThingInstance_Init(&clone);
	Utils_memcpy(&clone, ti, sizeof(ThingInstance));
	clone.arr_nodes = NULL;
	clone.arr_settings = NULL;
	for (int i = 0; i < arrlen(ti->arr_nodes); i += 1)
	{
		arrput(clone.arr_nodes, ti->arr_nodes[i]);
	}
	for (int i = 0; i < arrlen(ti->arr_settings); i += 1)
	{
		arrput(clone.arr_settings, ti->arr_settings[i]);
	}
	return clone;
}
bool ThingInstance_EqualTo(const ThingInstance* value1, const ThingInstance* value2)
{
	if (!Utils_StringEqualTo(value1->mName, value2->mName))
	{
		return false;
	}

	if (arrlen(value1->arr_nodes) == arrlen(value2->arr_nodes))
	{
		for (int i = 0; i < arrlen(value1->arr_nodes); i += 1)
		{
			if(!Point_EqualTo(value1->arr_nodes[i], value2->arr_nodes[i]))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}

	if (arrlen(value1->arr_settings) == arrlen(value2->arr_settings))
	{
		for (int i = 0; i < arrlen(value1->arr_settings); i += 1)
		{
			if (!StringPair_EqualTo(&value1->arr_settings[i], &value2->arr_settings[i]))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}
