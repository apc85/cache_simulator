#ifndef DATASTORE_H
#define DATASTORE_H

#include "confparser.h"
#include <gtk/gtk.h>

struct cacheLEVEL {
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
    SET=1,
    VALID=2,
    DIRTY=3,
    TIMES_ACCESSED=4,
    LAST_ACCESSED=5,
    FIRST_ACCESSED=6,
    TAG=7,
    CONTENT_CACHE=8,
    COLOR_CACHE=9,
    USER_CONTENT_CACHE=10,
    N_COLUMNS_CACHE=11
};

enum {
  COMPONET_OR_PROPERTY = 0,
  VALUE=1,
  NUM_COLS=2
};

void createMemoryModel();
void createCacheModel(int level);
void writeBlankLine(int level, long line);
void writeBlankMemoryPosition(long positionIndex);
void insertTextInBuffer(char* text, GtkTextBuffer *buffer);
GtkTreeModel *create_model_statistics(void);
void generateDataStorage();

#endif
