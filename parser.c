#include "parser.h"

char* str_replacementPolicy[]= {"lru", "lfu", "rnd", "fifo"};
char* str_writePolicy[]= {"wt", "wb"};
char* keysCPU[]= {"word_width", "address_width", "frequency", "trace_file"};
char* keysMEMORY[]= {"size", "access_time_1","access_time_burst", "page_size", "page_base_address"};
char* keysCACHE[]= {"line_size", "size","asociativity", "write_policy", "replacement","separated","column_bit_mask"};
char* str_true[]= {"1", "yes", "true"};
char* str_false[]= {"0","no","false"};

void parseConfInt(dictionary *ini, const char *key, long int *confVariable, int *errors) {
    const char * confString = iniparser_getstring(ini, key, NULL);
    long value = parseInt(confString);

    if(value == -1) {
        fprintf(stderr,"Error: %s value is not valid\n", key);
        errors++;
    } else if(value == -2) {
        fprintf(stderr,"Error: Missing mandatory key %s\n", key);
        errors++;
    } else {
        *confVariable = value;
    }
}

void parseConfLongK1000(dictionary *ini, const char *key, long int *confVariable, int *errors) {
    const char * confString = iniparser_getstring(ini, key, NULL);
    long value = parseLongK1000(confString);

    if(value == -1) {
        fprintf(stderr,"Error: %s value is not valid\n", key);
        errors++;
    } else if(value == -2) {
        fprintf(stderr,"Error: Missing mandatory key %s\n", key);
        errors++;
    } else {
        *confVariable = value;
    }
}

void parseConfLongK1024(dictionary *ini, const char *key, long int *confVariable, int *errors) {
    const char * confString = iniparser_getstring(ini, key, NULL);
    long value = parseLongK1024(confString);

    if(value == -1) {
        fprintf(stderr,"Error: %s value is not valid\n", key);
        errors++;
    } else if(value == -2) {
        fprintf(stderr,"Error: Missing mandatory key %s\n", key);
        errors++;
    } else {
        *confVariable = value;
    }
}

void parseConfDouble(dictionary *ini, const char *key, double *confVariable, int *errors) {
    const char * confString = iniparser_getstring(ini, key, NULL);
    double value = parseDouble(confString);

    if(value == -1) {
        fprintf(stderr,"Error: %s value is not valid\n", key);
        errors++;
    } else if(value == -2) {
        fprintf(stderr,"Error: Missing mandatory key %s\n", key);
        errors++;
    } else {
        *confVariable = value;
    }
}

void parseConfAddress(dictionary *ini, const char *key, long int *confVariable, int *errors) {
    const char * confString = iniparser_getstring(ini, key, NULL);
    long value = parseAddress(confString);

    if(value == -1) {
        fprintf(stderr,"Error: %s value is not valid\n", key);
        errors++;
    } else if(value == -2) {
        fprintf(stderr,"Error: Missing mandatory key %s\n", key);
        errors++;
    } else {
        *confVariable = value;
    }
}


/**
 * This functions read the simulator configuration file.
 * @param ini_name the file name
 *
 */
