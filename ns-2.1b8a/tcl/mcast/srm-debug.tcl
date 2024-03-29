# 
#  Copyright (c) 1997 by the University of Southern California
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
#	Author:		Kannan Varadhan	<kannan@isi.edu>
#	Version Date:	Mon Jun 30 15:51:33 PDT 1997
#
# @(#) $Header: /nfs/jade/vint/CVSROOT/ns-2/tcl/mcast/srm-debug.tcl,v 1.3 1997/10/23 20:53:38 kannan Exp $ (USC/ISI)
#

SRM/request instproc compute-delay {} {
	$self instvar C1_ C2_ agent_ sender_ backoff_
	set unif [uniform 0 1]
	set rancomp [expr $C1_ + $C2_ * $unif]
	set dist [$agent_ distance? $sender_]
	set delay [expr $rancomp * $dist]
	set absdelay [expr $delay * $backoff_]
	
	$self evTrace Q INTERVALS C1 $C1_ C2 $C2_ d $dist i $backoff_	\
			U $unif |D| $absdelay
	set delay
}

SRM/repair instproc compute-delay {} {
	$self instvar D1_ D2_ agent_ requestor_
	set unif [uniform 0 1]
	set rancomp [expr $D1_ + $D2_ * $unif]
	set dist [$agent_ distance? $requestor_]
	set delay [expr $rancomp * $dist]

	$self evTrace P INTERVALS D1 $D1_ D2 $D2_ d $dist U $unif |D| $delay
	set delay
}
