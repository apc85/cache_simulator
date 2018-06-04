#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include "gui.h"
#include "datastore.h"

enum {
    BLUE=0,
    BLACK=1,
    YELLOW=2,
    GREEN=3,
    RED=4,
    GREY=5,
    ORANGE=6,
    PURPLE=7,
    PINK=8,
    LIGHT_BLUE=9,
    LIGTH_GREE=10,
    WHITE=11
};

struct cacheLine{
  long line;
  long tag;
  long set;
  long * content;

  void * user_content;

  int valid;
  int dirty;
  int last_accessed;
  int times_accessed;
};

struct memoryPosition{
  long address;
  long content;
  void * user_content;
};

//para caches unificadas
void showCacheLine(int level, int i);
void readLineCache(int level, struct cacheLine* line, int i);
void writeLineCache(int level, struct cacheLine* line, int i);
void setColorCacheLine(int level, int i, int color);
void cacheReset(int level);
void writeBlankCacheLine(int level, long line);

//para caches divididas
void showCacheLineData(int level, int i);
void readLineCacheData(int level, struct cacheLine* line, int i);
void writeLineCacheData(int level, struct cacheLine* line, int i);
void setColorDataCacheLine(int level, int i, int color);
void cacheDataReset(int level);
void writeBlankDataCacheLine(int level, long line);

void showCacheLineInstructions(int level, int i);
void readLineCacheInstructions(int level, struct cacheLine* line, int i);
void writeLineCacheInstructions(int level, struct cacheLine* line, int i);
void setColorInstructionsCacheLine(int level, int i, int color);
void cacheInstructionReset(int level);
void writeBlankInstructionCacheLine(int level, long line);

//para la memory
int showPosicionMemory(int i);
int readPosicionMemory(struct memoryPosition *pos, int i);
int writePosicionMemory(struct memoryPosition *pos, int i);
int setColorMemoryPosition(int i, int color);
void memoryReset();
void writeBlankMemoryPosition(long positionIndex);

//para las estadisticas
void set_estatistics(char* component, char* property, char* value);
char* get_estatistics(char* component, char* property);

#endif