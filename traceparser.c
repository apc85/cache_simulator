#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gui.h"
#include "datamanipulation.h"
#include "traceparser.h"
#include "datastore.h"

struct memOperation* memoryOperations = NULL;

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
   int size=DEFAULT_SIZE;
   int data=DEFAULT_DATA;
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
         case 0: // Instruction or Data (One character)
            if(strlen(pch) != 1 || ( *pch != 'I' && *pch != 'D')) {
               fprintf(stderr,"memory operation must be Intruction (I) or Data (D). Line %d\n", lineNumber);
               return -1;
            }
            instructionOrData = *pch == 'I' ? INSTRUCTION : DATA;
            break;
         case 1: // Address (Must be hexadecimal)
            if(!isCorrectHexadecimal(pch)){
               fprintf(stderr,"invalid address. line %d\n", lineNumber);
               return -1;
            }
            address = strtol(pch, NULL, 16);
            break;
         case 2: // Load/Fetch or Store (One character)
            if(strlen(pch) != 1 || ( *pch != 'L' && *pch != 'S')) {
               fprintf(stderr,"memory operation must be Load/Fetch (L) or Store (S). Line %d\n", lineNumber);
               return -1;
            }
            operationType = *pch == 'L' ? LOAD : STORE;
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
            break;
         case 4: // Data (Must be a number)
            if(!isCorrectDecimal(pch)){
               fprintf(stderr,"invalid data. Line %d\n", lineNumber);
               return -1;					
            }
            data = atoi(pch);
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
/**
 * function for showing all values loaded from tracefile
 */
void showOperations(){
   fprintf(stderr,"\n");
   for(int i=0; i<numberOfOperations; i++){
      if(memoryOperations[i].hasBreakPoint){
         fprintf(stderr," ! ");
         if(memoryOperations[i].instructionOrData==INSTRUCTION){
            fprintf(stderr,"I ");
         }
         if(memoryOperations[i].instructionOrData==DATA){
            fprintf(stderr,"D ");
         }
      }else{
         if(memoryOperations[i].instructionOrData==INSTRUCTION){
            fprintf(stderr,"   I ");
         }
         if(memoryOperations[i].instructionOrData==DATA){
            fprintf(stderr,"   D ");
         }
      }
      char number[20];
      sprintf(number, "0x%lx",memoryOperations[i].address);
      int len= strlen(number);
      int numEspacios=16-len;
      fprintf(stderr," %s ", number);
      for(int j=0; j<numEspacios; j++){
         fprintf(stderr," ");
      }
      if(memoryOperations[i].operationType==LOAD){
         fprintf(stderr," L ");
      }
      if(memoryOperations[i].operationType==STORE){
         fprintf(stderr," S ");
      }
      char number2[20];
      sprintf(number2, " %d ", memoryOperations[i].size);
      len=strlen(number2);
      numEspacios=10-len;
      fprintf(stderr," %s ", number2);
      for(int j=0; j<numEspacios; j++){
         fprintf(stderr," ");
      }

      fprintf(stderr," %ld ", memoryOperations[i].data);
      fprintf(stderr,"\n");
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
   int numberOfLines = countLines(filename);
   if(numberOfLines == -1){
      return -1;
   }
   FILE *file;
   char *currentLine = NULL;
   size_t len = 0;
   size_t read;

   file=fopen(filename, "r");
   if (file == NULL){
      fprintf(stderr,"Error: can not open file %s.\n",filename);
      return -1;
   }

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

      //if theere is gui no data will be stored. Lines will be parsed at execution time
      if(useGUI){
      	  // Read memory operation from current line
      	  if(parseLine(currentLine, currentLineNumber, NULL) == -1){
      	      errors++;
      	  }
      //if there is not gui I store the memoperations from the parsed lines
      }else{

          if(parseLine(currentLine, currentLineNumber, &memoryOperations[numberOfOperations]) == -1){
      	      errors++;
      	  }
      }

      // Increment the number of memory operations read from the file
      numberOfOperations++;
   }


   if(errors==0){
#if DEBUG
      showOperations();
      fprintf(stderr,"\nTracefile was loaded correctly\n");
#endif
      return 0;
   }
   fprintf(stderr,"\n");
   return -1;
}

/**
 * Count the number of lines in the file. Returns -1 on errors.
 */
int countLines(char* filename){
   FILE *fp;
   int count = 0;
   char c;

   if ((fp = fopen(filename, "r")) == NULL)
      return -1;

   for (c = getc(fp); c != EOF; c = getc(fp))
      if (c == '\n')
         count = count + 1;
   fclose(fp);
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

