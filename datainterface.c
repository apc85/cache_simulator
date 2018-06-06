#include <math.h>
#include "datamanipulation.h"
#include "datainterface.h"

//colors defined to be used in the program
const char* const colores[]={"blue", "black", "yellow", "green", "red", "grey", "orange", "purple", "pink", "lightblue", "lightgreen", "white"};

/**
 * This function sets memory to its initial state.
 */
void resetMemory() {
   GtkTreeIter iter;	
   //get iterator at position 0
   gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(modelMEMORY),
         &iter,
         "0");
   //this is the number of words in the whole memory
   int numWords=memory.size/(cpu.word_width/8);
   //for the n words in the mory
   for(int i=memory.page_base_address; i<memory.page_base_address+memory.page_size; i+=(cpu.word_width/8)){
      char address[50];
      char content[50];
      //obtain adrress in string format
      sprintf(address, "%0*x", (int)cpu.address_width/4, i);
      //set initial content to 0 
      sprintf(content, "%0*x", (int)cpu.word_width/4, 0);
      /* Fill fields with data */
      gtk_list_store_set (modelMEMORY, &iter,
            CONTENT, content,
            ADDRESS, address,
            COLOR, "white",
            USER_CONTENT, NULL,
            -1);
      //move iter to next position to write the next memory word
      gtk_tree_model_iter_next (GTK_TREE_MODEL(modelMEMORY),
            &iter);
   }
}
/**
 * This function resets a unified cache. Sets the data cache to its initial state.
 * @param level where the cache is located. 
 */
void resetCache(int level){
   //I reset data cache as I had defined unified caches as divided caches which don't have instruction cache. Just to make it easier.
   resetDataCache(level);
}
/**
 * This function resets a Data cache. Sets the data cache to its initial state.
 * @param cache level where the cache is located. 
 */
