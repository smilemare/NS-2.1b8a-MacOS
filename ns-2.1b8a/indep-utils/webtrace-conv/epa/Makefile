# Generated automatically from Makefile.in by configure.
# 
#  Copyright (c) 1999 by the University of Southern California
#  All rights reserved.
# 
#  Permission to use, copy, modify, and distribute this software and its
#  documentation in source and binary forms for non-commercial purposes
#  and without fee is hereby granted, provided that the above copyright
#  notice appear in all copies and that both the copyright notice and
#  this permission notice appear in supporting documentation. and that
#  any documentation, advertising materials, and other materials related
#  to such distribution and use acknowledge that the software was
#  developed by the University of Southern California, Information
#  Sciences Institute.  The name of the University may not be used to
#  endorse or promote products derived from this software without
#  specific prior written permission.
# 
#  THE UNIVERSITY OF SOUTHERN CALIFORNIA makes no representations about
#  the suitability of this software for any purpose.  THIS SOFTWARE IS
#  PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
#  INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
# 
#  Other copyrights might apply to parts of this software and are so
#  noted when applicable.
# 
# $Header: /nfs/jade/vint/CVSROOT/ns-2/indep-utils/webtrace-conv/epa/Makefile.in,v 1.3 2000/02/16 18:08:26 haoboy Exp $

# Top level hierarchy
prefix  = /usr/local
# Pathname of directory to install the binary
BINDEST = /usr/local/bin

CC = c++
MKDEP	= ../../../conf/mkdep

INCLUDE = -I. -I/Users/smilemare/project/ns-allinone-2.1b8a/tclcl-1.0b11 -I/Users/smilemare/project/ns-allinone-2.1b8a/otcl-1.0a7 -I/Users/smilemare/project/ns-allinone-2.1b8a/include -I/Users/smilemare/project/ns-allinone-2.1b8a/include
CFLAGS = 
LDFLAGS = -framework CoreFoundation
LIBS = -L/Users/smilemare/project/ns-allinone-2.1b8a/lib -ltcl8.3  
INSTALL = /usr/bin/install -c

SRC = tr-stat.cc
OBJ = $(SRC:.cc=.o)

all: epa-tr-stat

epa-tr-stat: $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) $(INCLUDE) $(OBJ) $(LIBS)

install: epa-tr-stat
	$(INSTALL) -m 555 -o bin -g bin epa-tr-stat $(DESTDIR)$(BINDEST)

.SUFFIXES: .cc

.cc.o: 
	@rm -f $@
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $*.cc

clean: 
	@rm -f *~ *.o epa-tr-stat *core

depend: $(SRC)
	$(MKDEP) $(CFLAGS) $(INCLUDE) $(SRC)
