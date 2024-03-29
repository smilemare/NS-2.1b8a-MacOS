/*
 * Copyright (c) 1991,1993 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the Computer Systems
 *	Engineering Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#) $Header: /nfs/jade/vint/CVSROOT/nam-1/node.cc,v 1.58 2001/05/25 02:26:38 mehringe Exp $ (LBL)
 */

#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <tcl.h>

#include "view.h"
#include "netview.h"
#include "psview.h"
#include "editview.h"
#include "node.h"
#include "queue.h"
#include "feature.h"
#include "agent.h"
#include "edge.h"
#include "route.h"
#include "monitor.h"
#include "lan.h"
#include "paint.h"
#include "trace.h"

//----------------------------------------------------------------------
//  Wrapper for tcl creation of Nodes
//----------------------------------------------------------------------
static class NodeClass : public TclClass {
public:
  NodeClass() : TclClass("Node") {} 
  TclObject * create(int argc, const char*const* argv) {
    Node * node;
    double size = strtod(argv[6], NULL);

    // set <node> [new Node node_id type size]
    if (!strcmp(argv[5], "circle")) {
      node = new CircleNode(argv[4], size);
    } else if (!strcmp(argv[5], "box")) {
      node = new BoxNode(argv[4], size);
    } else  {
      node = new HexagonNode(argv[4], size);
    }
 
    return node;
  }
} class_node;




//----------------------------------------------------------------------
//----------------------------------------------------------------------
Node::Node(const char* name, double size) :
  Animation(0, 0),
  queue_(0),
  size_(size),
  nsize_((float) size),
  x_(0.),
  y_(0.),
  x_vel_(0.),
  y_vel_(0.),
  starttime_(0.),
  endtime_(0.),
  links_(0),
  routes_(0),
  agents_(0),
  anchor_(0),
  mark_(0),
  state_(UP),
  nm_(NULL),
  nMark_(0),
  dlabel_(0),
