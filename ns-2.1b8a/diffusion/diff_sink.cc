// Copyright (c) 2000 by the University of Southern California
// All rights reserved.
//
// Permission to use, copy, modify, and distribute this software and its
// documentation in source and binary forms for non-commercial purposes
// and without fee is hereby granted, provided that the above copyright
// notice appear in all copies and that both the copyright notice and
// this permission notice appear in supporting documentation. and that
// any documentation, advertising materials, and other materials related
// to such distribution and use acknowledge that the software was
// developed by the University of Southern California, Information
// Sciences Institute.  The name of the University may not be used to
// endorse or promote products derived from this software without
// specific prior written permission.
//
// THE UNIVERSITY OF SOUTHERN CALIFORNIA makes no representations about
// the suitability of this software for any purpose.  THIS SOFTWARE IS
// PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Other copyrights might apply to parts of this software and are so
// noted when applicable.
//

/********************************************************************/
/* diff_sink.cc : Chalermek Intanagonwiwat (USC/ISI)  08/21/99      */
/********************************************************************/

#include <stdlib.h>
#include "diff_sink.h"
#include "diffusion.h"
#include "diff_rate.h"
#include "hash_table.h"
#include "agent.h"
#include "packet.h"
#include "tclcl.h"
#include "random.h"
#include "god.h"

#define REPORT_PERIOD    1.0

extern char* MsgStr[];

void Report_Timer::expire(Event *) {
  a_->report();
}


void Sink_Timer::expire(Event *) {
  a_->timeout(0);
}

void Periodic_Timer::expire(Event *) {
  a_->bcast_interest();
}

static class SinkClass : public TclClass {
public:
  SinkClass() : TclClass("Agent/Diff_Sink") {}
  TclObject* create(int , const char*const* ) {
    return(new SinkAgent());
  }
} class_sink;


SinkAgent::SinkAgent() : Agent(PT_DIFF), data_type_(0), 
  running_(0), random_(0), sink_timer_(this), periodic_timer_(this),
  report_timer_(this)
{
  // set option first.

  APP_DUP_ = true;

  periodic_ = true;
  always_max_rate_ = false;

  // Bind Tcl and C++ Variables

  bind("data_type_", &data_type_);
  bind_time("interval_", &interval_);
  bind("packetSize_", &size_);
  bind("random_", &random_);
  bind("maxpkts_", &maxpkts_);

  // Initialize variables.

  // maxpkts_ = 2;
  pk_count=0;
  num_recv=0;
  num_send=0;
  RecvPerSec=0;

  cum_delay=0.0;

  data_counter = 0;
  simple_report_rate = ORIGINAL;

  last_arrival_time = -1.0;
}

void SinkAgent::start()
{
	running_ = 1;	
	sendpkt();
	sink_timer_.resched(interval_);
}

void SinkAgent::stop()
{
  if (running_) {
	running_ = 0;
  }
  
  if (periodic_ == true) {
    periodic_ = false;
    periodic_timer_.force_cancel();
  }
}


void SinkAgent::report()
{
  //  printf("SK %d: RecvPerSec %d at time %lf\n", here_.addr_, RecvPerSec, NOW);
  report_timer_.resched(REPORT_PERIOD);
  RecvPerSec = 0;
}


void SinkAgent::timeout(int)
{
	if (running_) {
		sendpkt();
		double t = interval_;
		if (random_)
			/* add some zero-mean white noise */
			t += interval_ * Random::uniform(-0.5, 0.5);
			sink_timer_.resched(t);
	}
}


void SinkAgent::sendpkt()
{
      if (pk_count >=  maxpkts_) {
        running_ = 0;
        return;
      }

      Packet* pkt = create_packet();
      hdr_diff* dfh = HDR_DIFF(pkt);
      hdr_ip* iph = HDR_IP(pkt);
      hdr_cmn*  cmh = HDR_CMN(pkt);


      data_counter = (data_counter + SUB_SAMPLED)% ORIGINAL;
      if (data_counter == SUB_SAMPLED) {
	dfh->report_rate = SUB_SAMPLED;
      } else {
	dfh->report_rate = ORIGINAL;
      }
      if (simple_report_rate < dfh->report_rate) {
	Packet::free(pkt);
	return;
      }
      
      cmh->size() = size_;
      dfh->mess_type = DATA;
      dfh->pk_num = pk_count;

      pk_count++;

      dfh->sender_id = here_;
      dfh->data_type = data_type_;
      dfh->forward_agent_id = here_; 

      dfh->num_next = 1;
      dfh->next_nodes[0] = here_.addr_;

      iph->src_ = here_;
      iph->dst_.addr_ = here_.addr_;
      iph->dst_.port_ = ROUTING_PORT;
       
      // Send the packet
      // printf("Source %s send packet (%x, %d) at %lf.\n", name(), 
      //    dfh->sender_id, dfh->pk_num, NOW);


      num_send++;
      dfh->attr[0] = data_type_;

      if (APP_DUP_ == true)
	dfh->attr[1] = 0;             // Represent detection of the same animal
      else
	dfh->attr[1] = here_.addr_;   // Detect a different animal. 

      dfh->attr[2] = num_send;       
      God::instance()->CountNewData(dfh->attr);
      send(pkt, 0);
}


