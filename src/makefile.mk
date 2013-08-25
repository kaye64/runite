OBJECTS += $(addprefix src/,cache.o hash.o file.o)

SUBDIRS = src/util
include $(addsuffix /makefile.mk, $(SUBDIRS))
