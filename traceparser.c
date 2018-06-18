#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gui.h"
#include "datamanipulation.h"
#include "traceparser.h"
#include "datastore.h"

struct memOperation* memoryOperations = NULL;

// Private functions
int countLines(FILE* fp);

/**
 * Parse a trace line. It receives a string with the trace line and its line number in the trace file.
 * It returns by reference a structure with the parsed information.
 */
int parseLine(char* line, int lineNumber, struct memOperation *operation){
#if DEBUG
   fprintf(stderr,"Parsing trace line --%s--\n", line);
#endif
   // Set default values for optional fields
   int hasBreakPoint=DEFAULT_HAS_BREAK_POINT;
   int size=cpu.word_width/8;
   long data=DEFAULT_DATA;
   int instructionOrData=DEFAULT_INSTRUCTION_OR_DATA;
   int operationType=DEFAULT_OPERATION_TYPE;
   long address=DEFAULT_ADDRESS; 


   // Check wether it is a breakpoint line
   if(line[0] == '!') {
      hasBreakPoint = 1;
      // Skip to begining of first field
      line++;
   }

   int fieldId = 0;
   char * pch = strtok (line," ");
   while (pch != NULL)
   {
      // Current field
      switch(fieldId) {
         case 0:// Load/Fetch or Store (One character)
            if(strlen(pch) != 1 || ( *pch != 'L' && *pch != 'S')) {
               fprintf(stderr,"memory operation must be Load/Fetch (L) or Store (S). Line %d\n", lineNumber);
               return -1;
            }
            operationType = *pch == 'L' ? LOAD : STORE;
            break; 
         case 1: // Address (Must be hexadecimal)
            if(!isCorrectHexadecimal(pch)){
               fprintf(stderr,"invalid address. line %d\n", lineNumber);
               return -1;
            }
            address = strtol(pch, NULL, 16);

            if(address<memory.page_base_address||address>=memory.page_base_address+memory.page_size){
	       fprintf(stderr,"address out of page range. line %d\n", lineNumber);
               return -1;
	    }
            break;
         case 2: // Instruction or Data (One character)
            if(strlen(pch) != 1 || ( *pch != 'I' && *pch != 'D')) {
               fprintf(stderr,"memory operation must be Intruction (I) or Data (D). Line %d\n", lineNumber);
               return -1;
            }
            instructionOrData = *pch == 'I' ? INSTRUCTION : DATA;
            if(operationType==STORE && instructionOrData==INSTRUCTION){
               fprintf(stderr,"You can not store (S) an Instruction (I). Line %d\n", lineNumber);
               return -1;
            }
            break;
         case 3: // Size (Must be number of bytes and power of two)
            if(!isCorrectDecimal(pch)){
               fprintf(stderr,"invalid size. Line %d\n", lineNumber);
               return -1;
            }
            size = atoi(pch);
            if(!isPowerOf2(size)){
               fprintf(stderr,"size must be power of 2. Line %d\n", lineNumber);
               return -1;
            }
            int word_bytes=cpu.word_width/8;
            if(size!=(word_bytes)){
               fprintf(stderr,"Only word size access is already implemented. Please, use %d bytes size. Line %d\n", word_bytes, lineNumber);
               return -1;
            }
            break;
         case 4: // Data (Must be a number)
            if(!isCorrectDecimal(pch)){
               fprintf(stderr,"invalid data. Line %d\n", lineNumber);
               return -1;					
            }
            data = atol(pch);
            if(ceil(log(data+1)/log(2))>(size*8)){
	       
               fprintf(stderr,"data value is too large to be stored in %d bytes. Line %d\n", word_bytes, lineNumber);
               return -1;
            }
            break;
         default: // Too many fields
            fprintf(stderr,"too many fields. Line %d\n", lineNumber);
            return -1;
      }
      // Get next field
      fieldId++;
      pch = strtok (NULL, " ");
   }
   if(fieldId < 3) {
      fprintf(stderr,"too few fields. Line %d\n", lineNumber);
      return -1;
   }

   if(operation!=NULL){

       operation->hasBreakPoint=hasBreakPoint;
       operation->size=size;
       operation->data=data;
       operation->instructionOrData=instructionOrData;
       operation->operationType=operationType;
       operation->address=address; 

   }
   return 0;
}

