#include "callbacks.h"
#include "datainterface.h"
#include "traceparser.h"
#include "simulator.h"

//CALLBACKS//////////////////////////////////////////////////////////////


/**
 * Callback function for setting simulation to its initial state.
 */
void callbackRestart( GtkWidget *widget, gpointer   data){
   // Reset each cache
   for(int i=0; i<numberCaches; i++){
      // If it is a divided cache, both parts must be reset
      if(caches[i].separated){
         resetDataCache(i);
         resetInstructionCache(i);
      } else {
         resetCache(i);
      }
   }
   resetMemory();

   // Go to the first line in the trace panel */
   GtkTextIter lineIterCurrent;
   gtk_text_buffer_get_iter_at_mark (buffer, &lineIterCurrent, marcaLineCurrent);
   GtkTextIter start;
   gtk_text_buffer_get_iter_at_offset (buffer, &start, 0);
   GtkTextIter end=lineIterCurrent;
   GtkTextIter lineIterSiguiente=lineIterCurrent;
   gtk_text_view_forward_display_line (GTK_TEXT_VIEW(text_view), &lineIterSiguiente);
   // Remove all tags for removing colour.
   gtk_text_buffer_remove_all_tags (buffer,
         &start,
         &end);
   gtk_text_buffer_get_iter_at_offset (buffer, &lineIterInicial, 0);
   marcaLineCurrent= gtk_text_mark_new (NULL, 1);
   gtk_text_buffer_add_mark (buffer,
         marcaLineCurrent,
         &lineIterInicial);
   // Scroll each panel to first line
   scrollMemoryToPos(0);
   scrollTextViewToLine(0);
   for(int i=0; i<numberCaches; i++){
      if(caches[i].separated){
         scrollDataCacheToRow(i, 0);
         scrollInstructionCacheToRow(i, 0);
      }else{
         scrollCacheToRow(i, 0);
      }
   }
}

/**
 * Callback fuction to perform the complete simulation
 */
void callbackSimulateAll( GtkWidget *widget, gpointer   data)
{
   char *currentLine;
   while(currentLine = nextLineTrace()) {
      if(preprocessTraceLine(currentLine)) {
         struct memOperation operation;
         parseLine(currentLine, -1, &operation);
         simulate_step(&operation);
      }
   }
}

/**
 * Callback fuction for advancing one step on simulation 
 */
void callbackNextStep( GtkWidget *widget, gpointer   data)
{
   removeAllColors();
   char *currentLine = nextLineTrace();
   if(currentLine!=NULL&&preprocessTraceLine(currentLine)) {
      struct memOperation operation;
      parseLine(currentLine, -1, &operation);
      simulate_step(&operation);
   }

}


#if DEBUG
/**
 * Callback fuction for testing the program. Only in debug mode
 */
void callbackTest( GtkWidget *widget, gpointer   data){

   //////pruebas
   setStatistics("Cache L2", "Fault rate", "tres");
   setStatistics("CPU", "Fault rate", "tres");
   setStatistics("Cache L3", "nueva categoria", "cuatro");
   setStatistics("Cache NUEVA", "nueva categoria2", "cuatro2");
   char *value;
   value=getStatistics("Cache L1", "Fault rate");
   printf("valor: %s\n", value);
   value=getStatistics("Cache NUEVA", "nueva categoria2");
   printf("valor: %s\n", value);
   value=getStatistics("Cache NUEVA", "ia2");
   printf("valor: %s\n", value);
   value=getStatistics("CaUEVA", "ia2");
   printf("valor: %s\n", value);
   struct memoryPosition pos;
   pos.address=256;
   pos.content=1024;
   char cadena[]="hola mundo. Este es el contenido del usuario.";
   pos.user_content=cadena;
   writeMemoryAddress(&pos, 134217732);
   showMemoryAddress(9);
   showMemoryAddress(10);
   showMemoryAddress(11);
   struct cacheLine line;
   line.tag=2;
   line.line=1;
   line.set=4;
   line.user_content="hola mundo. Este es el contenido del usuario en la linea cache.";
   line.dirty=2;
   line.valid=1;
   line.last_accessed=3;
   line.times_accessed=4;
   line.first_accessed=5;
   line.content=(long[]){1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
   void * user_use;
   writeLineCacheInstructions(0, &line, 7);
   line.content=(long[]){1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
   writeLineCacheData(0, &line, 7);
   line.content=(long[]){1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
   writeLineCache(1, &line, 7);
   showCacheLineInstructions(0, 7);
   showCacheLineData(0, 7);
   showCacheLine(1, 7);
   showCacheLine(1, 6);
   setMemoryAddressColor(134217732, 5);
   setColorDataCacheLine(0, 2, 9);
   setColorInstructionsCacheLine(0, 2, 10);
   //setColorInstructionsCacheLine(0, 2, 11);
   setColorCacheLine(1, 2, 5);
   //free((line).content);
   scrollMemoryToPos(134217832);
   scrollMemoryToPos(518);
   scrollDataCacheToRow(0, 32);
   scrollInstructionCacheToRow(0, 32);
   scrollCacheToRow(1, 32);

}
#endif


/* Destroy callback. This is used when you close the program*/
void destroy( GtkWidget *widget, gpointer   data )
{
   gtk_main_quit ();
}

/* Delete_event callback. This is used when you close the program*/
gint delete_event( GtkWidget *widget,
      GdkEvent  *event,
      gpointer   data )
{
   /* If you return FALSE in the "delete_event" signal handler,
    * GTK will emit the "destroy" signal. Returning TRUE means
    * you don't want the window to be destroyed.
    * This is useful for popping up 'are you sure you want to quit?'
    * type dialogs. */
   //g_print ("fin del programa\n");
   /* Change TRUE to FALSE and the main window will be destroyed with
    * a "delete_event". */
   return FALSE;
}
