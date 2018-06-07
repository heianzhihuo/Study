#-------------------------------------------------------------------------
#
# Makefile--
#   Makefile for the postgres backend (and the postmaster)
#
# Copyright (c) 1994, Regents of the University of California
#
# Functional notes:
#
#   Parallel make:  
#
#     This make file is set up so that you can do a parallel make (with 
#     the --jobs option of make) and make multiple subdirectories at 
#     once.  
#
#     However, the subdirectory make files are not so careful.
#     Normally, the --jobs option would get passed down to those
#     subdirectory makes, like any other make option, and they would
#     fail.  But there's a trick: Put a value (max number of
#     processes) on the --jobs option, e.g. --jobs=4.  Now, due to a
#     special feature of make, the --jobs option will not get passed
#     to the subdirectory makes.  (make does this because if you only
#     want 4 tasks running, then splitting the subdirectory makes into
#     multiple tasks would violate your wishes).
#
#
#
# Implementation notes:
#
#   We don't use $(LD) for linking.  We use $(CC) instead.  This is because
#   the $(CC) program apparently can do linking too, and it has certain
#   thinks like default options and search paths for libraries set up for 
#   it that the more primitive $(LD) doesn't have.
#
#
# IDENTIFICATION
#    $Header: /usr/local/cvsroot/postgres95/src/backend/Makefile,v 1.18 1997/01/05 21:17:21 bryanh Exp $
#
#-------------------------------------------------------------------------

SRCDIR = ..
include ../Makefile.global

#
#
OBJS = main/SUBSYS.o storage/SUBSYS.o access/SUBSYS.o utils/SUBSYS.o \
	   catalog/SUBSYS.o tcop/SUBSYS.o lib/SUBSYS.o nodes/SUBSYS.o

#catalog/SUBSYS.o tcop/SUBSYS.o

ifeq ($(MAKE_EXPORTS), true)
EXP = postgres$(EXPSUFF)
else
EXP =
endif

all: postgres

postgres: postgres_group2
	$(CC) $(LDFLAGS) -o postgres $(OBJS) $(LD_ADD)

postgres_group2: 
	$(MAKE) -C main       all 
	$(MAKE) -C storage    all   
	$(MAKE) -C access    all  
	$(MAKE) -C utils    all
	$(MAKE) -C catalog    all
	$(MAKE) -C tcop    all
	$(MAKE) -C lib    all
	$(MAKE) -C nodes    all
	       

# The postgres.o target is needed by the rule in Makefile.global that
# creates the exports file when MAKE_EXPORTS = true.
postgres.o: $(OBJS)
	$(CC) $(LDFLAGS) -r -o postgres.o $(OBJS) $(LD_ADD)


############################################################################
# The following targets are specified in make commands that appear in the
# make files in our subdirectories.


#############################################################################
clean:
	rm -f postgres fmgr.h parse.h \
	    global1.bki.source local1_template1.bki.source
	
	$(MAKE) -C main       clean 
	$(MAKE) -C storage    clean 
	$(MAKE) -C access    clean 
	$(MAKE) -C utils    clean 
	$(MAKE) -C catalog    clean
	$(MAKE) -C tcop   clean 
	$(MAKE) -C lib    clean
	$(MAKE) -C nodes    clean

.DEFAULT:
	$(MAKE) -C main       $@ 
	$(MAKE) -C storage    $@ 
	$(MAKE) -C access    $@ 
	$(MAKE) -C utils    $@ 
	$(MAKE) -C catalog    $@ 
	$(MAKE) -C tcop    $@ 
	$(MAKE) -C lib    $@
	$(MAKE) -C nodes    $@




#############################################################################
#
# Installation.
#
# Install the backend program (postgres) to the binary directory and 
# make a link as "postmaster".  Install the bki files templates and sample 
# files to the library directory.
#
# (History:  Before Release 2, make install generated a bki.source file
# and then used build parameters to convert it to a bki file, then installed
# that bki file in the /files subdirectory of the default data directory.
# Initdb then used the bki file to generate the database catalog classes.
# That had to change because (1) there can be more than one database system,
# and (2) the parameters of a database system should be set at initdb time,
# not at postgres build time.

install: $(LIBDIR) $(BINDIR) $(HEADERDIR) postgres $(EXP) fmgr.h\
         global1.bki.source local1_template1.bki.source \
         libpq/pg_hba.conf.sample 
	
	$(INSTALL) $(INSTL_EXE_OPTS) postgres $(BINDIR)/postgres
ifeq ($(MAKE_EXPORTS), true)
	$(INSTALL) $(INSTLOPTS) postgres$(EXPSUFF) $(LIBDIR)/postgres$(EXPSUFF)
endif
	@rm -f $(BINDIR)/postmaster
	cd $(BINDIR); ln -s postgres postmaster
	$(INSTALL) $(INSTLOPTS) fmgr.h $(HEADERDIR)/fmgr.h
	$(INSTALL) $(INSTLOPTS) global1.bki.source \
	  $(LIBDIR)/global1.bki.source
	$(INSTALL) $(INSTLOPTS) local1_template1.bki.source \
	  $(LIBDIR)/local1_template1.bki.source
	$(INSTALL) $(INSTLOPTS) libpq/pg_hba.conf.sample \
	  $(LIBDIR)/pg_hba.conf.sample

$(BINDIR) $(LIBDIR) $(HEADERDIR): 
	mkdir $@

#############################################################################
#
# Support for code development.
#
# Use target "quick" to build "postgres" when you know all the subsystems 
# are up to date.  It saves the time of doing all the submakes.
.PHONY: quick
quick: $(OBJS)
	$(CC) $(LDFLAGS) -o postgres $(OBJS) $(LD_ADD)

#
# Build the file, "./ID", used by the "gid" (grep-for-identifier) tool
#
IDFILE=	ID
.PHONY: $(IDFILE)
$(IDFILE):
	./makeID $(PORTNAME)

#
# Special rule to generate cpp'd version of a .c file.  This is
# especially useful given all the hellish macro processing going on.
# The cpp'd version has a .C suffix.  To create foo.C from foo.c, just
# type
#	make foo.C
#
%.cpp:	%.c
	$(CC) -E $(CFLAGS) $(<:.C=.c) | cat -s | cb | tr -s '\012*' '\012' \
	    > $(@F)