dictionary *readConfigurationFile(char * ini_name) {
    numberCaches = 0;
    int errors = 0;
    dictionary *ini;

    if((ini = iniparser_load(ini_name))==NULL) {
        fprintf(stderr, "Error loading file: %s\n", ini_name);
        return NULL;
    }

#if DEBUG
    iniparser_dump(ini, stderr);
    fprintf(stderr,"\n");
#endif

    int numberSections = iniparser_getnsec(ini);

    int numberCPUs = 0;
    int numberMemories = 0;

    /* Check that all the configuration file sections are correct.
     * No missing sections. No unknown sections. */
    for(int i=0; i<numberSections; i++) {
        const char * section=iniparser_getsecname(ini, i);

        // if the name of the section is cpu
        if(strcmp(section, "cpu")==0) {
            // count cpu sections. There can be only one cpu section
            numberCPUs++;
        // If the name of the section is memory
        } else if(strcmp(section, "memory")==0) {
            // Count memory sections. There can be only one memory section
            numberMemories++;
        // If the name of the section is like "cache..."
        } else if(strncmp(section, "cache", 5)==0) {
            int correctNum=1;
            // Get the length of the section name
            int len = strlen(section);
            // Error if there is no characters following 'cache'
            if(len <= 5) {
                fprintf(stderr,"Error: Invalid [cache] section. It must contain the cache level number. [cacheN]\n");
                correctNum=0;
                errors++;
            }

            // Check that the string following 'cache' is a number 
            const char *cacheNumberStr = section+5;
            for(int j=0; cacheNumberStr[j] && correctNum; j++) {
                if(!isdigit(cacheNumberStr[j])) {
                    correctNum = 0;
                    fprintf(stderr,"Error: Invalid cache section name [%s]\n", section);
                    errors++;
                }
            }

            // Parse the contents of the section if the number is correct
            if(correctNum) {
                int cacheNumber = atoi(cacheNumberStr);
                // Remember the highest cache level
                if(cacheNumber > numberCaches) {
                    numberCaches = cacheNumber;
                }
                checkSectionKeys(ini, section, NCLAVES_CACHE, keysCACHE, &errors);
            }
        // If the section name isn't "cpu" or "memory" and section name isn't like "cache..." then error.
        } else {
            fprintf(stderr,"Error: Unknown section name [%s]\n", section);
            errors++;
        }
    }

    // Check the mandatory [cpu] section
    if(numberCPUs==0) {
        fprintf(stderr,"Error: Missing mandatory section [cpu]\n");
        errors++;
    // Look for unknown keys in [cpu] section
    } else {
        checkSectionKeys(ini, "cpu", NCLAVES_CPU, keysCPU, &errors);
    }

    // Check the mandatory [memory] section
    if(numberMemories==0) {
        fprintf(stderr,"Error: Missing mandatory section [memory]\n");
        errors++;
    // Look for unknown keys in [memory] section
    } else {
        checkSectionKeys(ini, "memory", NCLAVES_MEMORY, keysMEMORY, &errors);
    }

    // Check that the number of cache levels is within range
    if(numberCaches>MAX_CACHES) {
        fprintf(stderr,"Error: The number of caches is excesive.\n");
        errors++;
    }

    // End with error message if there has been any errors
    if(errors > 0) {
        fprintf(stderr,"\nTotal errors: %d\n", errors);
        return NULL;
    }
    return ini;
}


