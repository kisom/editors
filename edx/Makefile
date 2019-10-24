
# Comment out GLIBS and GINCS to not compile with GTK
# GLIBS = -DDOGTK -lgtk -lgdk
GLIBS = $(pkg-config --libs gtk+-3.0 glib-2.0)
# GINCS = -I/usr/include/gtk-1.2 -I/usr/include/glib-1.2 -I/usr/lib/glib/include
GINCS = $(pkg-config --cflags gtk+-3.0 glib-2.0)
LIBS = -L/usr/X11R6/lib -lX11 $(GLIBS)
CFLAGS = -g -O2 -fomit-frame-pointer -pipe -Wall $(GINCS)
#CFLAGS = -g -O2 -fomit-frame-pointer -pipe -Wall -DGREEK
STRIP = @ls -al $@;strip $@;ls -al $@

# select by uncommenting the desired keyboard binding type
TYPE = WORDSTAR
#TYPE = EMACS

# Use this for solaris
#LIBS = -L/usr/openwin/lib -lsocket -lnsl -lresolv -lXext -lX11
#CFLAGS = -O2 -pipe -Wall

all: edx e

edx: edx.c eeng.c ws.c emacs.c Makefile
	gcc edx.c $(CFLAGS) -D$(TYPE) $(LIBS) -o edx
	$(STRIP)

e: mye.c eeng.c Makefile
	gcc mye.c -O2 -DSTATICBUF -fomit-frame-pointer -ltermcap -o e
	$(STRIP)

clean:
	-rm -f *.o *.bak *~ edx e
