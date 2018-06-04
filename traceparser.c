#include "traceparser.h"

/**
 * Funcion para parsear una line. REcibe por parametro el String de la line, la position de la line y la la operation de
 * memory que se va a almacenar.
 */
int parseLine(char* line, struct memOperation (*dirOperationGenerar), int indexLine){

          

          char* campos[6];

	  char * pch;

	  pch = strtok (line," ");
	
          int cont=0;
	  while (pch != NULL)
	  {
            if(cont==7){
		 printf("trace file error: too many fields . line %d\n", indexLine);
                 return -1;
	    }

            //printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEEE");
            
            if((campos[cont]=malloc(sizeof(char)*strlen(pch)+1))==0){
            	return -1;
	    }

	    strcpy(campos[cont],pch);
	    
     
	    pch = strtok (NULL, " ");  
            cont++;


	  }

	  if(cont <3){
		 printf("trace file error: missing fields. line %d\n", indexLine);
                 return -1;
	  }
	


		if(strcmp(campos[0], "!")==0){
			(*dirOperationGenerar).hasBreakPoint=1;


			if(strcmp(campos[1],"I")==0){
				(*dirOperationGenerar).instructionOrData=INSTRUCTION;
				if (!isCorrectHexadecimal(campos[2])){
					printf("trace file error: invalid address. line %d\n", indexLine+1);
					return -1;
				}
				long dir=strtol(campos[2], NULL, 16);
				
			        (*dirOperationGenerar).address=dir;
				if(strcmp(campos[3], "L")==0){
					(*dirOperationGenerar).operationType=LOAD;
				}else if(strcmp(campos[3], "S")==0){
					(*dirOperationGenerar).operationType=STORE;
				}else{
					printf("trace file error: Invalid operation type. line %d\n", indexLine+1);
					return -1;
				}
				
				if(cont>4){
					int size=atoi(campos[4]);

					if(!isCorrectDecimal(campos[4])){
						printf("trace file error: Invalid size. line %d\n", indexLine+1);
						return -1;
					}
					if(!isPowerOf2(size)){
						printf("trace file error: Size must be power of 2. line %d\n", indexLine+1);
						return -1;
					}
					(*dirOperationGenerar).size=size;
				}
				
				if(cont>5){
					long data=atoi(campos[5]);
					if(!isCorrectDecimal(campos[3])){
						printf("trace file error: Invalid data. line %d\n", indexLine+1);
						return -1;					
					}
					(*dirOperationGenerar).data=data;
				}
		
			}else if(strcmp(campos[1], "D")==0){

				(*dirOperationGenerar).instructionOrData=DATA;

				
				if (!isCorrectHexadecimal(campos[2])){
					printf("trace file error: invalid address. line %d\n", indexLine+1);
					return -1;
				}

				long dir=strtol(campos[2], NULL, 16);
				
				(*dirOperationGenerar).address=dir;
				if(strcmp(campos[3], "L")==0){
					(*dirOperationGenerar).operationType=LOAD;
				}else if(strcmp(campos[3], "S")==0){
					(*dirOperationGenerar).operationType=STORE;
				}else{
					printf("trace file error: Invalid operation type. line %d\n", indexLine+1);
					return -1;
				}
				
				if(cont>4){
					int size=atoi(campos[4]);

					if(!isCorrectDecimal(campos[4])){
						printf("trace file error: Invalid size. line %d\n", indexLine+1);
						return -1;
					}
					if(!isPowerOf2(size)){
						printf("trace file error: Size must be power of 2. line %d\n", indexLine+1);
						return -1;
					}
					(*dirOperationGenerar).size=size;
				}
				
				if(cont>5){
					long data=atoi(campos[5]);
					if(!isCorrectDecimal(campos[5])){
						printf("trace file error: Invalid data. line %d\n", indexLine+1);
						return -1;					
					}
					(*dirOperationGenerar).data=data;
				}
			}else{
				printf("trace file error: It must be data or instruction. line. line %d\n", indexLine+1);
				return -1;
			}


		}else if(strcmp(campos[0],"I")==0){
				(*dirOperationGenerar).hasBreakPoint=0;
				(*dirOperationGenerar).instructionOrData=INSTRUCTION;
				

				if (!isCorrectHexadecimal(campos[1])){
					printf("trace file error: invalid address. line %d\n", indexLine+1);
					return -1;
				}

				long dir=strtol(campos[1], NULL, 16);
				(*dirOperationGenerar).address=dir;
			
				if(strcmp(campos[2],"L")==0){
					(*dirOperationGenerar).operationType=LOAD;
				}else if(strcmp(campos[2], "S")==0){
					(*dirOperationGenerar).operationType=STORE;
				}else{
					printf("trace file error: Invalid operation type. line %d\n", indexLine+1);
					return -1;
				}
				
				if(cont>3){
					int size=atoi(campos[3]);

					if(!isCorrectDecimal(campos[3])){
						printf("trace file error: Invalid size. line %d\n", indexLine+1);
						return -1;
					}

					if(!isPowerOf2(size)){
						printf("trace file error: Size must be power of 2. line %d\n", indexLine+1);
						return -1;
					}
					(*dirOperationGenerar).size=size;
				}
				
				if(cont>4){
					long data=atoi(campos[4]);
					if(!isCorrectDecimal(campos[4])){
						printf("trace file error: Invalid data. line %d\n", indexLine+1);
						return -1;
					}
					(*dirOperationGenerar).data=data;
				}	

		}else if(strcmp(campos[0], "D")==0){
				(*dirOperationGenerar).hasBreakPoint=0;
				(*dirOperationGenerar).instructionOrData=DATA;
				
				if (!isCorrectHexadecimal(campos[1])){
					printf("trace file error: invalid address. line %d\n", indexLine+1);
					
					return -1;
				}

				long dir=strtol(campos[1], NULL, 16);

				(*dirOperationGenerar).address=dir;
			
				if(strcmp(campos[2], "L") ==0){
					(*dirOperationGenerar).operationType=LOAD;
				}else if(strcmp(campos[2], "S")==0){
					(*dirOperationGenerar).operationType=STORE;
				}else{
					printf("trace file error: Invalid operation type. line %d\n", indexLine+1);
					return -1;
				}
				
				if(cont>3){
					int size=atoi(campos[3]);

					if(!isCorrectDecimal(campos[3])){
						printf("trace file error: Invalid size. line %d\n", indexLine+1);
						return -1;
					}
					if(!isPowerOf2(size)){
						printf("trace file error: Size must be power of 2. line %d\n", indexLine+1);
						return -1;
					}
					(*dirOperationGenerar).size=size;
				}
				
				if(cont>4){
					long data=atoi(campos[4]);
					if(!isCorrectDecimal(campos[4])){
						printf("trace file error: Invalid data. line %d\n", indexLine+1);
						return -1;
					}
					(*dirOperationGenerar).data=data;
				}	

		}else{
			//compruebo si el campo IstruccionOrData en realidad es el siguiente. Si no lo es debería ser este.
			if(strcmp(campos[1],"D")!=0&&strcmp(campos[1],"I")!=0){

				printf("trace file error: It must be data or instruction. line %d\n", indexLine+1);

			//si el campo es el siguiente entonces se trata de user_use type de error de formato.
			}else{
			
				printf("trace file error: Wrong format. line %d\n", indexLine+1);
			}
			return -1;

		}



	return 0;

  
}


