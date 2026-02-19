/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ThingInstance.h"

#include "../utils/Macros.h"
#include "../utils/Utils.h"
#include "../io/BufferReader.h"
//#include "ThingSettings.h"
#include "../render/SpriteBatch.h"
#include "../math/Points.h"
#include "../utils/Logger.h"
#include "../utils/Cvars.h"
#include "../core/Game.h"
#include "../core/GameHelper.h"
#include "../resources/ResourceManager.h"
#include "../utils/Utils.h"
#include "../../third_party/stb_ds.h"

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE
#define HALF_TILE_SIZE GLOBAL_DEF_HALF_TILE_SIZE

const char* THINGINSTANCE_SETTING_DTN_OFFSET_X = "DTN_OFFSET_X";
const char* THINGINSTANCE_SETTING_DTN_OFFSET_Y = "DTN_OFFSET_Y";

const char* THINGINSTANCE_SETTING_BLN_DIFFICULTY_EASY = "BLN_DIFFICULTY_EASY";
const char* THINGINSTANCE_SETTING_BLN_DIFFICULTY_NORMAL = "BLN_DIFFICULTY_NORMAL";
const char* THINGINSTANCE_SETTING_BLN_DIFFICULTY_HARD = "BLN_DIFFICULTY_HARD";
const char* THINGINSTANCE_SETTING_BLN_DIFFICULTY_VERY_HARD = "BLN_DIFFICULTY_VERY_HARD";

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
	return NULL;
	//return OeResourceManagers_ThingSettingsManager.GetResourceData(mName); //UNUSED
}
void ThingInstance_Draw(ThingInstance* ti, SpriteBatch* spriteBatch, Color color, Point position, bool isSelected)
{
	//Draw(spriteBatch, color, 100, position, isSelected); //UNUSED
}
void ThingInstance_Draw2(ThingInstance* ti, SpriteBatch* spriteBatch, Color color, int32_t depth, Point position, bool isSelected)
{
	//WILLNOTDO 05152023 //UNUSED
	/*
	OeDrawTool.DrawRectangle(spriteBatch, color, depth - 1, new Rectangle(position.X, position.Y, GetWidth(), GetHeight()), 0, false);

	OeThingSettings settings = GetThingSettings();
	string sheetName;
	if (settings == null)
	{
		sheetName = OeUtils.NOT_SET;
	}
	else
	{
		sheetName = settings.mPreviewSheet;
	}

	Point center = position + OePoints.HalfTileSize();

	OeSheet.Draw(ref OeSheet.GetSheet(sheetName), spriteBatch, OeColors.WHITE, depth, true, true, null, new Vector2(center.X, center.Y), Vector2.One, 0);

	if (isSelected)
	{
		int32_t tileSize = TILE_SIZE
		int32_t halfTileSize = OeUtils.GetHalfTileSize();
		for (int32_t i = 0; i < mNodes.Count; i += 1)
		{
			Point nodeOne = mNodes[i];

			int32_t nodeOnePositionX = position.X + nodeOne.X * tileSize + halfTileSize;
			int32_t nodeOnePositionY = position.Y + nodeOne.Y * tileSize + halfTileSize;

			OeDrawTool.DrawRectangle(spriteBatch, OeColors.PINK, depth + 5, new Rectangle(nodeOnePositionX, nodeOnePositionY, tileSize, tileSize), 0, true);
			spriteBatch.DrawString("editor", i.ToString(), OeColors.BLUE, 200,
				new Vector2(nodeOnePositionX, nodeOnePositionY), OeAlign.CENTER, OeAlign.CENTER);

			int32_t size = 1;
			if (i == 0)
			{
				OeDrawTool.DrawLine(spriteBatch, OeColors.RED, 100, 4, 0, size, nodeOnePositionX, nodeOnePositionY, center.X, center.Y);
			}
			else if (i > 0)
			{
				Point nodeTwo = mNodes[i - 1];
				int32_t nodeTwoPositionX = position.X + nodeTwo.X * tileSize + halfTileSize;
				int32_t nodeTwoPositionY = position.Y + nodeTwo.Y * tileSize + halfTileSize;
				OeDrawTool.DrawLine(spriteBatch, OeColors.BLUE, 100, 4, 0, size, nodeOnePositionX, nodeOnePositionY, nodeTwoPositionX, nodeTwoPositionY);
			}
			if (i == mNodes.Count - 1 && mNodes.Count > 1)
			{
				OeDrawTool.DrawLine(spriteBatch, OeColors.YELLOW, 100, 4, 0, size, nodeOnePositionX, nodeOnePositionY, center.X, center.Y);
			}
		}
	}
	*/
}
const char* ThingInstance_GetName(ThingInstance* ti)
{
	return ti->mName;
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
ThingInstance* ThingInstance_CreateClone(ThingInstance* ti)
{
	ThingInstance* clone = (ThingInstance*)Utils_calloc(1, sizeof(ThingInstance));
	ThingInstance_Init(clone);
	Utils_memcpy(clone, ti, sizeof(ThingInstance));
	clone->arr_nodes = NULL;
	clone->arr_settings = NULL;
	for (int i = 0; i < arrlen(ti->arr_nodes); i += 1)
	{
		arrput(clone->arr_nodes, ti->arr_nodes[i]);
	}
	for (int i = 0; i < arrlen(ti->arr_settings); i += 1)
	{
		arrput(clone->arr_settings, ti->arr_settings[i]);
	}
	return clone;
}
