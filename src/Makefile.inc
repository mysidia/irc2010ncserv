IFLAGS=-l$(INCDIR)
LSUFFIX=obj
SUBDIRS=socket dns conf port misc msgs structs
OBJS=main.o proc.o
#LIBS=./socket.so ./resolver.so ./conf.so ./port.so ./misc.so
LIBS=./socket.$(LSUFFIX) ./resolver.$(LSUFFIX) ./conf.$(LSUFFIX) ./port.$(LSUFFIX) ./misc.$(LSUFFIX) ./structs.$(LSUFFIX)
CFLAGS=-ggdb -I$(INCDIR) -Wall -Wcast-align -Wmissing-declarations -Wpointer-arith -Werror
#CFLAGS=-ggdb -I$(INCDIR) -Wall -Wpointer-arith -Wcast-align -Wmissing-declarations -Woverloaded-virtual -Werror

INCLUDES=$(INCDIR)/defs.h $(INCDIR)/h.h $(INCDIR)/pircd.h $(SRCDIR)/src/port/port.h $(INCDIR)/hash.h
BIN=pIrcd
CC=gcc