int parseConfiguration(dictionary *ini) {
    int errors = 0;

    // READING CPU CONFIGURATION///////////////////////////////////////////////

    parseConfInt(ini,"cpu:address_width",&cpu.address_width,&errors);
    parseConfInt(ini,"cpu:word_width",&cpu.word_width,&errors);
    parseConfLongK1000(ini,"cpu:frequency",&cpu.frequency,&errors);

    // reading key cpu:trace_file
    const char * cpu_trace_file=          iniparser_getstring(ini, "cpu:trace_file", NULL);
    if(cpu_trace_file==NULL) {
        fprintf(stderr,"Error: Missing value cpu:trace_file\n");
        errors++;
    } else {
        cpu.trace_file=cpu_trace_file;
    }

    // READING MEMORY CONFIGURATION//////////////////////////////////////

    parseConfLongK1024(ini,"memory:size",&memory.size,&errors);
    parseConfDouble(ini,"memory:access_time_1",&memory.access_time_1,&errors);
    parseConfLongK1024(ini,"memory:page_size",&memory.page_size,&errors);
    parseConfAddress(ini,"memory:page_base_address",&memory.page_base_address,&errors);

    // READING ALL THE CACHES CONFIGURATION /////////////////////////////////////////
    // Browse the cache array and check the configuration of each cache.

    for(int cacheNumber=0; cacheNumber<numberCaches; cacheNumber++) {

        // This is for creating the string format in which the ini library receives the params.
        // Each value must be refered as section:key
        char param[50];
        sprintf(param, "cache%d:line_size", cacheNumber+1);
        parseConfLongK1024(ini, param, &caches[cacheNumber].line_size,&errors);

        // reading key cache:size
        sprintf(param, "cache%d:size", cacheNumber+1);
        parseConfLongK1024(ini, param, &caches[cacheNumber].size,&errors);

        // reading key cache:column_bit_mask
        sprintf(param, "cache%d:column_bit_mask", cacheNumber+1);
        const char * column_bit_mask=iniparser_getstring(ini, param, NULL);
        int isBinary=isCorrectBinary(column_bit_mask);
        if(isBinary==-1) {
            fprintf(stderr,"Error: cache%d:column_bit_mask value is not valid\n", cacheNumber+1);
            errors++;
        } else if(isBinary==-2) {
            fprintf(stderr,"Error: Missing value cache%d:column_bit_mask\n", cacheNumber+1);
            errors++;
        } else if(strlen(column_bit_mask)<N_BITS_MASK){
            fprintf(stderr,"Error: Bit mask is to small cache%d:column_bit_mask\n", cacheNumber+1);
            errors++;
        } else {
            caches[cacheNumber].column_bit_mask=column_bit_mask;
        }

        // reading key cache:asocitivity
        sprintf(param, "cache%d:asociativity", cacheNumber+1);
        const char * cache_asociativity=iniparser_getstring(ini, param, NULL);
        // si es F es de compleatamente asociativa. Un solo set. Tantas lines/set como lines totales.
        if(cache_asociativity!=NULL&&strcmp(cache_asociativity, "F")==0) {
            caches[cacheNumber].asociativity=caches[cacheNumber].size/caches[cacheNumber].line_size;
        } else {
            long long_asociativity=parseInt(cache_asociativity);
            if(long_asociativity==-1) {
                fprintf(stderr,"Error: cache%d:asociativity value is not valid\n", cacheNumber+1);
                errors++;
            } else if(long_asociativity==-2) {
                fprintf(stderr,"Error: Missing value cache%d:asociativity\n", cacheNumber+1);
                errors++;
            } else if(!isPowerOf2(long_asociativity)) {
                fprintf(stderr,"Error: The value of cache%d:asociativity must be power of 2\n", cacheNumber+1);
                errors++;
            } else {
                caches[cacheNumber].asociativity=long_asociativity;
            }
        }

        // reading key cache:write_policy
        sprintf(param, "cache%d:write_policy", cacheNumber+1);
        const char * cache_write_policy=iniparser_getstring(ini, param, NULL);
        long long_write_policy=parseWritePolicy(cache_write_policy);
        if(long_write_policy==-1) {
            fprintf(stderr,"Error: cache%d:write_policy value is not valid\n", cacheNumber+1);
            errors++;
        } else if(long_write_policy==-2) {
            fprintf(stderr,"Error: Missing value cache%d:write_policy\n", cacheNumber+1);
            errors++;
        } else {
            caches[cacheNumber].write_policy=long_write_policy;
        }

        // reading key cache:replacement
        sprintf(param, "cache%d:replacement", cacheNumber+1);

        const char * cache_replacement=iniparser_getstring(ini, param, NULL);
        long long_replacement=parseReplacementPolicy(cache_replacement);
        if(long_replacement==-1) {
            fprintf(stderr,"Error: cache%d:replacement value is not valid\n", cacheNumber+1);
            errors++;
        } else if(long_replacement==-2) {
            fprintf(stderr,"Error: Missing value cache%d:replacement\n", cacheNumber+1);
            errors++;
        } else {
            caches[cacheNumber].replacement=long_replacement;
        }

        // reading key cache:separated
        sprintf(param, "cache%d:separated", cacheNumber+1);

        const char * cache_separated=iniparser_getstring(ini, param, NULL);
        long long_separated=parseBoolean(cache_separated);
        if(long_separated==-1) {
            fprintf(stderr,"Error: cache%d:separated value is not valid\n", cacheNumber+1);
            errors++;
        } else if(long_separated==-2) {
            fprintf(stderr,"Error: Missing value cache%d:separated\n", cacheNumber+1);
            errors++;
        } else {
            caches[cacheNumber].separated=long_separated;
        }
    }

    // checking all the caches have the same line_size
    if(numberCaches>0){
       int previous=caches[0].line_size;
       for(int cacheNumber=1; cacheNumber<numberCaches; cacheNumber++) {
          if(caches[cacheNumber].line_size!=previous){
             fprintf(stderr,"Error: All the caches must have the same line_size.\n");
             errors++;
             break;
          }
       }
    }

    if(errors>0) {
        fprintf(stderr,"\nTotal errors: %d\n", errors);
        return -1;
    }
#if DEBUG
    showState();
    fprintf(stderr,"\nConfiguration file loaded correctly\n");
#endif
    return 0;
}


