#ifndef TRACEPARSER_H
#define TRACEPARSER_H

int numberOfOperations;

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
extern struct memOperation* memoryOperations;

int readTraceFile(char * filename);
int countLines(char* filename);
void freeMemory();
void showOperations();
int parseLine(char* line, int lineNumber, struct memOperation *dirOperationGenerar);
#endif
