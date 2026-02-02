/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "limits.h"

typedef enum DrawPhase
{
	OePhase_NOTHING = INT_MIN, //int.MinValue
	OePhase_DEFAULT = 0,
	OePhase_STEP1 = 1,
	OePhase_STEP2 = 2,
	OePhase_STEP3 = 3,
	OePhase_STEP4 = 4,
	OePhase_STEP5 = 5,
	OePhase_STEP6 = 6,
	OePhase_STEP7 = 7,
	OePhase_STEP8 = 8,
	OePhase_STEP9 = 9,
	OePhase_WALK = 10, //orangemoon
	OePhase_BOOST = 11,
	OePhase_MELEE_GROUND = 12,
	OePhase_MELEE_AIR = 13,
	OePhase_JUMP = 14,
	OePhase_ATTACK_GROUND = 15,
	OePhase_ATTACK_AIR = 16,
	OePhase_DOUBLE_JUMP = 17,
	OePhase_SLIDE = 18,
	OePhase_FALLING = 19,
	OePhase_FALLING_SHOOT = 20,
	OePhase_STUNNED = 21,
	OePhase_GROUND_SHOOT = 22,
	OePhase_JUMP_SHOOT = 23,
	OePhase_NUDGE = 24,
	OePhase_NUDGE_SHOOT = 25,
	OePhase_WALK_SHOOT = 26,
	OePhase_DEATH = 27,
	OePhase_SLIDE_SHOOT = 28,
	OePhase_IDLE2 = 29,
	OePhase_HANG = 30,
	OePhase_HANG_SHOOT = 31,
	OePhase_HANG_V = 32,
	OePhase_HANG_V_SHOOT = 33,
	OePhase_HANG_V_UP = 34,
	OePhase_HANG_V_UP_SHOOT = 35,
	OePhase_HANG_V_DOWN = 36,
	OePhase_HANG_V_DOWN_SHOOT = 37,
	OePhase_STAND = 38, //mutecrimson
	OePhase_ATTACK = 39,
	OePhase_RUNNING = 40,
	OePhase_CLIMBING = 41,
	OePhase_HANGING = 42,
	OePhase_JUMPING = 43,
	OePhase_HITCH = 44,
	OePhase_AIR_ATTACK = 45,
	OePhase_HANGING_IDLE = 46,
	OePhase_RESPAWN = 47,
	OePhase_WIND_RUN = 48,
	OePhase_LEFT = 49,
	OePhase_RIGHT = 50,
	OePhase_NEUTRAL = 51,
	OePhase_CROUCH = 52, //grim
	OePhase_ATTACK_STANDING = 53,
	OePhase_ATTACK_CROUCHING = 54,
	OePhase_ATTACK_AIR_DOWNRIGHT = 55,
	OePhase_ATTACK_AIR_RIGHT = 56,
	OePhase_ATTACK_PRE_DASH = 57, //SKELEMAN BOSS
	OePhase_ATTACK_DASH = 58,
	OePhase_ATTACK_BACKSTEP = 59,
	OePhase_ATTACK_PRE_CROSS_SLASH = 60,
	OePhase_ATTACK_CROSS_SLASH = 61,
	OePhase_ATTACK_LEAP = 62,
	OePhase_ATTACK_HOMING_BONE = 63,
	OePhase_ATTACK_BOUNCING_BONE = 64,
	OePhase_ATTACK_ACID = 65, //FROG BOSS
	OePhase_ATTACK_BIG_BUBBLE = 66,
	OePhase_ATTACK_BIG_HOP = 67,
	OePhase_ATTACK_ROLL = 68,
	OePhase_ATTACK_SHORT_HOP = 69,
	OePhase_ATTACK_SLAM = 70,
	OePhase_ATTACK_WALL = 71,
	OePhase_ATTACK_WARP = 72,
	OePhase_ATTACK_HORIZONTAL = 73, //SNAKE BOSS
	OePhase_ATTACK_VERTICAL = 74,
	OePhase_ATTACK_AIR_UPRIGHT = 75, //PLAYER
	OePhase_ATTACK_AIR_UP = 76,
	OePhase_ATTACK_AIR_DOWN = 77,
	OePhase_ATTACK_STANDING_UP = 78,
	OePhase_ATTACK_STANDING_UPRIGHT = 79,
	OePhase_FALLING_ALT = 80,
	OePhase_ATTACK_STANDING_SUBWEAPON = 81,
	OePhase_ATTACK_STUN_STATE = 82,
	OePhase_STEP10 = 83,
	OePhase_STEP11 = 84,
	OePhase_STEP12 = 85,
	OePhase_STEP13 = 86,
	OePhase_STEP14 = 87,
	OePhase_STEP15 = 88,
	OePhase_STEP16 = 89,
	OePhase_STEP17 = 90,
	OePhase_STEP18 = 91,
	OePhase_STEP19 = 92,
	OePhase_ROPE_CLIMB = 93,
	OePhase_ROPE_ATTACK = 94,
	OePhase_TAUNT = 95,
	OePhase_DODGING = 96,

	OePhase_MOVE_ATTACK_L_A = 97,
	OePhase_MOVE_ATTACK_L_B = 98,
	OePhase_MOVE_ATTACK_L_C = 99,

	OePhase_MOVE_ATTACK_H_A = 100,
	OePhase_MOVE_ATTACK_H_B = 101,
	OePhase_MOVE_ATTACK_H_C = 102,

	OePhase_MOVE_ATTACK_AIR_L_A = 103,
	OePhase_MOVE_ATTACK_AIR_L_B = 104,
	OePhase_MOVE_ATTACK_AIR_L_C = 105,

	OePhase_MOVE_ATTACK_AIR_H_A = 106,
	OePhase_MOVE_ATTACK_AIR_H_B = 107,
	OePhase_MOVE_ATTACK_AIR_H_C = 108,

	OePhase_MOVE_ATTACK_S1_A = 109,
	OePhase_MOVE_ATTACK_S1_B = 110,
	OePhase_MOVE_ATTACK_S1_C = 111,

	OePhase_MOVE_ATTACK_S2_A = 112,
	OePhase_MOVE_ATTACK_S2_B = 113,
	OePhase_MOVE_ATTACK_S2_C = 114,

	OePhase_MOVE_ATTACK_AIR_S1_A = 115,
	OePhase_MOVE_ATTACK_AIR_S1_B = 116,
	OePhase_MOVE_ATTACK_AIR_S1_C = 117,

	OePhase_MOVE_ATTACK_AIR_S2_A = 118,
	OePhase_MOVE_ATTACK_AIR_S2_B = 119,
	OePhase_MOVE_ATTACK_AIR_S2_C = 120,

	OePhase_MOVE_ATTACK_UP = 120,
	OePhase_MOVE_ATTACK_SIDE = 121,
	OePhase_MOVE_ATTACK_DOWN = 122,

	OePhase_MOVE_ATTACK_AIR_UP = 124,
	OePhase_MOVE_ATTACK_AIR_SIDE = 125,
	OePhase_MOVE_ATTACK_AIR_DOWN = 126,

	OePhase_DODGING_AIR = 128,

	OePhase_MOVE_ATTACK_SPECIAL_UP = 129,
	OePhase_MOVE_ATTACK_SPECIAL_SIDE = 130,
	OePhase_MOVE_ATTACK_SPECIAL_DOWN = 131,
	OePhase_MOVE_ATTACK_SPECIAL_NEUTRAL = 132,

	OePhase_MOVE_ATTACK_SPECIAL_AIR_UP = 133,
	OePhase_MOVE_ATTACK_SPECIAL_AIR_SIDE = 134,
	OePhase_MOVE_ATTACK_SPECIAL_AIR_DOWN = 135,
	OePhase_MOVE_ATTACK_SPECIAL_AIR_NEUTRAL = 136,

	OePhase_MORE_LEFT = 137,
	OePhase_MORE_RIGHT = 138,
	OePhase_INTRO = 139,
	OePhase_INTRO_FREEZE = 140,
	OePhase_DESPAWN = 141,
	OePhase_DESPAWN_SUPER = 142,
	OePhase_DESPAWN_INVULN = 143
} DrawPhase;

typedef struct DrawPhaseMap
{
	char* key;
	int32_t value;
} DrawPhaseMap;

DrawPhaseMap* OePhase_GetShDrawPhaseMap(void);

/*
const std::string& GetDefaultPhase();
const std::vector<std::string>& GetDrawPhaseArray();

void Init();*/