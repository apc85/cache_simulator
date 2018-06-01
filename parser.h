#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "iniparser.h"


  struct structCpu
  {
      long word_width;            // Número de bits en la word del procesador
      long frequency;             // Frecuencia en herzios. Ver Nota 1.
      long bus_frequency;         // Frecuencia en herzios del bus longerno. Ver Nota 1.
      long mem_bus_frequency;     // Frecuencia en herzios del bus. Ver Nota 1.
      const char* trace_file;     // File de trace
  };

  struct structMemory
  {
      long size;                  // Tamaño de la memory en bytes. Ver Nota 2.
      long bus_width;             // Tamaño del bus extremo en bits.
      long bus_frequency;         // Frecuencia en herzios del bus. Ver Nota 1.
      double access_time_1;           // Timepo de acceso en nanosegundos.
      double access_time_burst;
      long page_size;

  };

  struct structCache
  {
      long line_size;               // Tamaño de line en bytes. Ver Nota 2, aunque no has mucho sentido prácitco, pero seamos coherentes.
      long size;                    // Tamaño cache en bytes. Ver Nota 2.
      long asociativity;            // Asociatividad: 1= Mapeo directo, F= totalmente asociatva, cualquier user_use número potencia de dos.    
      long write_policy;            // Política de escritura: wt = write through, wb = write back
      long replacement;             // Política de reemplazo: lru, rand
      int separated;                // Si la memory cache es separada. Nota 3.
      const char* column_bit_mask;  //Esto indica que columnas se muestran y cuales se ocultan. Mascara en binario
  };

  #define N_REPLACEMENT 4
  char* str_replacementPolicy[4];
  enum replacement {LRU=0, LFU=1, RANDOM=2, FIFO=3};

  #define N_WRITE 2
  char* str_writePolicy[2];
  enum write_policy {WRITE_THROUGH=0, WRITE_BACK=1};


  #define NCLAVES_CPU 3
  char* keysCPU[NCLAVES_CPU];
  #define NCLAVES_MEMORY 4
  char* keysMEMORY[NCLAVES_MEMORY];
  #define NCLAVES_CACHE 7
  char* keysCACHE[NCLAVES_CACHE];

  #define N_TRUES 3
  char* str_true[3];
  #define N_FALSES 3
  char* str_false[3];

  #define MAX_CACHES 10
  int numberCaches;

  #define N_BITS_MASK 8

  struct structCpu cpu;
  struct structMemory memory;
  struct structCache caches[MAX_CACHES];


  int readFile(char * file);
  void showState();
  long parselongK1000(const char * cadena);
  long parselongK1024(const char * cadena);
  int parseBoolean(const char * cadena);
  int parseInt(const char * cadena);
  int parseReplacementPolicy(const char * cadena);
  int parseWritePolicy(const char * cadena);
  int isPowerOf2(long number);
  int isCorrectBinary(const char * cadena);
  double parseDouble(const char * cadena);

#endif
