#sources
sources=src/simpleDataLink.c \
lib/bufferUtils/src/bufferUtils.c \
lib/frameUtils/src/frameUtils.c

vpath %.c $(dir $(sources))
#objects
objects=$(addprefix $(builddir)/,$(notdir $(sources:.c=.o)))
#include paths
includes=-I inc/ \
-I lib/bufferUtils/inc/ \
-I lib/frameUtils/inc/
#output directory
builddir=build
#compiler flags
compflags=-Wall

all: $(objects)

$(builddir)/%.o: %.c | $(builddir)
	echo $(objects)
	$(CC) $(compflags) -o $@ -c $< $(includes)

example: examples/communicationExample.c $(objects) | $(builddir)
	$(CC) $(compflags) -o $(builddir)/communicationExample.o -c $< $(includes)
	$(CC) -o $(builddir)/communicationExample $(builddir)/communicationExample.o $(objects) $(includes)

$(builddir):
	mkdir $@

.PHONY: clean
clean:
	rm -r $(builddir)