#include "Macros.h"

#define TUNING_DIFFICULTY_EASY 0
#define TUNING_DIFFICULTY_NORMAL 1
#define TUNING_DIFFICULTY_HARD 2
#define TUNING_DIFFICULTY_VERY_HARD 3

typedef struct DynamicByteBuffer DynamicByteBuffer;
typedef struct BufferReader BufferReader;

typedef struct Tuning
{
	struct { char* key; float value; }*mDictionary;
} Tuning;

void Tuning_Init(Tuning* tuning);

Tuning* Tuning_CreateNewInstance();
void Tuning_Write(Tuning* tuning, DynamicByteBuffer* dbb);
void Tuning_Read(Tuning* tuning, BufferReader* br);
int Tuning_GetCurrentDifficulty();
void Tuning_SetCurrentDifficulty(int value);
int Tuning_GetHealth(const char* thingName);
int Tuning_GetScoreOnes(const char* thingName);
int Tuning_GetScoreFives(const char* thingName);
int Tuning_GetScoreTens(const char* thingName);
int Tuning_GetScoreTwenties(const char* thingName);
int Tuning_GetScoreFifties(const char* thingName);
int Tuning_GetScoreHundreds(const char* thingName);
int Tuning_GetScoreBase(const char* thingName);
float Tuning_GetTuning(const char* thingName, const char* dataName);
float Tuning_GetSystem777Tuning(const char* dataName);
int Tuning_GetSystem777TuningAsInt(const char* dataName);
float Tuning_GetGlobal777Tuning(const char* dataName);
int Tuning_GetGlobal777TuningAsInt(const char* dataName);
float Tuning_GetGlobalTuning(const char* thingName, const char* dataName);
int Tuning_GetGlobalTuningAsInt(const char* thingName, const char* dataName);
int Tuning_GetTuningAsInt(const char* thingName, const char* dataName);
float Tuning_GetHelper(const char* thingName, const char* dataName);
float Tuning_GetHelper2(const char* thingName, const char* dataName, int defaultValue);
float Tuning_GetHelper3(int difficulty, const char* thingName, const char* dataName, int defaultValue);
Tuning* Tuning_CreateNew();
void Tuning_CopyTo(Tuning* dst, Tuning* src);
void Tuning_Dispose(Tuning* tuning);
Tuning* Tuning_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);