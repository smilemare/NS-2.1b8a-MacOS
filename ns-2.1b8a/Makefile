# Generated automatically from Makefile.in by configure.
#  Copyright (c) 1994, 1995, 1996
# 	The Regents of the University of California.  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that: (1) source code distributions
#  retain the above copyright notice and this paragraph in its entirety, (2)
#  distributions including binary code include the above copyright notice and
#  this paragraph in its entirety in the documentation or other materials
#  provided with the distribution, and (3) all advertising materials mentioning
#  features or use of this software display the following acknowledgement:
#  ``This product includes software developed by the University of California,
#  Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
#  the University nor the names of its contributors may be used to endorse
#  or promote products derived from this software without specific prior
#  written permission.
#  THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
#  WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
# @(#) $Header: /nfs/jade/vint/CVSROOT/ns-2/Makefile.in,v 1.218 2001/05/23 16:37:10 haldar Exp $ (LBL)

#
# Various configurable paths (remember to edit Makefile.in, not Makefile)
#

# Top level hierarchy
prefix	= /usr/local
# Pathname of directory to install the binary
BINDEST	= /usr/local/bin
# Pathname of directory to install the man page
MANDEST	= /usr/local/man

BLANK	= # make a blank space.  DO NOT add anything to this line

# The following will be redefined under Windows (see WIN32 lable below)
CC	= gcc
CPP	= c++
LINK	= $(CPP)
MKDEP	= ./conf/mkdep
TCLSH	= /Users/smilemare/project/ns-allinone-2.1b8a/bin/tclsh8.3
TCL2C	= ../tclcl-1.0b11/tcl2c++
AR	= ar rc $(BLANK)

RANLIB	= ranlib
INSTALL	= /usr/bin/install -c
LN	= ln
TEST	= test
RM	= rm -f
PERL	= /usr/bin/perl


CCOPT	= 
STATIC	= 
LDFLAGS	= $(STATIC)
LDOUT	= -o $(BLANK)

DEFINE	= -DTCP_DELAY_BIND_ALL -DNO_TK -DNIXVECTOR -DTCLCL_CLASSINSTVAR  -DNDEBUG -DUSE_SHM -DHAVE_LIBTCLCL -DHAVE_TCLCL_H -DHAVE_LIBOTCL1_0A7 -DHAVE_OTCL_H -DHAVE_LIBTK8_3 -DHAVE_TK_H -DHAVE_LIBTCL8_3 -DHAVE_TCL_H  -DHAVE_CONFIG_H

INCLUDES = \
	-I. -I/usr/X11R6/include \
	-I/Users/smilemare/project/ns-allinone-2.1b8a/tclcl-1.0b11 -I/Users/smilemare/project/ns-allinone-2.1b8a/otcl-1.0a7 -I/Users/smilemare/project/ns-allinone-2.1b8a/include -I/Users/smilemare/project/ns-allinone-2.1b8a/include

LIB	= \
	-L/Users/smilemare/project/ns-allinone-2.1b8a/tclcl-1.0b11 -ltclcl -L/Users/smilemare/project/ns-allinone-2.1b8a/otcl-1.0a7 -lotcl -L/Users/smilemare/project/ns-allinone-2.1b8a/lib -ltk8.3 -L/Users/smilemare/project/ns-allinone-2.1b8a/lib -ltcl8.3 \
	-L/usr/X11R6/lib  -lX11 \
	 \
	-lm 
#	-L${exec_prefix}/lib \

CFLAGS	= $(CCOPT) $(DEFINE)

# Explicitly define compilation rules since SunOS 4's make doesn't like gcc.
# Also, gcc does not remove the .o before forking 'as', which can be a
# problem if you don't own the file but can write to the directory.
.SUFFIXES: .cc	# $(.SUFFIXES)

.cc.o:
	@rm -f $@
	$(CPP) -c $(CFLAGS) $(INCLUDES) -o $@ $*.cc

.c.o:
	@rm -f $@
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $*.c


GEN_DIR	= gen/
LIB_DIR	= lib/
NS	= ns
NSX	= nsx
NSE	= nse

# To allow conf/makefile.win overwrite this macro
# We will set these two macros to empty in conf/makefile.win since VC6.0
# does not seem to support the STL in gcc 2.8 and up. 
OBJ_STL = linkstate/ls.o linkstate/rtProtoLS.o
NS_TCL_LIB_STL = tcl/rtglib/ns-rtProtoLS.tcl

# WIN32: uncomment the following line to include specific make for VC++
# !include <conf/makefile.win>