/*
 * Checks that all the keys in a section are valid
 */
void checkSectionKeys(dictionary *ini, const char *section, int numberOfValidKeys, char *validKeys[], int *errors) {
   int nkeys = iniparser_getsecnkeys(ini, section);
   const char * keys[nkeys];
   iniparser_getseckeys(ini, section, keys);

   // Make sure that the section does not have unknown keys
   for(int i=0; i<nkeys; i++) {
      // By comparing each one with the known keys of a cache section
      for(int j=0; j<numberOfValidKeys; j++) {
         // TODO Possible buffer overflow: section is a string that comes from the user. It could be longer than 200.
         char key[200];
         sprintf(key, "%s:%s", section, validKeys[j]);
         if(strcmp(keys[i], key)==0) {
            // This key is known. Go to the next one.
            goto nextKey;
         }
      }
      // This key matches none of the known keys of a cache section
      fprintf(stderr,"Error: unknown key %s\n", keys[i]);
      (*errors)++;
      nextKey:;
   }
}



/*
 * shows the current state of read data
 */


void showState() {

    // show cpu info
    fprintf(stderr,"\nCPU\n");

    fprintf(stderr,"word_width:         [%ld bits] \n", cpu.word_width);
    fprintf(stderr,"address_width:      [%ld bits] \n", cpu.address_width);
    fprintf(stderr,"frecuency:          [%ld Hz] \n", cpu.frequency);
    fprintf(stderr,"bus_frequency:      [%ld Hz] \n", cpu.bus_frequency);
    fprintf(stderr,"mem_bus_frequency:  [%ld Hz] \n", cpu.mem_bus_frequency);
    fprintf(stderr,"trace_file:         [%s]\n", cpu.trace_file);


    // show memory info
    fprintf(stderr,"\nMEMORY\n");

    fprintf(stderr,"size:               [%ld bytes] \n",  memory.size);
    fprintf(stderr,"bus_width:          [%ld bits] \n", memory.bus_width);
    fprintf(stderr,"bus_frequency:      [%ld Hz] \n", memory.bus_frequency);
    fprintf(stderr,"access_time_1:      [%lf ns] \n", memory.access_time_1*1000000000);
    fprintf(stderr,"access_time_burst:  [%lf ns] \n", memory.access_time_burst*1000000000);
    fprintf(stderr,"page_size:          [%ld bytes] \n", memory.page_size);
    fprintf(stderr,"page_base_address:  [0x%lx] \n", memory.page_base_address);



    // show each cache info
    for(long i=0; i<numberCaches; i++) {
        fprintf(stderr,"\nCACHE L%ld\n", i+1);

        fprintf(stderr,"line_size:          [%ld bits] \n",  caches[i].line_size);
        fprintf(stderr,"size:               [%ld bytes] \n",  caches[i].size);
        fprintf(stderr,"asociativity:       [%ld]\n",  caches[i].asociativity);
        fprintf(stderr,"write_policy:       [%s]\n",   str_writePolicy[caches[i].write_policy]);
        fprintf(stderr,"replacement:        [%s]\n",   str_replacementPolicy[caches[i].replacement]);
        fprintf(stderr,"separated:          [%d]\n",   caches[i].separated);

    }

}




