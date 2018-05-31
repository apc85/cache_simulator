#ifndef DATOS_PROGRAMA_H
#define DATOS_PROGRAMA_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "iniparser.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "parser.h"
#include "trazaParser.h"
#include "dataInterface.h"

struct cacheLEVEL{

  GtkListStore *modelData;//estructura de data
  GtkListStore *modelInstruction;//estructura de data

};

GtkListStore *modelMEMORY;//estructura de data

struct cacheLEVEL cacheLevels[MAX_CACHES];

GtkTreeModel *estatistics_model;

enum {
    ADDRESS=0,
    CONTENT=1,
    COLOR=2,
    USER_CONTENT=3,
    N_COLUMNS=4
};
enum {
    LINE=0,
    TAG=1,
    SET=2,
    CONTENT_CACHE=3,
    COLOR_CACHE=4,
    USER_CONTENT_CACHE=5,
    VALID=6,
    DIRTY=7,
    TIMES_ACCESSED=8,
    LAST_ACCESSED=9,
    N_COLUMNS_CACHE=10
};

enum
{
  COMPONET_OR_PROPERTY = 0,
  VALUE=1,
  NUM_COLS=2
} ;


void createMemoryModel();
void createCacheModel(int level);
void writeBlankLine(int level, long line);
void contentArrayToString(long* array, char* content, int level);
void writeBlankMemoryPosition(long positionIndex);
void contentStringToArray(long* array, char* content, int level);
void insertTextInBuffer(char* text, GtkTextBuffer *buffer);
GtkTreeModel *create_model_statistics(void);
void generateDataStorage();


#endif
