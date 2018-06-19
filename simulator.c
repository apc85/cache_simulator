#include <stdio.h>
#include <string.h>
#include <math.h>
#include "confparser.h"
#include "traceparser.h"
#include "datainterface.h"
#include "simulator.h"

/* Private functions */
void incrementDoubleStatistics(char *component, char *property, double value);
void incrementIntegerStatistics(char *component, char *property, int value);
/*
struct memOperation{
  int hasBreakPoint;
  int instructionOrData;
  long address;
  int operationType;
  int size;
  long data;
}; */

struct response_type {
   double time;
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

   printf("Simulating operation: ");
   printMemOperation(stdout, operation);
   
   for(int cacheLevel=0; cacheLevel< numberCaches; cacheLevel++){
      unsigned tag = response.address >> (caches[cacheLevel].offsetBits+caches[cacheLevel].setBits);
      long line;
      response.time += caches[cacheLevel].access_time;
      if((line = findTagInCache(cacheLevel, tag)) > 0) {
         // Hit
         printf(">   Cache L%d: Hit\n", cacheLevel+1);
      } else {
         // Miss
         printf(">   Cache L%d: Miss 2^%d-1 = %f\n", cacheLevel+1,caches[cacheLevel].offsetBits, pow(2,caches[cacheLevel].offsetBits)-1);
         if(operation->operationType == LOAD) {
            response.size = caches[cacheLevel].numWords;
            response.address &= -1 << caches[cacheLevel].offsetBits;
            free(response.data);
            response.data = malloc((sizeof(unsigned))*caches[cacheLevel].numWords);
         } else {
            // Assuming WriteThrough and WriteNoAllocate
            
         }
      }
   }

   if(operation->operationType == LOAD) {
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

   for(int cacheLevel = numberCaches-1; cacheLevel >= 0; cacheLevel--){
      unsigned tag = response.address >> (caches[cacheLevel].offsetBits+caches[cacheLevel].setBits);
      unsigned set = (response.address >> caches[cacheLevel].offsetBits) & ((1 << caches[cacheLevel].setBits)-1);
      long line;
      if((line = findTagInCache(cacheLevel, tag)) > 0) {
         // Hit
         printf("<   Cache L%d: Hit\n", cacheLevel+1);
      } else {
         // Miss
         printf("<   Cache L%d: Miss\n", cacheLevel+1);
         if(operation->operationType == LOAD) {
            struct cacheLine cacheData;
            cacheData.dirty = 1;
            cacheData.valid = 1;
            cacheData.tag = tag;
            cacheData.content = response.data;
            writeCacheLine(cacheLevel, &cacheData, set*caches[cacheLevel].asociativity);
         } else {
            // Assuming WriteThrough and WriteNoAllocate
            
         }
      }
   }
   free(response.data);
   incrementIntegerStatistics("Totals", "Accesses", 1);
   incrementDoubleStatistics("Totals", "Access time", response.time);
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