/*
 * convert string into long. It can have a multiplier G for 10^9, M for 10^6 o K for 10^3. Any other char will result in error.
 * @param  String to be converted into long
 * @return long with converted value or error. -1 for wrong value error. -2 for null pointer error.
 */
long parseLongK1000(const char * cadena) {

    if(cadena==NULL) {
        return -2;
    }
    // Obtain the multiplier k, M, or G. Else error
    long len=strlen(cadena);
    long multiplicador=1;
    if(cadena[len-1]=='K'||cadena[len-1]=='k') {
        multiplicador=1000;
    } else if(cadena[len-1]=='M'||cadena[len-1]=='m') {
        multiplicador=1000000;
    } else if(cadena[len-1]=='G'||cadena[len-1]=='g') {
        multiplicador=1000000000;
    } else if(cadena[len-1]>'9'||cadena[len-1]<'0') {
        return -1;

    }

    // if something not numeric or multiplier. error return -1
    for(long i=0; i<len-1; i++) {

        if(cadena[i]>'9'||cadena[i]<'0') {
            return -1;
        }
    }

    if(len==1){
         return multiplicador;

    }

    return atoi(cadena)*multiplicador;
}

/*
 * Convert string into long. It can have a multiplier G for 2^30, M for 2^20 o K for 2^10. Any othe char will result in error.
 * @param  String to be converted into long
 * @return long with converted value or error. -1 for wrong value error. -2 for null pointer error.
 */
long parseLongK1024(const char * cadena) {

    if(cadena==NULL) {
        return -2;
    }

    // Obtain the multiplier k, M, or G. Else error
    long len=strlen(cadena);
    long multiplicador=1;
    if(cadena[len-1]=='K'||cadena[len-1]=='k') {
        multiplicador=1024;
    } else if(cadena[len-1]=='M'||cadena[len-1]=='m') {
        multiplicador=1048576;
    } else if(cadena[len-1]=='G'||cadena[len-1]=='g') {
        multiplicador=1073741824;
    } else if(cadena[len-1]>'9'||cadena[len-1]<'0') {
        return -1;

    }

    // if something not numeric or multiplier. error return -1
    for(long i=0; i<len-1; i++) {

        if(cadena[i]>'9'||cadena[i]<'0') {
            return -1;
        }
    }

    if(len==1){
         return multiplicador;

    }

    return atoi(cadena)*multiplicador;
}

/*
 * convert string into int.
 * @param  String to be converted into int
 * @return int with converted value or error. -1 for wrong value error. -2 for null pointer error.
 */
int parseInt(const char * cadena) {

    if(cadena==NULL) {
        return -2;
    }
    // Obtain the multiplier k, M, or G. Else error.
    int len=strlen(cadena);

    // if something not numeric or multiplier. error return -1
    for(long i=0; i<len; i++) {

        if(cadena[i]>'9'||cadena[i]<'0') {
            return -1;
        }
    }

    return atoi(cadena);
}

/*
 * Convert string into boolean.
 * @param  String to be converted into boolean. Possible strings: yes, no, true, false, 0, 1
 * @return int with converted value. 1 for true, 0 for false, -1 for wrong value error. -2 for null pointer error.
 */
