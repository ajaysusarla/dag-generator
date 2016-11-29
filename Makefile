CC = gcc

# Platform
UNAME := $(shell $(CC) -dumpmachine 2>&1 | grep -E -o "linux|darwin")

ifeq ($(UNAME), linux)
OSFLAGS = -DLINUX
DEBUG = -ggdb
else ifeq ($(UNAME), darwin)
OSFLAGS = -DMACOSX
DEBUG = -g
else ifeq ($(UNAME), solaris)
OSFLAGS = -DSOLARIS
DEBUG = -g
endif

EXTRA_CFLAGS = -Wall -Wextra -pedantic
LIBS =
CFLAGS = $(DEBUG) $(EXTRA_CFLAGS) $(OSFLAGS)

all: dag-gen-server dag-discover


dag-gen-server: dag-gen-server.c graph.o utils.o list.o
	$(CC) $(CFLAGS) -c dag-gen-server.c
	$(CC) $(CFLAGS) $(LIBS) \
		graph.o \
		dag-gen-server.o \
		list.o \
		utils.o \
		-o dag-gen-server

dag-discover: dag-discover.c graph.o utils.o
	$(CC) $(CFLAGS) -c dag-discover.c
	$(CC) $(CFLAGS) $(LIBS) graph.o dag-discover.o utils.o -o dag-discover

graph.o: graph.c graph.h
	$(CC) $(CFLAGS) -c graph.c

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -rf *.o dag-gen-server dag-discover *.dot

.PHONY: check-syntax

check-syntax:
	$(CC) $(CFLAGS) -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)
