#include "interfaz.h"


int generateInterface(int argc, char *argv[]) {

    //esto se usara mas adelante. Es para colorear texts.
    tagBlue = gtk_text_buffer_create_tag (buffer, "blue_foreground", "foreground", "blue", NULL);
    tagBlack = gtk_text_buffer_create_tag (buffer, "black_foreground", "foreground", "black", NULL);
    //Esto tabién se usara mas adelante es para recorrer el buffer de text del file trace
    gtk_text_buffer_get_iter_at_offset (buffer, &lineIterInicial, 0);
    marcaLineCurrent= gtk_text_mark_new (NULL, 1);
    gtk_text_buffer_add_mark (buffer,
                              marcaLineCurrent,
                              &lineIterInicial);

    /* This is called in all GTK applications. Arguments are parsed
     * from the command line and are returned, to the application. */
    gtk_init (&argc, &argv);

    /* create a new window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 700, 400);

    /* When the window is given the "delete_event" signal (this is given
     * by the window manager, usually by the "close" option, or on the
     * titlebar), we ask it to call the delete_event () function
     * as defined above. The data passed to the callback
     * function is NULL and is ignored in the callback function. */
    g_signal_connect (G_OBJECT (window), "delete_event",
                      G_CALLBACK (delete_event), NULL);

    /* Here we connect the "destroy" event to a signal handler.
     * This event occurs when we call gtk_widget_destroy() on the window,
     * or if we return FALSE in the "delete_event" callback. */
    g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (destroy), NULL);

    /* Sets the border width of the window. */
    gtk_container_set_border_width (GTK_CONTAINER (window), 5);

    verticalBox= gtk_vbox_new (FALSE, 5);
    gtk_container_add (GTK_CONTAINER (window), verticalBox);

    buttonBox= gtk_hbox_new (FALSE, 10);
    //gtk_container_add (GTK_CONTAINER (verticalBox), buttonBox);
    gtk_box_pack_start (GTK_BOX(verticalBox), buttonBox, FALSE, FALSE, 5);

    button1= gtk_button_new_with_label ("Next step");
    button2= gtk_button_new_with_label ("Simulate all");
    button3= gtk_button_new_with_label ("Restart");
    g_signal_connect (button1, "clicked", G_CALLBACK (callbackNextStep), (gpointer) NULL);
    g_signal_connect (button3, "clicked", G_CALLBACK (callbackRestart), (gpointer) NULL);



    //gtk_container_add (GTK_CONTAINER (buttonBox), button1);
    //gtk_container_add (GTK_CONTAINER (buttonBox), button2);

    gtk_box_pack_start (GTK_BOX(buttonBox), button1, FALSE, FALSE, 5);
    gtk_box_pack_start (GTK_BOX(buttonBox), button2, FALSE, FALSE, 5);
    gtk_box_pack_start (GTK_BOX(buttonBox), button3, FALSE, FALSE, 5);

///creo la tabla para los paneles

    table = gtk_table_new (2, 4, TRUE);
    /* Coloca la tabla en la ventana principal */
    //gtk_container_add (GTK_CONTAINER (verticalBox), table);
    gtk_box_pack_start (GTK_BOX(verticalBox), table, TRUE, TRUE, 5);



    gtk_table_set_row_spacings(GTK_TABLE(table), 10);
    gtk_table_set_col_spacings(GTK_TABLE(table), 10);