void resetDataCache(int level){
   //some memory hierarchy properties that will be used later
   int numLines=caches[level].size/caches[level].line_size;
   int lineSize=caches[level].line_size;
   int asociativity=caches[level].asociativity;
   int numsets=numLines/asociativity;
   int numWords=(caches[level].line_size*8)/cpu.word_width;
   //if cache is divided cache size is half
   if(caches[level].separated){
      numLines/=2;
   }
   //this is for filling each field with the correct amount of bits it must be represented with.
   int hexDigsSet=ceil(log(numsets)/log(16));
   int hexDigsLine=ceil(log(numLines)/log(16));
   int hexDigsTag=(cpu.address_width/4)-hexDigsSet-ceil(log(lineSize)/log(16));
   struct cacheLine* line;
   GtkTreeModel *model= GTK_TREE_MODEL(cacheLevels[level].modelData);
   GtkTreeIter iter;
   //get iter at first position
   gtk_tree_model_get_iter_first (model, &iter);
   //set each field to its initial value
   char cache_line[100];
   char cache_tag[100];
   char cache_set[100];
   void *user_content=NULL;
   int valid=0;
   int dirty=0;
   int times_accessed=0;
   int last_accessed=0;
   long cache_content[numWords];
   char cache_content_char[2000];
   for(int i=0; i<numWords; i++){
      cache_content[i]=0;
   }
   //this converts form long array to string representation.

   contentArrayToString(cache_content, cache_content_char, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   //I write all the reseted fields in each cache line
   for(int i=0; i<numLines; i++){ 
      sprintf(cache_line, "%0*x", hexDigsLine, i);
      sprintf(cache_tag, "%0*x", hexDigsTag, 0);
      sprintf(cache_set, "%0*x", hexDigsSet, i/asociativity);
      gtk_list_store_set (GTK_LIST_STORE(model), &iter,
            LINE, &cache_line,
            TAG, &cache_tag,
            SET, &cache_set,
            CONTENT_CACHE, cache_content_char,
            USER_CONTENT_CACHE, user_content,
            VALID, valid,
            DIRTY, dirty,
            TIMES_ACCESSED, times_accessed,
            LAST_ACCESSED, last_accessed,
            COLOR_CACHE, colores[WHITE],
            -1);
      //move iter to next position
      gtk_tree_model_iter_next (GTK_TREE_MODEL(model), &iter);
   }
} 
/**
 * This function resets a Instruction cache. Sets the data cache to its initial state.
 * @param level where the cache is located. 
 */
void resetInstructionCache(int level){
   //some memory hierarchy properties that will be used later
   int numLines=caches[level].size/caches[level].line_size;
   int lineSize=caches[level].line_size;
   int asociativity=caches[level].asociativity;
   int numsets=numLines/asociativity;
   int numWords=(caches[level].line_size*8)/cpu.word_width;
   //if cache is divided cache size is half
   if(caches[level].separated){
      numLines/=2;
   }
   //this is for filling each field with the correct amount of bits it must be represented with.
   int hexDigsSet=ceil(log(numsets)/log(16));
   int hexDigsLine=ceil(log(numLines)/log(16));
   int hexDigsTag=(cpu.address_width/4)-hexDigsSet-ceil(log(lineSize)/log(16));
   struct cacheLine* line;
   GtkTreeModel *model= GTK_TREE_MODEL(cacheLevels[level].modelInstruction);
   GtkTreeIter iter;
   //get iter at first position
   gtk_tree_model_get_iter_first (model, &iter);
   //set each field to its initial value
   char cache_line[100];
   char cache_tag[100];
   char cache_set[100];
   void *user_content=NULL;
   int valid=0;
   int dirty=0;
   int times_accessed=0;
   int last_accessed=0;
   long cache_content[numWords];
   char cache_content_char[2000];
   for(int i=0; i<numWords; i++){
      cache_content[i]=0;
   }
   //this converts form long array to string representation.
   contentArrayToString(cache_content, cache_content_char, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   //I write the all the reseted fields in each cache line
   for(int i=0; i<numLines; i++){ 
      sprintf(cache_line, "%0*x", hexDigsLine, i);
      sprintf(cache_tag, "%0*x", hexDigsTag, 0);
      sprintf(cache_set, "%0*x", hexDigsSet, i/asociativity);
      gtk_list_store_set (GTK_LIST_STORE(model), &iter,
            LINE, &cache_line,
            TAG, &cache_tag,
            SET, &cache_set,
            CONTENT_CACHE, cache_content_char,
            USER_CONTENT_CACHE, user_content,
            VALID, valid,
            DIRTY, dirty,
            TIMES_ACCESSED, times_accessed,
            LAST_ACCESSED, last_accessed,
            COLOR_CACHE, colores[WHITE],
            -1);
      //move iter to next position
      gtk_tree_model_iter_next (GTK_TREE_MODEL(model), &iter);
   }
} 
/**
 * This function shows all the values contained in the fields of a unified cache line
 * @param level which will be shown
 * @param i line index
 */
void showCacheLine(int level, int i){
   //I show data cache as I had defined unified caches as divided caches which don't have instruction cache. Just to make it easier.
   showCacheLineData(level, i);
}
/**
 * This function reads a cache line.
 * @param level which will be read
 * @param line. A pointer to a struct cacheLine were data will be placed. User must free line.content after calling the function. 
 * @param i line index
 */
void readLineCache(int level, struct cacheLine* line, int i){
   //I read data cache as I had defined unified caches as divided caches which don't have instruction cache. Just to make it easier.
   readLineCacheData(level, line, i);
}
/**
 * This function shows all the values contained in the fields of a data cache line
 * @param level which will be shown
 * @param i line index
 */
void showCacheLineData(int level, int i){
   struct cacheLine line;
   //first I read it
   readLineCacheData(level, &line, i);
   char contentString[2000];
   contentArrayToString((line).content, contentString, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   //I print the values.
   printf("------------------------------------------------------\n");
   printf("line: %lx     tag: %lx    set: %lx\n", line.line, line.tag, line.set);
   printf("content: %s\n", contentString);
   printf("user content: %s\n", (char*)line.user_content);
   printf("valid: %d   dirty: %d   last accessed: %d  times accessed: %d\n", line.valid, line.dirty, line.last_accessed, line.times_accessed);
   printf("------------------------------------------------------\n");
   free(line.content);
}
/**
 * This function reads a data cache line.
 * @param level which will be read
 * @param line. A pointer to a struct cacheLine were data will be placed. User must free line.content after calling the function. 
 * @param i line index
 */
void readLineCacheData(int level, struct cacheLine* line, int i){
   GtkTreeModel *model= GTK_TREE_MODEL(cacheLevels[level].modelData);
   GtkTreeIter iter;
   char iterstring[50];
   //this is the line index in string format
   sprintf(iterstring, "%d", i);	
   //I get an iter pointing to the line which will be read
   gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(model),
         &iter,
         iterstring);
   //this is for temporally storing read values 
   char* a;
   char* b;
   char* c;
   char* d;
   void* e;
   int f;
   int g;
   int h;
   int j;
   //read the values from data sruct
   gtk_tree_model_get (GTK_TREE_MODEL(model), &iter,
         LINE, &a,
         TAG, &b,
         SET, &c,
         CONTENT_CACHE, &d,
         USER_CONTENT_CACHE, &e,
         VALID, &f,
         DIRTY, &g,
         TIMES_ACCESSED, &h,
         LAST_ACCESSED, &j,
         -1);
   //number of words in a cache line
   int numWords = (caches[level].line_size*8)/cpu.word_width;
   long *array;
   array = malloc((sizeof(long))*numWords);
   //turn String format to long array.
   //contentStringToArray(array, d, level);
   contentStringToArray(array, d, numWords);
   //place values in struct return param so that caller user can get them
   line->line = strtol(a, NULL, 16);
   line->tag = strtol(b, NULL, 16);
   line->set = strtol(c, NULL, 16);
   line->content = array;
   line->user_content = e;
   line->valid = f;
   line->dirty = g;
   line->times_accessed = h;
   line->last_accessed = j;
}
/**
 * This function reads a instruction cache line.
 * @param level which will be read
 * @param line A pointer to a struct cacheLine were data will be placed. User must free line.content after calling the function. 
 * @param i line index
 */
void readLineCacheInstructions(int level, struct cacheLine* line, int i){
   GtkTreeModel *model= GTK_TREE_MODEL(cacheLevels[level].modelInstruction);
   GtkTreeIter iter;
   char iterstring[50];
   //this is the line index in string format
   sprintf(iterstring, "%d", i);	
   //I get an iter pointing to the line which will be read
   gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(model),
         &iter,
         iterstring);
   //this is for temporally storing read values 
   char* a;
   char* b;
   char* c;
   char* d;
   void* e;
   int f;
   int g;
   int h;
   int j;
   //read the values from data sruct
   gtk_tree_model_get (GTK_TREE_MODEL(model), &iter,
         LINE, &a,
         TAG, &b,
         SET, &c,
         CONTENT_CACHE, &d,
         USER_CONTENT_CACHE, &e,
         VALID, &f,
         DIRTY, &g,
         TIMES_ACCESSED, &h,
         LAST_ACCESSED, &j,
         -1);
   //number of words in a cache line
   int numWords=(caches[level].line_size*8)/cpu.word_width;
   long *array;
   array=malloc((sizeof(long))*numWords);
   //turn String format to long array.
   contentStringToArray(array, d, level);
   //place values in struct return param so that caller user can get them
   (*line).line=strtol(a, NULL, 16);
   (*line).tag=strtol(b, NULL, 16);
   (*line).set=strtol(c, NULL, 16);
   (*line).content=array;
   (*line).user_content=e;
   (*line).valid=f;
   (*line).dirty=g;
   (*line).times_accessed=h;
   (*line).last_accessed=j;
}
/**
 * This function reads a Instruction cache line.
 * @param level which will be read
 * @param cacheLine. A pointer to a struct cacheLine were data will be placed. User must free line.content after calling the function. 
 * @param i line index
 */
void showCacheLineInstructions(int level, int i){
   struct cacheLine line;
   //I read the cache line
   readLineCacheInstructions(level, &line, i);
   char contentString[2000];
   contentArrayToString((line).content, contentString, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   //I print the values of each field
   printf("------------------------------------------------------\n");
   printf("line: %lx     tag: %lx    set: %lx\n", line.line, line.tag, line.set);
   printf("content:%s\n", contentString);
   printf("user content: %s\n", (char*)line.user_content);
   printf("valid: %d   dirty: %d   last accessed: %d  times accessed: %d\n", line.valid, line.dirty, line.last_accessed, line.times_accessed);
   printf("------------------------------------------------------\n");
   free(line.content);
}
/**
 * This function writes a cache line.
 * @param level which will be written
 * @param line. A pointer to a struct cacheLine containing the data to be written.
 * @param i line index
 */
void writeLineCache(int level, struct cacheLine* line, int i){
   writeLineCacheData(level, line, i);
}
/**
 * This function writes a data cache line.
 * @param level which will be written
 * @param cacheLine. A pointer to a struct cacheLine containing the data to be written.
 * @param i line index
 */
void writeLineCacheData(int level, struct cacheLine* line, int i){
   GtkTreeModel *model= GTK_TREE_MODEL(cacheLevels[level].modelData);
   GtkTreeIter iter;
   char iterstring[50];
   sprintf(iterstring, "%d", i);	
   char a[100];
   char b[100];
   char c[100];
   void *d;
   int f;
   int g;
   int h;
   int j;
   int numLines=caches[level].size/caches[level].line_size;
   int lineSize=caches[level].line_size;
   int asociativity=caches[level].asociativity;
   int numsets=numLines/asociativity;
   int numWords=(caches[level].line_size*8)/cpu.word_width;
   int hexDigsSet=ceil(log(numsets)/log(16));
   int hexDigsLine=ceil(log(numLines)/log(16));
   int hexDigsTag=(cpu.address_width/4)-hexDigsSet-ceil(log(lineSize)/log(16));
   sprintf(a, "%0*lx", hexDigsLine, (*line).line);
   sprintf(b, "%0*lx", hexDigsTag,  (*line).tag);
   sprintf(c, "%0*lx", hexDigsSet,  (*line).set);
   d=(*line).user_content;
   f=(*line).valid;
   g=(*line).dirty;
   h=(*line).times_accessed;
   j=(*line).last_accessed;
   char contentString[2000];
   contentArrayToString((*line).content, contentString, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(model),
         &iter,
         iterstring);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         LINE, &a,
         TAG, &b,
         SET, &c,
         CONTENT_CACHE, contentString,
         USER_CONTENT_CACHE, d,
         VALID, f,
         DIRTY, g,
         TIMES_ACCESSED, h,
         LAST_ACCESSED, j,
         -1);
}
/**
 * This function writes a Instruction cache line.
 * @param level which will be written
 * @param line  A pointer to a struct cacheLine containing the data to be written.
 * @param i line index
 */
void writeLineCacheInstructions(int level, struct cacheLine* line, int i){
   GtkTreeModel *model= GTK_TREE_MODEL(cacheLevels[level].modelInstruction);
   GtkTreeIter iter;
   char iterstring[50];
   sprintf(iterstring, "%d", i);	
   char a[100];
   char b[100];
   char c[100];
   void* d;
   int f;
   int g;
   int h;
   int j;
   int numLines=caches[level].size/caches[level].line_size;
   int lineSize=caches[level].line_size;
   int asociativity=caches[level].asociativity;
   int numsets=numLines/asociativity;
   int numWords=(caches[level].line_size*8)/cpu.word_width;
   int hexDigsSet=ceil(log(numsets)/log(16));
   int hexDigsLine=ceil(log(numLines)/log(16));
   int hexDigsTag=(cpu.address_width/4)-hexDigsSet-ceil(log(lineSize)/log(16));
   sprintf(a, "%0*lx", hexDigsLine, (*line).line);
   sprintf(b, "%0*lx", hexDigsTag,  (*line).tag);
   sprintf(c, "%0*lx", hexDigsSet,  (*line).set);
   d=(*line).user_content;
   f=(*line).valid;
   g=(*line).dirty;
   h=(*line).times_accessed;
   j=(*line).last_accessed;
   char contentString[2000];
   contentArrayToString((*line).content, contentString, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(model),
         &iter,
         iterstring);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         LINE, &a,
         TAG, &b,
         SET, &c,
         CONTENT_CACHE, contentString,
         USER_CONTENT_CACHE, d,
         VALID, f,
         DIRTY, g,
         TIMES_ACCESSED, h,
         LAST_ACCESSED, j,
         -1);
}
/**
 * This function shows a memory position
 * @param i memory address
 * @return 0 if correct -1 if not word address error, -2 if out of page error
 */
