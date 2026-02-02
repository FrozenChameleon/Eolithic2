/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "OeState.h"

#include "../../third_party/stb_ds.h"
#include "../utils/Utils.h"

static const char* STR_DEFAULT = "DEFAULT";

static DrawStateMap* sh_draw_state_map;

DrawStateMap* OeState_GetShDrawStateMap(void)
{
	if (sh_draw_state_map == NULL)
	{
		sh_new_arena(sh_draw_state_map);

		shput(sh_draw_state_map, STR_DEFAULT, OeState_DEFAULT);
		shput(sh_draw_state_map, "HEAD", OeState_HEAD);
		shput(sh_draw_state_map, "SHOOT", OeState_SHOOT);
		shput(sh_draw_state_map, "NEW", OeState_NEW);
		shput(sh_draw_state_map, "TAGS", OeState_TAGS);
		shput(sh_draw_state_map, "WHEELS", OeState_WHEELS);
		shput(sh_draw_state_map, "WHEELFX", OeState_WHEELFX);
		shput(sh_draw_state_map, "MUZZLE", OeState_MUZZLE);
		shput(sh_draw_state_map, "STEP1", OeState_STEP1);
		shput(sh_draw_state_map, "STEP2", OeState_STEP2);
		shput(sh_draw_state_map, "STEP3", OeState_STEP3);
		shput(sh_draw_state_map, "STEP4", OeState_STEP4);
		shput(sh_draw_state_map, "STEP5", OeState_STEP5);
		shput(sh_draw_state_map, "STEP6", OeState_STEP6);
		shput(sh_draw_state_map, "STEP7", OeState_STEP7);
		shput(sh_draw_state_map, "STEP8", OeState_STEP8);
		shput(sh_draw_state_map, "STEP9", OeState_STEP9);
	}

	return sh_draw_state_map;
}
