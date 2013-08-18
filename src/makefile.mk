OBJECTS += $(addprefix src/,cache.o)

SUBDIRS = src/util
include $(addsuffix /makefile.mk, $(SUBDIRS))