int showPosicionMemory(int i){
   struct memoryPosition pos;
   //I read the memory position
   int returned=readPosicionMemory(&pos, i);
   if(returned!=0){
      return returned;
   }
   //I print the values
   printf("------------------------------------------------------\n");
   printf("address: %lx     content: %lx\n", pos.address, pos.content);
   printf("user content: %s\n", (char*)pos.user_content);
   printf("------------------------------------------------------\n");
   return 0;
}
/**
 * This function reads a memory position
 * @param pos. Read data will be placed in here. User must take care of freeing pos.content
 * @param i memory address
 * @return 0 if correct -1 if not word address error, -2 if out of page error
 */
int readPosicionMemory(struct memoryPosition *pos, int i){
   GtkTreeModel *model= GTK_TREE_MODEL(modelMEMORY);
   GtkTreeIter iter;
   char iterstring[50];
   //if not word address return error
   if(i%(cpu.word_width/8)!=0){
      return -1;
   }
   //if out of page return error
   if(i<memory.page_base_address || i>(memory.page_base_address+memory.page_size)){
      return -2;
   }
   //get the table row from the memory address
   i=(i-memory.page_base_address)/(cpu.word_width/8);   
   sprintf(iterstring, "%d", i);	
   gtk_tree_model_get_iter_from_string (model,
         &iter,
         iterstring);
   char* a;
   char* b;
   void* c=NULL;
   gtk_tree_model_get (model, &iter,
         ADDRESS, &a,
         CONTENT, &b,
         USER_CONTENT, &c,
         -1);
   (*pos).address=strtol(a, NULL, 16);
   (*pos).content=strtol(b, NULL, 16);
   (*pos).user_content=c;
}
/**
 * This function writes a memory position.
 * @param pos. Data to be written will be read from this struct.
 * @param i memory address
 * @return 0 if correct -1 if not word address error, -2 if out of page error
 */
