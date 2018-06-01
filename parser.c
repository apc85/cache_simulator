#include "parser.h"

char* str_replacementPolicy[]= {"lru", "lfu", "rnd", "fifo"};
char* str_writePolicy[]= {"wt", "wb"};
char* keysCPU[]= {"word_width", "frequency", /*"bus_frequency", "mem_bus_frequency",*/"trace_file"};
char* keysMEMORY[]= {"size", "access_time_1","access_time_burst", "page_size"/*, "bus_width", "bus_frequency","access_time"*/};
char* keysCACHE[]= {"line_size", "size","asociativity", "write_policy", "replacement","separated","column_bit_mask"};
char* str_true[]= {"1", "yes", "true"};
char* str_false[]= {"0","no","false"};



/**
 * This functions read the simulator configuration file.
 * @param ini_name the file name
 *
 */
int readFile(char * ini_name) {


    numberCaches=0;
    int errors=0;

    dictionary  *   ini ;

    ini = iniparser_load(ini_name);

    //load the file
    if (ini==NULL) {
        fprintf(stderr, "Error loading file: %s\n", ini_name);
        return -1 ;
    }

    iniparser_dump(ini, stderr);

    int numSections=iniparser_getnsec(ini);

    int ncpu=0;
    int nmem=0;
    int ncaches=0;

    printf("\n");

    //check all the file sections are correct. No missing sections. No unknown sections.


    for(int i=0; i<numSections; i++) {
        const char * section=iniparser_getsecname(ini, i);

        //if the name of the section is cpu es cpu
        if(strcmp(section, "cpu")==0) {
            //count cpu sections. There must be only one cpu section
            ncpu++;
            //If the name of the section is memory
        } else if(strcmp(section, "memory")==0) {
            //Count memory sections. There must be only one memory section
            nmem++;
            //If the name of the section is like "cache..."
        } else if(strncmp(section, "cache", 5)==0) {


            int len=strlen(section)-5;
            int numCorrecto=1;

            //Error if section length is 5. It must have a number at the end. cacheN
            if(len==0) {
                printf("Error: Invalid [cache] section. It must contain the cache level number. [cacheN]\n");
                numCorrecto=0;
                errors++;
            }




            //If number value is not valid.
            char num[len+1];
            for(int j=0; j<len && numCorrecto; j++) {
                num[j]=section[j+5];
                if(num[j]<'0'||num[j]>'9') {
                    numCorrecto=0;
                    printf("Error: Invalid cache section name [%s]\n", section);
                    errors++;
                }

            }

            //If the number is correct I check its internal sections
            if(numCorrecto) {

                //Highest cache level must be equal to the total amount of caches.
                int Ncache=atoi(num);
                if(Ncache>ncaches) {
                    ncaches=Ncache;
                }


                int nkeys=iniparser_getsecnkeys(ini, section);
                const char * keys[nkeys];
                iniparser_getseckeys(ini, section, keys);


                //Look for unknown keys in every cache section.
                for(int i=0; i<nkeys; i++) {

                    //Para eso comparo conn todas las keys posibles.
                    int correcto=0;
                    for(int j=0; j<NCLAVES_CACHE; j++) {
                        char key[200];
                        sprintf(key, "%s:%s", section, keysCACHE[j]);
                        if(strcmp(keys[i], key)==0) {
                            correcto=1;
                            break;
                        }
                    }
                    /* error key */
                    if(correcto==0) {
                        printf("Error: unknown key %s\n", keys[i]);
                        errors++;
                    }

                }

            }

            //if section name isn't "cpu" and section name isn't "memory" and section name isn't like "cache..." then error.
        } else {
            printf("Error: Unknown section name [%s]\n", section);
            errors++;
        }
    }

    //error if zero cpu sections where read
    if(ncpu==0) {
        printf("Error: Missing mandatory section [cpu]\n");
        errors++;

        //look for unknown keys in [cpu] section
    } else {
        int nkeys=iniparser_getsecnkeys(ini, "cpu");
        const char * keys[nkeys];
        iniparser_getseckeys(ini, "cpu", keys);

        //I compare each key with all the possible valid keys to ensure it is correct.
        for(int i=0; i<nkeys; i++) {

            int correcto=0;
            for(int j=0; j<NCLAVES_CPU; j++) {

                char key[200];
                sprintf(key, "cpu:%s", keysCPU[j]);
                if(strcmp(key, keys[i])==0) {
                    correcto=1;
                    break;
                }



            }

            /* error key */
            if(correcto==0) {
                printf("Error: Unknown key %s\n", keys[i]);
                errors++;
            }

        }


    }

    //error if zero memory section where read.
    if(nmem==0) {
        printf("Error: Missing mandatory section [memory]\n");
        errors++;
    } else {
        int nkeys=iniparser_getsecnkeys(ini, "memory");
        const char * keys[nkeys];
        iniparser_getseckeys(ini, "memory", keys);

        //I compare each key with all the possible valid keys to ensure it is correct.
        for(int i=0; i<nkeys; i++) {

            int correcto=0;
            for(int j=0; j<NCLAVES_MEMORY; j++) {

                char key[200];
                sprintf(key, "memory:%s", keysMEMORY[j]);
                if(strcmp(key, keys[i])==0) {
                    correcto=1;
                    break;
                }



            }
            /* error key */
            if(correcto==0) {
                printf("Error: Unknown key %s\n", keys[i]);
                errors++;
            }

        }


    }



    //error if the number of caches is higher than maximun amount of caches allowed
    if(ncaches>MAX_CACHES) {
        printf("Error: The number of caches is excesive.\n");
        errors++;
        printf("\nTotal errors: %d\n", errors);
        return -1;
    }

    numberCaches=ncaches;





    //READING CPU CONFIGURATION///////////////////////////////////////////////

    //reading key cpu:word_width
    const char * cpu_word_width= iniparser_getstring(ini, "cpu:word_width", NULL);
    long long_cpu_word_width= parseInt(cpu_word_width);

    if(long_cpu_word_width==-1) {
        printf("Error: cpu:word_width value is not valid\n");
        errors++;
    } else if(long_cpu_word_width==-2) {
        printf("Error: Missing value cpu:word_width\n");
        errors++;
    } else {
        cpu.word_width=long_cpu_word_width;
    }

    //reading key cpu:frequency
    const char * cpu_frequency= iniparser_getstring(ini, "cpu:frequency", NULL);
    long long_cpu_frequency= parselongK1000(cpu_frequency);

    if(long_cpu_frequency==-1) {
        printf("Error: cpu:frequency value is not valid\n");
        errors++;

    } else if(long_cpu_frequency==-2) {
        printf("Error: Missing value cpu:frequency\n");
        errors++;
    } else {
        cpu.frequency=long_cpu_frequency;
    }

    //reading key cpu:bus_frequency
    /*
    const char * cpu_bus_frequency= iniparser_getstring(ini, "cpu:bus_frequency", NULL);
    long long_cpu_bus_frequency= parselongK1000(cpu_bus_frequency);

    if(long_cpu_bus_frequency==-1) {
        printf("Error: cpu:bus_frequency value is not valid\n");
        errors++;
    } else if(long_cpu_bus_frequency==-2) {
        printf("Error: Missing value cpu:bus_frequency\n");
        errors++;
    } else {
        cpu.bus_frequency=long_cpu_bus_frequency;

    }
    */

    //reading key cpu:mem_bus_frequency
    /*
    const char * cpu_mem_bus_frequency= iniparser_getstring(ini, "cpu:mem_bus_frequency", NULL);
    long long_cpu_mem_bus_frequency= parselongK1000(cpu_mem_bus_frequency);

    if(long_cpu_mem_bus_frequency==-1) {
        printf("Error: cpu:mem_bus_frequency value is not valid\n");
        errors++;
    } else if(long_cpu_mem_bus_frequency==-2) {
        printf("Error: Missing value cpu:mem_bus_frequency\n");
        errors++;
    } else {
        cpu.mem_bus_frequency=long_cpu_mem_bus_frequency;
    }
    */

    //reading key cpu:trace_file
    const char * cpu_trace_file=          iniparser_getstring(ini, "cpu:trace_file", NULL);
    if(cpu_trace_file==NULL) {
        printf("Error: Missing value cpu:trace_file\n");
        errors++;
    } else {
        cpu.trace_file=cpu_trace_file;
    }






    //READING MEMORY CONFIGURATION//////////////////////////////////////

    //reading key memory:size
    const char * mem_size= iniparser_getstring(ini, "memory:size", NULL);
    long long_mem_size= parselongK1024(mem_size);

    if(long_mem_size==-1) {
        printf("Error: memory:mem_size value is not valid\n");
        errors++;
    } else if(long_mem_size==-2) {
        printf("Error: Missing value memory:mem_size\n");
        errors++;
    } else {
        memory.size=long_mem_size;
    }

    //reading key memory:access_time_1
    const char * mem_access_time_1=           iniparser_getstring(ini, "memory:access_time_1", NULL);
    double double_mem_access_time_1=            parseDouble(mem_access_time_1);

    if(double_mem_access_time_1==-1) {
        printf("Error: memory:access_time value is not valid\n");
        errors++;
    } else if(double_mem_access_time_1==-2) {
        printf("Error: Missing value memory:access_time\n");
        errors++;
    } else {
        memory.access_time_1=double_mem_access_time_1;
    }


    //reading key memory:access_time_burst
    const char * mem_access_time_burst=           iniparser_getstring(ini, "memory:access_time_burst", NULL);
    double double_mem_access_time_burst=            parseDouble(mem_access_time_burst);

    if(double_mem_access_time_burst==-1) {
        printf("Error: memory:access_time value is not valid\n");
        errors++;
    } else if(double_mem_access_time_burst==-2) {
        printf("Error: Missing value memory:access_time\n");
        errors++;
    } else {
        memory.access_time_burst=double_mem_access_time_burst;
    }

    //reading key memory:page_size
    const char * page_size= iniparser_getstring(ini, "memory:page_size", NULL);
    long long_page_size= parselongK1024(page_size);

    if(long_page_size==-1) {
        printf("Error: memory:page_size value is not valid\n");
        errors++;
    } else if(long_page_size==-2) {
        printf("Error: Missing value memory:page_size\n");
        errors++;
    } else {
        memory.page_size=long_page_size;
    }

    //reading key memory:bus_width
    /*
    const char * mem_bus_width= iniparser_getstring(ini, "memory:bus_width", NULL);
    long long_mem_bus_width= parseInt(mem_bus_width);

    if(long_mem_bus_width==-1) {
        printf("Error: memory:bus_width value is not valid\n");
        errors++;
    } else if(long_mem_bus_width==-2) {
        printf("Error: Missing value memory:bus_width\n");
        errors++;
    } else {
        memory.bus_width=long_mem_bus_width;
    }
    */

    //reading key memory:bus_frequency
    /*
    const char * mem_bus_frequency=       iniparser_getstring(ini, "memory:bus_frequency", NULL);
    long long_mem_bus_frequency=          parselongK1000(mem_bus_frequency);

    if(long_mem_bus_frequency==-1) {
        printf("Error: memory:bus_frequency value is not valid\n");
        errors++;
    } else if(long_mem_bus_frequency==-2) {
        printf("Error: Missing value memory:bus_frequency\n");
        errors++;
    } else {
        memory.bus_frequency=long_mem_bus_frequency;
    }
    

    //reading key memory:access_time
    const char * mem_access_time=         iniparser_getstring(ini, "memory:access_time", NULL);
    long long_mem_access_time=            parseInt(mem_access_time);

    if(long_mem_access_time==-1) {
        printf("Error: memory:access_time value is not valid\n");
        errors++;
    } else if(long_mem_access_time==-2) {
        printf("Error: Missing value memory:access_time\n");
        errors++;
    } else {
        memory.access_time=long_mem_access_time;
    }
    */





    //READING ALL THE CACHES CONFIGURATION /////////////////////////////////////////

    //Browse the cache array and check the configuration of each cache.

    for(int i=0; i<numberCaches; i++) {

        /*This is for creating the string format in which the ini library receives the params.
        Each value must be refered as section:key */
        char cache[50]="cache";
        sprintf(cache, "cache%d", i+1);
        char param[50];


        //reading key cache:line_size
        sprintf(param, "%s:line_size", cache);

        const char * cache_line_size=iniparser_getstring(ini, param, NULL);
        long long_line_size=parselongK1024(cache_line_size);
        if(long_line_size==-1) {
            printf("Error: cache%d:line_size value is not valid\n", i+1);
            errors++;
        } else if(long_line_size==-2) {
            printf("Error: Missing value cache%d:line_size\n", i+1);
            errors++;

        } else {
            caches[i].line_size=long_line_size;
        }



        //reading key cache:size
        sprintf(param, "%s:size", cache);

        const char * cache_size=iniparser_getstring(ini, param, NULL);
        long long_size=parselongK1024(cache_size);
        if(long_size==-1) {
            printf("Error: cache%d:size value is not valid\n", i+1);
            errors++;
        } else if(long_size==-2) {
            printf("Error: Missing value cache%d:size\n", i+1);
            errors++;
        } else {
            caches[i].size=long_size;
        }

        //reading key cache:column_bit_mask
        sprintf(param, "%s:column_bit_mask", cache);

        const char * column_bit_mask=iniparser_getstring(ini, param, NULL);
        int isBinary=isCorrectBinary(column_bit_mask);
        if(isBinary==-1) {
            printf("Error: cache%d:column_bit_mask value is not valid\n", i+1);
            errors++;
        } else if(isBinary==-2) {
            printf("Error: Missing value cache%d:column_bit_mask\n", i+1);
            errors++;
        } else if(strlen(column_bit_mask)<N_BITS_MASK){
            printf("Error: Bit mask is to small cache%d:column_bit_mask\n", i+1);
            errors++;
        }else{
            caches[i].column_bit_mask=column_bit_mask;
        }

        //reading key cache:asocitivity
        sprintf(param, "%s:asociativity", cache);

        const char * cache_asociativity=iniparser_getstring(ini, param, NULL);
        //si es F es de compleatamente asociativa. Un solo set. Tantas lines/set como lines totales.
        if(cache_asociativity!=NULL&&strcmp(cache_asociativity, "F")==0) {
            caches[i].asociativity=caches[i].size/caches[i].line_size;
        } else {

            long long_asociativity=parseInt(cache_asociativity);
            if(long_asociativity==-1) {
                printf("Error: cache%d:asociativity value is not valid\n", i+1);
                errors++;
            } else if(long_asociativity==-2) {
                printf("Error: Missing value cache%d:asociativity\n", i+1);
                errors++;
            } else if(!isPowerOf2(long_asociativity)) {
                printf("Error: The value of cache%d:asociativity must be power of 2\n", i+1);
                errors++;
            } else {
                caches[i].asociativity=long_asociativity;

            }

        }


        //reading key cache:write_policy
        sprintf(param, "%s:write_policy", cache);
        const char * cache_write_policy=iniparser_getstring(ini, param, NULL);
        long long_write_policy=parseWritePolicy(cache_write_policy);
        if(long_write_policy==-1) {
            printf("Error: cache%d:write_policy value is not valid\n", i+1);
            errors++;
        } else if(long_write_policy==-2) {
            printf("Error: Missing value cache%d:write_policy\n", i+1);
            errors++;
        } else {
            caches[i].write_policy=long_write_policy;
        }

        //reading key cache:replacement
        sprintf(param, "%s:replacement", cache);

        const char * cache_replacement=iniparser_getstring(ini, param, NULL);
        long long_replacement=parseReplacementPolicy(cache_replacement);
        if(long_replacement==-1) {
            printf("Error: cache%d:replacement value is not valid\n", i+1);
            errors++;
        } else if(long_replacement==-2) {
            printf("Error: Missing value cache%d:replacement\n", i+1);
            errors++;
        } else {
            caches[i].replacement=long_replacement;
        }



        //reading key cache:separated
        sprintf(param, "%s:separated", cache);

        const char * cache_separated=iniparser_getstring(ini, param, NULL);
        long long_separated=parseBoolean(cache_separated);
        if(long_separated==-1) {
            printf("Error: cache%d:separated value is not valid\n", i+1);
            errors++;
        } else if(long_separated==-2) {
            printf("Error: Missing value cache%d:separated\n", i+1);
            errors++;
        } else {
            caches[i].separated=long_separated;
        }
    }

    if(errors>0) {
        printf("\nTotal errors: %d\n", errors);
        return -1;
    } else {
        showState();
        printf("\nConfiguration file loaded correctly\n");
        return 0;
    }

}






