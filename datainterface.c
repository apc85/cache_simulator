#include <math.h>
#include "datamanipulation.h"
#include "datainterface.h"

//colors defined to be used in the program
const char* const colors[] = { "green", "red", "yellow", "green", "red", "grey", "orange", "purple", "pink", "lightblue", "lightgreen", "white"};

char *interfaceError = NULL;

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
   for(unsigned long i=memory.page_base_address; i<memory.page_base_address+memory.page_size; i+=(cpu.word_width/8)){
      /* Fill fields with data */
      gtk_list_store_set (modelMEMORY, &iter,
            CONTENT, 0,
            ADDRESS, i,
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
   GtkTreeModel *model= GTK_TREE_MODEL(cacheLevels[level].modelData);
   //this is for filling each field with the correct amount of bits it must be represented with.
   struct cacheLine* line;
   GtkTreeIter iter;
   //get iter at first position
   gtk_tree_model_get_iter_first (model, &iter);
   //set each field to its initial value
   unsigned cache_content[caches[level].numWords];
   for(int i=0; i<caches[level].numWords; i++){
      cache_content[i]=0;
   }
   //this converts form long array to string representation.
   char cache_content_char[2000];
   contentArrayToString(cache_content, cache_content_char, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   //I write all the reseted fields in each cache line
   for(int i=0; i<caches[level].numLines; i++){ 
      gtk_list_store_set (GTK_LIST_STORE(model), &iter,
            LINE, i,
            TAG, 0,
            SET, i/caches[level].asociativity,
            CONTENT_CACHE, cache_content_char,
            USER_CONTENT_CACHE, NULL,
            VALID, 0,
            DIRTY, 0,
            TIMES_ACCESSED, 0,
            LAST_ACCESSED, 0,
            FIRST_ACCESSED, 0,
            COLOR_CACHE, colors[WHITE],
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
   GtkTreeModel *model= GTK_TREE_MODEL(cacheLevels[level].modelInstruction);
   //this is for filling each field with the correct amount of bits it must be represented with.
   struct cacheLine* line;
   GtkTreeIter iter;
   //get iter at first position
   gtk_tree_model_get_iter_first (model, &iter);
   //set each field to its initial value
   unsigned cache_content[caches[level].numWords];
   for(int i=0; i<caches[level].numWords; i++){
      cache_content[i]=0;
   }
   //this converts form long array to string representation.
   char cache_content_char[2000];
   contentArrayToString(cache_content, cache_content_char, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   //I write all the reseted fields in each cache line
   for(int i=0; i<caches[level].numLines; i++){ 
      gtk_list_store_set (GTK_LIST_STORE(model), &iter,
            LINE, i,
            TAG, 0,
            SET, i/caches[level].asociativity,
            CONTENT_CACHE, cache_content_char,
            USER_CONTENT_CACHE, NULL,
            VALID, 0,
            DIRTY, 0,
            TIMES_ACCESSED, 0,
            LAST_ACCESSED, 0,
            FIRST_ACCESSED, 0,
            COLOR_CACHE, colors[WHITE],
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
   contentArrayToString(line.content, contentString, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   //I print the values.
   printf("------------------------------------------------------\n");
   printf("line: %x     tag: %x    set: %x\n", line.line, line.tag, line.set);
   printf("content: %s\n", contentString);
   printf("user content: %s\n", (char*)line.user_content);
   printf("valid: %d   dirty: %d   last accessed: %d  times accessed: %d  first accessed: %d\n", line.valid, line.dirty, line.last_accessed, line.times_accessed, line.first_accessed);
   printf("------------------------------------------------------\n");
   free(line.content);
}

long findTagInCache(int level, unsigned requestSet, unsigned requestTag) {
   GtkTreeModel *model= GTK_TREE_MODEL(cacheLevels[level].modelData);
   GtkTreeIter iter;
   unsigned set,tag,valid,via=0;
   // Get first via in this set
   int more = gtk_tree_model_iter_nth_child (model, &iter, NULL, requestSet*caches[level].asociativity);

   while (more && via < caches[level].asociativity)
   {
      gtk_tree_model_get (GTK_TREE_MODEL(model), &iter, SET, &set, TAG, &tag, VALID, &valid, -1);
      if(valid && tag == requestTag)
         return requestSet * caches[level].asociativity + via;
      // Get next via
      via++;
      more = gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &iter);
   } 
   return -1;
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
   char *contentString;
   gtk_tree_model_iter_nth_child (model, &iter, NULL, i);

   gtk_tree_model_get (GTK_TREE_MODEL(model), &iter,
         LINE, &line->line,
         TAG, &line->tag,
         SET, &line->set,
         CONTENT_CACHE, &contentString,
         USER_CONTENT_CACHE, &line->user_content,
         VALID, &line->valid,
         DIRTY, &line->dirty,
         TIMES_ACCESSED, &line->times_accessed,
         LAST_ACCESSED, &line->last_accessed,
         FIRST_ACCESSED, &line->first_accessed,
         -1);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         COLOR_CACHE, colors[READ],
         -1);
   //number of words in a cache line
   line->content = malloc((sizeof(long))*caches[level].numWords);
   //turn String format to long array.
   contentStringToArray(line->content, contentString, caches[level].numWords);
   g_free(contentString);
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
   char *contentString;
   contentArrayToString(line->content, contentString, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   gtk_tree_model_iter_nth_child (model, &iter, NULL, i);
   gtk_tree_model_get (GTK_TREE_MODEL(model), &iter,
         LINE, &line->line,
         TAG, &line->tag,
         SET, &line->set,
         CONTENT_CACHE, &contentString,
         USER_CONTENT_CACHE, &line->user_content,
         VALID, &line->valid,
         DIRTY, &line->dirty,
         TIMES_ACCESSED, &line->times_accessed,
         LAST_ACCESSED, &line->last_accessed,
         FIRST_ACCESSED, &line->first_accessed,
         -1);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         COLOR_CACHE, colors[READ],
         -1);
   //number of words in a cache line
   line->content = malloc((sizeof(unsigned))*caches[level].numWords);
   //turn String format to long array.
   contentStringToArray(line->content, contentString, caches[level].numWords);
   g_free(contentString);
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
   contentArrayToString(line.content, contentString, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   //I print the values of each field
   printf("------------------------------------------------------\n");
   printf("line: %x     tag: %x    set: %x\n", line.line, line.tag, line.set);
   printf("content:%s\n", contentString);
   printf("user content: %s\n", (char*)line.user_content);
   printf("valid: %d   dirty: %d   last accessed: %d  times accessed: %d  first accessed: %d\n", line.valid, line.dirty, line.last_accessed, line.times_accessed, line.first_accessed);
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

void writeCacheLine(int level, struct cacheLine *line, unsigned line_number) {
   GtkTreeModel *model= GTK_TREE_MODEL(cacheLevels[level].modelData);
   GtkTreeIter iter;
   char contentString[2000];
   contentArrayToString(line->content, contentString, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   printf("Write Content: ");
   for(int i = 0; i < caches[level].numWords; i++) {
      printf("%x ",line->content[i]);
   }
   printf(" ---> %s\n", contentString);
   gtk_tree_model_iter_nth_child (model, &iter, NULL, line_number);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         VALID, line->valid,
         DIRTY, line->dirty,
         TAG, line->tag,
         CONTENT_CACHE, contentString, 
         COLOR_CACHE, colors[WRITE],
         -1);
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
   char contentString[2000];
   contentArrayToString(line->content, contentString, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   gtk_tree_model_iter_nth_child (model, &iter, NULL, i);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         LINE, line->line,
         TAG, line->tag,
         SET, line->set,
         CONTENT_CACHE, contentString,
         USER_CONTENT_CACHE, line->user_content,
         VALID, line->valid,
         DIRTY, line->dirty,
         TIMES_ACCESSED, line->times_accessed,
         LAST_ACCESSED, line->last_accessed,
         FIRST_ACCESSED, line->first_accessed,
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
   char contentString[2000];
   contentArrayToString(line->content, contentString, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   gtk_tree_model_iter_nth_child (model, &iter, NULL, i);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         LINE, line->line,
         TAG, line->tag,
         SET, line->set,
         CONTENT_CACHE, contentString,
         USER_CONTENT_CACHE, line->user_content,
         VALID, line->valid,
         DIRTY, line->dirty,
         TIMES_ACCESSED, line->times_accessed,
         LAST_ACCESSED, line->last_accessed,
         FIRST_ACCESSED, line->first_accessed,
         -1);
}
/**
 * This function shows a memory position
 * @param address is the memory address
 * @return 0 if correct -1 if not word address error, -2 if out of page error
 */
int showMemoryAddress(long address){
   struct memoryPosition pos;
   //I read the memory position
   int returned=readMemoryAddress(&pos, address);
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
 * @param pos. Read data will be placed in here. User must take care of freeing pos.user_content
 * @param address is the memory address
 * @return 0 if correct -1 if not word address error, -2 if out of page error
 */
int readMemoryAddress(struct memoryPosition *pos, long address){
   GtkTreeModel *model= GTK_TREE_MODEL(modelMEMORY);
   GtkTreeIter iter;
   // if not word address return error
   if(address % (cpu.word_width/8)!=0){
      interfaceError = "not word address";
      return -1;
   }
   // if out of page return error
   if(address <memory.page_base_address || address >(memory.page_base_address+memory.page_size)){
      interfaceError = "out of page";
      return -2;
   }
   // get the table row from the memory address
   gtk_tree_model_iter_nth_child (model,
         &iter,
         NULL,
         (address-memory.page_base_address)/(cpu.word_width/8));
   gtk_tree_model_get (model, &iter,
         ADDRESS, &pos->address,
         CONTENT, &pos->content,
         USER_CONTENT, &pos->user_content,
         -1);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         COLOR, colors[READ],
         -1);
   return 0;
}
/**
 * This function writes a memory position.
 * @param pos. Data to be written will be read from this struct.
 * @param address is the memory address
 * @return 0 if correct -1 if not word address error, -2 if out of page error
 */
int writeMemoryAddress(struct memoryPosition *pos, long address){
   GtkTreeModel *model= GTK_TREE_MODEL(modelMEMORY);
   GtkTreeIter iter;
   // if not word address return error
   if(address % (cpu.word_width/8)!=0){
      interfaceError = "not word address";
      return -1;
   }
   //if out of page return error
   if(address < memory.page_base_address || address > (memory.page_base_address+memory.page_size)){
      interfaceError = "out of page";
      return -2;
   }
   //get the table row from the memory address
   gtk_tree_model_iter_nth_child (model,
         &iter,
         NULL,
         (address-memory.page_base_address)/(cpu.word_width/8));
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         ADDRESS, pos->address,
         CONTENT, pos->content,
         USER_CONTENT, pos->user_content,
         COLOR, colors[WRITE],
         -1);
   return 0;
}
/**
 * This function colours a memory position
 * @param address is the memory address
 * @param color. This is an enum type. It tell the function which color to use.
 */
int setMemoryAddressColor(long address, int color){
   GtkTreeModel *model= GTK_TREE_MODEL(modelMEMORY);
   GtkTreeIter iter;
   char iterstring[50];
   //if not word address return error
   if(address % (cpu.word_width/8) != 0){
      interfaceError = "not word address";
      return -1;
   }
   //if out of page return error
   if(address <memory.page_base_address || address >(memory.page_base_address+memory.page_size)){
      interfaceError = "out of page";
      return -2;
   }
   //get the table row from the memory address
   sprintf(iterstring, "%ld", (address-memory.page_base_address)/(cpu.word_width/8));	
   gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(model),
         &iter,
         iterstring);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         COLOR, colors[color],
         -1);
   return 0;
}
/**
 * This function sets a memory word position to its initial value
 * @param address is the memory address
 */
void clearMemoryAddress(long address){
   struct memoryPosition pos;
   pos.address=address * cpu.word_width/8;
   pos.content=0;
   writeMemoryAddress(&pos, address);
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
         COLOR_CACHE, colors[color],
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
         COLOR_CACHE, colors[color],
         -1);
}
/**
 * This function sets a cache line to its initial value
 * @param level to be written
 * @param line to be written
 */
void writeBlankCacheLine(int level, unsigned line){
   writeBlankDataCacheLine(level, line);
}

/**
 * This function sets a data cache line to its initial value
 * @param level to be written
 * @param line to be written
 */
void writeBlankDataCacheLine(int level, unsigned line){
   
   struct cacheLine cacheLineAdd;
   cacheLineAdd.line=line;
   cacheLineAdd.set=line/(caches[level].asociativity);
   cacheLineAdd.tag=0;
   cacheLineAdd.dirty=0;
   cacheLineAdd.valid=0;
   cacheLineAdd.last_accessed=0;
   cacheLineAdd.times_accessed=0;
   cacheLineAdd.first_accessed=0;
   unsigned lineContent[caches[level].numWords];
   for(unsigned i=0; i<caches[level].numWords; i++){
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
void writeBlankInstructionCacheLine(int level, unsigned line){
   
   struct cacheLine cacheLineAdd;
   cacheLineAdd.line=line;
   cacheLineAdd.set=line/(caches[level].asociativity);
   cacheLineAdd.tag=0;
   cacheLineAdd.dirty=0;
   cacheLineAdd.valid=0;
   cacheLineAdd.last_accessed=0;
   cacheLineAdd.times_accessed=0;
   cacheLineAdd.first_accessed=0;
   unsigned lineContent[caches[level].numWords];
   for(unsigned i=0; i<caches[level].numWords; i++){
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
   GtkTreeModel *tree_model=statistics_model;
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
   GtkTreeModel *tree_model=statistics_model;
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

/**
 * This function is used to print simulation statistics panel
 * @param fp file to where it will be printed
 */
void printStatistics(FILE* fp) {
   fprintf(fp, "\n------SIMULATION STATISTICS------\n\n");   
   GtkTreeModel *tree_model=statistics_model;
   GtkTreeIter iter;
   gtk_tree_model_get_iter_first (tree_model, &iter);
   int hasNext=1;
   int componentExists=0;
   //Search for the memory hierarchy componet
   while(hasNext){
      char *name;
      gtk_tree_model_get (tree_model, &iter,
            COMPONET_OR_PROPERTY, &name, -1);
      //print title
      printf("%s\n", name);
     

      //print children

      GtkTreeIter child;
      int hasNextProperty=gtk_tree_model_iter_children (tree_model,
            &child,
            &iter);

      

      while(hasNextProperty){
         char *name;
         char *value;
         gtk_tree_model_get (tree_model, &child,
               COMPONET_OR_PROPERTY, &name, -1);
         gtk_tree_model_get (tree_model, &child,
               VALUE, &value, -1);

         printf("        %s: %s\n", name, value);
         hasNextProperty=gtk_tree_model_iter_next (tree_model, &child);
      }



      hasNext=gtk_tree_model_iter_next (tree_model, &iter);
   }
   //If the componet or the property don't exist I create the componet and the property and I set the value return param to NULL
   
}


/**
 * This function is used to remove all the colors from the cache and memory tables
 */
void removeAllColors(){

   //remove colors from all caches
   for(int i=0; i<numberCaches; i++){

       GtkTreeModel *model= GTK_TREE_MODEL(cacheLevels[i].modelData);
       GtkTreeIter iter;
       int hasNext= gtk_tree_model_get_iter_first (model, &iter);

       while(hasNext){
           gtk_list_store_set (GTK_LIST_STORE(model), &iter, COLOR_CACHE, colors[WHITE], -1);
           hasNext=gtk_tree_model_iter_next (model, &iter);
       }

       if(caches[i].separated){

           model= GTK_TREE_MODEL(cacheLevels[i].modelInstruction);
           hasNext= gtk_tree_model_get_iter_first (model, &iter);

           while(hasNext){
              gtk_list_store_set (GTK_LIST_STORE(model), &iter, COLOR_CACHE, colors[WHITE], -1);
              hasNext=gtk_tree_model_iter_next (model, &iter);
           }
       }
   }

   //remove colors from memory
   GtkTreeIter iter;
   int hasNext= gtk_tree_model_get_iter_first (GTK_TREE_MODEL(modelMEMORY), &iter);
   while(hasNext){
   	gtk_list_store_set (GTK_LIST_STORE(modelMEMORY), &iter, COLOR, colors[WHITE], -1);
   	hasNext=gtk_tree_model_iter_next (GTK_TREE_MODEL(modelMEMORY), &iter);
   }

}