int writePosicionMemory(struct memoryPosition *pos, int i){
   GtkTreeModel *model= GTK_TREE_MODEL(modelMEMORY);
   GtkTreeIter iter;
   char iterstring[50];
   //if not word address return error
   if(i%(cpu.word_width/8)!=0){
      return -1;
   }
   //if out of page return error
   if(i<memory.page_base_address || i>(memory.page_base_address+memory.page_size)){
      return -2;
   }
   //get the table row from the memory address
   i=(i-memory.page_base_address)/(cpu.word_width/8);   
   sprintf(iterstring, "%d", i);	
   char a[100];
   char b[100];
   void* c=NULL;
   sprintf(a, "%0*lx", (int)cpu.address_width/4, (*pos).address);
   sprintf(b, "%0*lx", (int)cpu.word_width/4, (*pos).content);
   c=(*pos).user_content;
   gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(model),
         &iter,
         iterstring);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         ADDRESS, &a,
         CONTENT, &b,
         USER_CONTENT, c,
         -1);
}
/**
 * This function colours a memory position
 * @param i memory address
 * @param color. This is an enum type. It tell the function which color to use.
 */
int setColorMemoryPosition(int i, int color){
   GtkTreeModel *model= GTK_TREE_MODEL(modelMEMORY);
   GtkTreeIter iter;
   char iterstring[50];
   //if not word address return error
   if(i%(cpu.word_width/8)!=0){
      return -1;
   }
   //if out of page return error
   if(i<memory.page_base_address || i>(memory.page_base_address+memory.page_size)){
      return -2;
   }
   //get the table row from the memory address
   i=(i-memory.page_base_address)/(cpu.word_width/8);   
   sprintf(iterstring, "%d", i);	
   gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(model),
         &iter,
         iterstring);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         COLOR, colores[color],
         -1);
}
/**
 * This function colours a data cache line
 * @param level which will be coloured.
 * @param i memory address
 * @param color. This is an enum type. It tells the function which color to use.
 */
