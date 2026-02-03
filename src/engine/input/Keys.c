/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

 /* Derived from code by Ethan Lee (Copyright 2009-2024).
  * Released under the Microsoft Public License.
  * See fna.LICENSE for details.

  * Derived from code by the Mono.Xna Team (Copyright 2006).
  * Released under the MIT License.
  * See monoxna.LICENSE for details.
  */

#include "Keys.h"

static bool _mHasInit;
static Key _mArray[KEYS_AMOUNT_OF_KEYS];

static void Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	_mArray[0] = KEYS_NONE;
	_mArray[1] = KEYS_BACK;
	_mArray[2] = KEYS_TAB;
	_mArray[3] = KEYS_ENTER;
	_mArray[4] = KEYS_PAUSE;
	_mArray[5] = KEYS_CAPSLOCK;
	_mArray[6] = KEYS_KANA;
	_mArray[7] = KEYS_KANJI;
	_mArray[8] = KEYS_ESCAPE;
	_mArray[9] = KEYS_IMECONVERT;
	_mArray[10] = KEYS_IMENOCONVERT;
	_mArray[11] = KEYS_SPACE;
	_mArray[12] = KEYS_PAGEUP;
	_mArray[13] = KEYS_PAGEDOWN;
	_mArray[14] = KEYS_END;
	_mArray[15] = KEYS_HOME;
	_mArray[16] = KEYS_LEFT;
	_mArray[17] = KEYS_UP;
	_mArray[18] = KEYS_RIGHT;
	_mArray[19] = KEYS_DOWN;
	_mArray[20] = KEYS_SELECT;
	_mArray[21] = KEYS_PRINT;
	_mArray[22] = KEYS_EXECUTE;
	_mArray[23] = KEYS_PRINTSCREEN;
	_mArray[24] = KEYS_INSERT;
	_mArray[25] = KEYS_DELETE;
	_mArray[26] = KEYS_HELP;
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
	_mArray[63] = KEYS_LEFTWINDOWS;
	_mArray[64] = KEYS_RIGHTWINDOWS;
	_mArray[65] = KEYS_APPS;
	_mArray[66] = KEYS_SLEEP;
	_mArray[67] = KEYS_NUMPAD0;
	_mArray[68] = KEYS_NUMPAD1;
	_mArray[69] = KEYS_NUMPAD2;
	_mArray[70] = KEYS_NUMPAD3;
	_mArray[71] = KEYS_NUMPAD4;
	_mArray[72] = KEYS_NUMPAD5;
	_mArray[73] = KEYS_NUMPAD6;
	_mArray[74] = KEYS_NUMPAD7;
	_mArray[75] = KEYS_NUMPAD8;
	_mArray[76] = KEYS_NUMPAD9;
	_mArray[77] = KEYS_MULTIPLY;
	_mArray[78] = KEYS_ADD;
	_mArray[79] = KEYS_SEPARATOR;
	_mArray[80] = KEYS_SUBTRACT;
	_mArray[81] = KEYS_DECIMAL;
	_mArray[82] = KEYS_DIVIDE;
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
	_mArray[107] = KEYS_NUMLOCK;
	_mArray[108] = KEYS_SCROLL;
	_mArray[109] = KEYS_LEFTSHIFT;
	_mArray[110] = KEYS_RIGHTSHIFT;
	_mArray[111] = KEYS_LEFTCONTROL;
	_mArray[112] = KEYS_RIGHTCONTROL;
	_mArray[113] = KEYS_LEFTALT;
	_mArray[114] = KEYS_RIGHTALT;
	_mArray[115] = KEYS_BROWSERBACK;
	_mArray[116] = KEYS_BROWSERFORWARD;
	_mArray[117] = KEYS_BROWSERREFRESH;
	_mArray[118] = KEYS_BROWSERSTOP;
	_mArray[119] = KEYS_BROWSERSEARCH;
	_mArray[120] = KEYS_BROWSERFAVORITES;
	_mArray[121] = KEYS_BROWSERHOME;
	_mArray[122] = KEYS_VOLUMEMUTE;
	_mArray[123] = KEYS_VOLUMEDOWN;
	_mArray[124] = KEYS_VOLUMEUP;
	_mArray[125] = KEYS_MEDIANEXTTRACK;
	_mArray[126] = KEYS_MEDIAPREVIOUSTRACK;
	_mArray[127] = KEYS_MEDIASTOP;
	_mArray[128] = KEYS_MEDIAPLAYPAUSE;
	_mArray[129] = KEYS_LAUNCHMAIL;
	_mArray[130] = KEYS_SELECTMEDIA;
	_mArray[131] = KEYS_LAUNCHAPPLICATION1;
	_mArray[132] = KEYS_LAUNCHAPPLICATION2;
	_mArray[133] = KEYS_OEMSEMICOLON;
	_mArray[134] = KEYS_OEMPLUS;
	_mArray[135] = KEYS_OEMCOMMA;
	_mArray[136] = KEYS_OEMMINUS;
	_mArray[137] = KEYS_OEMPERIOD;
	_mArray[138] = KEYS_OEMQUESTION;
	_mArray[139] = KEYS_OEMTILDE;
	_mArray[140] = KEYS_CHATPADGREEN;
	_mArray[141] = KEYS_CHATPADORANGE;
	_mArray[142] = KEYS_OEMOPENBRACKETS;
	_mArray[143] = KEYS_OEMPIPE;
	_mArray[144] = KEYS_OEMCLOSEBRACKETS;
	_mArray[145] = KEYS_OEMQUOTES;
	_mArray[146] = KEYS_OEM8;
	_mArray[147] = KEYS_OEMBACKSLASH;
	_mArray[148] = KEYS_PROCESSKEY;
	_mArray[149] = KEYS_OEMCOPY;
	_mArray[150] = KEYS_OEMAUTO;
	_mArray[151] = KEYS_OEMENLW;
	_mArray[152] = KEYS_ATTN;
	_mArray[153] = KEYS_CRSEL;
	_mArray[154] = KEYS_EXSEL;
	_mArray[155] = KEYS_ERASEEOF;
	_mArray[156] = KEYS_PLAY;
	_mArray[157] = KEYS_ZOOM;
	_mArray[158] = KEYS_PA1;
	_mArray[159] = KEYS_OEMCLEAR;

	_mHasInit = true;
}