OBJ_CC = \
	baseband.o bt-classify.o bt-drr.o bt-host.o \
	bt-lc.o l2cap.o lbf.o lmp.o \
	random.o rng.o ranvar.o misc.o timer-handler.o \
	scheduler.o object.o \
	packet.o ip.o route.o connector.o ttl.o \
	trace.o trace-ip.o \
	classifier.o classifier-addr.o classifier-hash.o classifier-virtual.o \
	classifier-mcast.o classifier-bst.o classifier-mpath.o replicator.o \
	classifier-mac.o classifier-port.o src_rtg/classifier-sr.o \
        src_rtg/sragent.o src_rtg/hdr_src.o ump.o \
	app.o telnet.o tcplib-telnet.o \
	trafgen.o traffictrace.o pareto.o expoo.o cbr_traffic.o \
	tbf.o resv.o sa.o saack.o \
	measuremod.o estimator.o adc.o ms-adc.o timewindow-est.o acto-adc.o \
        pointsample-est.o salink.o actp-adc.o hb-adc.o expavg-est.o\
	param-adc.o null-estimator.o \
	adaptive-receiver.o vatrcvr.o consrcvr.o \
	agent.o message.o udp.o session-rtp.o rtp.o rtcp.o ivs.o \
	tcp.o tcp-sink.o tcp-reno.o tcp-newreno.o \
	tcp-vegas.o tcp-rbp.o tcp-full.o \
	baytcp/tcp-full-bay.o baytcp/ftpc.o baytcp/ftps.o \
	scoreboard.o tcp-sack1.o tcp-fack.o \
	tcp-asym.o tcp-asym-sink.o tcp-fs.o tcp-asym-fs.o \
	tcp-int.o chost.o tcp-session.o nilist.o \
	integrator.o queue-monitor.o flowmon.o loss-monitor.o \
	queue.o drop-tail.o simple-intserv-sched.o red.o \
	semantic-packetqueue.o semantic-red.o ack-recons.o \
	sfq.o fq.o drr.o cbq.o \
	hackloss.o errmodel.o fec.o\
	delay.o snoop.o \
	gaf/gaf.o \
	dynalink.o rtProtoDV.o net-interface.o \
	ctrMcast.o mcast_ctrl.o srm.o \
	sessionhelper.o delaymodel.o srm-ssm.o \
	srm-topo.o \
	mftp.o mftp_snd.o mftp_rcv.o codeword.o \
	alloc-address.o address.o \
	$(LIB_DIR)int.Vec.o $(LIB_DIR)int.RVec.o \
	$(LIB_DIR)dmalloc_support.o \
	realaudio/realaudio.o \
	lanRouter.o filter.o pkt-counter.o \
	Decapsulator.o Encapsulator.o encap.o \
	channel.o mac.o ll.o mac-802_11.o mac-802_3.o mac-tdma.o \
	mip.o mip-reg.o gridkeeper.o \
	propagation.o tworayground.o antenna.o omni-antenna.o \
	shadowing.o bi-connector.o node.o mobilenode.o \
	arp.o god.o dem.o topography.o modulation.o priqueue.o \
	phy.o wired-phy.o wireless-phy.o \
	mac-timers.o cmu-trace.o varp.o \
	dsdv/dsdv.o dsdv/rtable.o rtqueue.o rttable.o \
	imep/imep.o imep/dest_queue.o imep/imep_api.o \
	imep/imep_rt.o imep/rxmit_queue.o imep/imep_timers.o \
	imep/imep_util.o imep/imep_io.o \
	tora/tora.o tora/tora_api.o tora/tora_dest.o tora/tora_io.o \
	tora/tora_logs.o tora/tora_neighbor.o \
	dsr/dsragent.o dsr/hdr_sr.o dsr/mobicache.o dsr/path.o \
	dsr/requesttable.o dsr/routecache.o \
	aodv/aodv_logs.o aodv/aodv.o \
	ns-process.o \
	satgeometry.o sathandoff.o satlink.o satnode.o \
	satposition.o satroute.o sattrace.o \
	rap/raplist.o rap/rap.o rap/media-app.o rap/utilities.o \
	fsm.o tcp-abs.o \
	diffusion/diffusion.o diffusion/diff_rate.o diffusion/diff_prob.o \
	diffusion/diff_sink.o diffusion/flooding.o diffusion/omni_mcast.o \
	diffusion/hash_table.o diffusion/routing_table.o diffusion/iflist.o \
	tfrc.o tfrc-sink.o energy-model.o ping.o tcp-rfc793edu.o \
	rio.o semantic-rio.o tcp-sack-rh.o scoreboard-rh.o \
	plm/loss-monitor-plm.o plm/cbr-traffic-PP.o \
	linkstate/hdr-ls.o \
	mpls/classifier-addr-mpls.o mpls/ldp.o mpls/mpls-module.o \
	rtmodule.o classifier-hier.o addr-params.o \
         nix/hdr_nv.o nix/classifier-nix.o \
         nix/nixnode.o \
         routealgo/rnode.o \
         routealgo/bfs.o \
         routealgo/rbitmap.o \
         routealgo/rlookup.o \
         routealgo/routealgo.o \
         nix/nixvec.o \
	nix/nixroute.o \
	diffserv/dsred.o diffserv/dsEdge.o diffserv/dsCore.o diffserv/dsPolicy.o diffserv/dsredq.o \
	red-pd.o \
	pushback/rate-limit.o pushback/rate-limit-strategy.o \
	pushback/ident-tree.o pushback/agg-spec.o \
	pushback/logging-data-struct.o pushback/rate-estimator.o \
	pushback/pushback-queue.o pushback/pushback.o \
	parentnode.o \
	basetrace.o \
	simulator.o \
	