int parseBoolean(const char * cadena) {

    // if null return error -2
    if(cadena==NULL) {
        return -2;
    }

    // turn into lower case
    char cadenaMin[10];
    int i;
    for( i= 0; cadena[i]; i++) {
        cadenaMin[i] = tolower(cadena[i]);
    }
    cadenaMin[i]='\0';



    // check string content and retun equivalent boolean value.
    for(int i=0; i<N_TRUES; i++) {
        if(strcmp(str_true[i], cadenaMin)==0) {
            return 1;
        }

    }

    for(int i=0; i<N_FALSES; i++) {
        if(strcmp(str_false[i], cadenaMin)==0) {
            return 0;
        }

    }

    return -1;


}

/*
 * Convert string into enum which represent replacement policy.
 * @param  String to be converted into enum. Possible strings defined in str_replacementPolicy
 * @return enum  value or error. -2 for null pointer. -1 for wrong value error
 */
int parseReplacementPolicy(const char * cadena) {

    if(cadena==NULL) {
        return -2;
    }

    for(int i=0; i<N_REPLACEMENT; i++) {

        if(strcmp(cadena, str_replacementPolicy[i])==0) {
            return i;
        }
    }

    return -1;

}

/*
 * Convert string into enum which represent write policy.
 * @param  String to be converted into enum. Possible strings defined in str_replacementPolicy
 * @return enum  value or error. -2 for null pointer error. -1 for wrong value error
 */
int parseWritePolicy(const char * cadena) {

    // if null pointer retun error -2
    if(cadena==NULL) {
        return -2;
    }

    for(int i=0; i<N_WRITE; i++) {
        if(strcmp(cadena, str_writePolicy[i])==0) {
            return i;
        }
    }

    return -1;

}

/*
 * Check if a number is power of 2
 * @param number to check
 * @return boolean
 */
int isPowerOf2(long number) {

    return number && !(number & (number - 1));

}

/*
 * Checks if a string has binary value inside.
 * @param  cadena String to be checked
 * @return 1 if it is binary. -1 if it is not binary. -2 if NULL char* param
 */
int isCorrectBinary(const char * cadena) {

    // if null return error -2
    if(cadena==NULL) {
        return -2;
    }
    
    for(int i= 0; cadena[i]; i++) {
        if(cadena[i]!='0'&&cadena[i]!='1'){
        	return -1;
	}
    }
    return 1;
}

/*
 * convert string into double. It can have a multiplier p for 1e-12, n for 1e-9, u for 1e-6, m for 1e-3. Other char will result in error.
 * @param  String to be converted into double
 * @return long with converted value or error. -1 for wrong value error. -2 for null pointer error.
 */
double parseDouble(const char * cadena) {

    if(cadena==NULL) {
        return -2;
    }
    // Obtain the multiplier p, n, u, or m. Else error
    long len=strlen(cadena);
    double multiplicador=1;
    if(cadena[len-1]=='m') {
        multiplicador=1.0/1000.0;
    } else if(cadena[len-1]=='u') {
        multiplicador=1.0/1000000.0;
    } else if(cadena[len-1]=='n') {
        multiplicador=1.0/1000000000.0;
    } else if(cadena[len-1]=='p') {
        multiplicador=1.0/1000000000000.0;
    } else if(cadena[len-1]>'9'||cadena[len-1]<'0') {
        return -1;

    }

    // if something not numeric or multiplier. error return -1
    for(long i=0; i<len-1; i++) {

        if(cadena[i]>'9'||cadena[i]<'0') {
            return -1;
        }
    }

    return atoi(cadena)*multiplicador;
}

/*
 * convert string hex address into long. It can have a the format 0x...... 
 * Other char will result in error.
 * @param  page_base_address to be converted into long
 * @return long with converted value or error. -1 for wrong value error. -2 for null pointer error.
 */
long parseAddress(const char* page_base_address){

        if(page_base_address==NULL){
		return -2;
	}

        long toReturn= strtol(page_base_address, NULL, 16);
	return toReturn;
        // provisional TODO
	return 33;
}