void setColorDataCacheLine(int level, int i, int color){
   GtkTreeModel *model= GTK_TREE_MODEL(cacheLevels[level].modelData);
   GtkTreeIter iter;
   char iterstring[50];
   sprintf(iterstring, "%d", i);	
   gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(model),
         &iter,
         iterstring);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         COLOR_CACHE, colores[color],
         -1);
}
/**
 * This function colours a data cache line
 * @param level which will be coloured.
 * @param i memory address
 * @param color. This is an enum type. It tells the function which color to use.
 */
void setColorCacheLine(int level, int i, int color){
   setColorDataCacheLine(level, i, color);
}
/**
 * This function colours a Instruction cache line
 * @param level which will be coloured.
 * @param i memory address
 * @param color. This is an enum type. It tells the function which color to use.
 */
void setColorInstructionsCacheLine(int level, int i, int color){
   GtkTreeModel *model= GTK_TREE_MODEL(cacheLevels[level].modelInstruction);
   GtkTreeIter iter;
   char iterstring[50];
   sprintf(iterstring, "%d", i);	
   gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(model),
         &iter,
         iterstring);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         COLOR_CACHE, colores[color],
         -1);
}
/**
 * This function sets a memory word position to its initial value
 * @param positionIndex to reset
 */
void writeBlankMemoryPosition(long positionIndex){
   struct memoryPosition pos;
   pos.address=positionIndex*(cpu.word_width/8);
   pos.content=0;
   printf("%ld\n", positionIndex );
   writePosicionMemory(&pos, positionIndex);
}
/**
 * This function sets a cache line to its initial value
 * @param level to be written
 * @param line to be written
 */
