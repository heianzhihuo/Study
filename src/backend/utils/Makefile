#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for utils
#
# IDENTIFICATION
#    $Header: /usr/local/cvsroot/postgres95/src/backend/utils/Makefile,v 1.4 1996/11/09 06:22:52 momjian Exp $
#
#-------------------------------------------------------------------------

SRCDIR = ../..
include ../../Makefile.global

INCLUDE_OPT = -I.. \
              -I../../include

CFLAGS+=$(INCLUDE_OPT)

all: submake SUBSYS.o

OBJS = error/SUBSYS.o hash/SUBSYS.o mmgr/SUBSYS.o init/SUBSYS.o cache/SUBSYS.o adt/SUBSYS.o

SUBSYS.o: $(OBJS) 
	$(LD) -r -o SUBSYS.o $(OBJS)

.PHONY: submake clean dep
submake:
	$(MAKE) -C error SUBSYS.o
	$(MAKE) -C hash SUBSYS.o
	$(MAKE) -C mmgr SUBSYS.o
	$(MAKE) -C init SUBSYS.o
	$(MAKE) -C cache SUBSYS.o
	$(MAKE) -C adt SUBSYS.o


clean:
	rm -f SUBSYS.o
	$(MAKE) -C error clean 
	$(MAKE) -C hash  clean 
	$(MAKE) -C mmgr  clean
	$(MAKE) -C init  clean 
	$(MAKE) -C cache clean
	$(MAKE) -C adt clean
 

dep: fmgr.h fmgrtab.c
	$(CC) -MM $(INCLUDE_OPT) *.c >depend
	$(MAKE) -C error dep 
	$(MAKE) -C hash  dep 
	$(MAKE) -C mmgr  dep
	$(MAKE) -C init  dep 
	$(MAKE) -C cache  dep 
	$(MAKE) -C adt dep

ifeq (depend,$(wildcard depend))
include depend
endif

