#include "interfaz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "iniparser.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "parser.h"
#include "trazaParser.h"
#include "dataInterface.h"
#include "datos.h"
#include "callbacks.h"

int main(int argc, char *argv[]) {


    int error=readFile("L1.ini");
    //si ha habido error al readr el file de configuracion no se read el de trace
    if(error==0) {
        error=readFileTrace((char *)cpu.trace_file);
    }
    
    if(error!=0){
        exit(-1);
    }

    generateDataStorage();
    generateInterface(argc, argv);

    return 0;

}
