# Generated automatically from Makefile.in by configure.
#
# Copyright (c) 1991,1993 The Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this software
#    must display the following acknowledgement:
#	This product includes software developed by the Computer Systems
#	Engineering Group at Lawrence Berkeley Laboratory.
# 4. Neither the name of the University nor of the Laboratory may be used
#    to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# @(#) $Header: /nfs/jade/vint/CVSROOT/nam-1/Makefile.in,v 1.47 2001/04/27 01:01:58 mehringe Exp $
#

# Top level hierarchy
prefix = /usr/local
exec_prefix = ${prefix}
# Pathname of directory to install the binary
BINDEST = ${exec_prefix}/bin
# Pathname of directory to install the man page
MANDEST = ${prefix}/man

CC = gcc
CPP = c++
CCOPT = 
MKDEP = ./conf/mkdep
# Have to be the same as that defined in conf/makefile.win
TCL2C = ../tclcl-1.0b11/tcl2c++
TCLSH = /Users/smilemare/project/ns-allinone-2.1b8a/bin/tclsh8.3

#
# Remember to add a dependency if you add any tcl sources here.
#
LIB =  \
	-L/Users/smilemare/project/ns-allinone-2.1b8a/tclcl-1.0b11 -ltclcl -L/Users/smilemare/project/ns-allinone-2.1b8a/otcl-1.0a7 -lotcl -L/Users/smilemare/project/ns-allinone-2.1b8a/lib -ltk8.3 -L/Users/smilemare/project/ns-allinone-2.1b8a/lib -ltcl8.3 -L/Users/smilemare/project/ns-allinone-2.1b8a/zlib-1.1.3 -lz \
	-L/usr/X11R6/lib  -lX11 \
	 -lm 
INCLUDE = \
	-I. -I/Users/smilemare/project/ns-allinone-2.1b8a/tclcl-1.0b11 -I/Users/smilemare/project/ns-allinone-2.1b8a/otcl-1.0a7 -I/Users/smilemare/project/ns-allinone-2.1b8a/include -I/Users/smilemare/project/ns-allinone-2.1b8a/include -I/Users/smilemare/project/ns-allinone-2.1b8a/zlib-1.1.3 \
	-I/usr/X11R6/include 

STATIC = 
DEFINE = -DTCL_TK  -DNDEBUG -DUSE_SHM -DHAVE_LIBTCLCL -DHAVE_TCLCL_H -DHAVE_LIBOTCL1_0A7 -DHAVE_OTCL_H -DHAVE_LIBTK8_3 -DHAVE_TK_H -DHAVE_LIBTCL8_3 -DHAVE_TCL_H -DHAVE_LIBZ1_1_3 -DHAVE_ZLIB_H 
CFLAGS = $(CCOPT) $(DEFINE) $(INCLUDE)

INSTALL = /usr/bin/install -c
RANLIB = ranlib

BLANK	= # make a blank space.  DO NOT add anything to this line
AR	= ar rc $(BLANK)
LINK	= $(CPP)
LDFLAGS	= -framework CoreFoundation
LDOUT	= -o $(BLANK)
PERL	= perl
RM	= rm -f

# Explicitly define compilation rules since SunOS 4's make doesn't like gcc.
# Also, gcc does not remove the .o before forking 'as', which can be a
# problem if you don't own the file but can write to the directory.
.SUFFIXES: .cc $(.SUFFIXES)

.cc.o:
	rm -f $@; $(CPP) -o $@ -c $(CFLAGS) $*.cc

.c.o:
	rm -f $@; $(CC) -o $@ -c $(CFLAGS) $*.c

GEN_DIR = gen/
NAM	= nam

# WIN32: uncomment the following line to include specific make for VC++
# !include <conf/makefile.win>

OBJ_C = tkcompat.o tkUnixInit.o xwd.o

OBJ_CC = \
	netview.o netmodel.o edge.o packet.o node.o main.o \
	trace.o queue.o drop.o animation.o agent.o feature.o \
	route.o transform.o paint.o state.o monitor.o anetmodel.o \
	random.o rng.o view.o graphview.o netgraph.o tracehook.o\
	lan.o psview.o group.o editview.o tag.o address.o animator.o \
	wnetmodel.o nam_stream.o enetmodel.o testview.o parser.o \
	trafficsource.o

