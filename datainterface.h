#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include "gui.h"
#include "datastore.h"

enum {
    READ,
    WRITE ,
    YELLOW,
    GREEN,
    RED,
    GREY,
    ORANGE,
    PURPLE,
    PINK,
    LIGHT_BLUE,
    LIGTH_GREE,
    WHITE,
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
  int first_accessed;
};

struct memoryPosition{
  long address;
  long content;
  void * user_content;
};

extern char *interfaceError;

//para caches unificadas
void showCacheLine(int level, int i);
void readLineCache(int level, struct cacheLine* line, int i);
void writeLineCache(int level, struct cacheLine* line, int i);
void setColorCacheLine(int level, int i, int color);
void resetCache(int level);
void writeBlankCacheLine(int level, unsigned line);

//para caches divididas
void showCacheLineData(int level, int i);
void readLineCacheData(int level, struct cacheLine* line, int i);
void writeLineCacheData(int level, struct cacheLine* line, int i);
void setColorDataCacheLine(int level, int i, int color);
void resetDataCache(int level);
void writeBlankDataCacheLine(int level, unsigned line);

void showCacheLineInstructions(int level, int i);
void readLineCacheInstructions(int level, struct cacheLine* line, int i);
void writeLineCacheInstructions(int level, struct cacheLine* line, int i);
void setColorInstructionsCacheLine(int level, int i, int color);
void resetInstructionCache(int level);
void writeBlankInstructionCacheLine(int level, unsigned line);

// Function calls to manipulate the memory
int showMemoryAddress(long address);
int readMemoryAddress(struct memoryPosition *pos, long address);
int writeMemoryAddress(struct memoryPosition *pos, long address);
int setMemoryAddressColor(long address, int color);
void resetMemory();
void clearMemoryAddress(long address);

// Function calls to manage the performance and statistics storage
void setStatistics(char* component, char* property, char* value);
char* getStatistics(char* component, char* property);
void printStatistics(FILE* fp);

#endif