/*
 * shows the current state of read data
 */


void showState() {

    //show cpu info
    printf("\nCPU\n");

    printf("word_width:         [%ld bits] \n", cpu.word_width);
    printf("frecuency:          [%ld Hz] \n", cpu.frequency);
    printf("bus_frequency:      [%ld Hz] \n", cpu.bus_frequency);
    printf("mem_bus_frequency:  [%ld Hz] \n", cpu.mem_bus_frequency);
    printf("trace_file:         [%s]\n", cpu.trace_file);


    //show memory info
    printf("\nMEMORY\n");

    printf("size:               [%ld bytes] \n",  memory.size);
    printf("bus_width:          [%ld bits] \n", memory.bus_width);
    printf("bus_frequency:      [%ld Hz] \n", memory.bus_frequency);
    printf("access_time_1:      [%lf ns] \n", memory.access_time_1*1000000000);
    printf("access_time_burst:  [%lf ns] \n", memory.access_time_burst*1000000000);
    printf("page_size:          [%ld bytes] \n", memory.page_size);



    //show each cache info
    for(long i=0; i<numberCaches; i++) {
        printf("\nCACHE L%ld\n", i+1);

        printf("line_size:          [%ld bits] \n",  caches[i].line_size);
        printf("size:               [%ld bytes] \n",  caches[i].size);
        printf("asociativity:       [%ld]\n",  caches[i].asociativity);
        printf("write_policy:       [%s]\n",   str_writePolicy[caches[i].write_policy]);
        printf("replacement:        [%s]\n",   str_replacementPolicy[caches[i].replacement]);
        printf("separated:          [%d]\n",   caches[i].separated);

    }

}