const char* Keys_GetKeyName(Key key)
{
	switch (key)
	{
	case KEYS_NONE:
		return "None";
	case KEYS_BACK:
		return "Back";
	case KEYS_TAB:
		return "Tab";
	case KEYS_ENTER:
		return "Enter";
	case KEYS_PAUSE:
		return "Pause";
	case KEYS_CAPSLOCK:
		return "CapsLock";
	case KEYS_KANA:
		return "Kana";
	case KEYS_KANJI:
		return "Kanji";
	case KEYS_ESCAPE:
		return "Escape";
	case KEYS_IMECONVERT:
		return "ImeConvert";
	case KEYS_IMENOCONVERT:
		return "ImeNoConvert";
	case KEYS_SPACE:
		return "Space";
	case KEYS_PAGEUP:
		return "PageUp";
	case KEYS_PAGEDOWN:
		return "PageDown";
	case KEYS_END:
		return "End";
	case KEYS_HOME:
		return "Home";
	case KEYS_LEFT:
		return "Left";
	case KEYS_UP:
		return "Up";
	case KEYS_RIGHT:
		return "Right";
	case KEYS_DOWN:
		return "Down";
	case KEYS_SELECT:
		return "Select";
	case KEYS_PRINT:
		return "Print";
	case KEYS_EXECUTE:
		return "Execute";
	case KEYS_PRINTSCREEN:
		return "PrintScreen";
	case KEYS_INSERT:
		return "Insert";
	case KEYS_DELETE:
		return "Delete";
	case KEYS_HELP:
		return "Help";
	case KEYS_D0:
		return "D0";
	case KEYS_D1:
		return "D1";
	case KEYS_D2:
		return "D2";
	case KEYS_D3:
		return "D3";
	case KEYS_D4:
		return "D4";
	case KEYS_D5:
		return "D5";
	case KEYS_D6:
		return "D6";
	case KEYS_D7:
		return "D7";
	case KEYS_D8:
		return "D8";
	case KEYS_D9:
		return "D9";
	case KEYS_A:
		return "A";
	case KEYS_B:
		return "B";
	case KEYS_C:
		return "C";
	case KEYS_D:
		return "D";
	case KEYS_E:
		return "E";
	case KEYS_F:
		return "F";
	case KEYS_G:
		return "G";
	case KEYS_H:
		return "H";
	case KEYS_I:
		return "I";
	case KEYS_J:
		return "J";
	case KEYS_K:
		return "K";
	case KEYS_L:
		return "L";
	case KEYS_M:
		return "M";
	case KEYS_N:
		return "N";
	case KEYS_O:
		return "O";
	case KEYS_P:
		return "P";
	case KEYS_Q:
		return "Q";
	case KEYS_R:
		return "R";
	case KEYS_S:
		return "S";
	case KEYS_T:
		return "T";
	case KEYS_U:
		return "U";
	case KEYS_V:
		return "V";
	case KEYS_W:
		return "W";
	case KEYS_X:
		return "X";
	case KEYS_Y:
		return "Y";
	case KEYS_Z:
		return "Z";
	case KEYS_LEFTWINDOWS:
		return "LeftWindows";
	case KEYS_RIGHTWINDOWS:
		return "RightWindows";
	case KEYS_APPS:
		return "Apps";
	case KEYS_SLEEP:
		return "Sleep";
	case KEYS_NUMPAD0:
		return "NumPad0";
	case KEYS_NUMPAD1:
		return "NumPad1";
	case KEYS_NUMPAD2:
		return "NumPad2";
	case KEYS_NUMPAD3:
		return "NumPad3";
	case KEYS_NUMPAD4:
		return "NumPad4";
	case KEYS_NUMPAD5:
		return "NumPad5";
	case KEYS_NUMPAD6:
		return "NumPad6";
	case KEYS_NUMPAD7:
		return "NumPad7";
	case KEYS_NUMPAD8:
		return "NumPad8";
	case KEYS_NUMPAD9:
		return "NumPad9";
	case KEYS_MULTIPLY:
		return "Multiply";
	case KEYS_ADD:
		return "Add";
	case KEYS_SEPARATOR:
		return "Separator";
	case KEYS_SUBTRACT:
		return "Subtract";
	case KEYS_DECIMAL:
		return "Decimal";
	case KEYS_DIVIDE:
		return "Divide";
	case KEYS_F1:
		return "F1";
	case KEYS_F2:
		return "F2";
	case KEYS_F3:
		return "F3";
	case KEYS_F4:
		return "F4";
	case KEYS_F5:
		return "F5";
	case KEYS_F6:
		return "F6";
	case KEYS_F7:
		return "F7";
	case KEYS_F8:
		return "F8";
	case KEYS_F9:
		return "F9";
	case KEYS_F10:
		return "F10";
	case KEYS_F11:
		return "F11";
	case KEYS_F12:
		return "F12";
	case KEYS_F13:
		return "F13";
	case KEYS_F14:
		return "F14";
	case KEYS_F15:
		return "F15";
	case KEYS_F16:
		return "F16";
	case KEYS_F17:
		return "F17";
	case KEYS_F18:
		return "F18";
	case KEYS_F19:
		return "F19";
	case KEYS_F20:
		return "F20";
	case KEYS_F21:
		return "F21";
	case KEYS_F22:
		return "F22";
	case KEYS_F23:
		return "F23";
	case KEYS_F24:
		return "F24";
	case KEYS_NUMLOCK:
		return "NumLock";
	case KEYS_SCROLL:
		return "Scroll";
	case KEYS_LEFTSHIFT:
		return "LeftShift";
	case KEYS_RIGHTSHIFT:
		return "RightShift";
	case KEYS_LEFTCONTROL:
		return "LeftControl";
	case KEYS_RIGHTCONTROL:
		return "RightControl";
	case KEYS_LEFTALT:
		return "LeftAlt";
	case KEYS_RIGHTALT:
		return "RightAlt";
	case KEYS_BROWSERBACK:
		return "BrowserBack";
	case KEYS_BROWSERFORWARD:
		return "BrowserForward";
	case KEYS_BROWSERREFRESH:
		return "BrowserRefresh";
	case KEYS_BROWSERSTOP:
		return "BrowserStop";
	case KEYS_BROWSERSEARCH:
		return "BrowserSearch";
	case KEYS_BROWSERFAVORITES:
		return "BrowserFavorites";
	case KEYS_BROWSERHOME:
		return "BrowserHome";
	case KEYS_VOLUMEMUTE:
		return "VolumeMute";
	case KEYS_VOLUMEDOWN:
		return "VolumeDown";
	case KEYS_VOLUMEUP:
		return "VolumeUp";
	case KEYS_MEDIANEXTTRACK:
		return "MediaNextTrack";
	case KEYS_MEDIAPREVIOUSTRACK:
		return "MediaPreviousTrack";
	case KEYS_MEDIASTOP:
		return "MediaStop";
	case KEYS_MEDIAPLAYPAUSE:
		return "MediaPlayPause";
	case KEYS_LAUNCHMAIL:
		return "LaunchMail";
	case KEYS_SELECTMEDIA:
		return "SelectMedia";
	case KEYS_LAUNCHAPPLICATION1:
		return "LaunchApplication1";
	case KEYS_LAUNCHAPPLICATION2:
		return "LaunchApplication2";
	case KEYS_OEMSEMICOLON:
		return "OemSemicolon";
	case KEYS_OEMPLUS:
		return "OemPlus";
	case KEYS_OEMCOMMA:
		return "OemComma";
	case KEYS_OEMMINUS:
		return "OemMinus";
	case KEYS_OEMPERIOD:
		return "OemPeriod";
	case KEYS_OEMQUESTION:
		return "OemQuestion";
	case KEYS_OEMTILDE:
		return "OemTilde";
	case KEYS_CHATPADGREEN:
		return "ChatPadGreen";
	case KEYS_CHATPADORANGE:
		return "ChatPadOrange";
	case KEYS_OEMOPENBRACKETS:
		return "OemOpenBrackets";
	case KEYS_OEMPIPE:
		return "OemPipe";
	case KEYS_OEMCLOSEBRACKETS:
		return "OemCloseBrackets";
	case KEYS_OEMQUOTES:
		return "OemQuotes";
	case KEYS_OEM8:
		return "Oem8";
	case KEYS_OEMBACKSLASH:
		return "OemBackslash";
	case KEYS_PROCESSKEY:
		return "ProcessKey";
	case KEYS_OEMCOPY:
		return "OemCopy";
	case KEYS_OEMAUTO:
		return "OemAuto";
	case KEYS_OEMENLW:
		return "OemEnlW";
	case KEYS_ATTN:
		return "Attn";
	case KEYS_CRSEL:
		return "Crsel";
	case KEYS_EXSEL:
		return "Exsel";
	case KEYS_ERASEEOF:
		return "EraseEof";
	case KEYS_PLAY:
		return "Play";
	case KEYS_ZOOM:
		return "Zoom";
	case KEYS_PA1:
		return "Pa1";
	case KEYS_OEMCLEAR:
		return "OemClear";
	}
	return "?";
}
const Key* Keys_GetArray(void)
{
	Init();

	return _mArray;
}
int32_t Keys_GetArrayLength(void)
{
	return KEYS_AMOUNT_OF_KEYS;
}