///creo los buttones

    /* Creo la columna que representa a la memory */
    createPanelMemory();
    for(int i=0; i<numberCaches; i++) {
        createPanelCache(i);
    }





    vboxText  = gtk_vbox_new(FALSE, 2);

    //creo el text view
    //buffer = gtk_text_buffer_new (NULL);
    text_view = gtk_text_view_new_with_buffer (buffer);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_NONE);
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    //insertTextInPanel("holamundo");

    //creo su tag
    char cadena[200];
    sprintf(cadena, "TRACE FILE: %s", cpu.trace_file);
    GtkWidget *label;
    label =gtk_label_new (cadena);


    create_view_statistics();

    vbox_statistics  = gtk_vbox_new(FALSE, 2);

    //creo el text view
    //buffer = gtk_text_buffer_new (NULL);
    //text_view = gtk_text_view_new_with_buffer (buffer);
    //gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_NONE);
    scrolled_window2 = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window2), estatistics_view);

    //insertTextInPanel("holamundo");

    //creo su tag
    sprintf(cadena, "Simulation statistics");
    GtkWidget *label2;
    label2 =gtk_label_new (cadena);



    gtk_box_pack_start(GTK_BOX(vboxText), label, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(vboxText), scrolled_window, TRUE, TRUE, 1);
    gtk_box_pack_start(GTK_BOX(vbox_statistics), label2, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(vbox_statistics), scrolled_window2, TRUE, TRUE, 1);





    //gtk_container_add (GTK_CONTAINER (scrolled_window), text_view);
    //gtk_container_add (GTK_CONTAINER (scrolled_window), scrolled_window);

    // Coloco los buttones en la tabla


    //printf("%d\n", numberCaches);

    gtk_table_attach_defaults (GTK_TABLE (table), vboxText, 0, 1, 0, 1);
    gtk_table_attach_defaults (GTK_TABLE (table), vbox_statistics, 0, 1, 1, 2);
    gtk_table_attach_defaults (GTK_TABLE (table), vboxMEMORY, numberCaches+1, numberCaches+2, 0, 2);



    for(int i=0; i<numberCaches; i++) {
        if(!caches[i].separated) {
            gtk_table_attach_defaults (GTK_TABLE (table), cacheLevelPanels[i].vboxData, 1+i, 2+i, 0, 2);
        } else {
            gtk_table_attach_defaults (GTK_TABLE (table), cacheLevelPanels[i].vboxData, 1+i, 2+i, 0, 1);
            gtk_table_attach_defaults (GTK_TABLE (table), cacheLevelPanels[i].vboxInstruction, 1+i, 2+i, 1, 2);
        }
    }



    //gtk_table_attach_defaults (GTK_TABLE (table), button2, 1, 2, 0, 2);
    //gtk_table_attach_defaults (GTK_TABLE (table), button3, 2, 3, 0, 2);


    //insertTextInPanel("holamundo");
    /* and the window */




    gtk_widget_show_all(window);


    /* All GTK applications must have a gtk_main(). Control ends here
     * and waits for an event to occur (like a key press or
     * mouse event). */
    gtk_main ();

    //printf("%s\n", lineCurrent);
    return 0;
}




void nextLineTrace() {

    GtkTextIter lineIterCurrent;
    gtk_text_buffer_get_iter_at_mark (buffer, &lineIterCurrent, marcaLineCurrent);


    GtkTextIter start;
    gtk_text_buffer_get_iter_at_offset (buffer, &start, 0);
    GtkTextIter end=lineIterCurrent;



    GtkTextIter lineIterSiguiente=lineIterCurrent;
    gtk_text_view_forward_display_line (GTK_TEXT_VIEW(text_view), &lineIterSiguiente);

    /*
      gtk_text_buffer_remove_tag (buffer,
                                tagBlue,
                                &start,
                                &end);
    */
    //coloreo de negro la line anterior
    gtk_text_buffer_apply_tag (buffer, tagBlack, &start, &end);


    //coloreo de azul la line current
    gtk_text_buffer_apply_tag (buffer, tagBlue, &lineIterCurrent, &lineIterSiguiente);
    //scroll hasta que la nueva line es visible
    gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(text_view), &lineIterCurrent, 0.0, 0, 0.0, 0.0);



    //Almaceno la line Current
    lineCurrent=gtk_text_buffer_get_text (buffer, &lineIterCurrent, &lineIterSiguiente, 1);
    //printf("%s", lineCurrent);




    gtk_text_view_forward_display_line (GTK_TEXT_VIEW(text_view), &lineIterCurrent);


    gtk_text_buffer_move_mark (buffer, marcaLineCurrent, &lineIterCurrent);
    //gtk_text_buffer_add_mark (buffer, marcaLineCurrent, &lineIterCurrent);
    return;

}