# don't allow comments to follow continuation lines

#  mac-csma.o mac-multihop.o\
#	sensor-nets/landmark.o mac-simple-wireless.o \
#	sensor-nets/tags.o sensor-nets/sensor-query.o \
#	sensor-nets/flood-agent.o \

# what was here before is now in emulate/
OBJ_C =

OBJ_COMPAT = $(OBJ_GETOPT) win32.o
#XXX compat/win32x.o compat/tkConsole.o

OBJ_EMULATE_CC = \
	emulate/net-ip.o \
	emulate/net.o \
	emulate/tap.o \
	emulate/ether.o \
	emulate/internet.o \
	emulate/ping_responder.o \
	emulate/arp.o \
	emulate/icmp.o \
	emulate/net-pcap.o \
	emulate/nat.o  \
	emulate/iptap.o \
	emulate/tcptap.o

OBJ_EMULATE_C = \
	emulate/inet.o

OBJ_GEN = $(GEN_DIR)version.o $(GEN_DIR)ns_tcl.o $(GEN_DIR)ptypes.o

SRC =	$(OBJ_C:.o=.c) $(OBJ_CC:.o=.cc) \
	$(OBJ_EMULATE_C:.o=.c) $(OBJ_EMULATE_CC:.o=.cc) \
	tclAppInit.cc tkAppInit.cc 

OBJ =	$(OBJ_C) $(OBJ_CC) $(OBJ_GEN) $(OBJ_COMPAT)

CLEANFILES = ns nse nsx ns.dyn $(OBJ) $(OBJ_EMULATE_CC) \
	$(OBJ_EMULATE_C) tclAppInit.o \
	$(GEN_DIR)* $(NS).core core core.$(NS) core.$(NSX) core.$(NSE) \
	ptypes2tcl ptypes2tcl.o 

SUBDIRS=\
	indep-utils/cmu-scen-gen/setdest \
	indep-utils/webtrace-conv/dec \
	indep-utils/webtrace-conv/epa \
	indep-utils/webtrace-conv/nlanr \
	indep-utils/webtrace-conv/ucb

all: $(NS) all-recursive

all-recursive:
	for i in $(SUBDIRS); do ( cd $$i; $(MAKE) all; ) done

$(NS): $(OBJ) tclAppInit.o Makefile
	$(LINK) $(LDFLAGS) $(LDOUT)$@ \
		tclAppInit.o $(OBJ) $(LIB)

Makefile: Makefile.in
	@echo "Makefile.in is newer than Makefile."
	@echo "You need to re-run configure."
	false

$(NSE): $(OBJ) tclAppInit.o $(OBJ_EMULATE_CC) $(OBJ_EMULATE_C)
	$(LINK) $(LDFLAGS) $(LDOUT)$@ \
		tclAppInit.o $(OBJ) \
		$(OBJ_EMULATE_CC) $(OBJ_EMULATE_C)  $(LIB) -lpcap

ns.dyn: $(OBJ) tclAppInit.o
	$(LINK) $(LDFLAGS) -o $@ \
		tclAppInit.o $(OBJ) $(LIB)

PURIFY	= purify -cache-dir=/tmp
ns-pure: $(OBJ) tclAppInit.o
	$(PURIFY) $(LINK) $(LDFLAGS) -o $@ \
		tclAppInit.o $(OBJ) $(LIB)

