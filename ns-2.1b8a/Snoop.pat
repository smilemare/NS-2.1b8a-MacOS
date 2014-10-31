diff -rNc ns-allinone-2.1b7a/ns-2.1b7a/snoop.cc ns-allinone-2.1b7aNNew/ns-2.1b7a/snoop.cc
*** ns-allinone-2.1b7a/ns-2.1b7a/snoop.cc	Wed Sep  8 23:22:47 1999
--- ns-allinone-2.1b7aNNew/ns-2.1b7a/snoop.cc	Thu Apr  5 14:56:03 2001
***************
*** 165,182 ****
  {
  	Tcl &tcl = Tcl::instance();
  	hdr_ip *iph = hdr_ip::access(p);
! 	
  	/* get-snoop creates a snoop object if none currently exists */
! 	if (h == 0)
  		/* In ns, addresses have ports embedded in them. */
  		tcl.evalf("%s get-snoop %d %d", name(), iph->daddr(),
  			  iph->saddr()); 
! 	else
  		tcl.evalf("%s get-snoop %d %d", name(), iph->saddr(),
  			  iph->daddr());
  	
  	Snoop *snoop = (Snoop *) TclObject::lookup(tcl.result());
  	snoop->recv(p, h);
  	if (integrate_)
  		tcl.evalf("%s integrate %d %d", name(), iph->saddr(),
  			  iph->daddr());
--- 165,187 ----
  {
  	Tcl &tcl = Tcl::instance();
  	hdr_ip *iph = hdr_ip::access(p);
! 
  	/* get-snoop creates a snoop object if none currently exists */
! 	hdr_cmn *ch = HDR_CMN(p);
! 	if(ch->direction() == hdr_cmn::UP) 
! 		/* get-snoop creates a snoop object if none currently exists */
  		/* In ns, addresses have ports embedded in them. */
  		tcl.evalf("%s get-snoop %d %d", name(), iph->daddr(),
  			  iph->saddr()); 
!        
! 	else  
  		tcl.evalf("%s get-snoop %d %d", name(), iph->saddr(),
  			  iph->daddr());
  	
  	Snoop *snoop = (Snoop *) TclObject::lookup(tcl.result());
+ 	
  	snoop->recv(p, h);
+ 	
  	if (integrate_)
  		tcl.evalf("%s integrate %d %d", name(), iph->saddr(),
  			  iph->daddr());
***************
*** 190,207 ****
   * Call snoop_data() if TCP packet and forward it on if it's an ack.
   */
  void
! Snoop::recv(Packet* p, Handler* h)
! {
! 	if (h == 0) {		// from MAC classifier
  		handle((Event *) p);
  		return;
  	}
  	packet_t type = hdr_cmn::access(p)->ptype();
  	/* Put packet (if not ack) in cache after checking, and send it on */
! 	if (type == PT_TCP)
  		snoop_data(p);
  	else if (type == PT_ACK)
  		snoop_wired_ack(p);
  	parent_->sendDown(p);	/* vector to LLSnoop's sendto() */
  }
  
--- 195,222 ----
   * Call snoop_data() if TCP packet and forward it on if it's an ack.
   */
  void
! Snoop::recv(Packet* p, Handler* h )
! {	
! 	
! 	hdr_cmn *ch = HDR_CMN(p);	
! 	if(ch->direction() == hdr_cmn::UP) {
  		handle((Event *) p);
  		return;
  	}
+ 	
  	packet_t type = hdr_cmn::access(p)->ptype();
  	/* Put packet (if not ack) in cache after checking, and send it on */
! 	
! 	if (type == PT_TCP) 
  		snoop_data(p);
+ 	
  	else if (type == PT_ACK)
  		snoop_wired_ack(p);
+ 	
+ 	ch->direction() = hdr_cmn::DOWN;  // Ben added
+ 
+ 	int seq = hdr_tcp::access(p)->seqno();
+ 
  	parent_->sendDown(p);	/* vector to LLSnoop's sendto() */
  }
  
***************
*** 212,217 ****
--- 227,233 ----
  void
  Snoop::handle(Event *e)
  {
+ 
  	Packet *p = (Packet *) e;
  	packet_t type = hdr_cmn::access(p)->ptype();
  	//int seq = hdr_tcp::access(p)->seqno();
***************
*** 224,238 ****
  		return;
  	}
  
! 	if (type == PT_ACK)
! 		prop = snoop_ack(p);
  	else if (type == PT_TCP) /* XXX what about TELNET? */
  		snoop_wless_data(p);
  
  	if (prop == SNOOP_PROPAGATE)
  		s.schedule(recvtarget_, e, parent_->delay());
  	else {			// suppress ack
! /*		printf("---- %f suppressing ack %d\n", s.clock(), seq);*/
  		Packet::free(p);
  	}
  }
--- 240,257 ----
  		return;
  	}
  
