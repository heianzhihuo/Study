#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for utils/init
#
# IDENTIFICATION
#    $Header: /usr/local/cvsroot/postgres95/src/backend/utils/init/Makefile,v 1.4 1996/11/14 10:24:32 bryanh Exp $
#
#-------------------------------------------------------------------------

SRCDIR = ../../..
include ../../../Makefile.global

INCLUDE_OPT = -I../.. \
              -I../../port/$(PORTNAME) \
              -I../../../include

CFLAGS += $(INCLUDE_OPT)

OBJS = globals.o miscinit.o enbl.o

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

