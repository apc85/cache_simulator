#include <stdio.h>
#include <ctype.h>
#include <getopt.h>

#include "confparser.h"
#include "traceparser.h"
#include "datastore.h"
#include "gui.h"
#include "simulator.h"

#define PROGRAM_NAME "cache_simulator"
#define VERSION "0.1"

void printUsage() {
   printf(
      "Usage: %s [OPTION]... <file>.ini\n"
      "Simulate the memory hierarchy defined in <file>.ini.\n"
      "This a list of the options accepted:\n"
      "\n"
      "  -g    toggle GUI\n"
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

   // Parse command line arguments
   opterr = 0;
   while ((c = getopt (argc, argv, "vhg")) != -1)
    switch (c)
      {
      case 'g':
         useGUI = !useGUI;
        break;
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


    // Check that there is at leas one configuration file 
    if(optind == argc) {
       fprintf (stderr, "Must supply a <file>.ini on the command line.\n");
       return 1;
    }

    // Read configuration file
    dictionary *ini;
    if((ini = readConfigurationFile(argv[optind])) == NULL) {
       return 1;
    }

    // Parse read configuration and populate global configuration variables.
    if(parseConfiguration(ini) != 0) {
       return 1;
    }

    // Load trace file specified in the configuration file
    if(readTraceFile((char *)cpu.trace_file) != 0) {
       return 1;
    }

    // Create simulator data structures
    generateDataStorage();

    // Start GUI...
    if(useGUI) {
       generateGUI(argc, argv);
    // ... or run batch simulation
    } else {
       simulate();
    }

    // Free memory allocated by readTraceFile()
    freeMemory();

    return 0;
}
