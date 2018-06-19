#include <stdio.h>
#include <string.h>
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

void simulate_step(struct memOperation *operation) {
   double time = 0.0;
   printf("Simulating operation: ");
   printMemOperation(stdout, operation);
   
   for(int i=0; i< numberCaches; i++){
      //createCacheModel(i);
   }

   if(operation->operationType == LOAD) {
      struct memoryPosition pos;
      time += memory.access_time_1;
      if(readMemoryAddress(&pos, operation->address) < 0) {
         fprintf(stderr, "error in simulation: %s\n", interfaceError);
         return;
      }
   } else { 
      struct memoryPosition pos;
      pos.address = operation->address;
      pos.content = operation->data;
      time += memory.access_time_1;
      if(writeMemoryAddress(&pos, operation->address) < 0) {
         fprintf(stderr, "error in simulation: %s\n", interfaceError);
         return;
      }
   }
   incrementIntegerStatistics("Totals", "Accesses", 1);
   incrementDoubleStatistics("Totals", "Access time", time);
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

