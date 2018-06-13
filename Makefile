

MODULES = gui.c datainterface.c datamanipulation.c confparser.c iniparser.c datastore.c callbacks.c traceparser.c dictionary.c simulator.c

DEBUG=0
CFLAGS += -DDEBUG=${DEBUG}  $(shell pkg-config --cflags gtk+-2.0) --coverage
LIBS = $(shell pkg-config --libs gtk+-2.0) -lm

cache_simulator: mainprogram.c ${MODULES}
	gcc -o $@ ${CFLAGS} $< ${MODULES} ${LIBS}

#cache_simulator: mainprogram.c ${MODULES}
#	gcc ${CFLAGS} $< ${MODULES} -o $@ -pthread -I/usr/include/gtk-2.0 -I/usr/lib/x86_64-linux-gnu/gtk-2.0/include -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/libpng12 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng12 -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/freetype2 -lgtk-x11-2.0 -lgdk-x11-2.0 -lpangocairo-1.0 -latk-1.0 -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lpangoft2-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lfontconfig -lfreetype -lm

clean:
	rm -fr *~
	rm *.g*

