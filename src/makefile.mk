OBJECTS += $(addprefix src/,cache.o hash.o)

SUBDIRS = src/util
include $(addsuffix /makefile.mk, $(SUBDIRS))