GtkWidget * createPanelMemory() {

    GtkTreeIter   iter;
    GtkTreeViewColumn* column;

    vboxMEMORY = gtk_vbox_new(FALSE, 2);


    //createMemoryModel();



    viewMEMORY = gtk_tree_view_new_with_model(GTK_TREE_MODEL(modelMEMORY));


    GtkTreeSelection * selection= gtk_tree_view_get_selection (GTK_TREE_VIEW(viewMEMORY));
    gtk_tree_selection_set_mode (selection, GTK_SELECTION_NONE);




    column = gtk_tree_view_column_new_with_attributes("Address",
             gtk_cell_renderer_text_new(),
             "text", ADDRESS,
             "background", COLOR,
             NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(viewMEMORY), column);

    column = gtk_tree_view_column_new_with_attributes("Content",
             gtk_cell_renderer_text_new(),
             "text", CONTENT,
             "background", COLOR,
             NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(viewMEMORY), column);






    GtkWidget *scwin = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scwin), viewMEMORY);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwin), GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_ALWAYS);



    GtkWidget *label;
    label =gtk_label_new ("MEMORY");
    gtk_box_pack_start(GTK_BOX(vboxMEMORY), label, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(vboxMEMORY), scwin, TRUE, TRUE, 1);


    return vboxMEMORY;

}
















