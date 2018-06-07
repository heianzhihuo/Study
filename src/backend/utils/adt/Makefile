#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for utils/adt
#
# IDENTIFICATION
#    $Header: /usr/local/cvsroot/postgres95/src/backend/utils/adt/Makefile,v 1.3 1996/11/06 10:30:35 scrappy Exp $
#
#-------------------------------------------------------------------------

SRCDIR = ../../..
include ../../../Makefile.global

INCLUDE_OPT = -I../.. \
              -I../../port/$(PORTNAME) \
              -I../../../include

CFLAGS+=$(INCLUDE_OPT)

OBJS = name.o

all: SUBSYS.o

SUBSYS.o: $(OBJS)
	$(LD) -r -o SUBSYS.o $(OBJS)

depend dep:
	$(CC) -MM $(INCLUDE_OPT) *.c >depend

clean: 
	rm -f SUBSYS.o $(OBJS)

ifeq (depend,$(wildcard depend))
include depend
endif

