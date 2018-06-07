#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for the access methods module
#
# IDENTIFICATION
#    $Header: /usr/local/cvsroot/postgres95/src/backend/access/Makefile,v 1.2 1996/11/10 03:11:46 bryanh Exp $
#
#-------------------------------------------------------------------------

OBJS = transam/SUBSYS.o common/SUBSYS.o 


all: submake SUBSYS.o

SUBSYS.o: $(OBJS)
	$(LD) -r -o SUBSYS.o $(OBJS)

.PHONY: submake
submake:
	$(MAKE) -C transam SUBSYS.o
	$(MAKE) -C common SUBSYS.o

clean:
	rm -f SUBSYS.o
	$(MAKE) -C transam  clean
	$(MAKE) -C common clean

.DEFAULT: 
	$(MAKE) -C transam  $@
	$(MAKE) -C common $@