/**
 * Funcion para mostrar por consola los valores de la trace que se han almacenado en memory.
 */
void showOperations(){

    printf("\n");
    for(int i=0; i<numOperations; i++){

       if(operationsDeMemory[i].hasBreakPoint){
	   printf(" ! ");

		if(operationsDeMemory[i].instructionOrData==INSTRUCTION){
		   printf("I ");
		}

		if(operationsDeMemory[i].instructionOrData==DATA){
		   printf("D ");
		}
        }else{

		if(operationsDeMemory[i].instructionOrData==INSTRUCTION){
		   printf("   I ");
		}

		if(operationsDeMemory[i].instructionOrData==DATA){
		   printf("   D ");
		}
	}

	char number[20];
	sprintf(number, "0x%lx",operationsDeMemory[i].address);
	int len= strlen(number);

	int numEspacios=16-len;
	printf(" %s ", number);
	for(int j=0; j<numEspacios; j++){
		printf(" ");
	}

        if(operationsDeMemory[i].operationType==LOAD){
	   printf(" L ");
        }

        if(operationsDeMemory[i].operationType==STORE){
	   printf(" S ");
        }

	char number2[20];
        sprintf(number2, " %d ", operationsDeMemory[i].size);
	len=strlen(number2);
	numEspacios=10-len;

	printf(" %s ", number2);
	for(int j=0; j<numEspacios; j++){
		printf(" ");
	}
	

        printf(" %ld ", operationsDeMemory[i].data);

	printf("\n");


    }




}






