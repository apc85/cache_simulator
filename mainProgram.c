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
#include <getopt.h>
#include "parser.h"
#include "trazaParser.h"
#include "dataInterface.h"
#include "datos.h"
#include "callbacks.h"

#define PROGRAM_NAME "cache_simulator"
#define VERSION "0.1"

void printUsage() {
   printf(
      "Usage: %s [OPTION]... <file>.ini\n"
      "Simulate the memory hierarchy defined in <file>.ini.\n"
      "This a list of the options accepted:\n"
      "\n"
      "  -h    display this help and exit\n"
      "  -v    output version information and exit\n"
      "\n"
      "Exit status:\n"
      " 0  if OK,\n"
      " 1  if configuration or other kind of errors.\n"
      "\n", PROGRAM_NAME);
}

int main(int argc, char *argv[]) {

   int c;

   /* Parse command line arguments */
   opterr = 0;
   while ((c = getopt (argc, argv, "vh")) != -1)
    switch (c)
      {
      case 'v':
        printf("%s version %s\n",PROGRAM_NAME,VERSION);
        return 0;
      case 'h':
        printUsage();
        return 0;
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort();
      }

    /* Check that there is at leas one configuration file */
    if(optind == argc) {
       fprintf (stderr, "Must supply a <file>.ini on the command line.\n");
       return 1;
    }

    /* Load configuration file */
    if(readFile(argv[optind]) != 0) {
       return 1;
    }
    /* Load trace file specified in the configuration file */
    if(readFileTrace((char *)cpu.trace_file) != 0) {
       return 1;
    }

    /* Create simulator data structures */
    generateDataStorage();

    /* Start GUI */
    generateInterface(argc, argv);

    return 0;
}
