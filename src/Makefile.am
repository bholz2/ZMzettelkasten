## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = ZMzettelkasten

ZMzettelkasten_SOURCES = \
	main.c globals.h \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	my_file.c my_file.h \
	my_text.c my_text.h \
	my_zettel.c my_zettel.h \
	my_functions.c my_functions.h

ZMzettelkasten_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)