/**
 * Funcion que carga la informacion del file en el programa. Si se produce algun error retorna -1.
 */

int readFileTrace(char * filename){


    printf("\n");

    numLines=0;
    numOperations=0;
    mallocated=1;
    int errors=0;
    int numLines= countLines(filename);
    if(numLines==-1){
	return -1;
    }
    numLines=numLines;

    FILE *file;
    char *line = NULL;
    size_t len = 0;
    char read;
    file=fopen(filename, "r");

    if (file == NULL){
	  printf("Error: file de trace no encontrado\n");
  	  return -1;
    }

    //printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

    operationsDeMemory=malloc(sizeof(struct memOperation)*numLines);
    if(operationsDeMemory==NULL){
	printf("Execution failure: It was not possible to allocate memory. line\n");
	//printf("lines numbers: %d\n", (int)operationsDeMemory);
	mallocated=0;
	return -1;
    }
    
    int indexLineCurrent=-1;
    int OperationCurrent=0;

    GtkTextBuffer *buffer = gtk_text_buffer_new (NULL);
    //leo todas las lines del file
    while ((read = getline(&line, &len, file)) != -1) {
	indexLineCurrent++;
	int lenLine=strlen(line);
	char lineCurrent[lenLine+1];
	strcpy(lineCurrent, line);

        
        insertTextInBuffer(line, buffer);

	//elimino tabuladores, comentarios y saltos de line. 
	for(int i=0; lineCurrent[i]!='\0'; i++){

		if(lineCurrent[i]=='\t'){
			lineCurrent[i]=' ';
		}

		if(lineCurrent[i]=='#'||lineCurrent[i]=='\n'){
			lineCurrent[i]='\0';
			break;
		}

	}

	
	int estaInBlanco=1;
	//Compruebo si la line esta en blanco 
	for(int i=0; lineCurrent[i]!='\0'; i++){

		if(lineCurrent[i]!=' '){
			estaInBlanco=0;
			break;
		}
	}
	


	if(estaInBlanco){
		continue;
	}

	//establezco valores por defecto
	operationsDeMemory[OperationCurrent].hasBreakPoint=0;
	operationsDeMemory[OperationCurrent].instructionOrData=INSTRUCTION;
        operationsDeMemory[OperationCurrent].address=0;
        operationsDeMemory[OperationCurrent].operationType=LOAD;
        operationsDeMemory[OperationCurrent].size=DEFAULT_SIZE;
        operationsDeMemory[OperationCurrent].data=DEFAULT_DATA;

	
	//parseo lines current

        if(parseLine(lineCurrent, &operationsDeMemory[OperationCurrent], indexLineCurrent)==-1){
		errors++;
		

	}

	OperationCurrent++;
    }
	numOperations=OperationCurrent;


    if(errors==0){

	showOperations();
	printf("\nTracefile was loaded correctly\n");
	return 0;
    }

    printf("\n");

    return -1;

}


/**
 * Funcion para contar lines de un file. si el file no existe retorna -1.
 */

int countLines(char* filename){


    FILE *fp;
    int count = 0;
    char c;
 

    fp = fopen(filename, "r");
 
    if (fp == NULL)
    {
        return -1;
    }
 
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n')
            count = count + 1;
 
    fclose(fp);
 
 
    return count;


}


/**
 * Funcion para freer la memory reservada por el programa.
 */
void freeMemory(){
	if(operationsDeMemory!=NULL){
		//printf("%ld\n",(long)operationsDeMemory);
		free(operationsDeMemory);
		operationsDeMemory=NULL;
	}

}




/**
 * Funcion para comprobar si el number hexadecimal representado por el string es correcto.
 */
int isCorrectHexadecimal(char * number){

	if(strlen(number)<2){
		return 0;
	}
	if(number[0]!='0'){
		return 0;
	}
	if(number[1]!='x'&&number[1]!='X'){
		return 0;
	}
	
	for(int i=2; i<strlen(number); i++){
		if(number[i]<'0'||(number[i]>'9'&&number[i]<'A')||(number[i]>'F'&&number[i]<'a')||number[i]>'f'){
			return 0;

		}
	}

	return 1;

}

/**
 * Funcion para comprobar si el number decimal representado por el string es correcto.
 */
int isCorrectDecimal(char * number){

	for(int i=0; i<number[i]!='\0'; i++){
		if(number[i]<'0'||number[i]>'9'){
			return 0;
		}
	}

	return 1;
}



