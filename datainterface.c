#include <math.h>
#include "datamanipulation.h"
#include "datainterface.h"
#include "simulator.h"

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

void resetCacheModel(GtkTreeModel *model,int level){
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
 * This function resets a Data cache. Sets the data cache to its initial state.
 * @param cache level where the cache is located. 
 */
void resetCache(int level){
   if(cacheLevels[level].modelData) 
      resetCacheModel(GTK_TREE_MODEL(cacheLevels[level].modelData),level);
   if(cacheLevels[level].modelInstruction) 
      resetCacheModel(GTK_TREE_MODEL(cacheLevels[level].modelInstruction),level);
}

/**
 * This function shows all the values contained in the fields of a data cache line
 * @param level which will be shown
 * @param i line index
 */
void showLineFromCache(int instructionOrData, int level, int i){
   struct cacheLine line;
   //first I read it
   readLineFromCache(instructionOrData, level, &line, i);
   char contentString[2000];
   contentArrayToString(line.content, contentString, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   //I print the values.
   printf("------------------------------------------------------\n");
   printf("line: %x     tag: %x    set: %x\n", line.line, line.tag, line.set);
   printf("content: %s\n", contentString);
   printf("user content: %s\n", (char*)line.user_content);
   printf("valid: %d   dirty: %d   last accessed: %d  times accessed: %d  first accessed: %d\n",
            line.valid, line.dirty, line.lastAccess, line.accessCount, line.firstAccess);
   printf("------------------------------------------------------\n");
   free(line.content);
}

long findTagInCache(int instructionOrData, int level, unsigned requestSet, unsigned requestTag) {
   GtkTreeModel *model;
   GtkTreeIter iter;

   if(caches[level].separated == 0 || instructionOrData == DATA) {
      model= GTK_TREE_MODEL(cacheLevels[level].modelData);
      printf("Data\n");
   }
   else {
      model= GTK_TREE_MODEL(cacheLevels[level].modelInstruction);
      printf("Instruction %d %d %d==%d\n",level, caches[level].separated, instructionOrData, DATA);
   }
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
void readLineFromCache(int instructionOrData, int level, struct cacheLine* line, int lineNumber){
   GtkTreeModel *model;
   GtkTreeView *view;
   GtkTreeIter iter;
   char *contentString;

   if(!caches[level].separated || instructionOrData == DATA) {
      model = GTK_TREE_MODEL(cacheLevels[level].modelData);
      view = GTK_TREE_VIEW(cacheLevelPanels[level].viewData);
      scrollDataCacheToRow(level, lineNumber);
   }
   else {
      model= GTK_TREE_MODEL(cacheLevels[level].modelInstruction);
      view = GTK_TREE_VIEW(cacheLevelPanels[level].viewInstruction);
      scrollInstructionCacheToRow(level, lineNumber);
   }
   gtk_tree_model_iter_nth_child (model, &iter, NULL, lineNumber);

   gtk_tree_model_get (GTK_TREE_MODEL(model), &iter,
         LINE, &line->line,
         TAG, &line->tag,
         SET, &line->set,
         CONTENT_CACHE, &contentString,
         USER_CONTENT_CACHE, &line->user_content,
         VALID, &line->valid,
         DIRTY, &line->dirty,
         TIMES_ACCESSED, &line->accessCount,
         LAST_ACCESSED, &line->lastAccess,
         FIRST_ACCESSED, &line->firstAccess,
         -1);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         COLOR_CACHE, colors[READ],
         TIMES_ACCESSED, line->accessCount+1,
         LAST_ACCESSED, cycle,
         -1);

   //number of words in a cache line
   line->content = malloc((sizeof(long))*caches[level].numWords);
   //turn String format to long array.
   contentStringToArray(line->content, contentString, caches[level].numWords);
   g_free(contentString);
}

void readFlagsFromCache(int instructionOrData, int level, struct cacheLine* line, int lineNumber){
   GtkTreeModel *model;
   GtkTreeIter iter;
   char *contentString;

   if(!caches[level].separated || instructionOrData == DATA)
      model= GTK_TREE_MODEL(cacheLevels[level].modelData);
   else
      model= GTK_TREE_MODEL(cacheLevels[level].modelInstruction);

   gtk_tree_model_iter_nth_child (model, &iter, NULL, lineNumber);

   gtk_tree_model_get (GTK_TREE_MODEL(model), &iter,
         LINE, &line->line,
         TAG, &line->tag,
         SET, &line->set,
         VALID, &line->valid,
         DIRTY, &line->dirty,
         TIMES_ACCESSED, &line->accessCount,
         LAST_ACCESSED, &line->lastAccess,
         FIRST_ACCESSED, &line->firstAccess,
         -1);
}
/**
 * This function writes a cache line.
 * @param level which will be written
 * @param line. A pointer to a struct cacheLine containing the data to be written.
 * @param i line index
 */
void writeLineToCache(int instructionOrData, int level, struct cacheLine *line, unsigned lineNumber) {
   GtkTreeModel *model;
   GtkTreeView *view;
   GtkTreeIter iter;
   char contentString[2000];

   if(!caches[level].separated || instructionOrData == DATA) {
      model = GTK_TREE_MODEL(cacheLevels[level].modelData);
      view = GTK_TREE_VIEW(cacheLevelPanels[level].viewData);
      scrollDataCacheToRow(level, lineNumber);
   }
   else {
      model= GTK_TREE_MODEL(cacheLevels[level].modelInstruction);
      view = GTK_TREE_VIEW(cacheLevelPanels[level].viewInstruction);
      scrollInstructionCacheToRow(level, lineNumber);
   }
   contentArrayToString(line->content, contentString, (caches[level].line_size*8)/cpu.word_width, cpu.word_width/4);
   printf("Write Content: ");
   for(int i = 0; i < caches[level].numWords; i++) {
      printf("%x ",line->content[i]);
   }
   printf(" ---> %s\n", contentString);
   gtk_tree_model_iter_nth_child (model, &iter, NULL, lineNumber);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         VALID, line->valid,
         DIRTY, line->dirty,
         TAG, line->tag,
         CONTENT_CACHE, contentString, 
         COLOR_CACHE, colors[WRITE],
         TIMES_ACCESSED, 1,
         LAST_ACCESSED, cycle,
         FIRST_ACCESSED, cycle,
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

   scrollMemoryToPos(address);
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
   scrollMemoryToPos(address);
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
      int hasNextProperty=gtk_tree_model_iter_children(tree_model,
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

