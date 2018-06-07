#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for access/common
#
# IDENTIFICATION
#    $Header: /usr/local/cvsroot/postgres95/src/backend/access/common/Makefile,v 1.6 1996/11/05 07:42:39 scrappy Exp $
#
#-------------------------------------------------------------------------

SRCDIR = ../../..
include ../../../Makefile.global

INCLUDE_OPT = -I../.. \
              -I../../port/$(PORTNAME) \
              -I../../../include

CFLAGS+=$(INCLUDE_OPT) 

OBJS = tupdesc.o  

all: SUBSYS.o

SUBSYS.o: $(OBJS)
	$(LD) -r -o SUBSYS.o $(OBJS)



dep: ../../fmgr.h
	$(CC) -MM $(INCLUDE_OPT) *.c >depend

clean: 
	rm -f SUBSYS.o $(OBJS)

ifeq (depend,$(wildcard depend))
include depend
endif

