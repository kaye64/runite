CFLAGS = -g -std=gnu99 -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
INCLUDE_DIRS = -Iinclude/
OUT = librunite.a

SUBDIRS = src
OBJECTS :=

include $(addsuffix /makefile.mk, $(SUBDIRS))

all: $(OUT)

$(OUT): $(OBJECTS)
	ar rcs $@ $^

%.o: %.c
	gcc -c $(CFLAGS) $(INCLUDE_DIRS) -o $@ $^

clean:
	-rm -f $(OUT) $(OBJECTS)

.PHONY: all clean
