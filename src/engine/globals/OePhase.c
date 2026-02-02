/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "OePhase.h"

#include "../../third_party/stb_ds.h"

static const char* STR_DEFAULT = "DEFAULT";

static DrawPhaseMap* sh_draw_phase_map;

DrawPhaseMap* OePhase_GetShDrawPhaseMap(void)
{
	if (sh_draw_phase_map == NULL)
	{
		sh_new_arena(sh_draw_phase_map);

		shput(sh_draw_phase_map, STR_DEFAULT, OePhase_DEFAULT);
		shput(sh_draw_phase_map, "STEP1", OePhase_STEP1);
		shput(sh_draw_phase_map, "STEP2", OePhase_STEP2);
		shput(sh_draw_phase_map, "STEP3", OePhase_STEP3);
		shput(sh_draw_phase_map, "STEP4", OePhase_STEP4);
		shput(sh_draw_phase_map, "STEP5", OePhase_STEP5);
		shput(sh_draw_phase_map, "STEP6", OePhase_STEP6);
		shput(sh_draw_phase_map, "STEP7", OePhase_STEP7);
		shput(sh_draw_phase_map, "STEP8", OePhase_STEP8);
		shput(sh_draw_phase_map, "STEP9", OePhase_STEP9);
		shput(sh_draw_phase_map, "WALK", OePhase_WALK); //ORANGE
		shput(sh_draw_phase_map, "BOOST", OePhase_BOOST);
		shput(sh_draw_phase_map, "MELEE_GROUND", OePhase_MELEE_GROUND);
		shput(sh_draw_phase_map, "MELEE_AIR", OePhase_MELEE_AIR);
		shput(sh_draw_phase_map, "JUMP", OePhase_JUMP);
		shput(sh_draw_phase_map, "ATTACK_GROUND", OePhase_ATTACK_GROUND);
		shput(sh_draw_phase_map, "ATTACK_AIR", OePhase_ATTACK_AIR);
		shput(sh_draw_phase_map, "DOUBLE_JUMP", OePhase_DOUBLE_JUMP);
		shput(sh_draw_phase_map, "SLIDE", OePhase_SLIDE);
		shput(sh_draw_phase_map, "FALLING", OePhase_FALLING);
		shput(sh_draw_phase_map, "FALLING_SHOOT", OePhase_FALLING_SHOOT);
		shput(sh_draw_phase_map, "STUNNED", OePhase_STUNNED);
		shput(sh_draw_phase_map, "GROUND_SHOOT", OePhase_GROUND_SHOOT);
		shput(sh_draw_phase_map, "JUMP_SHOOT", OePhase_JUMP_SHOOT);
		shput(sh_draw_phase_map, "NUDGE", OePhase_NUDGE);
		shput(sh_draw_phase_map, "NUDGE_SHOOT", OePhase_NUDGE_SHOOT);
		shput(sh_draw_phase_map, "WALK_SHOOT", OePhase_WALK_SHOOT);
		shput(sh_draw_phase_map, "DEATH", OePhase_DEATH);
		shput(sh_draw_phase_map, "SLIDE_SHOOT", OePhase_SLIDE_SHOOT);
		shput(sh_draw_phase_map, "IDLE2", OePhase_IDLE2);
		shput(sh_draw_phase_map, "HANG", OePhase_HANG);
		shput(sh_draw_phase_map, "HANG_SHOOT", OePhase_HANG_SHOOT);
		shput(sh_draw_phase_map, "HANG_V", OePhase_HANG_V);
		shput(sh_draw_phase_map, "HANG_V_SHOOT", OePhase_HANG_V_SHOOT);
		shput(sh_draw_phase_map, "HANG_V_UP", OePhase_HANG_V_UP);
		shput(sh_draw_phase_map, "HANG_V_UP_SHOOT", OePhase_HANG_V_UP_SHOOT);
		shput(sh_draw_phase_map, "HANG_V_DOWN", OePhase_HANG_V_DOWN);
		shput(sh_draw_phase_map, "HANG_V_DOWN_SHOOT", OePhase_HANG_V_DOWN_SHOOT);
		shput(sh_draw_phase_map, "STAND", OePhase_STAND); //MUTE
		shput(sh_draw_phase_map, "ATTACK", OePhase_ATTACK);
		shput(sh_draw_phase_map, "RUNNING", OePhase_RUNNING);
		shput(sh_draw_phase_map, "CLIMBING", OePhase_CLIMBING);
		shput(sh_draw_phase_map, "HANGING", OePhase_HANGING);
		shput(sh_draw_phase_map, "JUMPING", OePhase_JUMPING);
		shput(sh_draw_phase_map, "HITCH", OePhase_HITCH);
		shput(sh_draw_phase_map, "AIR_ATTACK", OePhase_AIR_ATTACK);
		shput(sh_draw_phase_map, "HANGING_IDLE", OePhase_HANGING_IDLE);
		shput(sh_draw_phase_map, "RESPAWN", OePhase_RESPAWN);
		shput(sh_draw_phase_map, "WIND_RUN", OePhase_WIND_RUN);

		shput(sh_draw_phase_map, "LEFT", OePhase_LEFT);
		shput(sh_draw_phase_map, "RIGHT", OePhase_RIGHT);
		shput(sh_draw_phase_map, "NEUTRAL", OePhase_NEUTRAL);

		shput(sh_draw_phase_map, "CROUCH", OePhase_CROUCH);
		shput(sh_draw_phase_map, "ATTACK_STANDING", OePhase_ATTACK_STANDING);
		shput(sh_draw_phase_map, "ATTACK_CROUCHING", OePhase_ATTACK_CROUCHING);
		shput(sh_draw_phase_map, "ATTACK_AIR_DOWNRIGHT", OePhase_ATTACK_AIR_DOWNRIGHT);
		shput(sh_draw_phase_map, "ATTACK_AIR_RIGHT", OePhase_ATTACK_AIR_RIGHT);

		//boss skeleton
		shput(sh_draw_phase_map, "ATTACK_PRE_DASH", OePhase_ATTACK_PRE_DASH);
		shput(sh_draw_phase_map, "ATTACK_DASH", OePhase_ATTACK_DASH);
		shput(sh_draw_phase_map, "ATTACK_BACKSTEP", OePhase_ATTACK_BACKSTEP);
		shput(sh_draw_phase_map, "ATTACK_PRE_CROSS_SLASH", OePhase_ATTACK_PRE_CROSS_SLASH);
		shput(sh_draw_phase_map, "ATTACK_CROSS_SLASH", OePhase_ATTACK_CROSS_SLASH);
		shput(sh_draw_phase_map, "ATTACK_LEAP", OePhase_ATTACK_LEAP);
		shput(sh_draw_phase_map, "ATTACK_HOMING_BONE", OePhase_ATTACK_HOMING_BONE);
		shput(sh_draw_phase_map, "ATTACK_BOUNCING_BONE", OePhase_ATTACK_BOUNCING_BONE);

		//frog
		shput(sh_draw_phase_map, "ATTACK_ACID", OePhase_ATTACK_ACID);
		shput(sh_draw_phase_map, "ATTACK_BIG_BUBBLE", OePhase_ATTACK_BIG_BUBBLE);
		shput(sh_draw_phase_map, "ATTACK_BIG_HOP", OePhase_ATTACK_BIG_HOP);
		shput(sh_draw_phase_map, "ATTACK_ROLL", OePhase_ATTACK_ROLL);
		shput(sh_draw_phase_map, "ATTACK_SHORT_HOP", OePhase_ATTACK_SHORT_HOP);
		shput(sh_draw_phase_map, "ATTACK_SLAM", OePhase_ATTACK_SLAM);
		shput(sh_draw_phase_map, "ATTACK_WALL", OePhase_ATTACK_WALL);
		shput(sh_draw_phase_map, "ATTACK_WARP", OePhase_ATTACK_WARP);

		//snake
		shput(sh_draw_phase_map, "ATTACK_HORIZONTAL", OePhase_ATTACK_HORIZONTAL);
		shput(sh_draw_phase_map, "ATTACK_VERTICAL", OePhase_ATTACK_VERTICAL);

		shput(sh_draw_phase_map, "ATTACK_AIR_UPRIGHT", OePhase_ATTACK_AIR_UPRIGHT);
		shput(sh_draw_phase_map, "ATTACK_AIR_UP", OePhase_ATTACK_AIR_UP);
		shput(sh_draw_phase_map, "ATTACK_AIR_DOWN", OePhase_ATTACK_AIR_DOWN);
		shput(sh_draw_phase_map, "ATTACK_STANDING_UP", OePhase_ATTACK_STANDING_UP);
		shput(sh_draw_phase_map, "ATTACK_STANDING_UPRIGHT", OePhase_ATTACK_STANDING_UPRIGHT);

		shput(sh_draw_phase_map, "FALLING_ALT", OePhase_FALLING_ALT);

		shput(sh_draw_phase_map, "ATTACK_STANDING_SUBWEAPON", OePhase_ATTACK_STANDING_SUBWEAPON);

		shput(sh_draw_phase_map, "ATTACK_STUN_STATE", OePhase_ATTACK_STUN_STATE);

		shput(sh_draw_phase_map, "STEP10", OePhase_STEP10);
		shput(sh_draw_phase_map, "STEP11", OePhase_STEP11);
		shput(sh_draw_phase_map, "STEP12", OePhase_STEP12);
		shput(sh_draw_phase_map, "STEP13", OePhase_STEP13);
		shput(sh_draw_phase_map, "STEP14", OePhase_STEP14);
		shput(sh_draw_phase_map, "STEP15", OePhase_STEP15);
		shput(sh_draw_phase_map, "STEP16", OePhase_STEP16);
		shput(sh_draw_phase_map, "STEP17", OePhase_STEP17);
		shput(sh_draw_phase_map, "STEP18", OePhase_STEP18);
		shput(sh_draw_phase_map, "STEP19", OePhase_STEP19);

		shput(sh_draw_phase_map, "ROPE_CLIMB", OePhase_ROPE_CLIMB);
		shput(sh_draw_phase_map, "ROPE_ATTACK", OePhase_ROPE_ATTACK);

		shput(sh_draw_phase_map, "TAUNT", OePhase_TAUNT);
		shput(sh_draw_phase_map, "DODGING", OePhase_DODGING);

		shput(sh_draw_phase_map, "MOVE_ATTACK_L_A", OePhase_MOVE_ATTACK_L_A);
		shput(sh_draw_phase_map, "MOVE_ATTACK_L_B", OePhase_MOVE_ATTACK_L_B);
		shput(sh_draw_phase_map, "MOVE_ATTACK_L_C", OePhase_MOVE_ATTACK_L_C);

		shput(sh_draw_phase_map, "MOVE_ATTACK_H_A", OePhase_MOVE_ATTACK_H_A);
		shput(sh_draw_phase_map, "MOVE_ATTACK_H_B", OePhase_MOVE_ATTACK_H_B);
		shput(sh_draw_phase_map, "MOVE_ATTACK_H_C", OePhase_MOVE_ATTACK_H_C);

		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_L_A", OePhase_MOVE_ATTACK_AIR_L_A);
		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_L_B", OePhase_MOVE_ATTACK_AIR_L_B);
		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_L_C", OePhase_MOVE_ATTACK_AIR_L_C);

		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_H_A", OePhase_MOVE_ATTACK_AIR_H_A);
		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_H_B", OePhase_MOVE_ATTACK_AIR_H_B);
		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_H_C", OePhase_MOVE_ATTACK_AIR_H_C);

		shput(sh_draw_phase_map, "MOVE_ATTACK_S1_A", OePhase_MOVE_ATTACK_S1_A);
		shput(sh_draw_phase_map, "MOVE_ATTACK_S1_B", OePhase_MOVE_ATTACK_S1_B);
		shput(sh_draw_phase_map, "MOVE_ATTACK_S1_C", OePhase_MOVE_ATTACK_S1_C);

		shput(sh_draw_phase_map, "MOVE_ATTACK_S2_A", OePhase_MOVE_ATTACK_S2_A);
		shput(sh_draw_phase_map, "MOVE_ATTACK_S2_B", OePhase_MOVE_ATTACK_S2_B);
		shput(sh_draw_phase_map, "MOVE_ATTACK_S2_C", OePhase_MOVE_ATTACK_S2_C);

		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_S1_A", OePhase_MOVE_ATTACK_AIR_S1_A);
		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_S1_B", OePhase_MOVE_ATTACK_AIR_S1_B);
		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_S1_C", OePhase_MOVE_ATTACK_AIR_S1_C);

		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_S2_A", OePhase_MOVE_ATTACK_AIR_S2_A);
		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_S2_B", OePhase_MOVE_ATTACK_AIR_S2_B);
		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_S2_C", OePhase_MOVE_ATTACK_AIR_S2_C);

		shput(sh_draw_phase_map, "MOVE_ATTACK_UP", OePhase_MOVE_ATTACK_UP);
		shput(sh_draw_phase_map, "MOVE_ATTACK_SIDE", OePhase_MOVE_ATTACK_SIDE);
		shput(sh_draw_phase_map, "MOVE_ATTACK_DOWN", OePhase_MOVE_ATTACK_DOWN);

		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_UP", OePhase_MOVE_ATTACK_AIR_UP);
		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_SIDE", OePhase_MOVE_ATTACK_AIR_SIDE);
		shput(sh_draw_phase_map, "MOVE_ATTACK_AIR_DOWN", OePhase_MOVE_ATTACK_AIR_DOWN);

		shput(sh_draw_phase_map, "DODGING_AIR", OePhase_DODGING_AIR);

		shput(sh_draw_phase_map, "MOVE_ATTACK_SPECIAL_UP", OePhase_MOVE_ATTACK_SPECIAL_UP);
		shput(sh_draw_phase_map, "MOVE_ATTACK_SPECIAL_SIDE", OePhase_MOVE_ATTACK_SPECIAL_SIDE);
		shput(sh_draw_phase_map, "MOVE_ATTACK_SPECIAL_DOWN", OePhase_MOVE_ATTACK_SPECIAL_DOWN);
		shput(sh_draw_phase_map, "MOVE_ATTACK_SPECIAL_NEUTRAL", OePhase_MOVE_ATTACK_SPECIAL_NEUTRAL);

		shput(sh_draw_phase_map, "MOVE_ATTACK_SPECIAL_AIR_UP", OePhase_MOVE_ATTACK_SPECIAL_AIR_UP);
		shput(sh_draw_phase_map, "MOVE_ATTACK_SPECIAL_AIR_SIDE", OePhase_MOVE_ATTACK_SPECIAL_AIR_SIDE);
		shput(sh_draw_phase_map, "MOVE_ATTACK_SPECIAL_AIR_DOWN", OePhase_MOVE_ATTACK_SPECIAL_AIR_DOWN);
		shput(sh_draw_phase_map, "MOVE_ATTACK_SPECIAL_AIR_NEUTRAL", OePhase_MOVE_ATTACK_SPECIAL_AIR_NEUTRAL);

		shput(sh_draw_phase_map, "MORE_LEFT", OePhase_MORE_LEFT);
		shput(sh_draw_phase_map, "MORE_RIGHT", OePhase_MORE_RIGHT);

		shput(sh_draw_phase_map, "INTRO", OePhase_INTRO);
		shput(sh_draw_phase_map, "INTRO_FREEZE", OePhase_INTRO_FREEZE);

		shput(sh_draw_phase_map, "DESPAWN", OePhase_DESPAWN);
		shput(sh_draw_phase_map, "DESPAWN_SUPER", OePhase_DESPAWN_SUPER);
		shput(sh_draw_phase_map, "DESPAWN_INVULN", OePhase_DESPAWN_INVULN);
	}

	return sh_draw_phase_map;
}