void writeBlankCacheLine(int level, long line){
   writeBlankDataCacheLine(level, line);
}
/**
 * This function sets a data cache line to its initial value
 * @param level to be written
 * @param line to be written
 */
void writeBlankDataCacheLine(int level, long line){
   int numLines=caches[level].size/caches[level].line_size;
   int asociativity=caches[level].asociativity;
   int numsets=numLines/asociativity;
   int numWords=(caches[level].line_size*8)/cpu.word_width;
   if(caches[level].separated){
      numLines/=2;
   }
   struct cacheLine cacheLineAdd;
   cacheLineAdd.line=line;
   cacheLineAdd.set=line/asociativity;
   cacheLineAdd.tag=0;
   cacheLineAdd.dirty=0;
   cacheLineAdd.valid=0;
   cacheLineAdd.last_accessed=0;
   cacheLineAdd.times_accessed=0;
   long lineContent[numWords];
   for(int i=0; i<numWords; i++){
      lineContent[i]=0;	
   }
   cacheLineAdd.content=lineContent;
   cacheLineAdd.user_content=NULL;
   writeLineCacheData(level, &cacheLineAdd, line);
}
/**
 * This function sets a Instruction cache line to its initial value
 * @param level to be written
 * @param line to be written
 */
void writeBlankInstructionCacheLine(int level, long line){
   int numLines=caches[level].size/caches[level].line_size;
   int asociativity=caches[level].asociativity;
   int numsets=numLines/asociativity;
   int numWords=(caches[level].line_size*8)/cpu.word_width;
   if(caches[level].separated){
      numLines/=2;
   }
   struct cacheLine cacheLineAdd;
   cacheLineAdd.line=line;
   cacheLineAdd.set=line/asociativity;
   cacheLineAdd.tag=0;
   cacheLineAdd.dirty=0;
   cacheLineAdd.valid=0;
   cacheLineAdd.last_accessed=0;
   cacheLineAdd.times_accessed=0;
   long lineContent[numWords];
   for(int i=0; i<numWords; i++){
      lineContent[i]=0;	
   }
   cacheLineAdd.content=lineContent;
   cacheLineAdd.user_content=NULL;
   writeLineCacheInstructions(level, &cacheLineAdd, line);
}
/**
 * This function is used to add a property or value to the simulation statistics panel
 * @param component String containig the name of the componet
 * @param property String containig the name of the component's property
 * @param value String containing the value which that property will be setted to.
 */