void createPanelCache(int level) {



    //Creo la cache. en caso de que sea dividia esta será la parte de data
    GtkTreeIter   iter;
    GtkTreeViewColumn* column;
    GtkListStore *modelData;
    GtkWidget *vboxData;
    GtkWidget *viewData;


    vboxData = gtk_vbox_new(FALSE, 2);



    int numLines=caches[level].size/caches[level].line_size;
    int asociativity=caches[level].asociativity;
    int numsets=numLines/asociativity;
    int numWords=(caches[level].line_size*8)/cpu.word_width;
    //printf("\ncache level %d: lines: %d, asociativity: %d, sets: %d, words line: %d\n", level+1, numLines, asociativity, numsets, numWords);


    //createCacheModel(level);
    modelData=cacheLevels[level].modelData;




    viewData = gtk_tree_view_new_with_model(GTK_TREE_MODEL(modelData));

    GtkTreeSelection * selectionData= gtk_tree_view_get_selection (GTK_TREE_VIEW(viewData));
    gtk_tree_selection_set_mode (selectionData, GTK_SELECTION_NONE);


    //CREO LAS COLUMNAS A MOSTRAR EN LA TABLA. No confundir con las columnas de la estructura de dados model
    column = gtk_tree_view_column_new_with_attributes("Line",
             gtk_cell_renderer_text_new(),
             "text", LINE,
             "background", COLOR_CACHE,
             NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(viewData), column);


    if(asociativity>1&&asociativity<numLines) {
        column = gtk_tree_view_column_new_with_attributes("Set",
                 gtk_cell_renderer_text_new(),
                 "text", SET,
                 "background", COLOR_CACHE,
                 NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(viewData), column);

    }


    column = gtk_tree_view_column_new_with_attributes("Tag",
             gtk_cell_renderer_text_new(),
             "text", TAG,
             "background", COLOR_CACHE,
             NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(viewData), column);





    column = gtk_tree_view_column_new_with_attributes("Content",
             gtk_cell_renderer_text_new(),
             "text", CONTENT_CACHE,
             "background", COLOR_CACHE,
             NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(viewData), column);

    ////////////////////////////////////////////////////////




    GtkWidget *scwin = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scwin), viewData);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwin), GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_ALWAYS);

    GtkWidget *label;


    //si es no es una cache dividida pongo el titulo y return de la funcion
    if(!caches[level].separated) {
        char cadena[50];
        sprintf(cadena, "CACHE L%d", level+1);
        label =gtk_label_new (cadena);
        gtk_box_pack_start(GTK_BOX(vboxData), label, FALSE, FALSE, 1);
        gtk_box_pack_start(GTK_BOX(vboxData), scwin, TRUE, TRUE, 1);


        cacheLevels[level].modelData=modelData;
        cacheLevelPanels[level].vboxData=vboxData;
        cacheLevelPanels[level].viewData=viewData;

        return;
        //si es dividida lo titulo como cache de data y creo la parte correspondiente a instructions
    } else {

        char cadena[50];
        sprintf(cadena, "CACHE L%d DATOS", level+1);
        label =gtk_label_new (cadena);
        gtk_box_pack_start(GTK_BOX(vboxData), label, FALSE, FALSE, 1);
        gtk_box_pack_start(GTK_BOX(vboxData), scwin, TRUE, TRUE, 1);


        cacheLevels[level].modelData=modelData;
        cacheLevelPanels[level].vboxData=vboxData;
        cacheLevelPanels[level].viewData=viewData;




        GtkTreeIter   iterInstruction;
        GtkTreeViewColumn* columnInstruction;
        GtkListStore *modelInstruction;
        GtkWidget *vboxInstruction;
        GtkWidget *viewInstruction;


        vboxInstruction = gtk_vbox_new(FALSE, 2);

        int numLines=caches[level].size/caches[level].line_size;
        int asociativity=caches[level].asociativity;

        modelInstruction=cacheLevels[level].modelInstruction;



        viewInstruction = gtk_tree_view_new_with_model(GTK_TREE_MODEL(modelInstruction));

        GtkTreeSelection * selectionInstruction= gtk_tree_view_get_selection (GTK_TREE_VIEW(viewInstruction));
        gtk_tree_selection_set_mode (selectionInstruction, GTK_SELECTION_NONE);



        //CREO LAS COLUMNAS A MOSTRAR EN LA TABLA. No confundir con las columnas de la estructura de dados model
        column = gtk_tree_view_column_new_with_attributes("Line",
                 gtk_cell_renderer_text_new(),
                 "text", LINE,
                 "background", COLOR_CACHE,
                 NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(viewInstruction), column);


        if(asociativity>1&&asociativity<numLines) {
            column = gtk_tree_view_column_new_with_attributes("Set",
                     gtk_cell_renderer_text_new(),
                     "text", SET,
                     "background", COLOR_CACHE,
                     NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(viewInstruction), column);

        }



        columnInstruction = gtk_tree_view_column_new_with_attributes("Tag",
                            gtk_cell_renderer_text_new(),
                            "text", TAG,
                            "background", COLOR_CACHE,
                            NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(viewInstruction), columnInstruction);




        columnInstruction = gtk_tree_view_column_new_with_attributes("Content",
                            gtk_cell_renderer_text_new(),
                            "text", CONTENT_CACHE,
                            "background", COLOR_CACHE,
                            NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(viewInstruction), columnInstruction);

        ///////////////////////////////////////////////////




        GtkWidget *scwinInstruction = gtk_scrolled_window_new(NULL, NULL);
        gtk_container_add(GTK_CONTAINER(scwinInstruction), viewInstruction);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwinInstruction), GTK_POLICY_AUTOMATIC,
                                       GTK_POLICY_ALWAYS);

        GtkWidget *labelInstruction;

        char cadenaInstructions[50];
        sprintf(cadenaInstructions, "CACHE L%d INSTRUCCIONES", level+1);
        labelInstruction =gtk_label_new (cadenaInstructions);

        gtk_box_pack_start(GTK_BOX(vboxInstruction), labelInstruction, FALSE, FALSE, 1);
        gtk_box_pack_start(GTK_BOX(vboxInstruction), scwinInstruction, TRUE, TRUE, 1);


        cacheLevels[level].modelInstruction=modelInstruction;
        cacheLevelPanels[level].vboxInstruction=vboxInstruction;
        cacheLevelPanels[level].viewInstruction=viewInstruction;

        return;


    }

}