void SinkAgent::bcast_interest()
{
      Packet* pkt = create_packet();
      hdr_diff* dfh = HDR_DIFF(pkt);
      hdr_ip* iph = HDR_IP(pkt);

      // Set message type, packet number and sender ID
      dfh->mess_type = INTEREST;
      dfh->pk_num = pk_count;
      pk_count++;
      dfh->sender_id = here_;	
      dfh->data_type = data_type_;
      dfh->forward_agent_id = here_; 

      dfh->report_rate = SUB_SAMPLED;
      dfh->num_next = 1;
      dfh->next_nodes[0] = here_.addr_;

      iph->src_ = here_;
      iph->dst_.addr_ = here_.addr_;
      iph->dst_.port_ = ROUTING_PORT;
 
 

      // Send the packet

      // printf("Sink %s send packet (%x, %d) at %f to %x.\n", 
      //    name_, dfh->sender_id,
      //     dfh->pk_num, 
      //     NOW,
      //     iph->dst_);

      send(pkt, 0);	
      if (periodic_ == true)
	periodic_timer_.resched(INTEREST_PERIODIC);
}


void SinkAgent::data_ready()
{
      // Create a new packet
      Packet* pkt = create_packet();

      // Access the Sink header for the new packet:
      hdr_diff* dfh = HDR_DIFF(pkt);
      hdr_ip* iph = HDR_IP(pkt);

      // Set message type, packet number and sender ID
      dfh->mess_type = DATA_READY;
      dfh->pk_num = pk_count;
      pk_count++;
      dfh->sender_id = here_;	
      dfh->data_type = data_type_;
      dfh->forward_agent_id = here_; 

      dfh->num_next = 1;
      dfh->next_nodes[0] = here_.addr_;

      iph->src_ = here_;
      iph->dst_.addr_ = here_.addr_;
      iph->dst_.port_ = ROUTING_PORT;
 
      send(pkt, 0);
}


void SinkAgent::Terminate() 
{
#ifdef DEBUG_OUTPUT
  printf("SINK %d : TYPE %d : terminates (send %d, recv %d, cum_delay %f)\n", 
	 here_.addr_, data_type_, num_send, num_recv, cum_delay);
#endif
}


int SinkAgent::command(int argc, const char*const* argv)
{
  if (argc == 2) {

    if (strcmp(argv[1], "enable-duplicate") == 0) {
      APP_DUP_ = true;
      return TCL_OK;
    }

    if (strcmp(argv[1], "disable-duplicate") == 0) {
      APP_DUP_ = false;
      return TCL_OK;
    }

    if (strcmp(argv[1], "always-max-rate") == 0) {
      always_max_rate_ = true;
      return TCL_OK;
    }

    if (strcmp(argv[1], "terminate") == 0) {
      Terminate();
      return TCL_OK;
    }

    if (strcmp(argv[1], "announce") == 0) {
      bcast_interest();
      report_timer_.resched(REPORT_PERIOD);

      return (TCL_OK);
    }

    if (strcmp(argv[1], "ready") == 0) {
      God::instance()->data_pkt_size = size_;
      data_ready();
      return (TCL_OK);
    }

    if (strcmp(argv[1], "send") == 0) {
      sendpkt();   
      return (TCL_OK);
    }

    if (strcmp(argv[1], "cbr-start") == 0) {
       start();
       return (TCL_OK);
    }

    if (strcmp(argv[1], "stop") == 0) {
	stop();
        report_timer_.force_cancel();
	return (TCL_OK);
    }

  }

  if (argc == 3) {
    if (strcmp(argv[1], "data-type") == 0) {
      data_type_ = atoi(argv[2]);
      return (TCL_OK);
    }
  }

  return (Agent::command(argc, argv));
}


void SinkAgent::recv(Packet* pkt, Handler*)
{
  hdr_diff* dfh = HDR_DIFF(pkt);

  /*
  printf("SK %d recv (%x, %x, %d) %s size %d at time %lf\n", here_.addr_, 
	 (dfh->sender_id).addr_, (dfh->sender_id).port_,
	 dfh->pk_num, MsgStr[dfh->mess_type], cmh->size(), NOW);
  */

  if (data_type_ != dfh->data_type) {
      printf("Hey, What are you doing? I am not a sink for %d. I'm a sink for %d. \n", dfh->data_type, data_type_);
      Packet::free(pkt);
      return;
  }


  switch(dfh->mess_type) {
    case DATA_REQUEST :

      if (always_max_rate_ == false)
	simple_report_rate = dfh->report_rate;
      
      if (!running_) start();

      //      printf("I got a data request for data rate %d at %lf. Will send it right away.\n",
      //	     simple_report_rate, NOW);

      break;

    case DATA_STOP :

      if (running_) stop();
      break;

    case DATA :

      if (APP_DUP_ == true) {
	if (DataTable.GetHash(dfh->attr) != NULL) {
	  Packet::free(pkt);
	  return;
	} else {
	  DataTable.PutInHash(dfh->attr);
	}
      }

      cum_delay = cum_delay + (NOW - dfh->ts_);
      num_recv++;
      RecvPerSec++;
      God::instance()->IncrRecv();

      /*
      if (last_arrival_time > 0.0) {
	printf("SK %d: Num_Recv %d, InterArrival %lf\n", here_.addr_, 
	       num_recv, (NOW)-last_arrival_time);
      }
      */
      last_arrival_time = NOW;

      break;

    default:

      break;
  }

  Packet::free(pkt);
}


void SinkAgent::reset()
{
}


void SinkAgent:: set_addr(ns_addr_t address)
{
  here_=address;
}


int SinkAgent:: get_pk_count()
{
  return pk_count;
}


void SinkAgent:: incr_pk_count()
{
  pk_count++;
}


Packet * SinkAgent:: create_packet()
{
  Packet *pkt = allocpkt();

  if (pkt==NULL) return NULL;

  hdr_cmn*  cmh = HDR_CMN(pkt);

  cmh->size() = 36;

  hdr_diff* dfh = HDR_DIFF(pkt);
  dfh->ts_ = NOW; 

  return pkt;
}





