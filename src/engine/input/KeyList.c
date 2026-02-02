/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "KeyList.h"

#include "stdint.h"
#include "stdbool.h"
#include "Keys.h"

static bool _mHasInit;
static int32_t _mArray[KEYS_AMOUNT_OF_KEYS];

static void Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	_mArray[0] = KEYS_None;
	_mArray[1] = KEYS_Back;
	_mArray[2] = KEYS_Tab;
	_mArray[3] = KEYS_Enter;
	_mArray[4] = KEYS_Pause;
	_mArray[5] = KEYS_CapsLock;
	_mArray[6] = KEYS_Kana;
	_mArray[7] = KEYS_Kanji;
	_mArray[8] = KEYS_Escape;
	_mArray[9] = KEYS_ImeConvert;
	_mArray[10] = KEYS_ImeNoConvert;
	_mArray[11] = KEYS_Space;
	_mArray[12] = KEYS_PageUp;
	_mArray[13] = KEYS_PageDown;
	_mArray[14] = KEYS_End;
	_mArray[15] = KEYS_Home;
	_mArray[16] = KEYS_Left;
	_mArray[17] = KEYS_Up;
	_mArray[18] = KEYS_Right;
	_mArray[19] = KEYS_Down;
	_mArray[20] = KEYS_Select;
	_mArray[21] = KEYS_Print;
	_mArray[22] = KEYS_Execute;
	_mArray[23] = KEYS_PrintScreen;
	_mArray[24] = KEYS_Insert;
	_mArray[25] = KEYS_Delete;
	_mArray[26] = KEYS_Help;
	_mArray[27] = KEYS_D0;
	_mArray[28] = KEYS_D1;
	_mArray[29] = KEYS_D2;
	_mArray[30] = KEYS_D3;
	_mArray[31] = KEYS_D4;
	_mArray[32] = KEYS_D5;
	_mArray[33] = KEYS_D6;
	_mArray[34] = KEYS_D7;
	_mArray[35] = KEYS_D8;
	_mArray[36] = KEYS_D9;
	_mArray[37] = KEYS_A;
	_mArray[38] = KEYS_B;
	_mArray[39] = KEYS_C;
	_mArray[40] = KEYS_D;
	_mArray[41] = KEYS_E;
	_mArray[42] = KEYS_F;
	_mArray[43] = KEYS_G;
	_mArray[44] = KEYS_H;
	_mArray[45] = KEYS_I;
	_mArray[46] = KEYS_J;
	_mArray[47] = KEYS_K;
	_mArray[48] = KEYS_L;
	_mArray[49] = KEYS_M;
	_mArray[50] = KEYS_N;
	_mArray[51] = KEYS_O;
	_mArray[52] = KEYS_P;
	_mArray[53] = KEYS_Q;
	_mArray[54] = KEYS_R;
	_mArray[55] = KEYS_S;
	_mArray[56] = KEYS_T;
	_mArray[57] = KEYS_U;
	_mArray[58] = KEYS_V;
	_mArray[59] = KEYS_W;
	_mArray[60] = KEYS_X;
	_mArray[61] = KEYS_Y;
	_mArray[62] = KEYS_Z;
	_mArray[63] = KEYS_LeftWindows;
	_mArray[64] = KEYS_RightWindows;
	_mArray[65] = KEYS_Apps;
	_mArray[66] = KEYS_Sleep;
	_mArray[67] = KEYS_NumPad0;
	_mArray[68] = KEYS_NumPad1;
	_mArray[69] = KEYS_NumPad2;
	_mArray[70] = KEYS_NumPad3;
	_mArray[71] = KEYS_NumPad4;
	_mArray[72] = KEYS_NumPad5;
	_mArray[73] = KEYS_NumPad6;
	_mArray[74] = KEYS_NumPad7;
	_mArray[75] = KEYS_NumPad8;
	_mArray[76] = KEYS_NumPad9;
	_mArray[77] = KEYS_Multiply;
	_mArray[78] = KEYS_Add;
	_mArray[79] = KEYS_Separator;
	_mArray[80] = KEYS_Subtract;
	_mArray[81] = KEYS_Decimal;
	_mArray[82] = KEYS_Divide;
	_mArray[83] = KEYS_F1;
	_mArray[84] = KEYS_F2;
	_mArray[85] = KEYS_F3;
	_mArray[86] = KEYS_F4;
	_mArray[87] = KEYS_F5;
	_mArray[88] = KEYS_F6;
	_mArray[89] = KEYS_F7;
	_mArray[90] = KEYS_F8;
	_mArray[91] = KEYS_F9;
	_mArray[92] = KEYS_F10;
	_mArray[93] = KEYS_F11;
	_mArray[94] = KEYS_F12;
	_mArray[95] = KEYS_F13;
	_mArray[96] = KEYS_F14;
	_mArray[97] = KEYS_F15;
	_mArray[98] = KEYS_F16;
	_mArray[99] = KEYS_F17;
	_mArray[100] = KEYS_F18;
	_mArray[101] = KEYS_F19;
	_mArray[102] = KEYS_F20;
	_mArray[103] = KEYS_F21;
	_mArray[104] = KEYS_F22;
	_mArray[105] = KEYS_F23;
	_mArray[106] = KEYS_F24;
	_mArray[107] = KEYS_NumLock;
	_mArray[108] = KEYS_Scroll;
	_mArray[109] = KEYS_LeftShift;
	_mArray[110] = KEYS_RightShift;
	_mArray[111] = KEYS_LeftControl;
	_mArray[112] = KEYS_RightControl;
	_mArray[113] = KEYS_LeftAlt;
	_mArray[114] = KEYS_RightAlt;
	_mArray[115] = KEYS_BrowserBack;
	_mArray[116] = KEYS_BrowserForward;
	_mArray[117] = KEYS_BrowserRefresh;
	_mArray[118] = KEYS_BrowserStop;
	_mArray[119] = KEYS_BrowserSearch;
	_mArray[120] = KEYS_BrowserFavorites;
	_mArray[121] = KEYS_BrowserHome;
	_mArray[122] = KEYS_VolumeMute;
	_mArray[123] = KEYS_VolumeDown;
	_mArray[124] = KEYS_VolumeUp;
	_mArray[125] = KEYS_MediaNextTrack;
	_mArray[126] = KEYS_MediaPreviousTrack;
	_mArray[127] = KEYS_MediaStop;
	_mArray[128] = KEYS_MediaPlayPause;
	_mArray[129] = KEYS_LaunchMail;
	_mArray[130] = KEYS_SelectMedia;
	_mArray[131] = KEYS_LaunchApplication1;
	_mArray[132] = KEYS_LaunchApplication2;
	_mArray[133] = KEYS_OemSemicolon;
	_mArray[134] = KEYS_OemPlus;
	_mArray[135] = KEYS_OemComma;
	_mArray[136] = KEYS_OemMinus;
	_mArray[137] = KEYS_OemPeriod;
	_mArray[138] = KEYS_OemQuestion;
	_mArray[139] = KEYS_OemTilde;
	_mArray[140] = KEYS_ChatPadGreen;
	_mArray[141] = KEYS_ChatPadOrange;
	_mArray[142] = KEYS_OemOpenBrackets;
	_mArray[143] = KEYS_OemPipe;
	_mArray[144] = KEYS_OemCloseBrackets;
	_mArray[145] = KEYS_OemQuotes;
	_mArray[146] = KEYS_Oem8;
	_mArray[147] = KEYS_OemBackslash;
	_mArray[148] = KEYS_ProcessKey;
	_mArray[149] = KEYS_OemCopy;
	_mArray[150] = KEYS_OemAuto;
	_mArray[151] = KEYS_OemEnlW;
	_mArray[152] = KEYS_Attn;
	_mArray[153] = KEYS_Crsel;
	_mArray[154] = KEYS_Exsel;
	_mArray[155] = KEYS_EraseEof;
	_mArray[156] = KEYS_Play;
	_mArray[157] = KEYS_Zoom;
	_mArray[158] = KEYS_Pa1;
	_mArray[159] = KEYS_OemClear;

	_mHasInit = true;
}

const int32_t* KeyList_GetArray(void)
{
	Init();

	return _mArray;
}
int32_t KeyList_GetArrayLength(void)
{
	return KEYS_AMOUNT_OF_KEYS;
}