void printMemOperation(FILE *fp, struct memOperation *operation) {
   fprintf(fp, operation->hasBreakPoint ? "! " : "  ");
   fprintf(fp, operation->operationType==LOAD ? "L " : "S ");
   fprintf(fp, "%0*lx ", (int)cpu.address_width/4, operation->address);
   fprintf(fp, operation->instructionOrData==INSTRUCTION ? "I " : "D ");
   fprintf(fp, "% d ", operation->size);
   fprintf(fp, "%ld", operation->data);
   fprintf(fp,"\n");
}

/**
 * function for showing all values loaded from tracefile
 */
void showOperations(){
   for(int i=0; i<numberOfOperations; i++){
      printMemOperation(stderr,&memoryOperations[i]);
   }
}

/** 
 * Remove comments and other string operations on a line from a trace file.
 * Returns 1 if the line is not empty, after removing the comments.
 */
int preprocessTraceLine(char *currentLine) {
   int emptyLine=1;
   for(int i=0; currentLine[i]!='\0'; i++){
      // Replace tabs with spaces
      if(currentLine[i]=='\t'){
         currentLine[i]=' ';
      }
      // Trim lines at comment characters
      if(currentLine[i]=='#'||currentLine[i]=='\n'){
         currentLine[i]='\0';
         break;
      }
      // Check if there are any non-space characters
      if(currentLine[i] != ' '){
         emptyLine=0;
      }
   }
   return !emptyLine;
}

/**
 * Function to load the trace file information in the program. It returns -1 when it encounters errors.
 * It allocates memory, freeMemory() should be called when the memory operations read are no longer necessary.
 */
int readTraceFile(char * filename){
   numberOfOperations = 0;
   int errors = 0;
   FILE *file;
   char *currentLine = NULL;
   size_t len = 0;
   size_t read;

#if DEBUG
    printf("Loading trace file: %s\n", filename);
#endif

   file=fopen(filename, "r");
   if (file == NULL){
      fprintf(stderr,"Error: can not open file %s.\n",filename);
      return -1;
   }

   int numberOfLines = countLines(file);
   rewind(file);

   memoryOperations=NULL;
   if(!useGUI){
           if((memoryOperations = malloc(sizeof(struct memOperation)*numberOfLines)) == NULL){
               fprintf(stderr,"Execution failure: It was not possible to allocate memory.\n");
               return -1;
           }
   }

   int currentLineNumber=0;

   if(useGUI){
         buffer = gtk_text_buffer_new (NULL);
   }
   // Read all the lines in the file
   while ((read = getline(&currentLine, &len, file)) != -1) {
      currentLineNumber++;
      if(useGUI){
            insertTextInBuffer(currentLine, buffer);
      }

      // Skip empty lines
      if(!preprocessTraceLine(currentLine)){
         continue;
      }

      struct memOperation *currentMemOperation=NULL;
      
      //if there is not gui data will be stored. I there is gui thre is not need to store as lines will be parsed at execution time
      if(!useGUI){
           currentMemOperation=&memoryOperations[numberOfOperations];
      }

      if(parseLine(currentLine, currentLineNumber, currentMemOperation) == -1){
      	      errors++;
      }

      // Increment the number of memory operations read from the file
      numberOfOperations++;
   }

 
   if(errors==0){

#if DEBUG
      if(!useGUI){
      	   showOperations();
      }
      fprintf(stderr,"\nTracefile was loaded correctly\n");
#endif

      return 0;
   }
   fprintf(stderr,"\n");

   return -1;
}

/**
 * Count the number of lines in the file.
 */
int countLines(FILE* fp){
   int count = 0;

   for (char c = getc(fp); c != EOF; c = getc(fp))
      if (c == '\n')
         count++;
   return count;
}

/**
 * Free memory allocated by this module.
 */
void freeMemory(){
   if(memoryOperations != NULL){
      free(memoryOperations);
      memoryOperations=NULL;
   }
}

