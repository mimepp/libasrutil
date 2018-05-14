## Process this file with automake to produce Makefile.in
include $(abs_top_builddir)/src/rules.mk

SUBDIRS = . tests

DIST_SUBDIRS = $(SUBDIRS)

lib_LTLIBRARIES = libasrutil.la

libasrutil_la_SOURCES = \
	asrutil.c			\
	asrutil_curl.c			\
	asrutil_text_translate.c	\
	asrutil_util.c			\
	asrutil_voice_engine.c
	
libasrutil_la_LIBADD = $(ASRUTIL_LIB_LDFLAGS)

AM_CPPFLAGS = -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64

#bin_PROGRAMS = asrutil

#asrutil_SOURCES = \
#	asrutil.h		\
#	asrutil.c

#asrutil_LDADD = $(ASRUTIL_BIN_LDFLAGS)

EXTRA_DIST = \
	rules.mk			\
	asrutil_curl.h			\
	asrutil.h			\
	asrutil_osd.h			\
	asrutil_text_translate.h	\
	asrutil_util.h			\
	asrutil_voice_engine.h

asrutildir=$(includedir)/asrutil
asrutil_HEADERS = \
	asrutil_curl.h			\
	asrutil.h			\
	asrutil_osd.h			\
	asrutil_text_translate.h	\
	asrutil_util.h			\
	asrutil_voice_engine.h

CLEANFILES =

if WITH_GOBJECT_INTROSPECTION
asrutil-1.0.gir: $(lib_LTLIBRARIES) $(G_IR_SCANNER) Makefile.am
	$(G_IR_SCANNER) \
	--warn-error \
	-v \
	--namespace asrutil \
	--nsversion 1.0 \
	--add-include-path=$(srcdir) \
	--add-include-path=$(builddir) \
	--library=$(lib_LTLIBRARIES) \
	--output $@ \
	--verbose \
	--pkg=glib-2.0 \
	$(asrutil_HEADERS) \
	$(libasrutil_la_SOURCES)

girdir = $(datadir)/gir-1.0
gir_DATA = asrutil-1.0.gir

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = asrutil-1.0.typelib

$(typelibs_DATA): $(gir_DATA)
	g-ir-compiler \
	--includedir=$(srcdir) \
	--includedir=$(builddir) \
	-o $@ $<

CLEANFILES += asrutil-1.0.gir $(typelibs_DATA)

endif # WITH_GOBJECT_INTROSPECTION
