/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../../game/ComponentType.h"
#include "../utils/Macros.h"
#include "../components/Camera.h"
#include "PackIterator.h"

typedef struct ComponentPack
{
	size_t _mComponentSizeInBytes; //Fixed to all of same type.
	ComponentType _mComponentType;
	int32_t _mMaximumCapacity;
	uint8_t* _mDummy;
	int32_t _mLength; //Change per instance of pack in memory.
	int32_t _mCapacity;
	Entity* Entities;
	uint8_t* Components;
} ComponentPack;

void ComponentPack_Init(ComponentType ctype, ComponentPack* pack, size_t componentSizeInBytes, int32_t initialSize);
void* ComponentPack_TryGetFirstSetComponent(ComponentPack* pack, bool* wasSuccessful);
void* ComponentPack_TryGetComponent(ComponentPack* pack, Entity entity, bool* wasSuccessful);
bool ComponentPack_HasComponent(ComponentPack* pack, Entity entity);
void ComponentPack_SetMaximumCapacity(ComponentPack* pack, int32_t value);
void* ComponentPack_GetDummy(ComponentPack* pack);
int32_t ComponentPack_Length(ComponentPack* pack);
bool ComponentPack_Next(ComponentPack* pack, PackIterator* iter);
int32_t ComponentPack_GetFirstSetEntityLocation(ComponentPack* pack );
int32_t ComponentPack_GetEntityLocation(ComponentPack* pack, int32_t entityNumber);
bool ComponentPack_IsAnyEntityInPack(ComponentPack* pack );
void* ComponentPack_GetFirstSetComponent(ComponentPack* pack );
Entity ComponentPack_GetFirstSetEntity(ComponentPack* pack);
void* ComponentPack_GetComponent2(ComponentPack* pack, Entity entity, bool isNotExclusive);
void* ComponentPack_GetComponent(ComponentPack* pack, Entity entity);
void* ComponentPack_GetComponentAtIndex(ComponentPack* pack, int32_t index);
void* ComponentPack_Set2(ComponentPack* pack, Entity entity, bool isNotExclusive);
void* ComponentPack_Set(ComponentPack* pack, Entity entity);
void ComponentPack_Unset(ComponentPack* pack, int32_t entityNumber);
void ComponentPack_UnsetAtIndex(ComponentPack* pack, int32_t index);
void ComponentPack_UnsetAll(ComponentPack* pack);
void ComponentPack_CopyTo(ComponentPack* pack, ComponentPack* to);
ComponentPack* ComponentPack_CreateCopy(ComponentPack* pack);
void ComponentPack_Dispose(ComponentPack* pack);