! 	if (type == PT_ACK) 
! 		prop = snoop_ack(p); 
! 
  	else if (type == PT_TCP) /* XXX what about TELNET? */
  		snoop_wless_data(p);
  
  	if (prop == SNOOP_PROPAGATE)
  		s.schedule(recvtarget_, e, parent_->delay());
  	else {			// suppress ack
! 		/*		printf("---- %f suppressing ack %d\n", s.clock(), seq);*/
! 		
! 		int seq = hdr_tcp::access(p)->seqno();
  		Packet::free(p);
  	}
  }
***************
*** 247,255 ****
  	Scheduler &s = Scheduler::instance();
  	int seq = hdr_tcp::access(p)->seqno();
  	int resetPending = 0;
- 
- //	printf("%x snoop_data: %f sending packet %d\n", this, s.clock(), seq);
  	
  	if (fstate_ & SNOOP_ALIVE && seq == 0)
  		reset();
  	fstate_ |= SNOOP_ALIVE;
--- 266,273 ----
  	Scheduler &s = Scheduler::instance();
  	int seq = hdr_tcp::access(p)->seqno();
  	int resetPending = 0;
  	
+ 	//	printf("%x snoop_data: %f sending packet %d\n", this, s.clock(), seq);
  	if (fstate_ & SNOOP_ALIVE && seq == 0)
  		reset();
  	fstate_ |= SNOOP_ALIVE;
***************
*** 263,268 ****
--- 281,287 ----
  	 * Only if the ifq is NOT full do we insert, since otherwise we want
  	 * congestion control to kick in.
  	 */
