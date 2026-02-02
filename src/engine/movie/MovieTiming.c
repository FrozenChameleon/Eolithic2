/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "MovieTiming.h"

#include "../io/File.h"
#include "../utils/Utils.h"
#include "../utils/IStringArray.h"

MovieTiming* MovieTiming_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	MovieTiming* mt = (MovieTiming*)Utils_calloc(1, sizeof(MovieTiming));
	{
		IStringArray* lotsOfStrings = NULL;
		{
			MString* bigString = NULL;
			File_ReadAllToBigString(&bigString, br);
			lotsOfStrings = IStringArray_Create();
			Utils_SplitString(MString_Text(bigString), MString_Capacity(bigString), ',', lotsOfStrings);
			MString_Dispose(&bigString);
		}
		int32_t len = (int32_t)IStringArray_Length(lotsOfStrings);
		mt->timings = (int32_t*)Utils_calloc(len, sizeof(int32_t));
		mt->len = len;
		for (int32_t i = 0; i < len; i += 1)
		{
			int32_t theTiming = Utils_ParseInt(IStringArray_Get(lotsOfStrings, i));
			mt->timings[i] = theTiming;
		}
		
		IStringArray_Dispose(lotsOfStrings);
	}
	return mt;
}
void MovieTiming_Dispose(MovieTiming* mt)
{
	Utils_free(mt->timings);
	Utils_free(mt);
}
