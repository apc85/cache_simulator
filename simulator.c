#include <stdio.h>
#include <string.h>
#include <math.h>
#include "confparser.h"
#include "datainterface.h"
#include "datamanipulation.h"
#include "simulator.h"

unsigned long cycle = 0;

/* Private functions */
void incrementDoubleStatistics(char *component, char *property, double value);
void incrementIntegerStatistics(char *component, char *property, int value);
int selectVia(int instructionOrData, int cacheLevel, int set);

struct response_type {
   double time;
   int resolved;
   unsigned address;
   unsigned size;
   unsigned *data;
};

void simulate_step(struct memOperation *operation) {
   struct response_type response;
   response.size = operation->size/4;
   response.address = operation->address;
   response.time = 0.0;
   response.data = malloc((sizeof(unsigned)));
   response.data[0] = operation->data;
   response.resolved = -1;

   printf("Simulating operation: ");
   printMemOperation(stdout, operation);
   
   for(int cacheLevel=0; cacheLevel< numberCaches; cacheLevel++){
      unsigned tag = response.address >> (caches[cacheLevel].offsetBits+caches[cacheLevel].setBits);
      unsigned set = (response.address >> caches[cacheLevel].offsetBits) & ((1 << caches[cacheLevel].setBits)-1);
      unsigned offset = ( response.address & ((1 << caches[cacheLevel].offsetBits)-1) ) >> 2;
      long line;
      response.time += caches[cacheLevel].access_time;
      if((line = findTagInCache(operation->instructionOrData, cacheLevel, set, tag)) != -1) {
         // Hit
         printf(">   Cache L%d: Hit (%ld)\n", cacheLevel+1, line);
         struct cacheLine cacheData;
         cacheData.content = malloc((sizeof(long))*caches[cacheLevel].numWords);
         readLineFromCache(operation->instructionOrData, cacheLevel, &cacheData, line);
         if(response.size == 1) {
            response.data[0] = cacheData.content[offset];
            printf("Will get %d -> %d\n",offset, response.data[0]);

         } else {
         }
         response.resolved = cacheLevel;
         break;
      } else {
         // Miss
         printf(">   Cache L%d: Miss 2^%d-1 = %f\n", cacheLevel+1,caches[cacheLevel].offsetBits, pow(2,caches[cacheLevel].offsetBits)-1);
         if(operation->operation == LOAD) {
            response.size = caches[cacheLevel].numWords;
            response.address &= -1 << caches[cacheLevel].offsetBits;
            free(response.data);
            response.data = malloc((sizeof(unsigned))*caches[cacheLevel].numWords);
         } else {
            // Assuming WriteThrough and WriteNoAllocate
            
         }
      }
   }

   if(response.resolved < 0) {
      response.resolved = numberCaches;
      if(operation->operation == LOAD) {
         struct memoryPosition pos;
         response.time += memory.access_time_1;
         for(unsigned i=0, address=response.address; i < response.size; i++, address+=cpu.word_width/8) {
            if(readMemoryAddress(&pos, address) < 0) {
               fprintf(stderr, "error in simulation: %s addr:%x\n", interfaceError, address);
               return;
            }
            response.data[i] = pos.content;
         }
      } else { 
         struct memoryPosition pos;
         pos.address = operation->address;
         pos.content = operation->data;
         response.time += memory.access_time_1;
         for(unsigned i=0, address=response.address; i < response.size; i++, address+=cpu.word_width/8) {
            pos.content = response.data[i];
            if(writeMemoryAddress(&pos, address) < 0) {
               fprintf(stderr, "error in simulation: %s addr:%x\n", interfaceError, address);
               return;
            }
         }
      }
   }

   for(int cacheLevel = response.resolved-1; cacheLevel >= 0; cacheLevel--){
      unsigned tag = response.address >> (caches[cacheLevel].offsetBits+caches[cacheLevel].setBits);
      unsigned set = (response.address >> caches[cacheLevel].offsetBits) & ((1 << caches[cacheLevel].setBits)-1);
      long line;
      if((line = findTagInCache(operation->instructionOrData, cacheLevel, tag, set)) > 0) {
         // Hit
         printf("<   Cache L%d: Hit\n", cacheLevel+1);
      } else {
         // Miss
         printf("<   Cache L%d: Miss\n", cacheLevel+1);
         if(operation->operation == LOAD) {
            struct cacheLine cacheData;
            cacheData.dirty = 1;
            cacheData.valid = 1;
            cacheData.tag = tag;
            cacheData.content = response.data;
            int via = selectVia(operation->instructionOrData, cacheLevel, set);
            writeLineToCache(operation->instructionOrData, cacheLevel, &cacheData, via);
         } else {
            // Assuming WriteThrough and WriteNoAllocate
            
         }
      }
   }
   printf("Got %d\n",response.data[0]);
   free(response.data);
   incrementIntegerStatistics("Totals", "Accesses", 1);
   incrementDoubleStatistics("Totals", "Access time", response.time);
   cycle++;
}

int selectVia(int instructionOrData, int cacheLevel, int set) {
   struct cacheLine cacheData;
   int lruLine = -1;
   int lruTime = -1;
   int lfuLine = -1;
   int lfuCount = -1;
   int fifoLine = -1;
   int fifoTime = -1;
   int firstLine = set*caches[cacheLevel].asociativity;
   for(int i = 0, via = rand() % caches[cacheLevel].asociativity; i < caches[cacheLevel].asociativity; i++, via=(via+1) % caches[cacheLevel].asociativity) {
      int line = firstLine + via;
      readFlagsFromCache(instructionOrData, cacheLevel, &cacheData, line);
      if(cacheData.valid == 0)
         return line;
      if(lruLine == -1 || lruTime > cacheData.lastAccess) {
         lruLine = line;
         lruTime = cacheData.lastAccess;
      }
      if(lfuLine == -1 || lfuCount > cacheData.accessCount) {
         lfuLine = line;
         lfuCount = cacheData.accessCount;
      }
      if(fifoLine == -1 || fifoTime > cacheData.firstAccess) {
         fifoLine = line;
         fifoTime = cacheData.firstAccess;
      }
   }
   // LRU=0, LFU=1, RANDOM=2, FIFO=3
   if(caches[cacheLevel].replacement == RANDOM) {
      return set * caches[cacheLevel].asociativity
             + rand() % caches[cacheLevel].asociativity;
   }
   else if(caches[cacheLevel].replacement == LRU) {
      return lruLine;
   }
   else if(caches[cacheLevel].replacement == LFU) {
      return lfuLine;
   }
   else if(caches[cacheLevel].replacement == FIFO) {
      return fifoLine;
   }
   else
      return set * caches[cacheLevel].asociativity;
}

void incrementDoubleStatistics(char *component, char *property, double value) {
   double oldValue = 0.0;
   char *oldValueString = getStatistics(component,property);
   if(oldValueString) 
      oldValue = strtod(oldValueString, NULL);
   char tmp[20];
   sprintf(tmp, "%lf", oldValue+value);
   setStatistics(component, property, tmp);
}

void incrementIntegerStatistics(char *component, char *property, int value) {
   int oldValue = 0.0;
   char *oldValueString = getStatistics(component,property);
   if(oldValueString) 
      oldValue = atoi(oldValueString);
   char tmp[20];
   sprintf(tmp, "%d", oldValue+value);
   setStatistics(component, property, tmp);
}

void simulate() {
   for(int i=0; i<numberOfOperations; i++){
      simulate_step(&memoryOperations[i]);
   }
}

