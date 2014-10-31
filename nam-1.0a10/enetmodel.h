//
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
// $Header: /nfs/jade/vint/CVSROOT/nam-1/enetmodel.h,v 1.11 2001/03/26 20:55:31 mehringe Exp $
//
// Network model for Editor

#ifndef nam_enetmodel_h
#define nam_enetmodel_h

#include "netmodel.h"
#include "trafficsource.h"

// The ratio of node radius and mean edge length in the topology

class EditorNetModel : public NetModel {
public:
	EditorNetModel(const char *animator);
	virtual ~EditorNetModel();
	void BoundingBox(BBox&);

	Node * addNode(Node * node);
	Node * addNode(int node_id);
	int addNode(float cx, float cy);
  Edge* addEdge(Edge * edge);
	int addLink(Node*, Node*);
  int addLink(int source_id, int destination_id);
	int addAgent(Agent * agent, Node * node);
	int addAgent(Node * src, char * agent_type, float cx, float cy);
  int addAgentLink(Agent*, Agent*);
  int addAgentLink(int source_id, int destination_id);
  int addTrafficSource(TrafficSource * traffic_source, Agent * agent);
  int addTrafficSource(Agent * agent, char * type,
                       float cx, float cy);
  
  TrafficSource * lookupTrafficSource(int id);

	int writeNsScript(FILE * file);
	int saveAsEnam(FILE * file);

	void setNodeProperty(int,char*,int);
  void setAgentProperty(int,char*,int);
	void setLinkProperty(int,int,char*,int);
  void setTrafficSourceProperty(int id, char * value, char * variable);
	void layout();

	void removeLink(Edge *);
	void removeNode(Node *);
	void removeAgent(Agent *a);
  void removeTrafficSource(TrafficSource * ts);

	inline EditorNetModel* wobj() { return wobj_; }
protected:
	EditorNetModel *wobj_;
	int command(int argc, const char*const* argv);


private:
	int node_id_count;
	double  pxmin_;
  double  pymin_;
	double  pxmax_;
  double  pymax_;

  TrafficSource * traffic_sources_;

};


#endif // nam_enetmodel_h
