//
// Copyright (c) 1997-2001 by the University of Southern California
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
// $Header: /nfs/jade/vint/CVSROOT/nam-1/enetmodel.cc,v 1.28 2001/06/04 20:36:25 mehringe Exp $
//
// Network model with Editor layout

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "random.h"
#include "view.h"
#include "netview.h"
#include "animation.h"
#include "queue.h"
#include "edge.h"
#include "node.h"
#include "agent.h"
#include "sincos.h"
#include "state.h"
#include "packet.h"
#include "enetmodel.h"
#include "trafficsource.h"

static int agent_number = 0;

class EditorNetworkModelClass : public TclClass {
public:
	EditorNetworkModelClass() : TclClass("NetworkModel/Editor") {}
	TclObject* create(int argc, const char*const* argv) {
		if (argc < 5) 
			return 0;
		return (new EditorNetModel(argv[4]));
	}
} editornetworkmodel_class;

EditorNetModel::EditorNetModel(const char *editor) : NetModel(editor)
{
	node_id_count = 0;
  traffic_sources_ = NULL;

	bind("Wpxmin_", &pxmin_);
        bind("Wpymin_", &pymin_);
	bind("Wpxmax_", &pxmax_);
        bind("Wpymax_", &pymax_);
}

EditorNetModel::~EditorNetModel()
{
}

void EditorNetModel::BoundingBox(BBox& bb)
{                       
	// by default, 800X1000 internal drawing area
        bb.xmin = pxmin_;
	bb.ymin = pymin_;
        bb.xmax = pxmax_;
        bb.ymax = pymax_;

        for (Animation* a = drawables_; a != 0; a = a->next())
                a->merge(bb);

	pxmin_ = bb.xmin;
	pymin_ = bb.ymin;
	pxmax_ = bb.xmax;
	pymax_ = bb.ymax;
}

//----------------------------------------------------------------------
// Node * 
// EditorNetModel::addNode(Node * node)
//----------------------------------------------------------------------
Node * 
EditorNetModel::addNode(Node * node) {
  if (!lookupNode(node->num())) {
    node->next_ = nodes_;
    nodes_ = node;
    node->Animation::insert(&drawables_);
    return node;
  }
  return NULL;
}

//----------------------------------------------------------------------
// int
// EditorNetModel::addNode(int node_id)
//----------------------------------------------------------------------
Node *
EditorNetModel::addNode(int node_id) {
  Node * node = NULL;
  static char name[TRACE_LINE_MAXLEN];
  double size = 10.0; // This is the default node size taken from
                      // parser.cc under the 'n' animation event

  sprintf(name, "%d", node_id);
  node = new CircleNode(name, size);     
  node->init_color("black");
    
  if (node_id_count <= node_id) {
    node_id_count = node_id + 1;
  }

  if (!addNode(node)) {
    delete node;
    node = NULL; 
  }

  return node;
}
 