//  lcolor_(0),
//  dcolor_(0),  // This is some type of hack to not use dcolor
//  ncolor_(0),
  direction_(0) {

  next_ = NULL;

  label_ = new char[strlen(name) + 1];
  strcpy(label_, name);
  addr_ = nn_ = atoi(name); /*XXX*/
        
  lcolor_ = NULL;
  dcolor_ = NULL; 
  ncolor_ = NULL;
  init_color("black");
  dx_ = 0.0;
  dy_ = 0.0;

  tcl_script_label_ = NULL;
  tcl_script_ = NULL;

  wireless_ = false;
  paint_ = Paint::instance()->thick();

  // OTcl variable bindings
  //bind("color", ncolor_);  // Color needs to be fixed
  //bind("
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
Node::~Node() {
	if (nm_ != NULL) {
		NodeMark *p = nm_;
		nm_ = nm_->next;
		delete p;
	}
	delete label_;
	if (queue_ != NULL)
		delete queue_;

  if (tcl_script_) {
    delete [] tcl_script_;
  }

  if (tcl_script_label_) {
    delete [] tcl_script_label_;
  }
}

//----------------------------------------------------------------------
// void 
// Node::init_color(const char *clr) {
//----------------------------------------------------------------------
void 
Node::init_color(const char *clr) {
  color(clr);
  lcolor(clr);
  ncolor(clr);
  dcolor(clr);
  oldPaint_ = paint_;
}

void Node::set_down(char *color)
{
	// If current color is down, don't change it again.
	// Assuming only one down color. User can change this behavior
	// by adding tcl code for link-up and link-down events.
	if (state_ == UP) {
		int pno = Paint::instance()->lookup(color, 3);
		oldPaint_ = paint_;
		paint_ = pno;
		state_ = DOWN;
	}
}

void Node::set_up()
{
	if (state_ == DOWN) {
		state_ = UP;
		toggle_color();
	}
}

float Node::distance(float x, float y) const
{
	return ((x_-x) * (x_-x) + (y_-y) * (y_-y));
}

void Node::size(double s)
{
	size_ = s;
	update_bb();
}

// We don't need one update_bb() for every derived class because their
// sizes are all based on 0.5*size_. Otherwise we'll have to do update_bb
// individually
void Node::update_bb()
{
	double off = 0.5 * size_ + NodeMarkScale * size_; // thick circles
	if (nMark_ > 0) 
		off += nMark_ * NodeMarkScale * size_;
	/*XXX*/
	bb_.xmin = x_ - off;
	bb_.ymin = y_ - off;
	bb_.xmax = x_ + off;
	bb_.ymax = y_ + off;
}

// Moved to Animation::inside(). Why not put a generic one based on BBox there?
//
//int Node::inside(double, float px, float py) const
//{
//	return (px >= bb_.xmin && px <= bb_.xmax &&
//		py >= bb_.ymin && py <= bb_.ymax);
//}

const char* Node::info() const
{
	static char text[128];
	sprintf(text, "Node: %s", label_);
	return (text);
}

//----------------------------------------------------------------------
// const char * 
// Node::property()
//----------------------------------------------------------------------
const char * 
Node::property() {
  static char text[256];
  char *p;
  rgb * color;   

  color = Paint::instance()->paint_to_rgb(paint_);

  
  // object type and id
  p = text;
  sprintf(text, "{\"Node %d\" nn_ title \"NODE %d\"} ", nn_, nn_);
  
  // node size
  p = &text[strlen(text)];
  sprintf(p, "{Size size_ text %.1f} ", size_);
  
  // node color 
  p = &text[strlen(text)];
  sprintf(p, "{Color COLOR color %s} ", color->colorname);

  // label if it exists
  p = &text[strlen(text)];
  if (dlabel_) {
   sprintf(p, "{Label dlabel_ text %s} ", dlabel_);
  } else {
   sprintf(p, "{Label dlabel_ text  } ");
  }
  return(text);
}

const char* Node::getname() const
{
	static char text[128];
	sprintf(text, "n %s", label_);
	return (text);
}

void Node::monitor(Monitor *m, double /*now*/, char *result, int /*len*/)
{
  monitor_=m;
  sprintf(result, "Node: %s", label_);
  return;
}

void Node::add_link(Edge* e)
{
	e->next_ = links_;
	links_ = e;
}

void Node::delete_link(Edge* e) 
{
	Edge *h, *k;
	h = links_;
	for (k = links_; k != 0; k = k->next_) {
	    if (k->src() == e->src() && k->dst() == e->dst()) {
		if (k == links_) {
		    links_ = k->next_;
		    break;
		} else {
		    h->next_ = k->next_;
		    break;
		}
	    }

	    h = k;
        }	
}


void Node::add_agent(Agent* a)
{
  a->next_ = agents_;
  agents_ = a;
}

void Node::delete_agent(Agent* r)
{
  /*given a agent, remove it from a node's agent list*/
  Agent *ta1, *ta2;
  ta1=agents_;
  ta2=agents_;
  while ((ta1!=r)&&(ta1!=NULL))
    {
      ta2=ta1;
      ta1=ta1->next();
    }
  if (ta1==r)
    {
      ta2->next(ta1->next());
      if (ta1==agents_)
	agents_=ta1->next();
    }
}

Agent *Node::find_agent(char *name) const
{
  Agent *ta=NULL;
  ta=agents_;
  while (ta!=NULL)
    {
      if (strcmp(ta->name(), name)==0)
	return ta;
      ta=ta->next();
    }
  return NULL;
}

void Node::add_route(Route* r)
{
  r->next_ = routes_;
  routes_ = r;
}

void Node::delete_route(Route* r)
{
  /*given a route, remove it from a node's route list*/
  Route *tr1, *tr2;
  tr1=routes_;
  tr2=routes_;
  while ((tr1!=r)&&(tr1!=NULL))
    {
      tr2=tr1;
      tr1=tr1->next();
    }
  if (tr1==r)
    {
      tr2->next(tr1->next());
      if (tr1==routes_)
	routes_=tr1->next();
    }
  if (routes_!=NULL)
    {
      /*need to reposition routes on this edge*/
      Edge *e=tr1->edge();
      int ctr=0;
      for (tr2=routes_;tr2!=NULL;tr2=tr2->next())
	if (tr2->edge()==e)
	  tr2->place(e->x0(), e->y0(), ctr++);
    }
}

Route *Node::find_route(Edge *e, int group, int pktsrc, int oif) const
{
  Route *tr=NULL;
  tr=routes_;
  while (tr!=NULL)
    {
      if (tr->matching_route(e, group, pktsrc, oif)==1)
	return tr;
      tr=tr->next();
    }
  return NULL;
}

void Node::place_route(Route *r)
{
	if (r->node() != this)
		return;
	if (r->marked() == 0) {
		r->place(r->edge()->x0(), r->edge()->y0());
		r->mark(1);
	}
}

// Used when topology is relayout
void Node::place_all_routes()
{
	Route *tr = routes_;
	while (tr != NULL) {
		place_route(tr);
		tr = tr->next();
	}
}

// Clear mark on routes so we can replace them
void Node::clear_routes()
{
	Route *tr = routes_;
	while (tr != NULL) {
		tr->mark(0);
		tr = tr->next();
	}
}

int Node::no_of_routes(Edge *e) const
{
  Route *tr=routes_;
  int no_of_routes=0;
  while (tr!=NULL)
    {
      if (tr->edge()==e)
	no_of_routes++;
      tr=tr->next();
    }
  return no_of_routes;
 }

void
Node::label(const char* name, int anchor) {
	delete []label_;
	label_ = new char[strlen(name) + 1];
	strcpy(label_, name);
	anchor_ = anchor;
}

void Node::lcolor(const char* name)
{
	if (name[0] == 0) {
		if (lcolor_) {
			delete []lcolor_;
			lcolor_ = 0;
		}
		return;
	}
  
	if (lcolor_)
		delete []lcolor_;
	lcolor_ = new char[strlen(name) + 1];
	strcpy(lcolor_, name);
}

void Node::dlabel(const char* name)
{
	if (name[0] == 0) {
		if (dlabel_) {
			delete []dlabel_;
			dlabel_ = 0;
		}
		return;
	}
    
	if (dlabel_)
		delete []dlabel_;
	dlabel_ = new char[strlen(name) + 1];
	strcpy(dlabel_, name);
}

//----------------------------------------------------------------------
// void
// Node::dcolor(const char* name)
//----------------------------------------------------------------------
void Node::dcolor(const char* name) {
  if (name[0] == 0) {
    if (dcolor_) {
      delete []dcolor_;
      dcolor_ = 0;
    }
    return;
  }

  if (dcolor_)
    delete []dcolor_;
  dcolor_ = new char[strlen(name) + 1];
  strcpy(dcolor_, name);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
void
Node::ncolor(const char* name) {
  if (name[0] == 0) {
    if (ncolor_) {
      delete []ncolor_;
      ncolor_ = 0;
    }
    return;
  }

  if (ncolor_)
    delete []ncolor_;

  ncolor_ = new char[strlen(name) + 1];
  strcpy(ncolor_, name);
}

void Node::direction(const char* name)
{
        if (name[0] == 0) {
                if (direction_) {
                        direction_ = 0;
                }
                return;
        }
        if (!strcmp(name, "EAST"))
               direction_ = 1;
        else if (!strcmp(name, "SOUTH"))
               direction_ = 2;
        else if (!strcmp(name, "WEST"))
               direction_ = 3;
        else if (!strcmp(name, "NORTH"))
               direction_ = 4;
        else
               direction_ = 0;
}


void Node::add_sess_queue(unsigned int, Queue *q)
{
	if (queue_ != NULL)
		// Currently we only allow one queue per node
		return;
	queue_ = q;
	queue_->place(size_*0.5, x_+size_*.05, y_+size_*0.5);
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
char * Node::getTclScript()  {
  return tcl_script_;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
char * Node::getTclScriptLabel()  {
  return tcl_script_label_;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
void Node::setTclScript(const char * label, const char * script) {
  if (tcl_script_label_) {
    delete [] tcl_script_label_;
  }
  tcl_script_label_ = new char[strlen(label)];
  strcpy(tcl_script_label_, label);

  if (tcl_script_) {
    delete [] tcl_script_;
  }
  tcl_script_ = new char[strlen(script)];
  strcpy(tcl_script_, script);
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
NodeMark* Node::find_mark(char *name) {
	NodeMark *p = nm_;
	while (p != NULL) {
		if (strcmp(p->name, name) == 0)
			break;
		p = p->next;
	}
	return p;
}

int Node::add_mark(char *name, char *color, char *shape)
{
	if (find_mark(name) != NULL)
		return 1;

	NodeMark *cm = new NodeMark(name);
	cm->color = Paint::instance()->lookup(color, 2);
	if (strcmp(shape, "circle") == 0) 
		cm->shape = NodeMarkCircle;
	else if (strcmp(shape, "square") == 0)
		cm->shape = NodeMarkSquare;
	else if (strcmp(shape, "hexagon") == 0)
		cm->shape = NodeMarkHexagon;

	if (nm_ != NULL)
		cm->next = nm_;
	nm_ = cm;
	nMark_++;
	update_bb();
	return 0;
}

void Node::delete_mark(char *name)
{
	NodeMark **p;
	p = &nm_;
	while (*p != NULL) {
		if (strcmp((*p)->name, name) == 0) {
			NodeMark *q = *p;
			*p = (*p)->next;
			delete q;
			nMark_--;
			update_bb();
			return;
		}
		p = &((*p)->next);
	}
}

void Node::update(double now)
{
    if (now >= endtime_ || now <= starttime_ ) {return;}

    double xpos = xorig_ + (now - starttime_)*x_vel_;
    double ypos = yorig_ + (now - starttime_)*y_vel_;

    place(xpos,ypos);

}


void Node::draw_mark(View* nv) const
{
	NodeMark *cm;
	double s = size_ * 0.7;
	for (cm = nm_; cm != NULL; cm = cm->next, s += size_ * NodeMarkScale) {
		switch (cm->shape) {
		case NodeMarkCircle:
			nv->circle(x_, y_, s, cm->color);
			break;
		case NodeMarkSquare: {
			double x[2], y[2];
			x[0] = x_ - s, x[1] = x_ + s;
			y[0] = y_ - s, y[1] = y_ + s;
			nv->rect(x[0], y[0], x[1], y[1], cm->color);
			break;
		}
		case NodeMarkHexagon: {
			float x[6], y[6];
			double qd = 0.5 * s;
			x[0] = x_ - s;  y[0] = y_;
			x[1] = x_ - qd; y[1] = y_ + s;
			x[2] = x_ + qd; y[2] = y_ + s;
			x[3] = x_ + s;  y[3] = y_;
			x[4] = x_ + qd; y[4] = y_ - s;
			x[5] = x_ - qd; y[5] = y_ - s;
			nv->polygon((float *)x, (float *)y, 6, cm->color);
			break;
		}
		}
	}
}

void Node::drawlabel(View* nv) const
{
	/*XXX node number */
	if (label_ != 0) 
              nv->string(x_, y_, size_, label_, anchor_, lcolor_);
	if (monitor_!=NULL)
		monitor_->draw(nv, x_, y_-size_/2);
	if (dlabel_ != 0) {
                switch (direction_) {
                case 0:
		    nv->string(x_, y_+size_, size_*0.7, dlabel_, 0, dcolor_);
                    break;
                case 1:
		    nv->string(x_-size_*1.5, y_, size_*0.7, dlabel_, 0, 
			       dcolor_);
                    break;
                case 2:
		    nv->string(x_, y_-size_, size_*0.7, dlabel_, 0, dcolor_);
                    break;
                case 3:
		    nv->string(x_+size_*1.5, y_, size_*0.7, dlabel_, 0, 
			       dcolor_);
                    break;
                case 4:
		    nv->string(x_,           y_+size_,   size_*0.7, dlabel_, 0,
			       dcolor_);
                    break;
                default:
		    nv->string(x_,           y_+size_,   size_*0.7, dlabel_, 0,
			       dcolor_);
                    break;
                }
        }
}

void Node::reset(double)
{
	//XXX why should reset to black??? 
	//	paint_ = Paint::instance()->thick();
}

void Node::place(double x, double y)
{ 
	x_ = x;
	y_ = y;
	mark_ = 1;
	update_bb();

	// Should we place queues here too? 
	if (queue_ != NULL) 
		queue_->place(0.5 * size_, x_+0.5*size_, y_+0.5*size_);
}

// Move by a displacement in *window* coordinates
void Node::move(EditView *v, float wdx, float wdy)
{
	float cx, cy;

	// First get our position in window coordinates
	cx = x_, cy = y_;
	v->map(cx, cy);
	cx += wdx, cy += wdy;
	v->imap(cx, cy);
	place(cx, cy);

	// Because all placements are centrally organized by AutoNetModel, 
	// we'll have to call back. :(
	v->moveNode(this);
}

Edge *Node::find_edge(int dst) const 
{
	for (Edge *e = links_; e != 0; e = e->next_) 
		if (e->dst() == dst)
			return e;
	return NULL;
}

int Node::save(FILE *file)
{
	char buffer[TRACE_LINE_MAXLEN], *p;
	rgb *color;
	color=Paint::instance()->paint_to_rgb(paint_);

	buffer[0] = 0;
	if (state_ == UP)
		strcpy(buffer, "-S UP");
	else if (state_==DOWN)
		strcpy(buffer, "-S DOWN");
	p = buffer + strlen(buffer);
	if (dlabel_)
		sprintf(p, " -b \"%s\"", dlabel_);
	fprintf(file, "n -t * -s %d -v %s -c %s -z %f %s\n", 
		nn_, style(), color->colorname, size_, buffer);
	return(0);
}

//----------------------------------------------------------------------
// int
// Node::writeNsScript(FILE *file)
//----------------------------------------------------------------------
int
Node::writeNsScript(FILE *file) {
  rgb * color;
  color = Paint::instance()->paint_to_rgb(paint_);

  fprintf(file, "set node(%d) [$ns node]\n", nn_);
  fprintf(file, "## node(%d) at %f,%f\n", nn_, x_, y_);
//  fprintf(file, "$node(%d) size \"%f\"\n", nn_, size_);
  fprintf(file, "$node(%d) color \"%s\"\n", nn_, color->colorname);
  if (dlabel_) {
    fprintf(file, "$ns at 0.0 \"$node(%d) label %s\"\n",nn_, dlabel_);
  }
  return 0;
}


//----------------------------------------------------------------------
// int
// Node::saveAsEnam(FILE *file) {
//----------------------------------------------------------------------
int
Node::saveAsEnam(FILE *file) {
  char state[10];
  rgb *color;
  color=Paint::instance()->paint_to_rgb(paint_);
  
  if (state_ == UP) {
    strcpy(state, " -S UP");
  } else if (state_ == DOWN) {
    strcpy(state, " -S DOWN");
  } else {
    state[0]='\0';
  }
  fprintf(file, "##n -t * -s %d -v %s -c %s -i %s -b %s -z %f -x %f -y %f %s\n",
                nn_, style(), color->colorname, color->colorname,
                dlabel_, size_, x_, y_, state);
  
  return(0);
}


BoxNode::BoxNode(const char* name, double size) : Node(name, size)
{
	BoxNode::size(size);
}

void BoxNode::size(double s)
{
	Node::size(s);
	double delta = 0.5 * s;
	x0_ = x_ - delta;
	y0_ = y_ - delta;
	x1_ = x_ + delta;
	y1_ = y_ + delta;
}

void BoxNode::draw(View* nv, double now) {
	nv->rect(x0_, y0_, x1_, y1_, paint_);
	drawlabel(nv);
	draw_mark(nv);
}

void BoxNode::place(double x, double y)
{
	Node::place(x, y);
	double delta = 0.5 * size_;
	x0_ = x_ - delta;
	y0_ = y_ - delta;
	x1_ = x_ + delta;
	y1_ = y_ + delta;
}

CircleNode::CircleNode(const char* name, double size) : Node(name, size)
{
	CircleNode::size(size);
}

void CircleNode::size(double s)
{
	Node::size(s);
	radius_ = 0.5 * s;
}

void 
CircleNode::draw(View* nv, double now) {
	nv->circle(x_, y_, radius_, paint_);
	drawlabel(nv);
	draw_mark(nv);
}

HexagonNode::HexagonNode(const char* name, double size) : Node(name, size)
{
	HexagonNode::size(size);
}

void HexagonNode::size(double s)
{
	Node::size(s);
	double hd = 0.5 * s;
	double qd = 0.5 * hd;

	xpoly_[0] = x_ - hd; ypoly_[0] = y_;
	xpoly_[1] = x_ - qd; ypoly_[1] = y_ + hd;
	xpoly_[2] = x_ + qd; ypoly_[2] = y_ + hd;
	xpoly_[3] = x_ + hd; ypoly_[3] = y_;
	xpoly_[4] = x_ + qd; ypoly_[4] = y_ - hd;
	xpoly_[5] = x_ - qd; ypoly_[5] = y_ - hd;
}

void HexagonNode::draw(View* nv, double now) {
	nv->polygon(xpoly_, ypoly_, 6, paint_);
	drawlabel(nv);
	draw_mark(nv);
}

void HexagonNode::place(double x, double y)
{
	Node::place(x, y);

	double hd = 0.5 * size_;
	double qd = 0.5 * hd;
	xpoly_[0] = x_ - hd; ypoly_[0] = y_;
	xpoly_[1] = x_ - qd; ypoly_[1] = y_ + hd;
	xpoly_[2] = x_ + qd; ypoly_[2] = y_ + hd;
	xpoly_[3] = x_ + hd; ypoly_[3] = y_;
	xpoly_[4] = x_ + qd; ypoly_[4] = y_ - hd;
	xpoly_[5] = x_ - qd; ypoly_[5] = y_ - hd;
}


VirtualNode::VirtualNode(const char* name, Lan *lan) : 
  Node(name, 0), lan_(lan)
{
}

void VirtualNode::size(double s)
{
	Node::size(0);
	lan_->size(s);
}

void VirtualNode::draw(View* nv, double now) {
  printf("drawing vn at %f %f\n", x_, y_);
}

void VirtualNode::place(double x, double y)
{
	Node::place(x, y);
	lan_->place(x, y);
}

void VirtualNode::arrive_packet(Packet *p, Edge *e, double atime)
{
        lan_->arrive_packet(p,e,atime);
}

void VirtualNode::delete_packet(Packet *p)
{
        lan_->delete_packet(p);
}

double VirtualNode::x(Edge *e) const
{
  return lan_->x(e);
}

double VirtualNode::y(Edge *e) const
{
  return lan_->y(e);
}