GtkWidget *create_view_statistics(void) {
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    GtkTreeModel *model= estatistics_model;
    GtkWidget *view=     estatistics_view;

    view = gtk_tree_view_new();

    /* --- Column #1 --- */

    col = gtk_tree_view_column_new();

    gtk_tree_view_column_set_title(col, "First Name");

    /* pack tree view column into tree view */
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

    renderer = gtk_cell_renderer_text_new();

    /* pack cell renderer into tree view column */
    gtk_tree_view_column_pack_start(col, renderer, TRUE);

    /* connect 'text' property of the cell renderer to
     *  model column that contains the first name */
    gtk_tree_view_column_add_attribute(col, renderer, "text", COMPONET_OR_PROPERTY);


    /* --- Column #2 --- */

    col = gtk_tree_view_column_new();

    gtk_tree_view_column_set_title(col, "Last Name");

    /* pack tree view column into tree view */
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

    renderer = gtk_cell_renderer_text_new();

    /* pack cell renderer into tree view column */
    gtk_tree_view_column_pack_start(col, renderer, TRUE);

    /* connect 'text' property of the cell renderer to
     *  model column that contains the last name */
    gtk_tree_view_column_add_attribute(col, renderer, "text", VALUE);

    /* set 'weight' property of the cell renderer to
     *  bold print (we want all last names in bold) */
    g_object_set(renderer,
                 "weight", PANGO_WEIGHT_BOLD,
                 "weight-set", TRUE,
                 NULL);




    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(view),0);

    g_object_unref(model); /* destroy model automatically with view */

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(view)),
                                GTK_SELECTION_NONE);

    estatistics_view= view;
    return view;
}










void wordHexadecimal(char * returned, int number) {
    int numDigitos=0;

    if(number!=0) {
        numDigitos=(log(number)/log(16));
    }
    int totalDigitos=cpu.word_width/4;
    int numCeros=totalDigitos-numDigitos-1;

    char cadena[50]="";
    int i;
    for(i=0; i<numCeros; i++) {
        cadena[i]='0';
    }
    cadena[i+2]='\0';
    sprintf(returned, "%s%x", cadena, number);


}

void wordHexadecimalN(char * returned, int number, int totalDigitos) {
    int numDigitos=0;

    if(number!=0) {
        numDigitos=(log(number)/log(16));
    }

    int numCeros=totalDigitos-numDigitos-1;

    char cadena[50]="";
    int i;
    for(i=0; i<numCeros; i++) {
        cadena[i]='0';
    }
    cadena[i+2]='\0';
    sprintf(returned, "%s%x", cadena, number);


}



void insertTextInPanel(char* text) {
    GtkTextBuffer *buffer;
    GtkTextMark *mark;
    GtkTextIter iter;
    buffer =gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    mark = gtk_text_buffer_get_insert (buffer);
    gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);

    /* Insert newline (only if there's already text in the buffer). */
    //if (gtk_text_buffer_get_char_count(buffer))
    //gtk_text_buffer_insert (buffer, &iter, "\n", 1);

    gtk_text_buffer_insert (buffer, &iter, text, -1);
}




void scrollTextViewToLine(long line) {
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_line (buffer,
                                      &iter,
                                      line);
    //scroll hasta que la nueva line es visible
    gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(text_view), &iter, 0.0, 0, 0.0, 0.0);


    return;

}



void scrollMemoryToRow(long row) {
    char rowChar[50];
    sprintf(rowChar, "%ld", row);
    GtkTreePath * path=gtk_tree_path_new_from_string (rowChar);
    gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(viewMEMORY),
                                  path,
                                  NULL,
                                  TRUE,
                                  0.5,
                                  0);
}

void scrollCacheToRow(int level, long row) {

    scrollDataCacheToRow(level, row);


}
void scrollDataCacheToRow(int level, long row) {
    char rowChar[50];
    sprintf(rowChar, "%ld", row);
    GtkTreePath * path=gtk_tree_path_new_from_string (rowChar);
    gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(cacheLevelPanels[level].viewData),
                                  path,
                                  NULL,
                                  TRUE,
                                  0.5,
                                  0);

}





void scrollInstructionCacheToRow(int level, long row) {

    char rowChar[50];
    sprintf(rowChar, "%ld", row);
    GtkTreePath * path=gtk_tree_path_new_from_string (rowChar);
    gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(cacheLevelPanels[level].viewInstruction),
                                  path,
                                  NULL,
                                  TRUE,
                                  0.5,
                                  0);

}




