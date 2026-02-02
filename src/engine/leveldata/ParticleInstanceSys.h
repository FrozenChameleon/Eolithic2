/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../systems/System.h"
#include "ParticleInstance.h"

void ParticleInstanceSys_Setup(ParticleInstance* data, const char* name, Particle* particleData, float x, float y);
bool ParticleInstanceSys_UpdateRoutine(ParticleInstance* data);
System* ParticleInstanceSys_CreateSystem(void);