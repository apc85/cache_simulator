#ifndef TRACEPARSER_H
#define TRACEPARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interfaz.h"
#include "datamanipulation.h"

int numLines;
int numOperations;
int mallocated;

enum instructionOrData {DATA=1, INSTRUCTION=2};
enum loadOrStore {LOAD=3, STORE=4};

#define DEFAULT_SIZE 0;
#define DEFAULT_DATA 0;

struct memOperation{
  int hasBreakPoint;
  int instructionOrData;
  long address;
  int operationType;
  int size;
  long data;
};

//array que almacena las operations de memory leidas del file de trace
struct memOperation* operationsDeMemory;

int readFileTrace(char * filename);
int countLines(char* filename);
void freeMemory();
void showOperations();
int parseLine(char* line, struct memOperation (*dirOperationGenerar), int indexLine);
int isCorrectHexadecimal(char * number);
int isCorrectDecimal(char * number);
#endif