//----------------------------------------------------------------------
// int 
// EditorNetModel::addNode(float cx, float cy)
//  - Create a node using the specified name
//    and the default size and insert it into this
//    NetModel's list of drawables.
//----------------------------------------------------------------------
int 
EditorNetModel::addNode(float cx, float cy) {
  Node * node;

  // Find the next node id number
  if (node_id_count == 0 ) {
    for (node = nodes_; node != 0; node = node->next_) {
      if (node->num() > node_id_count) {
        node_id_count = node->num(); 
      }
    }
    node_id_count++;
  }
   
  node = addNode(node_id_count);
  if (node) {
    node->place(cx,cy);
  }

  return (TCL_OK);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
Edge *
EditorNetModel::addEdge(Edge * edge) {
  enterEdge(edge); // adds edge to a hash table

  edge->Animation::insert(&drawables_); // adds to list of drawable objects

  placeEdge(edge, edge->getSourceNode());

  return edge;
}

//----------------------------------------------------------------------
// int
// EditorNetModel::addLink(Node * src, Node * dst)
//   - All links are duplex links by default
//----------------------------------------------------------------------
int
EditorNetModel::addLink(Node * src, Node * dst) {
  double bw = 10.0;
  double delay = 1.0;
  double length = 0.0;
  double angle = 1.0;
  Edge * e;

  // Create the forward edge
  e = new Edge(src, dst, 25, bw, delay, length, angle);
  e->init_color("black");

  enterEdge(e);
  e->Animation::insert(&drawables_);
  src->add_link(e);

  placeEdge(e, src);

   
  // Create the reverse edge
  e = new Edge(dst, src, 25, bw, delay, length, angle);

  e->init_color("black");
  enterEdge(e);
  e->Animation::insert(&drawables_);
  dst->add_link(e);

  placeEdge(e, dst);
  
  return (TCL_OK);
}

//----------------------------------------------------------------------
// int EditorNetModel::addLink(int source_id, int destination_id)
//----------------------------------------------------------------------
int 
EditorNetModel::addLink(int source_id, int destination_id) {
  Node * source_node, * destination_node;

  source_node = lookupNode(source_id);
  destination_node = lookupNode(destination_id);

  if (source_node && destination_node) {
    return addLink(source_node, destination_node);
  }

  return TCL_ERROR;
}

//----------------------------------------------------------------------
// int
// EditorNetModel::addAgent(Agent * agent, Node * node)
//----------------------------------------------------------------------
int
EditorNetModel::addAgent(Agent * agent, Node * node) {
  placeAgent(agent, node);
    
  // Add to animations list so that it will be redrawn on all updates
  agent->Animation::insert(&animations_);
  agent->node_ = node;
  node->add_agent(agent);

  return (TCL_OK);
}

//----------------------------------------------------------------------
// int
// EditorNetModel::addAgent(Node *src, char * agent_type, float cx, float cy)
//----------------------------------------------------------------------
int
EditorNetModel::addAgent(Node * src, char * agent_type, float cx, float cy) {
  Agent *a;
  Node *n = lookupNode(src->num());

  if (n == 0) {
    return 0;
  } else {
    a = new BoxAgent(agent_type, n->size());
    placeAgent(a, src);

    // Add to animations list so that it will be redrawn on all updates
    a->Animation::insert(&animations_);
    a->node_ = n;
    //a->number_ = agent_number++;
    agent_number++;
    n->add_agent(a);
  }
  return (TCL_OK);
}

//----------------------------------------------------------------------
// int
// EditorNetModel::addAgentLink(Agent *src_agent, Agent *dst_agent)
//   - Need to add more constraints on which agents can connect to 
//     each other
// 
//----------------------------------------------------------------------
int EditorNetModel::addAgentLink(Agent *src_agent, Agent *dst_agent) {

  if (!src_agent->AgentPartner_ && !dst_agent->AgentPartner_) {
    // Only connect agents that don't have a partner yet

    if ((strcmp(src_agent->name(), "TCP") == 0) ||
        (strcmp(src_agent->name(), "TCP/Reno") == 0) ||
        (strcmp(src_agent->name(), "TCP/NewReno") == 0) ||
        (strcmp(src_agent->name(), "TCP/Vegas") == 0) ||
        (strcmp(src_agent->name(), "TCP/Sack1") == 0) ||
        (strcmp(src_agent->name(), "TCP/Fack") == 0) ||
        (strcmp(src_agent->name(), "UDP") == 0)) {
      src_agent->AgentRole_ = SOURCE;
    }
    if ((strcmp(src_agent->name(), "TCPSink") == 0) ||
        (strcmp(src_agent->name(), "TCPSink/DelAck") == 0) ||
        (strcmp(src_agent->name(), "TCPSink/Sack1") == 0) ||
        (strcmp(src_agent->name(), "Null") == 0)) {
      src_agent->AgentRole_ = DESTINATION;
    }

    if ((strcmp(dst_agent->name(), "TCP") == 0) ||
        (strcmp(dst_agent->name(), "TCP/Reno") == 0) ||
        (strcmp(dst_agent->name(), "TCP/NewReno") == 0) ||
        (strcmp(dst_agent->name(), "TCP/Vegas") == 0) ||
        (strcmp(dst_agent->name(), "TCP/Sack1") == 0) ||
        (strcmp(dst_agent->name(), "TCP/Fack") == 0) ||
        (strcmp(dst_agent->name(), "UDP") == 0)) {
      dst_agent->AgentRole_ = SOURCE;
    }
    if ((strcmp(dst_agent->name(), "TCPSink") == 0) ||
        (strcmp(dst_agent->name(), "TCPSink/DelAck") == 0) ||
        (strcmp(dst_agent->name(), "TCPSink/Sack1") == 0) ||
        (strcmp(dst_agent->name(), "Null") == 0)) {
      dst_agent->AgentRole_ = DESTINATION;
    }
   

    if ((src_agent->AgentRole_ == SOURCE && 
         dst_agent->AgentRole_ == DESTINATION) ||
        (dst_agent->AgentRole_ == SOURCE && 
         src_agent->AgentRole_ == DESTINATION)) {
      // Only allows SOURCES and DESTINATION agents to 
      // connect to each other
      src_agent->AgentPartner_ = dst_agent;
      dst_agent->AgentPartner_ = src_agent;
    }
  }

  return (TCL_OK);
}

//----------------------------------------------------------------------
// int
// EditorNetModel::addAgentLink(int source_id, int destination_id) {
//----------------------------------------------------------------------
int
EditorNetModel::addAgentLink(int source_id, int destination_id) {
  Agent * source, * destination;

  source = lookupAgent(source_id);
  destination = lookupAgent(destination_id);

  if (source && destination) {
   return addAgentLink(source, destination);
  }

  return TCL_ERROR;
}


//----------------------------------------------------------------------
// int
// EditorNetModel::addTrafficSource(TrafficSource * traffic_source,
//                                  Agent * agent)
//----------------------------------------------------------------------
int
EditorNetModel::addTrafficSource(TrafficSource * traffic_source,
                                 Agent * agent) {

  if (traffic_source && agent) {
    traffic_source->attachTo(agent);

    // Add to animations list so that it will be redrawn on all updates
    traffic_source->Animation::insert(&animations_);

    // Add to master list of all trafficsources_ which is used to 
    // look up a traffic source for editing purposes
    traffic_source->editornetmodel_next_ = traffic_sources_;
    traffic_sources_ = traffic_source;
    return TCL_OK;
  }
  return TCL_ERROR;
}


//----------------------------------------------------------------------
// int
// EditorNetModel::addTrafficSource(Agent * agent, char * type,
//                                  float cx, float cy)
//----------------------------------------------------------------------
int
EditorNetModel::addTrafficSource(Agent * agent, char * type,
                                  float cx, float cy) {
  TrafficSource * traffic_source;
  if (agent) {
    traffic_source = new TrafficSource(type, agent->size());
    traffic_source->attachTo(agent);

    // Add to animations list so that it will be redrawn on all updates
    traffic_source->Animation::insert(&animations_);

    // Add to master list of all trafficsources_ which is used to 
    // look up a traffic source for editing purposes
    traffic_source->editornetmodel_next_ = traffic_sources_;
    traffic_sources_ = traffic_source;
  }
  return (TCL_OK);
}

//----------------------------------------------------------------------
// TrafficSource *
// EditorNetModel::lookupTrafficSource(int id)
//----------------------------------------------------------------------
TrafficSource *
EditorNetModel::lookupTrafficSource(int id) {
  TrafficSource * ts;

  for (ts = traffic_sources_; ts; ts = ts->editornetmodel_next_) {
    if (ts->number() == id) {
      break;
    }
  }

  return ts;
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void EditorNetModel::setNodeProperty(int id, char* pv, int pn) {
  Node *n = lookupNode(id);

  if (n == NULL) {
    fprintf(stderr, "Node %d does not exist.\n", id);
    return;
  }

  switch (pn) {
    case 0: {
      double s = atof(pv);
      n->size(s);
      break;
    }
    case 1:  { // color
      n->init_color(pv);
      break;
    }
    case 2: { //label
      if (strcmp(pv, "") != 0) 
        n->dlabel(pv);
      break;
    }
  }
}

void EditorNetModel::setAgentProperty(int id, char* pv, int pn)
{
	Agent *a = lookupAgent(id);
        if (a == NULL) {
          fprintf(stderr, "Nonexisting agent %d.\n", id);
          return;
        }

	switch (pn) {
	     case 0:    // initial window size
	       {
		    int size = atoi(pv);
		    a->windowInit(size);
		    break;
	       }
	     case 1:    // window size
	       {
		   int size = atoi(pv);
		   a->window(size);
		   break;
	       }
	     case 2:     // maximum cwnd_
	       {
		   int size = atoi(pv);
		   a->maxcwnd(size);
		   break;
	       }
	     case 3:     // tracevar
	       {
		   a->tracevar(pv);
		   break;
		}
	     case 4:     // start-at
	       {
		 float time = atof(pv);
		 a->startAt(time);
		 break;
	       }
	     case 5:     // FTP producemore
	       {
		 int size = atoi(pv);
		 a->produce(size);
		 break;
	       }
             case 6:     // stop-at
	       {
		 float time = atof(pv);
		 a->stopAt(time);
		 break;
	       }
	     case 7:    // Flow ID + Color
	       {
	          if ((strcmp(pv, "(null)") == 0) || (strcmp(pv, "") == 0)) {
		            pv = "black";
			    a->flowcolor(pv);
		  } else {
		            a->flowcolor(pv);
		  } 
		 break;
	       }
	     case 8:    // CBR packetSize
	       {
		 int size = atoi(pv);
		 a->packetSize(size);
		 break;
	       }
	     case 9:    // CBR interval
	       {
		 float size = atof(pv);
		 a->interval(size);
		 break;
	       }

	}
}

void EditorNetModel::setLinkProperty(int sid,int did, char* pv, int pn)
{
	EdgeHashNode *h = lookupEdge(sid, did);
	EdgeHashNode *g = lookupEdge(did, sid);

        if (h == NULL || g == NULL ) {
            fprintf(stderr, "Nonexisting link %d.%d\n", sid, did);
            return;
        }

        switch (pn) {
            case 0: //bandwidth
            {
                double s = atof(pv);
		(h->edge)->setBW(s);
		(g->edge)->setBW(s);
                break; 
            }
            case 1:     //color
            {
                //int color = atoi(pv);
                (h->edge)->init_color(pv);
                (g->edge)->init_color(pv);
                break;
            }
            case 2:     //delay
            {
                double s = atof(pv);
		(h->edge)->setDelay(s);
		(g->edge)->setDelay(s);
                break;
            }
	    case 3:     //label
	      {
		if (strcmp(pv, "") != 0) {
		        (h->edge)->dlabel(pv);
			(g->edge)->dlabel(pv);
		}
		break;
	      }
	     default: break;
        }
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
void
EditorNetModel::setTrafficSourceProperty(int id, char * value,
                                         char * variable) {
  TrafficSource * trafficsource;
  
  trafficsource = lookupTrafficSource(id);
  if (trafficsource) {
    if (strcmp(variable, "start_") == 0) {
      //trafficsource->startAt(atof(value));
      trafficsource->timelist.add(strtod(value, NULL));
  
    } else if (strcmp(variable, "stop_") == 0) {
      //trafficsource->stopAt(atof(value));
      trafficsource->timelist.add(strtod(value, NULL));

    } else if (strcmp(variable, "interval_") == 0) {
      trafficsource->setInterval(atof(value));

    } else if (strcmp(variable, "maxpkts_") == 0) {
      trafficsource->setMaxpkts(atoi(value));

    } else if (strcmp(variable, "addtime") == 0) {
      trafficsource->timelist.add(atof(value));

    } else if (strcmp(variable, "removetime") == 0) {
      trafficsource->timelist.remove(atof(value));

    } else if (strcmp(variable, "timelist") == 0) {
      trafficsource->timelist.setList(value);
    }

  } else {
    fprintf(stderr, " Unable to find traffic source with id %d\n", id);
  }

}


//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
int EditorNetModel::command(int argc, const char *const *argv) {               
  FILE *file;
  if (argc == 2) {
    if (strcmp(argv[1], "layout") == 0) {
      layout();
      return (TCL_OK);
    }

  } else if (argc == 3) {
    // Write out nam editor file information
    if (strcmp(argv[1], "saveasenam") == 0) {
      // argv syntax is as follows:
      // <net> saveasenam filename
//      file = fopen(argv[2], "w");
//      if (file) {
//        fprintf(stderr, "Writing nam editor information to %s...",argv[2]);
       // saveAsEnam(file);
//        fclose(file);
//        fprintf(stderr, "done.\n");
//      } else {
 //       fprintf(stderr, "nam: Unable to open file: %s\n", argv[2]);
 //     }
      return (TCL_OK);

    // Write out ns script information
    } else if (strcmp(argv[1], "saveasns") == 0) {
      // argv syntax is as follows:
      // <net> saveasns filename
      file = fopen(argv[2], "w");
      if (file) {
//        fprintf(stderr, "Writing ns script to %s...",argv[2]);
        writeNsScript(file);
        fclose(file);
        fprintf(stderr, "done.\n");
      } else {
        fprintf(stderr, "nam: Unable to open file: %s\n", argv[2]);
      }
      return (TCL_OK);
    }
  }      
  return (NetModel::command(argc, argv));
}                  

//----------------------------------------------------------------------
//----------------------------------------------------------------------
void EditorNetModel::layout() {
  Node *n;
  for (n = nodes_; n != 0; n = n->next_)
    for (Edge* e = n->links(); e != 0; e = e->next_)
      placeEdge(e, n);
}

//----------------------------------------------------------------------
// int 
// EditorNetModel::saveAsNs(FILE * file)
//----------------------------------------------------------------------
int 
EditorNetModel::writeNsScript(FILE * file) {
  Node * node;
  Edge * edge;      
  Agent * agent;
  TrafficSource * ts;
  double finish_time = 0.0;
  
  // Add a disclaimer

  fprintf(file, "#------------------------------------------------------- \n");
  fprintf(file, "# This ns script has been created by the nam editor.\n");
  fprintf(file, "# If you edit it manually, the nam editor might not\n");
  fprintf(file, "# be able to open it properly in the future.\n");
  fprintf(file, "#\n");
  fprintf(file, "# EDITING BY HAND IS AT YOUR OWN RISK!\n");
  fprintf(file, "#------------------------------------------------------- \n");

  fprintf(file, "# Create a new simulator object.\n");
  fprintf(file, "set ns [new Simulator]\n");
  fprintf(file, "# Create a nam trace datafile.\n");
  fprintf(file, "set namfile [open trace.nam w]\n");
  fprintf(file, "$ns namtrace-all $namfile\n");

  // write out node creation infomation
  fprintf(file, "\n# Create nodes.\n");
  for (node = nodes_; node; node = node->next_) {
    node->writeNsScript(file);
  }

  // write out link information
  fprintf(file, "\n# Create links between nodes.\n");
  for (node = nodes_; node ; node = node->next_) {
    for (edge = node->links(); edge; edge = edge->next_) {
      edge->writeNsScript(file);
    }
  }

  // write out agents and it's traffic sources
  fprintf(file, "\n# Create agents.\n");
  for (node = nodes_; node; node = node->next_) {
    for (agent = node->agents(); agent; agent = agent->next_) {
      agent->writeNsDefinitionScript(file);
    }
  }

  // Write out agent connections
  // An agent can connect to only one other agent
  fprintf(file, "\n# Connect agents.\n");
  for (node = nodes_; node; node = node->next_) {
    for (agent = node->agents(); agent; agent = agent->next_) {
      agent->writeNsConnectionScript(file);
    }
  }

  // Find time at which to stop the simulation
  for (ts = traffic_sources_; ts; ts = ts->editornetmodel_next_) {
    if (finish_time < ts->stopAt()) {
      finish_time = ts->stopAt();
    }
  }
    

  fprintf(file, "# Run the simulation\n");
  fprintf(file, "proc finish {} {\n  global ns namfile\n  $ns flush-trace\n");
  fprintf(file, "  close $namfile\n  exec nam trace.nam &  \n  exit 0\n  }\n");
  fprintf(file, "$ns at %f \"finish\"\n", finish_time);
  fprintf(file, "$ns run\n");
  return 0;
}

int EditorNetModel::saveAsEnam(FILE * file) {
  Node *n;
  Edge *e;      
  Agent *a;
  int ret;       
  if (file == 0)   {
    return -1;  
  }             

  // save warning info
  fprintf(file, "## All lines starting with ## are used by the nam editor.\n");
  fprintf(file, "## Please do not try to edit them manually.\n");

  // save version info
  fprintf(file, "##V -t * -v 1.0a10 -a 0\n");

  // save page size
  fprintf(file, "##W -t * -x %f -y %f -X %f -Y %f\n", 
                pxmin_, pymin_, pxmax_, pymax_);

  // save node information
  for (n = nodes_; n != 0; n = n->next_) {   
    ret = n->saveAsEnam(file); 
    if (ret != 0) {
      fclose(file);
      return -1;
    }           
  }

  //save link information
  for (n = nodes_; n != 0; n = n->next_)
    for(e= n->links(); e !=0; e = e->next_) {           
      ret = e->saveAsEnam(file);
      if (ret!=0) {
        fclose(file);   
        return -1;
      }
    } 

  // save agent information
  for (n = nodes_; n != 0; n = n->next_) 
    for (a= n->agents(); a !=0; a = a->next_) {
      ret = a->saveAsEnam(file);
      if (ret!=0) {
        fclose(file);
        return -1;
      }

      // Save traffic source information
    }

  fprintf(file, "## Please do not try to edit this file above this line.\n\n");
  return 0; 
}

//----------------------------------------------------------------------
// void
// EditorNetModel::removeNode(Node *n)
//   - remove node n from the nodes_ list and delete it
//----------------------------------------------------------------------
void EditorNetModel::removeNode(Node *n) {
  Node * previous, * run;
  Edge * edge;
  Agent * agent;
  previous = nodes_;     

  for (run = nodes_; run; run = run->next_) {
    if (n->num() == run->num()) {
      // when deleting the last node
      if (run->num() == nodes_->num()) {
        nodes_ = nodes_->next_;
        break;
      } else {
        previous->next_ = run->next_; 
        break;
      }
    }
    previous = run;
  }
  run->next_ = NULL;

  // Remove it from list of drawables
  run->detach();  

  // delete edges of the nodes_
  for (edge = run->links(); edge != 0; edge = edge->next_) {
    removeLink(edge);
  }

  // delete agents on the nodes_
  for (agent = run->agents(); agent != 0; agent = agent->next_) {
    removeAgent(agent);
  }

  delete run;
}

//----------------------------------------------------------------------
// void
// EditorNetModel::removeLink(Edge *e)
// 
//----------------------------------------------------------------------
void
EditorNetModel::removeLink(Edge *e) {
	EdgeHashNode * h, * g;
  Edge * e1, * e2;
	int src = e->src();
	int dst = e->dst();

	h = lookupEdge(src, dst);
	g = lookupEdge(dst, src);

	if (h == 0 || g == 0) {
		// h,g = 0 or h,g !=0
		return;
	} 

	e1 = h->edge;
	e2 = g->edge;
 
  // defined in netmodel.cc
	removeEdge(e1);
	removeEdge(e2);
 
	e1->detach();
	e2->detach();
 
	Node* srcnode = e1->start();
	Node* dstnode = e2->start();

	// it is a duplex by default
	srcnode->delete_link(e1);
	dstnode->delete_link(e2); 

	delete e1;
	delete e2;
}

//----------------------------------------------------------------------
// void 
// EditorNetModel::removeAgent(Agent *a)
//   - removes and deletes an Agent and all of its Traffic Sources
//----------------------------------------------------------------------
void 
EditorNetModel::removeAgent(Agent *a) {
  Node * n;
  TrafficSource * ts, * next_ts;
  n = a->node_;
  
  if (a->AgentPartner_ != NULL) {
    a->AgentPartner_->AgentPartner_ = NULL;
    a->AgentPartner_->AgentRole_ = 0;
  }
  n->delete_agent(a);
  a->detach();

  for (ts = a->traffic_sources_; ts; ts = next_ts) {
    // Save next traffic source in list since this
    // traffic source will be deleted in removeTrafficSource
    next_ts = ts->next_;
    removeTrafficSource(ts);
  }

  delete a;
}


//----------------------------------------------------------------------
// void 
// EditorNetModel::removeTrafficSource(TrafficSource * ts) {
//----------------------------------------------------------------------
void 
EditorNetModel::removeTrafficSource(TrafficSource * ts) {
  TrafficSource * run, * previous;

  // Run down the list of traffic sources
  previous = NULL;
  for (run = traffic_sources_; run; run = run->editornetmodel_next_) {
    if (run == ts) {
      break;
    }
    previous = run;
  }

  if (previous) {
    previous->editornetmodel_next_ = ts->editornetmodel_next_;
  } else {
    if (run == traffic_sources_) {
      // ts is at the front of the list
      traffic_sources_ = ts->editornetmodel_next_;
    }
  }
  ts->editornetmodel_next_ = NULL;

  ts->removeFromAgent();

  delete ts; 
}