+ 
  	if (parent_->ifq()->length() < parent_->ifq()->limit()-1)
  		resetPending = snoop_insert(p);
  	if (toutPending_ && resetPending == SNOOP_TAIL) {
***************
*** 288,296 ****
  int
  Snoop::snoop_insert(Packet *p)
  {
  	int i, seq = hdr_tcp::access(p)->seqno(), retval=0;
  
! 	if (seq <= lastAck_)
  		return retval;
  	
  	if (fstate_ & SNOOP_FULL) {
--- 307,318 ----
  int
  Snoop::snoop_insert(Packet *p)
  {
+ 
+ 
+ 
  	int i, seq = hdr_tcp::access(p)->seqno(), retval=0;
  
! 	if (seq <= lastAck_) 
  		return retval;
  	
  	if (fstate_ & SNOOP_FULL) {
***************
*** 311,335 ****
  	} else {
  		for (i = buftail_; i != bufhead_; i = next(i)) {
  			hdr_snoop *sh = hdr_snoop::access(pkts_[i]);
! 			if (sh->seqno() == seq) {
  				sh->numRxmit() = 0;
! 				sh->senderRxmit() = 1;
  				sh->sndTime() = Scheduler::instance().clock();
  				return SNOOP_TAIL;
! 			} else if (sh->seqno() > seq) {
  				Packet *temp = pkts_[prev(buftail_)];
  				for (int j = buftail_; j != i; j = next(j)) 
  					pkts_[prev(j)] = pkts_[j];
  				i = prev(i);
! 				pkts_[i] = temp;
  				buftail_ = prev(buftail_);
  				break;
  			}
  		}
  		if (i == bufhead_)
  			bufhead_ = next(bufhead_);
  	}
  	savepkt_(p, seq, i);
  	if (bufhead_ == buftail_)
  		fstate_ |= SNOOP_FULL;
  	/* 
--- 333,368 ----
  	} else {
  		for (i = buftail_; i != bufhead_; i = next(i)) {
  			hdr_snoop *sh = hdr_snoop::access(pkts_[i]);
! 			if (sh->seqno() == seq) {  // cached before
! 
  				sh->numRxmit() = 0;
! 				sh->senderRxmit() = 1; //must be a sender retr
  				sh->sndTime() = Scheduler::instance().clock();
  				return SNOOP_TAIL;
! 			} else if (sh->seqno() > seq) { 
! 
! 				//not cached before, should insert in the middle
! 				// find the position it should be: prev(i)
!  
  				Packet *temp = pkts_[prev(buftail_)];
  				for (int j = buftail_; j != i; j = next(j)) 
  					pkts_[prev(j)] = pkts_[j];
  				i = prev(i);
! 				pkts_[i] = temp;   // seems not necessary. Ben comments
  				buftail_ = prev(buftail_);
  				break;
  			}
  		}
+ 
+ 		// This should not happen, since seq must be > lastSeen, which is 
+ 		// handled before in the first if.   Ben comments
  		if (i == bufhead_)
  			bufhead_ = next(bufhead_);
  	}
+ 	
+ 	// save in the buffer
  	savepkt_(p, seq, i);
+ 	
  	if (bufhead_ == buftail_)
  		fstate_ |= SNOOP_FULL;
  	/* 
***************
*** 352,357 ****
--- 385,391 ----
  		retval = SNOOP_TAIL;
  	} else
  		lastSeen_ = seq;
+ 	
  	return retval;
  }
  
***************
*** 377,382 ****
--- 411,417 ----
  	Packet *pkt;
  
  	int ack = hdr_tcp::access(p)->seqno();
+ 
  	/*
  	 * There are 3 cases:
  	 * 1. lastAck_ > ack.  In this case what has happened is
***************
*** 391,423 ****
  	 * 3. lastAck_ < ack.  Set lastAck_ = ack, and update
  	 *    the head of the buffer queue. Also clean up ack'd packets.
  	 */
! 	if (fstate_ & SNOOP_CLOSED || lastAck_ > ack)
  		return SNOOP_PROPAGATE;	// send ack onward
  	if (lastAck_ == ack) {	
  		/* A duplicate ack; pure window updates don't occur in ns. */
  		pkt = pkts_[buftail_];
! 		if (pkt == 0)
  			return SNOOP_PROPAGATE;
  		hdr_snoop *sh = hdr_snoop::access(pkt);
  		if (pkt == 0 || sh->seqno() > ack + 1) 
  			/* don't have packet, letting thru' */
! 			return SNOOP_PROPAGATE;
  		/* 
  		 * We have the packet: one of 3 possibilities:
  		 * 1. We are not expecting any dupacks (expDupacks_ == 0)
  		 * 2. We are expecting dupacks (expDupacks_ > 0)
  		 * 3. We are in an inconsistent state (expDupacks_ == -1)
  		 */
  		if (expDupacks_ == 0) {	// not expecting it
  #define RTX_THRESH 1
  			static int thresh = 0;
! 			if (thresh++ < RTX_THRESH) {
  				/* no action if under RTX_THRESH */
  				return SNOOP_PROPAGATE;
! 			}
  			thresh = 0;
  			if (sh->senderRxmit()) 
  				return SNOOP_PROPAGATE;
  			/*
  			 * Otherwise, not triggered by sender.  If this is
  			 * the first dupack recd., we must determine how many
--- 426,470 ----
  	 * 3. lastAck_ < ack.  Set lastAck_ = ack, and update
  	 *    the head of the buffer queue. Also clean up ack'd packets.
  	 */
! 	if (fstate_ & SNOOP_CLOSED || lastAck_ > ack) 
  		return SNOOP_PROPAGATE;	// send ack onward
+ 
  	if (lastAck_ == ack) {	
  		/* A duplicate ack; pure window updates don't occur in ns. */
+ 
  		pkt = pkts_[buftail_];
! 		
! 		if (pkt == 0) 
  			return SNOOP_PROPAGATE;
+ 		
  		hdr_snoop *sh = hdr_snoop::access(pkt);
+ 
  		if (pkt == 0 || sh->seqno() > ack + 1) 
  			/* don't have packet, letting thru' */
! 		        return SNOOP_PROPAGATE;
! 
  		/* 
  		 * We have the packet: one of 3 possibilities:
  		 * 1. We are not expecting any dupacks (expDupacks_ == 0)
  		 * 2. We are expecting dupacks (expDupacks_ > 0)
  		 * 3. We are in an inconsistent state (expDupacks_ == -1)
  		 */
+ 
+ 			
  		if (expDupacks_ == 0) {	// not expecting it
  #define RTX_THRESH 1
+ 			
  			static int thresh = 0;
! 			if (thresh++ < RTX_THRESH) 
  				/* no action if under RTX_THRESH */
  				return SNOOP_PROPAGATE;
! 			
  			thresh = 0;
+ 			
+ 			// if the packet is a sender retransmission, pass on
  			if (sh->senderRxmit()) 
  				return SNOOP_PROPAGATE;
+ 			
  			/*
  			 * Otherwise, not triggered by sender.  If this is
  			 * the first dupack recd., we must determine how many
***************
*** 425,436 ****
  			 * rexmit the desired packet.  Note that expDupacks_
  			 * will be -1 if we miscount for some reason.
  			 */
  			expDupacks_ = bufhead_ - expNextAck_;
  			if (expDupacks_ < 0)
  				expDupacks_ += SNOOP_MAXWIND;
  			expDupacks_ -= RTX_THRESH + 1;
  			expNextAck_ = next(buftail_);
! 			if (sh->numRxmit() == 0)
  				return snoop_rxmit(pkt);
  		} else if (expDupacks_ > 0) {
  			expDupacks_--;
--- 472,486 ----
  			 * rexmit the desired packet.  Note that expDupacks_
  			 * will be -1 if we miscount for some reason.
  			 */
+ 			
+ 			
  			expDupacks_ = bufhead_ - expNextAck_;
  			if (expDupacks_ < 0)
  				expDupacks_ += SNOOP_MAXWIND;
  			expDupacks_ -= RTX_THRESH + 1;
  			expNextAck_ = next(buftail_);
! 
! 			if (sh->numRxmit() == 0) 
  				return snoop_rxmit(pkt);
  		} else if (expDupacks_ > 0) {
  			expDupacks_--;
***************
*** 442,452 ****
--- 492,506 ----
  		} else		// let sender deal with it
  			return SNOOP_PROPAGATE;
  	} else {		// a new ack
+ 
  		fstate_ &= ~SNOOP_NOACK; // have seen at least 1 new ack
+ 
  		/* free buffers */
  		double sndTime = snoop_cleanbufs_(ack);
+ 		
  		if (sndTime != -1)
  			snoop_rtt(sndTime);
+ 
  		expDupacks_ = 0;
  		expNextAck_ = buftail_;
  		lastAck_ = ack;
***************
*** 560,576 ****
  {
  	Scheduler &s = Scheduler::instance();
  	double sndTime = -1;
! 
  	if (toutPending_)
  		s.cancel(toutPending_);
  	toutPending_ = 0;
  	if (empty_())
  		return sndTime;
  	int i = buftail_;
  	do {
  		hdr_snoop *sh = hdr_snoop::access(pkts_[i]);
  		int seq = hdr_tcp::access(pkts_[i])->seqno();
! 
  		if (seq <= ack) {
  			sndTime = sh->sndTime();
  			Packet::free(pkts_[i]);
--- 614,632 ----
  {
  	Scheduler &s = Scheduler::instance();
  	double sndTime = -1;
! 	
  	if (toutPending_)
  		s.cancel(toutPending_);
  	toutPending_ = 0;
+ 
  	if (empty_())
  		return sndTime;
+ 
  	int i = buftail_;
  	do {
  		hdr_snoop *sh = hdr_snoop::access(pkts_[i]);
  		int seq = hdr_tcp::access(pkts_[i])->seqno();
! 		
  		if (seq <= ack) {
  			sndTime = sh->sndTime();
  			Packet::free(pkts_[i]);
***************
*** 591,596 ****
--- 647,653 ----
  		hdr_snoop *sh = hdr_snoop::access(pkts_[buftail_]);
  		tailTime_ = sh->sndTime();
  	}
+ 
  	return sndTime;
  }
  
***************
*** 646,654 ****
  Snoop::snoop_qlong()
  {
  	/* For now only instantaneous lengths */
! 	if (parent_->ifq()->length() <= 3*parent_->ifq()->limit()/4)
! 		return 1;
! 	return 0;
  }
  
  /*
--- 703,712 ----
  Snoop::snoop_qlong()
  {
  	/* For now only instantaneous lengths */
! 	//	if (parent_->ifq()->length() <= 3*parent_->ifq()->limit()/4)
! 	
! 	return 1;
! 		//	return 0;
  }
  
  /*
***************
*** 661,672 ****
  	if (pkt != 0) {
  		hdr_snoop *sh = hdr_snoop::access(pkt);
  		if (sh->numRxmit() < SNOOP_MAX_RXMIT && snoop_qlong()) {
! /*			&& sh->seqno() == lastAck_+1)  */
! 
  #if 0
  			printf("%f Rxmitting packet %d\n", s.clock(), 
  			       hdr_tcp::access(pkt)->seqno());
  #endif
  			sh->sndTime() = s.clock();
  			sh->numRxmit() = sh->numRxmit() + 1;
  			Packet *p = pkt->copy();
--- 719,735 ----
  	if (pkt != 0) {
  		hdr_snoop *sh = hdr_snoop::access(pkt);
  		if (sh->numRxmit() < SNOOP_MAX_RXMIT && snoop_qlong()) {
! 			/*			&& sh->seqno() == lastAck_+1)  */
! 			
  #if 0
  			printf("%f Rxmitting packet %d\n", s.clock(), 
  			       hdr_tcp::access(pkt)->seqno());
  #endif
+ 			
+ 			// need to specify direction, in this case, down
+ 			hdr_cmn *ch = HDR_CMN(pkt);       
+ 			ch->direction() = hdr_cmn::DOWN;  // Ben added
+ 
  			sh->sndTime() = s.clock();
  			sh->numRxmit() = sh->numRxmit() + 1;
  			Packet *p = pkt->copy();
***************
*** 699,706 ****
  		return;
  	if ((snoop_->bufhead_ != snoop_->buftail_) || 
  	    (snoop_->fstate_ & SNOOP_FULL)) {
! /*		printf("%f timeout\n", Scheduler::instance().clock());*/
  		if (snoop_->snoop_rxmit(p) == SNOOP_SUPPRESS)
  			snoop_->expNextAck_ = snoop_->next(snoop_->buftail_);
  	}
  }
--- 762,772 ----
  		return;
  	if ((snoop_->bufhead_ != snoop_->buftail_) || 
  	    (snoop_->fstate_ & SNOOP_FULL)) {
! 		//		printf("%f Snoop timeout\n", Scheduler::instance().clock());
  		if (snoop_->snoop_rxmit(p) == SNOOP_SUPPRESS)
  			snoop_->expNextAck_ = snoop_->next(snoop_->buftail_);
  	}
  }
+ 
+ 
+ 
diff -rNc ns-allinone-2.1b7a/ns-2.1b7a/tcl/ex/snoop/MySnoop.tcl ns-allinone-2.1b7aNNew/ns-2.1b7a/tcl/ex/snoop/MySnoop.tcl
*** ns-allinone-2.1b7a/ns-2.1b7a/tcl/ex/snoop/MySnoop.tcl	Wed Dec 31 19:00:00 1969
--- ns-allinone-2.1b7aNNew/ns-2.1b7a/tcl/ex/snoop/MySnoop.tcl	Thu Mar 29 21:40:51 2001
***************
*** 0 ****
--- 1,158 ----
+ puts "sourcing tcl/lan/vlan.tcl..."
+ source tcl/lan/vlan.tcl
+ source tcl/lan/ns-mac.tcl
+ 
+ 
+ set opt(tr)	out.tr
+ set opt(namtr)	"MySnoop.nam"
+ set opt(seed)	0
+ set opt(stop)	10
+ set opt(node)	2
+ 
+ set opt(qsize)	100
+ set opt(bw)	10Mb
+ set opt(delay)	1ms
+ set opt(ll)	LL
+ set opt(ifq)	Queue/DropTail
+ set opt(mac)	Mac/802_3
+ set opt(chan)	Channel
+ set opt(tcp)	TCP/Reno
+ set opt(sink)	TCPSink
+ 
+ set opt(app)	FTP
+ 
+ set loss_prob 10
+ 
+ 
+ proc finish {} {
+ 	global ns opt
+ 
+ 	$ns flush-trace
+ 
+ 	exec nam $opt(namtr) &
+ 
+ 	exit 0
+ }
+ 
+ 
+ proc create-trace {} {
+ 	global ns opt
+ 
+ 	if [file exists $opt(tr)] {
+ 		catch "exec rm -f $opt(tr) $opt(tr)-bw [glob $opt(tr).*]"
+ 	}
+ 
+ 	set trfd [open $opt(tr) w]
+ 	$ns trace-all $trfd
+ 	if {$opt(namtr) != ""} {
+ 		$ns namtrace-all [open $opt(namtr) w]
+ 	}
+ 	return $trfd
+ }
+ 
+ proc add-error {LossyLink} {
+ 
+     global loss_prob
+     
+     # creating the uniform distribution random variable
+     set loss_random_variable [new RandomVariable/Uniform] 
+     $loss_random_variable set min_ 0    # set the range of the random variable;
+     $loss_random_variable set max_ 100
+     
+     # create the error model;
+     set loss_module [new ErrorModel]  
+     $loss_module drop-target [new Agent/Null] 
+     $loss_module set rate_ $loss_prob  # set error rate to (0.1 = 10 / (100 - 0));
+     # error unit: packets (the default);
+     $loss_module unit pkt      
+     
+     # attach random var. to loss module;
+     $loss_module ranvar $loss_random_variable 
+ 
+     # keep a handle to the loss module;
+     #set sessionhelper [$ns create-session $n0 $tcp0] 
+     $LossyLink errormodule $loss_module
+ 
+ 
+ 
+ }
+ 
+ proc create-topology {} {
+ 	global ns opt 
+ 	global lan node s d
+ 
+ 	set num $opt(node)
+ 	for {set i 1} {$i < $num} {incr i} {
+ 		set node($i) [$ns node]
+ 		lappend nodelist $node($i)
+ 	}
+ 	
+ 
+ 	set lan [$ns make-lan $nodelist $opt(bw) \
+ 			$opt(delay) $opt(ll) $opt(ifq) $opt(mac) $opt(chan)]
+ 	
+ 
+ 	set opt(ll) LL/LLSnoop
+ 
+ 	set opt(ifq) Queue/DropTail
+ 	$opt(ifq) set limit_ 100
+ 
+ 	# set up snoop agent
+ 	set node(0) [$ns node]
+ 	$lan addNode [list $node(0)]  $opt(bw) $opt(delay) $opt(ll) $opt(ifq) $opt(mac)
+ 	
+ 	# set source and connect to node(0)
+ 	set s [$ns node]
+ 	$ns duplex-link $s $node(0) 5Mb 20ms DropTail
+ 	$ns queue-limit $s $node(0) 100000
+ 	$ns duplex-link-op $s $node(0) orient right
+ 
+ 	# set dest and connect to node(1)
+ 	set d [$ns node]
+ 	$ns duplex-link $node(1) $d 5Mb 10ms DropTail
+ 	$ns queue-limit $node(1) $d 100000
+ 	$ns duplex-link-op $d $node(1) orient left
+ 
+ 
+ 
+ 	set LossyLink [$ns link $node(1) $d]  
+ 
+ 	add-error $LossyLink
+ }
+ 
+ ## MAIN ##
+ 
+ set ns [new Simulator]
+ 
+ set trfd [create-trace]
+ 
+ create-topology
+ 
+ #set tcp0 [$ns create-connection TCP/Reno $s TCPSink $node(1) 0]
+ #$tcp0 set window_ 30
+ 
+ 
+ #Create a infinite source agent (FTP) tcp and attach it to node n0
+ set tcp0 [new Agent/TCP/Reno]
+ $tcp0 set backoff_ 2 
+ $tcp0 set window_ 30
+ $ns attach-agent $s $tcp0
+ 
+ set tcp_snk0 [new Agent/TCPSink]
+ $ns attach-agent $d $tcp_snk0
+ 
+ $ns connect $tcp0 $tcp_snk0
+ 
+ 
+ set ftp0 [$tcp0 attach-app FTP]
+ 
+ $ns at 0.0 "$ftp0 start"
+ $ns at $opt(stop) "finish"
+ $ns run
+ 
+ 
+ 
+ 
+ 
+ 
+ 
diff -rNc ns-allinone-2.1b7a/ns-2.1b7a/tcl/lan/vlan.tcl ns-allinone-2.1b7aNNew/ns-2.1b7a/tcl/lan/vlan.tcl
*** ns-allinone-2.1b7a/ns-2.1b7a/tcl/lan/vlan.tcl	Thu Sep 14 14:19:26 2000
--- ns-allinone-2.1b7aNNew/ns-2.1b7a/tcl/lan/vlan.tcl	Thu Apr  5 15:40:05 2001
***************
*** 269,275 ****
  	$ll_ up-target $iface_
  	$ll_ down-target $ifq_
  	$ll_ mac $mac_
! 	
  	$ifq_ target $mac_
  	
  	$mac_ up-target $ll_
--- 269,276 ----
  	$ll_ up-target $iface_
  	$ll_ down-target $ifq_
  	$ll_ mac $mac_
! 	$ll_ ifq $ifq_
! 
  	$ifq_ target $mac_
  	
  	$mac_ up-target $ll_
