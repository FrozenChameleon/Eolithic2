/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ComponentPack.h"

#include "../utils/Utils.h"
#include "../utils/Logger.h"
#include "../utils/Exception.h"
#include "../utils/MString.h"

static const char* GetPackComponentName(ComponentPack* pack)
{
	return ComponentType_GetComponentName(pack->_mComponentType);
}
static void LogNothingEntityWarning(ComponentPack* pack)
{
	Logger_LogWarning("Attempted to use a nothing entity in a component pack");
}
static void GrowComponentPackIfNeeded(ComponentPack* pack, int32_t newCapacity)
{
	if (pack->_mCapacity >= newCapacity)
	{
		return;
	}

	pack->Entities = (Entity*)Utils_grow(pack->Entities, sizeof(Entity) * pack->_mCapacity, sizeof(Entity) * newCapacity);
	pack->Components = (uint8_t*)Utils_grow(pack->Components, pack->_mComponentSizeInBytes * pack->_mCapacity, pack->_mComponentSizeInBytes * newCapacity);
	pack->_mCapacity = newCapacity;
}

void ComponentPack_Init(ComponentType ctype, ComponentPack* pack, size_t componentSizeInBytes, int32_t initialSize)
{
	Utils_memset(pack, 0, sizeof(ComponentPack));

	pack->_mComponentType = ctype;
	pack->_mComponentSizeInBytes = componentSizeInBytes;
	pack->_mDummy = (uint8_t*)Utils_calloc(1, componentSizeInBytes);

	GrowComponentPackIfNeeded(pack, initialSize);
}
void* ComponentPack_TryGetFirstSetComponent(ComponentPack* pack, bool* wasSuccessful)
{
	int32_t loc = ComponentPack_GetFirstSetEntityLocation(pack);
	if (loc != -1)
	{
		*wasSuccessful = true;
		return pack->Components + (loc * pack->_mComponentSizeInBytes);
	}
	else
	{
		*wasSuccessful = false;
		return ComponentPack_GetDummy(pack);
	}
}
void* ComponentPack_TryGetComponent(ComponentPack* pack, Entity entity, bool* wasSuccessful)
{
	int32_t loc = ComponentPack_GetEntityLocation(pack, entity);
	if (loc != -1)
	{
		*wasSuccessful = true;
		return pack->Components + (loc * pack->_mComponentSizeInBytes);
	}
	else
	{
		*wasSuccessful = false;
		return ComponentPack_GetDummy(pack);
	}
}
bool ComponentPack_HasComponent(ComponentPack* pack, Entity entity)
{
	return (ComponentPack_GetEntityLocation(pack, entity) != -1);
}
void ComponentPack_SetMaximumCapacity(ComponentPack* pack, int32_t value)
{
	pack->_mMaximumCapacity = value;
}
void* ComponentPack_GetDummy(ComponentPack* pack)
{
	return &pack->_mDummy;
}
int32_t ComponentPack_Length(ComponentPack* pack)
{
	return pack->_mLength;
}
bool ComponentPack_Next(ComponentPack* pack, PackIterator* iter)
{
	bool isComplete = false;
	while (!isComplete)
	{
		if (iter->mSeekPosition >= pack->_mLength)
		{
			return false;
		}

		if (pack->Entities[iter->mSeekPosition] != ENTITY_NOTHING)
		{
			iter->mIndex = iter->mSeekPosition;
			iter->mEntity = pack->Entities[iter->mIndex];
			iter->mComponent = ComponentPack_GetComponentAtIndex(pack, iter->mIndex);
			iter->mSeekPosition += 1;
			return true;
		}
		else
		{
			iter->mSeekPosition += 1;
		}
	}

	return false;
}
int32_t ComponentPack_GetFirstSetEntityLocation(ComponentPack* pack)
{
	for (int32_t i = 0; i < pack->_mLength; i += 1)
	{
		if (pack->Entities[i] != ENTITY_NOTHING)
		{
			return i;
		}
	}
	return -1;
}
int32_t ComponentPack_GetEntityLocation(ComponentPack* pack, int32_t entityNumber)
{
	if (entityNumber == ENTITY_NOTHING)
	{
		LogNothingEntityWarning(pack);
		return -1;
	}

	for (int32_t i = 0; i < pack->_mLength; i += 1)
	{
		if (pack->Entities[i] == entityNumber)
		{
			return i;
		}
	}

	return -1;
}
bool ComponentPack_IsAnyEntityInPack(ComponentPack* pack)
{
	if (pack->_mLength > 0)
	{
		return true;
	}
	return false;
}
void* ComponentPack_GetFirstSetComponent(ComponentPack* pack)
{
	int32_t loc = ComponentPack_GetFirstSetEntityLocation(pack);
	if (loc != -1)
	{
		return pack->Components + (loc * pack->_mComponentSizeInBytes);
	}
	else
	{
		{
			MString* tempString = NULL;
			MString_Combine2(&tempString, "Reached maximum capacity for pack: ", GetPackComponentName(pack));
			Exception_Run(MString_Text(tempString), false);
			MString_Dispose(&tempString);
		}
		return pack->Components;
	}
}
Entity ComponentPack_GetFirstSetEntity(ComponentPack* pack)
{
	int32_t loc = ComponentPack_GetFirstSetEntityLocation(pack);
	if (loc != -1)
	{
		return pack->Entities[loc];
	}

	{
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Cannot retrieve any entity in pack: ", GetPackComponentName(pack));
		Exception_Run(MString_Text(tempString), false);
		MString_Dispose(&tempString);
	}
	return ENTITY_NOTHING;
}
void* ComponentPack_GetComponent2(ComponentPack* pack, Entity entity, bool isNotExclusive)
{
	return ComponentPack_Set2(pack, entity, isNotExclusive);
}
void* ComponentPack_GetComponent(ComponentPack* pack, Entity entity)
{
	return ComponentPack_GetComponent2(pack, entity, false);
}
void* ComponentPack_GetComponentAtIndex(ComponentPack* pack, int32_t index)
{
	return pack->Components + (index * pack->_mComponentSizeInBytes);
}
void* ComponentPack_Set2(ComponentPack* pack, Entity entity, bool isNotExclusive)
{
	if (entity == ENTITY_NOTHING)
	{
		LogNothingEntityWarning(pack);
		return pack->Components;
	}

	if (!isNotExclusive)
	{
		int32_t loc = ComponentPack_GetEntityLocation(pack, entity);
		if (loc != -1)
		{
			return pack->Components + (loc * pack->_mComponentSizeInBytes);
		}
	}

	for (int32_t i = 0; i < pack->_mCapacity; i += 1)
	{
		if (pack->Entities[i] != ENTITY_NOTHING)
		{
			continue;
		}
	
		if ((i + 1) > pack->_mLength)
		{
			pack->_mLength = (i + 1);
		}
		pack->Entities[i] = entity;
		void* componentToReturn = ComponentPack_GetComponentAtIndex(pack, i);
		Utils_memset(componentToReturn, 0, pack->_mComponentSizeInBytes);
		return componentToReturn;
	}

	if ((pack->_mMaximumCapacity > 0) && (pack->_mCapacity >= pack->_mMaximumCapacity))
	{
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Reached maximum capacity for pack name: ", GetPackComponentName(pack));
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
		return &pack->_mDummy;
	}

	{
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Component pack is expanding for pack: ", GetPackComponentName(pack));
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}

	GrowComponentPackIfNeeded(pack, pack->_mCapacity * 2);

	return ComponentPack_Set2(pack, entity, isNotExclusive);
}
void* ComponentPack_Set(ComponentPack* pack, Entity entity)
{
	return ComponentPack_Set2(pack, entity, false);
}
void ComponentPack_Unset(ComponentPack* pack, int32_t entityNumber)
{
	if (entityNumber == ENTITY_NOTHING)
	{
		LogNothingEntityWarning(pack);
		return;
	}

	for (int32_t i = 0; i < pack->_mLength; i += 1)
	{
		if (pack->Entities[i] == entityNumber)
		{
			ComponentPack_UnsetAtIndex(pack, i);
		}
	}
}
void ComponentPack_UnsetAtIndex(ComponentPack* pack, int32_t index)
{
	pack->Entities[index] = ENTITY_NOTHING;

	if (pack->_mLength == (index + 1))
	{
		for (int32_t i = (pack->_mCapacity - 1); i >= 0; i -= 1) //Need to find new last entity
		{
			if (pack->Entities[i] != ENTITY_NOTHING)
			{
				pack->_mLength = (i + 1);
				return;
			}
		}

		pack->_mLength = 0; //Empty...
	}
}
void ComponentPack_UnsetAll(ComponentPack* pack)
{
	for (int32_t i = 0; i < pack->_mCapacity; i += 1)
	{
		pack->Entities[i] = ENTITY_NOTHING;
	}
	pack->_mLength = 0;
}
void ComponentPack_CopyTo(ComponentPack* from, ComponentPack* to)
{
	to->_mComponentSizeInBytes = from->_mComponentSizeInBytes;
	to->_mComponentType = from->_mComponentType;
	to->_mMaximumCapacity = from->_mMaximumCapacity;

	to->_mLength = from->_mLength;
	GrowComponentPackIfNeeded(to, from->_mCapacity);

	Utils_memcpy(to->Entities, from->Entities, sizeof(Entity) * from->_mCapacity);  //Copy all entity data!
	Utils_memcpy(to->Components, from->Components, (from->_mComponentSizeInBytes * from->_mLength)); //Copy all component data!
}
ComponentPack* ComponentPack_CreateCopy(ComponentPack* pack)
{
	ComponentPack* copyPack = (ComponentPack*)Utils_calloc(1, sizeof(ComponentPack));
	ComponentPack_Init(pack->_mComponentType, copyPack, pack->_mComponentSizeInBytes, 1);
	ComponentPack_CopyTo(pack, copyPack);
	return copyPack;
}
void ComponentPack_Dispose(ComponentPack* pack)
{
	Utils_free(pack->_mDummy);
	Utils_free(pack->Entities);
	Utils_free(pack->Components);
	Utils_memset(pack, 0, sizeof(pack));
}