void setStatistics(char* component, char* property, char* value){
   GtkTreeModel *tree_model=estatistics_model;
   GtkTreeIter iter;
   int isntEmpty=gtk_tree_model_get_iter_first (tree_model, &iter);
   int hasNext=1;
   int componentExists=0;
   //Search for the memory hierarchy componet
   while(hasNext&&isntEmpty){
      char *name;
      gtk_tree_model_get (tree_model, &iter,
            COMPONET_OR_PROPERTY, &name, -1);
      //found
      if(!strcmp(component, name)){
         componentExists=1;
         break;
      }
      hasNext=gtk_tree_model_iter_next (tree_model, &iter);
   }
   //If the componet exists I search for the property
   if(componentExists){
      GtkTreeIter child;
      int hasChildren=gtk_tree_model_iter_children (tree_model,
            &child,
            &iter);
      int hasNext=1;
      int propertyExists=0;
      //Search for the componets property
      while(hasNext&&hasChildren){
         char *name;
         gtk_tree_model_get (tree_model, &child,
               COMPONET_OR_PROPERTY, &name, -1);
         //found
         if(!strcmp(property, name)){
            propertyExists=1;
            break;
         }
         hasNext=gtk_tree_model_iter_next (tree_model, &child);
      }
      //If the componet's property exists I set the value
      if(propertyExists){
         gtk_tree_store_set(GTK_TREE_STORE(tree_model), &child,
               //COMPONET_OR_PROPERTY, "probando",
               VALUE, value,
               -1);
         //If the componet's property doesn't exist I add the property and set the value
      }else{
         gtk_tree_store_append(GTK_TREE_STORE(tree_model), &child, &iter);
         gtk_tree_store_set(GTK_TREE_STORE(tree_model), &child,
               COMPONET_OR_PROPERTY, property,
               VALUE, value,
               -1);
      }
      //If the componet doesn't exist I create the componet and the property and I set the value
   }else{
      GtkTreeIter child;
      gtk_tree_store_append(GTK_TREE_STORE(tree_model), &iter, NULL);
      gtk_tree_store_set(GTK_TREE_STORE(tree_model), &iter,
            COMPONET_OR_PROPERTY, component,
            -1);
      gtk_tree_model_iter_children (tree_model,
            &child,
            &iter);
      gtk_tree_store_append(GTK_TREE_STORE(tree_model), &child, &iter);
      gtk_tree_store_set(GTK_TREE_STORE(tree_model), &child,
            COMPONET_OR_PROPERTY, property,
            VALUE, value,
            -1);
   }
}
/**
 * This function is used to read a value from the simulation statistics panel
 * @param component String containig the name of the componet
 * @param property String containig the name of the component's property
 * @return String containing th value
 */
char* getStatistics(char* component, char* property){
   GtkTreeModel *tree_model=estatistics_model;
   GtkTreeIter iter;
   gtk_tree_model_get_iter_first (tree_model, &iter);
   int hasNext=1;
   int componentExists=0;
   //Search for the memory hierarchy componet
   while(hasNext){
      char *name;
      gtk_tree_model_get (tree_model, &iter,
            COMPONET_OR_PROPERTY, &name, -1);
      //found
      if(!strcmp(component, name)){
         componentExists=1;
         break;
      }
      hasNext=gtk_tree_model_iter_next (tree_model, &iter);
   }
   //If the componet exists I search for the property
   if(componentExists){
      GtkTreeIter child;
      gtk_tree_model_iter_children (tree_model,
            &child,
            &iter);
      int hasNext=1;
      int propertyExists=0;
      //Search for the componet's property
      while(hasNext){
         char *name;
         gtk_tree_model_get (tree_model, &child,
               COMPONET_OR_PROPERTY, &name, -1);
         //found
         if(!strcmp(property, name)){
            propertyExists=1;
            break;
         }
         hasNext=gtk_tree_model_iter_next (tree_model, &child);
      }
      //If the componet's property exists I get the value
      if(propertyExists){
         char* value;
         gtk_tree_model_get(GTK_TREE_MODEL(tree_model), &child,
               //COMPONET_OR_PROPERTY, "probando",
               VALUE, &value,
               -1);
         return value;
      }
   }
   //If the componet or the property don't exist I create the componet and the property and I set the value return param to NULL
   return NULL;
}