OBJ_GEN = \
	$(GEN_DIR)version.o $(GEN_DIR)nam_tcl.o
SRC_GEN = \
	$(GEN_DIR)version.c $(GEN_DIR)nam_tcl.cc

OBJ =	$(OBJ_C) $(OBJ_CC) $(OBJ_GEN)
SRC =	$(OBJ_C:.o=.c) $(OBJ_CC:.o=.cc)

CLEANFILES = $(NAM) $(OBJ) $(SRC_GEN) core core.nam

NAM_TCL_LIB = \
	tcl/nam-lib.tcl \
	tcl/nam-default.tcl \
	tcl/balloon.tcl \
	tcl/snapshot.tcl \
	tcl/animator.tcl \
	tcl/anim-ctrl.tcl \
	tcl/netModel.tcl \
	tcl/autoNetModel.tcl \
	tcl/build-ui.tcl \
	tcl/annotation.tcl \
	tcl/node.tcl \
	tcl/monitor.tcl \
	tcl/stats.tcl \
	tcl/www.tcl \
	tcl/menu_file.tcl \
	tcl/menu_view.tcl \
  tcl/NamgraphView.tcl \
  tcl/NamgraphModel.tcl \
  tcl/TimesliderNamgraphView.tcl \
  tcl/TimesliderView.tcl \
  tcl/TimesliderModel.tcl \
  tcl/observer.tcl \
  tcl/observable.tcl \
  tcl/wirelessNetModel.tcl \
  tcl/editorNetModel.tcl \
  tcl/EditorMenu.tcl \
  tcl/Editor.tcl \
  tcl/Editor-FileParser.tcl

$(NAM):	$(OBJ) $(SCRYOBJ) Makefile
	$(RM) $@
	$(LINK) $(STATIC) $(LDFLAGS) $(LDOUT)$@ \
		$(OBJ) $(SCRYOBJ) $(LIB)

Makefile: Makefile.in
	@echo "Makefile.in is newer than Makefile."
	@echo "You need to re-run configure."
	false

$(GEN_DIR)nam_tcl.cc: $(NAM_TCL_LIB)
	$(RM) $@
	$(TCLSH) bin/tcl-expand.tcl tcl/nam-lib.tcl | $(TCL2C) et_nam > $@

$(GEN_DIR)version.c: VERSION
	$(RM) $@
	$(TCLSH) bin/string2c.tcl version < VERSION > $@

install: force
	$(INSTALL) -m 555 -o bin -g bin nam $(DESTDIR)$(BINDEST)

clean:
	$(RM) $(CLEANFILES)

distclean:
	$(RM) $(CLEANFILES) Makefile config.cache config.log config.status

tar:	force
	tar=$(TAR_PREFIX)-nam-`cat VERSION`.tar.gz ; \
	rm -f $$tar ; \
	tar cfhF -  $(TAR) | gzip -c > $$tar

depend: $(SRC)
	$(MKDEP) $(CFLAGS) $(SRC)

srctar:
	@cwd=`pwd` ; dir=`basename $$cwd` ; \
	    name=nam-`cat VERSION | tr A-Z a-z` ; \
	    tar=nam-src-`cat VERSION`.tar.gz ; \
	    list="" ; \
	    for i in `cat FILES` ; do list="$$list $$name/$$i" ; done; \
	    echo \
	    "(rm -f $$tar; cd .. ; ln -s $$dir $$name)" ; \
	     (rm -f $$tar; cd .. ; ln -s $$dir $$name) ; \
	    echo \
	    "(cd .. ; tar cfh $$tar [lots of files])" ; \
	     (cd .. ; tar cfh - $$list) | gzip -c > $$tar ; \
	    echo \
	    "rm ../$$name; chmod 444 $$tar" ;  \
	     rm ../$$name; chmod 444 $$tar

force:

# Create makefile.vc for Win32 development by replacing:
# "# !include ..." 	-> 	"!include ..."
makefile.vc: Makefile.in
	$(PERL) bin/gen-vcmake.pl < Makefile.in > makefile.vc
#	$(PERL) -pe 's/^# (\!include)/\!include/o' < Makefile.in > makefile.vc
