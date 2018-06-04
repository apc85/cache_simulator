#ifndef INTERFAZ_H
#define INTERFAZ_H

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
#include "confparser.h"
#include "traceparser.h"
#include "dataInterface.h"
#include "datastore.h"
#include "callbacks.h"

/* GtkWidget is the storage type for widgets */
   GtkWidget *window;



   GtkWidget *text_view;
   GtkWidget *scrolled_window;
   GtkWidget *scrolled_window2;
   GtkWidget *vboxText;
   GtkWidget *vbox_statistics;

   GtkWidget *table;
   GtkWidget *verticalBox;

   GtkWidget *button1;
   GtkWidget *button2;
   GtkWidget *button3;

   GtkWidget *buttonBox;
   GtkTextBuffer *buffer;

   GtkTextMark *marcaLineCurrent;

   char *lineCurrent;
   GtkTextTag *tagBlue;
   GtkTextTag *tagBlack;

   GtkTextIter lineIterInicial;

   GtkTreeIter iter;



struct cacheLevelPanel{

  //GtkListStore *modelData;//estructura de data
  GtkWidget *vboxData;//container de la tabla
  GtkWidget *viewData;//widget tabla

  //GtkListStore *modelInstruction;//estructura de data
  GtkWidget *vboxInstruction;//container de la tabla
  GtkWidget *viewInstruction;//widget tabla


};




//GtkListStore *modelMEMORY;//estructura de data
GtkWidget *vboxMEMORY;//container de la tabla
GtkWidget *viewMEMORY;//widget tabla

struct cacheLevelPanel cacheLevelPanels[MAX_CACHES];

GtkWidget *estatistics_view;



int generateInterface();
GtkWidget * createPanelMemory();
void createPanelCache(int level);
GtkWidget *create_view_statistics(void);
void wordHexadecimal(char * returned, int number);
void insertTextInPanel(char* text);
void nextLineTrace();
void wordHexadecimalN(char * returned, int number, int totalDigitos);
void addressHexadecimal(char * returned, int number);
int scrollMemoryToPos(long address);
void scrollCacheToRow(int level, long row);
void scrollDataCacheToRow(int level, long row);
void scrollInstructionCacheToRow(int level, long row);
void scrollTextViewToLine(long line);

#endif