NS_TCL_LIB = \
	tcl/lib/ns-compat.tcl \
	tcl/lib/ns-default.tcl \
	tcl/lib/ns-errmodel.tcl \
	tcl/lib/ns-lib.tcl \
	tcl/lib/ns-link.tcl \
	tcl/lib/ns-mobilenode.tcl \
	tcl/lib/ns-btnode.tcl \
	tcl/lib/ns-sat.tcl \
	tcl/lib/ns-cmutrace.tcl \
	tcl/lib/ns-node.tcl \
	tcl/lib/ns-rtmodule.tcl \
	tcl/lib/ns-hiernode.tcl \
	tcl/lib/ns-packet.tcl \
	tcl/lib/ns-queue.tcl \
	tcl/lib/ns-source.tcl \
	tcl/lib/ns-nam.tcl \
	tcl/lib/ns-trace.tcl \
	tcl/lib/ns-agent.tcl \
	tcl/lib/ns-random.tcl \
	tcl/lib/ns-namsupp.tcl \
	tcl/lib/ns-address.tcl \
	tcl/lib/ns-intserv.tcl \
	tcl/lib/ns-autoconf.tcl \
	tcl/rtp/session-rtp.tcl \
	tcl/lib/ns-mip.tcl \
	tcl/rtglib/dynamics.tcl \
	tcl/rtglib/route-proto.tcl \
	tcl/rtglib/algo-route-proto.tcl \
	tcl/rtglib/ns-rtProtoLS.tcl \
        tcl/interface/ns-iface.tcl \
	tcl/mcast/BST.tcl \
        tcl/mcast/ns-mcast.tcl \
        tcl/mcast/McastProto.tcl \
        tcl/mcast/DM.tcl \
	tcl/mcast/srm.tcl \
	tcl/mcast/srm-adaptive.tcl \
	tcl/mcast/srm-ssm.tcl \
	tcl/mcast/timer.tcl \
	tcl/mcast/McastMonitor.tcl \
	tcl/mcast/mftp_snd.tcl \
	tcl/mcast/mftp_rcv.tcl \
	tcl/mcast/mftp_rcv_stat.tcl \
	tcl/mobility/dsdv.tcl \
	tcl/mobility/dsr.tcl \
        tcl/ctr-mcast/CtrMcast.tcl \
        tcl/ctr-mcast/CtrMcastComp.tcl \
        tcl/ctr-mcast/CtrRPComp.tcl \
	tcl/rlm/rlm.tcl \
	tcl/rlm/rlm-ns.tcl \
	tcl/session/session.tcl \
	tcl/lib/ns-route.tcl \
	tcl/emulate/ns-emulate.tcl \
	tcl/lan/vlan.tcl \
	tcl/lan/abslan.tcl \
	tcl/lan/ns-ll.tcl \
	tcl/lan/ns-mac.tcl \
	tcl/plm/plm.tcl \
	tcl/plm/plm-ns.tcl \
	tcl/plm/plm-topo.tcl \
	tcl/mpls/ns-mpls-classifier.tcl \
	tcl/mpls/ns-mpls-ldpagent.tcl \
	tcl/mpls/ns-mpls-node.tcl \
	tcl/mpls/ns-mpls-simulator.tcl \
	tcl/lib/ns-pushback.tcl \
	tcl/lib/ns-srcrt.tcl \
	

$(GEN_DIR)ns_tcl.cc: $(NS_TCL_LIB)
	$(TCLSH) bin/tcl-expand.tcl tcl/lib/ns-lib.tcl  | $(TCL2C) et_ns_lib > $@

$(GEN_DIR)version.c: VERSION
	$(RM) $@
	$(TCLSH) bin/string2c.tcl version_string < VERSION > $@

$(GEN_DIR)ptypes.cc: ptypes2tcl packet.h
	./ptypes2tcl > $@

ptypes2tcl: ptypes2tcl.o
	$(LINK) $(LDFLAGS) $(LDOUT)$@ ptypes2tcl.o

ptypes2tcl.o: ptypes2tcl.cc packet.h

install: force install-ns install-man install-recursive

install-ns: force
	$(INSTALL) -m 555 -o bin -g bin ns $(DESTDIR)$(BINDEST)

install-man: force
	$(INSTALL) -m 444 -o bin -g bin ns.1 $(DESTDIR)$(MANDEST)/man1

install-recursive: force
	for i in $(SUBDIRS); do ( cd $$i; $(MAKE) install; ) done

clean:
	$(RM) $(CLEANFILES)

AUTOCONF_GEN = tcl/lib/ns-autoconf.tcl
distclean:
	$(RM) $(CLEANFILES) Makefile config.cache config.log config.status \
	    gnuc.h os-proto.h $(AUTOCONF_GEN)

depend: $(SRC)
	$(MKDEP) $(CFLAGS) $(INCLUDES) $(SRC)

srctar:
	@cwd=`pwd` ; dir=`basename $$cwd` ; \
	    name=ns-`cat VERSION | tr A-Z a-z` ; \
	    tar=ns-src-`cat VERSION`.tar.gz ; \
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

test:	force
	./validate

# Create makefile.vc for Win32 development by replacing:
# "# !include ..." 	-> 	"!include ..."
makefile.vc:	Makefile.in
	$(PERL) bin/gen-vcmake.pl < Makefile.in > makefile.vc
#	$(PERL) -pe 's/^# (\!include)/\!include/o' < Makefile.in > makefile.vc
