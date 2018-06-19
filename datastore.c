#include <ctype.h>

#include "datamanipulation.h"
#include "datastore.h"
#include "gui.h"
#include "datainterface.h"

/**
 * Funcntion that creates the data structures for memory data storage.
 *
 */
void createMemoryModel(){
   modelMEMORY = gtk_list_store_new(N_COLUMNS,
         G_TYPE_UINT,   /* ADDRESS */
         G_TYPE_UINT,    /* CONTENT */
         G_TYPE_STRING,   /* COLOR */
         G_TYPE_POINTER
         );
   for(unsigned long i=memory.page_base_address; i<memory.page_base_address+memory.page_size; i+=(cpu.word_width/8)){
      gtk_list_store_append(modelMEMORY, &iter);
      /* Fill fields with some data */
      gtk_list_store_set (modelMEMORY, &iter,
            CONTENT, 0,
            ADDRESS, i,
            //COLOR, "blue",
            -1);
   }
}

/**
 * Function that creates the data structures for a cache level.
 * @level whose data structure will be created.
 */
void createCacheModel(int level){
   //Creo la cache. en caso de que sea dividia esta será la parte de data
   GtkTreeIter   iter;
   GtkTreeIter   iterInstruction;
   GtkTreeViewColumn* column;
   GtkListStore *modelData;
   GtkListStore *modelInstruction;
   /*
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
    */
   modelData= gtk_list_store_new(N_COLUMNS_CACHE,
         G_TYPE_STRING,   
         G_TYPE_STRING,   
         G_TYPE_UINT,   
         G_TYPE_UINT,    
         G_TYPE_UINT,   
         G_TYPE_UINT,
         G_TYPE_UINT,
         G_TYPE_STRING,
         G_TYPE_STRING,
         G_TYPE_STRING,
         G_TYPE_POINTER
         );
   cacheLevels[level].modelData=modelData;
   if(caches[level].separated){
      modelInstruction= gtk_list_store_new(N_COLUMNS_CACHE,
            G_TYPE_STRING,   
            G_TYPE_STRING,   
            G_TYPE_UINT,   
            G_TYPE_UINT,    
            G_TYPE_UINT,   
            G_TYPE_UINT,
            G_TYPE_UINT,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_POINTER
            );
      cacheLevels[level].modelInstruction=modelInstruction;
   }
   //modelMEMORY=modelData;	
   int numLines=caches[level].size/caches[level].line_size;
   int asociativity=caches[level].asociativity;
   int numsets=numLines/asociativity;
   int numWords=(caches[level].line_size*8)/cpu.word_width;
   if(caches[level].separated){
      numLines/=2;
   }
#if DEBUG
   fprintf(stderr,"cache level %d: lines: %d, asociativity: %d, sets: %d, words line: %d\n", level+1, numLines, asociativity, numsets, numWords);
#endif
   for(int i=0; i<numLines; i++){
      gtk_list_store_append(modelData, &iter);
      if(caches[level].separated){ 
         gtk_list_store_append(modelInstruction, &iterInstruction);
      }
      if(caches[level].separated){
         writeBlankDataCacheLine(level, i);
         writeBlankInstructionCacheLine(level, i);
      }else{
         writeBlankCacheLine(level, i);
      }
   }
}

/**
 * Function that generates all data structures for the program.
 *
 */
void generateDataStorage(){
   createMemoryModel();
   for(int i=0; i< numberCaches; i++){
      createCacheModel(i);
   }
   create_model_statistics();
}

/**
 * Function that inserts text into the buffer.
 * @param text to be inserted.
 * @param buffer in which th etext will be inserted.
 */
void insertTextInBuffer(char* text, GtkTextBuffer *buffer){
   GtkTextMark *mark;
   GtkTextIter iter;
   mark = gtk_text_buffer_get_insert (buffer);
   gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);
   gtk_text_buffer_insert (buffer, &iter, text, -1);
}

/**
 * Function that creates the data structure for simulation statistics.
 */
GtkTreeModel *create_model_statistics(void){
   GtkTreeIter    toplevel, child;
   GtkTreeModel *model = GTK_TREE_MODEL(gtk_tree_store_new(NUM_COLS,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING));
   /* Append a top level row and leave it empty */
   gtk_tree_store_append(GTK_TREE_STORE(model), &toplevel, NULL);
   gtk_tree_store_set(GTK_TREE_STORE(model), &toplevel,
         COMPONET_OR_PROPERTY, "CPU",
         -1);
   /* Append a second top level row, and fill it with some data */
   gtk_tree_store_append(GTK_TREE_STORE(model), &toplevel, NULL);
   gtk_tree_store_set(GTK_TREE_STORE(model), &toplevel,
         COMPONET_OR_PROPERTY, "Memory",
         -1);
   for(int i=0; i<numberCaches; i++){
      char currentCache[100];
      sprintf(currentCache, "Cache L%d", i+1);
      gtk_tree_store_append(GTK_TREE_STORE(model), &toplevel, NULL);
      gtk_tree_store_set(GTK_TREE_STORE(model), &toplevel,
            COMPONET_OR_PROPERTY, currentCache,
            -1);
      /* Append a child to the second top level row, and fill in some data */
      gtk_tree_store_append(GTK_TREE_STORE(model), &child, &toplevel);
      gtk_tree_store_set(GTK_TREE_STORE(model), &child,
            COMPONET_OR_PROPERTY, "Faults",
            VALUE, "",
            -1);
      gtk_tree_store_append(GTK_TREE_STORE(model), &child, &toplevel);
      gtk_tree_store_set(GTK_TREE_STORE(model), &child,
            COMPONET_OR_PROPERTY, "Hits",
            VALUE, "",
            -1);
      gtk_tree_store_append(GTK_TREE_STORE(model), &child, &toplevel);
      gtk_tree_store_set(GTK_TREE_STORE(model), &child,
            COMPONET_OR_PROPERTY, "Fault rate",
            VALUE, "",
            -1);
      gtk_tree_store_append(GTK_TREE_STORE(model), &child, &toplevel);
      gtk_tree_store_set(GTK_TREE_STORE(model), &child,
            COMPONET_OR_PROPERTY, "Hit rate",
            VALUE, "",
            -1);
   }
   statistics_model= model;
   return GTK_TREE_MODEL(model);
}
