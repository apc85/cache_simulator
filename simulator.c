#include <stdio.h>
#include <string.h>
#include "traceparser.h"
#include "simulator.h"

void simulate_step(struct memOperation *operation) {
   printf("Simulating operation\n");
}

void simulate() {
   for(int i=0; i<numberOfOperations; i++){
      simulate_step(&memoryOperations[i]);
   }
}