/*
 * convert string into long. It can have a multiplier G for 10^9, M for 10^6 o K for 10^3. Any other char will result in error.
 * @param  String to be converted into long
 * @return long with converted value or error. -1 for wrong value error. -2 for null pointer error.
 */
long parselongK1000(const char * cadena) {

    if(cadena==NULL) {
        return -2;
    }
    //Obtain the multiplier k, M, or G. Else error
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

    //if something not numeric or multiplier. error return -1
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
long parselongK1024(const char * cadena) {

    if(cadena==NULL) {
        return -2;
    }

    //Obtain the multiplier k, M, or G. Else error
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

    //if something not numeric or multiplier. error return -1
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
    //Obtain the multiplier k, M, or G. Else error.
    int len=strlen(cadena);

    //if something not numeric or multiplier. error return -1
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

    //if null return error -2
    if(cadena==NULL) {
        return -2;
    }

    //turn into lower case
    char cadenaMin[10];
    int i;
    for( i= 0; cadena[i]; i++) {
        cadenaMin[i] = tolower(cadena[i]);
    }
    cadenaMin[i]='\0';



    //check string content and retun equivalent boolean value.
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

    //if null pointer retun error -2
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

    //if null return error -2
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
    //Obtain the multiplier p, n, u, or m. Else error
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

    //if something not numeric or multiplier. error return -1
    for(long i=0; i<len-1; i++) {

        if(cadena[i]>'9'||cadena[i]<'0') {
            return -1;
        }
    }

    return atoi(cadena)*multiplicador;
}

