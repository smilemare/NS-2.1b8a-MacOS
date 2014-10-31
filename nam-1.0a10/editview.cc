/*
 * Copyright (c) 1997 by the University of Southern California
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation in source and binary forms for non-commercial purposes
 * and without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both the copyright notice and
 * this permission notice appear in supporting documentation. and that
 * any documentation, advertising materials, and other materials related
 * to such distribution and use acknowledge that the software was
 * developed by the University of Southern California, Information
 * Sciences Institute.  The name of the University may not be used to
 * endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THE UNIVERSITY OF SOUTHERN CALIFORNIA makes no representations about
 * the suitability of this software for any purpose.  THIS SOFTWARE IS
 * PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Other copyrights might apply to parts of this software and are so
 * noted when applicable.
 *
 * $Header: /nfs/jade/vint/CVSROOT/nam-1/editview.cc,v 1.26 2001/05/25 02:26:38 mehringe Exp $
 */

#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif

#include <ctype.h>
#include <math.h>

#include "view.h"
#include "bbox.h"
#include "netmodel.h"
#include "editview.h"
#include "tclcl.h"
#include "paint.h"
#include "tag.h"
#include "node.h"
#include "edge.h"
#include "agent.h"

void EditView::DeleteCmdProc(ClientData cd)
{
	EditView *ev = (EditView *)cd;
	if (ev->tk_ != NULL) {
		Tk_DestroyWindow(ev->tk_);
	}
}

EditView::EditView(const char *name, NetModel* m) 
        : NetView(name), defTag_(NULL)
{
	if (tk_!=0) {
		Tcl& tcl = Tcl::instance();
		cmd_ = Tcl_CreateCommand(tcl.interp(), Tk_PathName(tk_), 
					 command, 
					 (ClientData)this, DeleteCmdProc);
	}
	char str[256];
	model_ = m;
	sprintf(str, "def%-u", (long)this);
	defTag_ = new Tag(str);
	model_->add_tag(defTag_);
	objType_ = NONE;
}

//----------------------------------------------------------------------
// EditView::EditView(const char *name, NetModel* m,
//                    int width, int height)
//----------------------------------------------------------------------
EditView::EditView(const char *name, NetModel* m, int width, int height)
  : NetView(name, SQUARE, width, height), defTag_(NULL) {

  if (tk_!=0) {
    Tcl& tcl = Tcl::instance();
    cmd_ = Tcl_CreateCommand(tcl.interp(), Tk_PathName(tk_),
                             command, (ClientData) this, DeleteCmdProc);
  }
  char str[256];
  model_ = m;
  sprintf(str, "def%-u", (long)this);
  defTag_ = new Tag(name);
  model_->add_tag(defTag_);
  objType_ = NONE;
}

EditView::~EditView()
{
	model_->remove_view(this);
	if (defTag_ != NULL) {
		model_->delete_tag(defTag_->name());
		delete defTag_;
		defTag_ = NULL;
	}
	// Delete Tcl command created
//  	Tcl& tcl = Tcl::instance();
//  	Tcl_DeleteCommandFromToken(tcl.interp(), cmd_);
}

//----------------------------------------------------------------------
// int 
// EditView::command(ClientData cd, Tcl_Interp* tcl,
//                   int argc, char **argv)
//    Parse Tcl command interface
//      - setPoint, addNode, addLink addAgent, deleteObject, etc...
//----------------------------------------------------------------------
int EditView::command(ClientData cd, Tcl_Interp* tcl,
                      int argc, char **argv) {
  EditorNetModel * editor_net_model;
  Node * node, * source, * destination;
  Edge * edge;
  Agent * source_agent, * destination_agent;
  TrafficSource * traffic_source;
  double world_x, world_y;

  
  if (argc < 2) {
    Tcl_AppendResult(tcl, "\"", argv[0], "\": arg mismatch", 0);
    return (TCL_ERROR);
  }

  char c = argv[1][0];
  int length = strlen(argv[1]);
  EditView * ev = (EditView *) cd;
  editor_net_model = (EditorNetModel *) ev->model_;

  float cx, cy;
  int flag;

  // Following implements several useful interactive commands of 
  // Tk's canvas. Their syntax and semantics are kept as close to
  // those of Tk canvas as possible.

  if ((c == 'd') && (strncmp(argv[1], "draw", length) == 0)) {
    // Redraw seen at time = argv[2]
    ev->draw(strtod(argv[2],NULL));
    return TCL_OK;

  } else if ((c == 'c') && (strncmp(argv[1], "close", length) == 0)) {
    ev->destroy();
    return TCL_OK;

  } else if ((c == 'd') &&
             (strncmp(argv[1], "dctag", length) == 0)) {
    /*
     * <view> dctag
     * 
     * Delete the current selection, start all over.
     */
    if (ev->defTag_ != NULL)
      ev->defTag_->remove();
    ev->draw();
    return (TCL_OK);

  } else if ((c == 's') &&
             (strncmp(argv[1], "setPoint", length) == 0)) {
    /*
     * <view> setPoint x y addFlag
     * 
     * Select the object (if any) under point (x, y). If no such
     * object, set current point to (x, y)
     */
    if (argc != 5) {
      Tcl_AppendResult(tcl, "wrong # args: should be \"",
                            argv[0], " setPoint x y addFlag\"", 
                            (char *)NULL);
      return (TCL_ERROR);
    }
    cx = strtod(argv[2], NULL);
    cy = strtod(argv[3], NULL);
    flag = strtol(argv[4], NULL, 10);
    return ev->cmdSetPoint(cx, cy, flag);

  } else if ((c == 'd') && (strncmp(argv[1], "deleteObject", length) == 0)) { 
    cx = strtod(argv[2], NULL);
    cy = strtod(argv[3], NULL);
    return  ev->cmdDeleteObj(cx, cy);

  } else if ((c == 'm') && (strncmp(argv[1], "moveTo", length) == 0)) { 
    /*
     * <view> moveto x y
     * 
     * Move the current selected object (if any) or the 
     * rubber band to point (x, y)
     */
    if (argc != 4) {
      Tcl_AppendResult(tcl, "wrong # args: should be \"",
                             argv[0],
                            " moveTo x y\"", (char *)NULL);
      return (TCL_ERROR);
    }
    cx = strtod(argv[2], NULL);
    cy = strtod(argv[3], NULL);
    return ev->cmdMoveTo(cx, cy);

  } else if ((c == 'r') && (strncmp(argv[1], "relPoint", length) == 0)) {
    /*
     * <view> relPoint x y
     * 
     * If any object is selected, set the object's
     * position to point (x,y); otherwise set the rubber
     * band rectangle and select all the objects in that
     * rectangle Note: we need a default tag for the
     * selection in rubber band.  
     */
    if (argc != 4) {
      Tcl_AppendResult(tcl, "wrong # args: should be \"",
           argv[0],
           " relPoint x y\"", (char *)NULL);
      return (TCL_ERROR);
    }
    cx = strtod(argv[2], NULL);
    cy = strtod(argv[3], NULL);
    return ev->cmdReleasePoint(cx, cy);
  } else if ((c == 's') && (strncmp(argv[1], "setNodeProperty", length) == 0)) {
    // <view> setNodeProperty nodeid nodepropertyvalue properyname
    int nodeid = atoi(argv[2]);
    int propertyname = atoi(argv[4]);
    return ev->cmdsetNodeProperty(nodeid, argv[3], propertyname);

  } else if ((c == 's') && (strncmp(argv[1], "setAgentProperty", length) == 0)) {
     // <view> setNodeProperty nodeid nodepropertyvalue properyname
    int agentid = atoi(argv[2]);
    int propertyname = atoi(argv[4]);
    return ev->cmdsetAgentProperty(agentid, argv[3], propertyname);

  } else if ((c == 's') && (strncmp(argv[1], "setLinkProperty", length) == 0)) {
    // <view> setLinkProperty sid did propertyvalue properyname
    int source_id = atoi(argv[2]);
    int destination_id = atoi(argv[3]);
    int propertyname = atoi(argv[5]);
    return ev->cmdsetLinkProperty(source_id, destination_id,
                                  argv[4], propertyname);

  } else if ((c == 's') &&
             (strncmp(argv[1], "setTrafficSourceProperty", length) == 0)) {
    // <view> setTrafficSourceProperty id propertyvalue properyname
    int id = atoi(argv[2]);
    char * value = argv[3];
    char * variable_name = argv[4];
    return ev->cmdsetTrafficSourceProperty(id, value, variable_name);
     
  } else if ((c == 'a') && (strncmp(argv[1], "addNode", length) == 0)) {
    // <view> addNode x y
    //   - add a new Node under current (x,y) with default size
    cx = strtod(argv[2], NULL);
    cy = strtod(argv[3], NULL);
    return ev->cmdaddNode(cx, cy);

  } else if ((c == 'i') && (strncmp(argv[1], "importNode", length) == 0)) {
    // Used when reading a nam editor file to add a node created in tcl
    // to this editview and its editornetmodel

    node = (Node *) TclObject::lookup(argv[2]);
    world_x = atof(argv[3]);
    world_y = atof(argv[4]);

    if (node) {
      if (editor_net_model->addNode(node)) {
        node->place(world_x, world_y);
        ev->draw();
        return TCL_OK;
      }
    }

    fprintf(stderr, "Error creating node.\n");
    return TCL_ERROR;

  } else if ((c == 'a') && (strncmp(argv[1], "attachNodes", length) == 0)) {
    // <view> linkNodes source_id destination_id
    //   - link 2 existing nodes to each other

    edge = (Edge *) TclObject::lookup(argv[2]);
    source = (Node *) TclObject::lookup(argv[3]);
    destination = (Node *) TclObject::lookup(argv[4]);
    edge->attachNodes(source, destination);
    editor_net_model->addEdge(edge);

    //editor_net_model->addLink(source_id, destination_id);
    ev->draw();  
    return TCL_OK;

  } else if ((c == 'a') && (strncmp(argv[1], "addLink", length) == 0)) {
    //  <view> addLink x y 
    //    - add a new Link if its start AND end point is inside of 
    cx = strtod(argv[2], NULL);
    cy = strtod(argv[3], NULL);
    return ev->cmdaddLink(cx, cy);

  } else if ((c == 'a') && (strncmp(argv[1], "addAgent", length) == 0)) {
    // <view> addAgent x y agent
    //   - add a new agent
    cx = strtod(argv[2], NULL);
    cy = strtod(argv[3], NULL);
    char * agent_name = argv[4];
    return ev->cmdaddAgent(cx, cy, agent_name);

  } else if ((c == 'i') && (strncmp(argv[1], "importAgent", length) == 0)) {
    source_agent = (Agent *) TclObject::lookup(argv[2]);
    node = (Node *) TclObject::lookup(argv[3]);
    return editor_net_model->addAgent(source_agent, node);

  } else if ((c == 'l') && (strncmp(argv[1], "linkAgents", length) == 0)) {
    // <view> linkAgents source_id destination_id
    //   - link 2 existing agents to each other
    source_agent = (Agent *) TclObject::lookup(argv[2]);
    destination_agent = (Agent *) TclObject::lookup(argv[3]);
    return editor_net_model->addAgentLink(source_agent, destination_agent);

  } else if ((c == 'a') &&
             (strncmp(argv[1], "addTrafficSource", length) == 0)) {
     // <view> addTrafficSource x y agent
     //   -  add a new TrafficSource
    cx = strtod(argv[2], NULL);
    cy = strtod(argv[3], NULL);
    // argv[4] is the name of the traffic source
    return ev->cmdaddTrafficSource(cx, cy, argv[4]);

  } else if ((c == 'a') &&
             (strncmp(argv[1], "attachTrafficSource", length) == 0)) {
    traffic_source = (TrafficSource *) TclObject::lookup(argv[2]);
    source_agent = (Agent *) TclObject::lookup(argv[3]);
    return editor_net_model->addTrafficSource(traffic_source, source_agent);

  } else if ((c == 'g') && (strncmp(argv[1], "getObject", length) == 0)) {
                /*
                 * <view> getObject x y 
                 *
                 *  return obj under current point
                 */
                cx = strtod(argv[2], NULL);
                cy = strtod(argv[3], NULL);
                return ev->cmdgetCurrentObj(cx, cy);
  } else if ((c == 'g') && (strncmp(argv[1], "getObjectProperty", length) == 0)) {
                /*
                 * <view> getObjectPropert x y 
                 *
     *  return obj property under current point
                 */
                cx = strtod(argv[2], NULL);
                cy = strtod(argv[3], NULL);
                return ev->cmdgetObjProperty(cx, cy);
  } else if ((c == 'v') && (strncmp(argv[1], "view_mode", length)==0)) {
    /*
     * <view> view_mode
     * 
     * clear defTag_ and change to view mode
     */
    ev->view_mode();
    return TCL_OK;
  }

  return (NetView::command(cd, tcl, argc, argv));
}

int EditView::cmdsetNodeProperty(int id, char *pv, int pn)
{
	EditorNetModel* emodel_ = (EditorNetModel *)model_;	
  // goto enetmodel.cc
	emodel_->setNodeProperty(id,pv,pn);

	EditType oldt_;
	oldt_ = objType_;
        objType_ = END_OBJECT;
        draw();
        objType_ = NONE;
        model_->update(model_->now());
        objType_ = oldt_;

	return(TCL_OK);
}

int EditView::cmdsetAgentProperty(int id, char *pv, int pn)
{
        EditorNetModel* emodel_ = (EditorNetModel *)model_;
	emodel_->setAgentProperty(id,pv,pn);
	draw();  
	return(TCL_OK);
}


int EditView::cmdsetLinkProperty(int sid, int did, char *pv, int pn) {
  EditorNetModel* emodel_ = (EditorNetModel *)model_;
  emodel_->setLinkProperty(sid,did, pv,pn);

  draw();

  return(TCL_OK);
}

//----------------------------------------------------------------------
// int
// EditView::cmdsetTrafficSourceProperty(int sid, int did, char *pv, int pn)
//----------------------------------------------------------------------
int
EditView::cmdsetTrafficSourceProperty(int id, char * value, char * variable) {
  EditorNetModel* emodel_ = (EditorNetModel *) model_;
  emodel_->setTrafficSourceProperty(id, value, variable);
  draw();
  return(TCL_OK);
}        

//----------------------------------------------------------------------
// int 
// EditView::cmdgetObjProperty(float cx, float cy)
//   - get properties for a selected object
//----------------------------------------------------------------------
int 
EditView::cmdgetObjProperty(float cx, float cy) { 
	Animation *p;
	Tcl& tcl = Tcl::instance();

  // matrix_ comes from class View (view.h)
  //  - imap is the inverse mapping
  //    (i.e from screen to world coordinate systems)
	matrix_.imap(cx, cy);

  // Finds the object which contains the coordinate (cx, cy)
	p = model_->inside(cx, cy);
	
	if (p == NULL) {
	   tcl.resultf("NONE");
	} else {
	   tcl.resultf("%s",p->property());
 	}
	return(TCL_OK);	
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
int EditView::cmdgetCurrentObj(float cx, float cy)
{ 
	Tcl& tcl = Tcl::instance();
	matrix_.imap(cx, cy);
	Animation *p = model_->inside(cx, cy);
	if (p == NULL) {
	   tcl.resultf("NONE");
	} else {
	   tcl.resultf("%s",p->info());
 	}
	return(TCL_OK);	

}


//----------------------------------------------------------------------
// int
// EditView::cmdaddLink(float cx, float cy)
//   - Note: all cx_ and cy_ below are in *window* coordinates
//----------------------------------------------------------------------
int
EditView::cmdaddLink(float cx, float cy) {
  Animation * p;
  static Animation * old_p;
  EditorNetModel* emodel_ = (EditorNetModel *) model_;

  cx_ = cx;
  cy_ = cy;
  matrix_.imap(cx, cy);

  // Do we have a node  on current point ?
  p = model_->inside(cx, cy);

  if (p == NULL) {
    defTag_->remove();
    if (objType_ == START_LINK) {
       objType_ = NONE;
      draw();
    }
    return (TCL_OK);
  }

  if (p->classid() == ClassNodeID) {
    if (objType_ != START_LINK) {
      old_p = p;                /* remember the orig node */
      startSetObject(p, cx_, cy_);
      objType_ = START_LINK;
     } else if ((old_p == p)||(old_p->classid()!=ClassNodeID)) {
       objType_ = NONE;
       draw();
       // to support making link by click-&-click 
       objType_ = START_LINK; 
     } else {
       // add a new link b/w the two nodes
       startSetObject(p, cx_, cy_);

       Node *src, *dst;
       src = (Node *)old_p;
       dst = (Node *)p;
       emodel_->addLink(src, dst);
       objType_ = END_OBJECT;
     
       // Erase old positions
       defTag_->draw(this, model_->now());
       // At least we should redo scale estimation and
       // place everything
       defTag_->move(this, rb_.xmax - oldx_, rb_.ymax - oldy_);
       model_->recalc();
       model_->render(this);
       defTag_->remove();
       objType_ = NONE;
       draw();
       objType_ = NONE;
     }

  } else if (p->classid() == ClassAgentID) {
    if (objType_ != START_LINK) {
      old_p = p;
      startSetObject(p, cx_, cy_);
      objType_ = START_LINK;
    } else if ((old_p == p)||(old_p->classid()!= ClassAgentID)) {
      objType_ = NONE;
      draw();
      // to support making agent-link by click-&-click
      objType_ = START_LINK;
    } else {
      startSetObject(p, cx_, cy_);

      Agent *src_agent, *dst_agent;
      src_agent = (Agent *)old_p;
      dst_agent = (Agent *)p;
      emodel_->addAgentLink(src_agent, dst_agent);
      objType_ = END_OBJECT;

      defTag_->draw(this, model_->now());
      defTag_->move(this, rb_.xmax - oldx_, rb_.ymax - oldy_);
      model_->recalc();
      model_->render(this);
      defTag_->remove();
      objType_ = NONE;
      draw();
    }
  } else {
    objType_ = NONE;
  }
  return(TCL_OK);
}


//----------------------------------------------------------------------
// int 
// EditView::cmdaddAgent(float cx, float cy, char* agent)
//----------------------------------------------------------------------
int EditView::cmdaddAgent(float cx, float cy, char* agent) {
  Animation * p;
  Node * src;
  EditorNetModel* emodel_ = (EditorNetModel *)model_;

  // Switch from screen coordinates to world coordinates
  matrix_.imap(cx, cy);

  // Do we have a node on the clicked point ?
  p = model_->inside(cx, cy);
  if (p == NULL) {
    defTag_->remove();
    return (TCL_OK);
  } 

  if (p->classid() == ClassNodeID) {
    // Yes, we have a node to which we can add the agent
    src = (Node *) p;
    // goto EditorNetModel::addAgent(...) in enetmodel.cc
    emodel_->addAgent(src, agent, cx, cy);
    draw();
  } else {
    return (TCL_OK);
  }
  return(TCL_OK);
}


//----------------------------------------------------------------------
// int 
// EditView::cmdaddNode(float cx, float cy)
//   Note: all cx_ and cy_ below are in *window* coordinates
//----------------------------------------------------------------------
int EditView::cmdaddNode(float cx, float cy) {
  matrix_.imap(cx, cy);
  EditorNetModel* emodel_ = (EditorNetModel *)model_;

  emodel_->addNode(cx, cy);		
  draw();
  return(TCL_OK);
}

//----------------------------------------------------------------------
// int
// EditView::cmdDeleteObj(float cx, float cy)
//----------------------------------------------------------------------
int
EditView::cmdDeleteObj(float cx, float cy) {
  Animation * p;  // I don't know why p was chosen for the varible name
                  // Just to confuse other programmers, I guess
  
  // First of all, clear the old point 
  cx_ = cx;
  cy_ = cy;
                
  // Do we have an animation object on the clicked point?
  matrix_.imap(cx, cy);
  p = model_->inside(cx, cy);
  if (p == NULL) {
    defTag_->remove();
    return (TCL_OK);
  }       
  
  // If object is already selected unselect it
  if (p->isTagged()) {
    if (p->numTag() > 1) {
      fprintf(stderr, "Error: More than one tag for an object %d!\n",
                       p->id());
      p = NULL;
    } else {
      p = p->getLastTag();
    }
  }

  // Only nodes, links, agents, and traffic sources can be deleted
  if ((p->classid() != ClassNodeID) &&
      (p->classid() != ClassEdgeID) &&
      (p->classid() != ClassAgentID) &&
      (p->classid() != ClassTrafficSourceID)) {
    p = NULL;
  }

  if (p == NULL) {
    defTag_->remove();
  } else {
    if (p->classid() == ClassNodeID) {
      Node * n = (Node *) p;
      EditorNetModel * emodel_ = (EditorNetModel *) model_;
      emodel_->removeNode(n);
      draw();
    } 
    if (p->classid() == ClassEdgeID) {
      Edge * e = (Edge *) p;
      EditorNetModel * emodel_ = (EditorNetModel *) model_;
      emodel_->removeLink(e);
      draw();
    } 
    if (p->classid() == ClassAgentID) {
      Agent * a = (Agent *) p;
      EditorNetModel* emodel_ = (EditorNetModel *) model_;
      emodel_->removeAgent(a);
      draw();
    }
    if (p->classid() == ClassTrafficSourceID) {
      TrafficSource * ts = (TrafficSource *) p;
      EditorNetModel* emodel_ = (EditorNetModel *) model_;
      emodel_->removeTrafficSource(ts);
      draw();
    }

  }

  return (TCL_OK);
}

//----------------------------------------------------------------------
// int 
// EditView::cmdaddTrafficSource(float cx, float cy, char * type)
//----------------------------------------------------------------------
int
EditView::cmdaddTrafficSource(float cx, float cy, char * type) {
  Animation * object;
  Agent * agent;
  EditorNetModel* emodel_ = (EditorNetModel *)model_;

  // Switch from screen coordinates to world coordinates
  matrix_.imap(cx, cy);

  // Do we have an agent on the clicked point ?
  object = model_->inside(cx, cy);
  if (object == NULL) {
    defTag_->remove();
    return (TCL_OK);
  } 

  if (object->classid() == ClassAgentID) {
    // Yes, we have a node to which we can add the agent
    agent = (Agent *) object;
    // goto EditorNetModel::addTrafficSource(...) in enetmodel.cc
    emodel_->addTrafficSource(agent, type, cx, cy);
    draw();
  } else {
    return (TCL_OK);
  }
  return(TCL_OK);
}

// ---------------------------------------------------------------------
// int 
// EditView::cmdSetPoint(float cx, float cy, int bAdd)
//   - Note: all cx_ and cy_ below are in *window* coordinates
// ---------------------------------------------------------------------
int 
EditView::cmdSetPoint(float cx, float cy, int bAdd) {
  // First of all, clean the old group
  cx_ = cx, cy_ = cy;

  // Inverse Map cx, cy to world coordinates
  matrix_.imap(cx, cy);

  // Do we have anything on current point?
  Animation *p = model_->inside(cx, cy);

  // If nothing at this point start selection rubberband
  if (p == NULL) {
    defTag_->remove();
    startRubberBand(cx_, cy_);
    return (TCL_OK);
  }

  // Otherwise look to move object if it is a node or a Tag
  // -- what is a tag?
  if (p->isTagged()) {
    if (p->numTag() > 1) {
      fprintf(stderr, "Error: More than one tag forobject %d!\n",
                       p->id());
      p = NULL;
    } else {
      p = p->getLastTag();
    }
  }
  
  // Only nodes and tags can be moved
  if ((p->classid() != ClassNodeID) &&
      (p->classid() != ClassTagID)) {
    p = NULL;
  }

  if (p == NULL) {
    defTag_->remove();
    startRubberBand(cx_, cy_);
  } else {
    // If a single non-tag object is selected, or explicitly 
    // instructed, remove the previous selection
    if (!bAdd && (p != defTag_)) {
      defTag_->remove();
    }
    // Just tags the object as being selected.
    // Draws a box around the tagged object
    startSetObject(p, cx_, cy_);
  }

  return (TCL_OK);
}

//----------------------------------------------------------------------
// int
// EditView::cmdMoveTo(float cx, float cy)
//   - Moves object to end location
//   - Ends the rubber band selection
//----------------------------------------------------------------------
int EditView::cmdMoveTo(float cx, float cy) {
  // cx and cy should be in screen (canvas) coordinates
  cx_ = cx;
  cy_ = cy;

  switch (objType_) {
    case START_RUBBERBAND:
    case MOVE_RUBBERBAND:
      oldx_ = rb_.xmax;
      oldy_ = rb_.ymax;
      rb_.xmax = cx_;
      rb_.ymax = cy_;
      objType_ = MOVE_RUBBERBAND;
      clip_ = rb_;
      clip_.adjust();

      if (clip_.xmin > oldx_) 
        clip_.xmin = oldx_;

      if (clip_.xmax < oldx_) 
        clip_.xmax = oldx_;

      if (clip_.ymin > oldy_) 
        clip_.ymin = oldy_;

      if (clip_.ymax < oldy_) 
        clip_.ymax = oldy_;

      break;

    case START_OBJECT:
    case MOVE_OBJECT: {
      oldx_ = rb_.xmax;
      oldy_ = rb_.ymax;
      rb_.xmax = cx_;
      rb_.ymax = cy_;
      objType_ = MOVE_OBJECT;
      clip_.clear();
      defTag_->merge(clip_);
      matrix_.map(clip_.xmin, clip_.ymin);
      matrix_.map(clip_.xmax, clip_.ymax);
      clip_.adjust();
      // Actual move and final bbox computation is done in render
      break;

      case START_LINK:
      case MOVE_LINK:
        // I believe this is never reached and should be removed
        // maybe it is changed in the future
        oldx_ = rb_.xmax;
        oldy_ = rb_.ymax;
        rb_.xmax = cx_;
        rb_.ymax = cy_;
        objType_ = MOVE_LINK;
        clip_ = rb_;
        clip_.adjust();
        if (clip_.xmin > oldx_)
          clip_.xmin = oldx_;
        if (clip_.xmax < oldx_)
          clip_.xmax = oldx_;
        if (clip_.ymin > oldy_)
          clip_.ymin = oldy_;
        if (clip_.ymax < oldy_)
          clip_.ymax = oldy_;
        break;
      }
    default:
      // to avoid segmentation fault from click-n-dragging
      return (TCL_OK);
  }
  draw();
  return (TCL_OK);
}

int EditView::cmdReleasePoint(float cx, float cy)
{
	cx_ = cx, cy_ = cy;

	switch (objType_) {
	case START_RUBBERBAND:
	case MOVE_RUBBERBAND: {
		oldx_ = rb_.xmax; oldy_ = rb_.ymax;
		rb_.xmax = cx_, rb_.ymax = cy_;
		// Need to add the region to defTag_
		clip_ = rb_;
		clip_.adjust();
		BBox bb = clip_;
		matrix_.imap(bb.xmin, bb.ymin);
		matrix_.imap(bb.xmax, bb.ymax);
		bb.adjust();
		model_->tagArea(bb, defTag_, 1);

		// We can do a total redraw
		objType_ = NONE;
		draw();
		break;
	}
	case START_OBJECT:
	case MOVE_OBJECT: {
		oldx_ = rb_.xmax; oldy_ = rb_.ymax;
		rb_.xmax = cx_, rb_.ymax = cy_;
		clip_.clear();
		defTag_->merge(clip_);
		matrix_.map(clip_.xmin, clip_.ymin);
		matrix_.map(clip_.xmax, clip_.ymax);
		clip_.adjust();

		// Later in render() we'll compute the real bbox
		objType_ = END_OBJECT;
		draw();
		objType_ = NONE;
		model_->update(model_->now());
		break;
	}

        case START_LINK:
        case MOVE_LINK: {		
	       objType_ = START_LINK;
	       draw();
	       model_->update(model_->now());
               cmdaddLink(cx_, cy_);
	       break;
        }

	default:
		// This cannot happen!
		objType_ = NONE;
		draw();
	}
	return (TCL_OK);
}

//----------------------------------------------------------------------
// void EditView::draw()
//----------------------------------------------------------------------
void EditView::draw() {
  if (objType_ == NONE) {
    View::draw();
  } else {
    // Ignore the cleaning part
    render();
    
    // XXX Don't understand why clip's height and width need to 
    // increase 3 to draw tagged objects correctly. 
    XCopyArea(Tk_Display(tk_), offscreen_, Tk_WindowId(tk_), 
              background_,(int)clip_.xmin, (int)clip_.ymin, 
             (int)clip_.width()+3, (int)clip_.height()+3, 
             (int)clip_.xmin, (int)clip_.ymin);
  }
}
//----------------------------------------------------------------------
// void EditView::draw(double current_time)
//----------------------------------------------------------------------
void EditView::draw(double current_time) {
	model_->update(current_time);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
void EditView::xline(float x0, float y0, float x1, float y1, GC gc) {
  XDrawLine(Tk_Display(tk_), offscreen_, gc, (int) x0, (int) y0,
                                             (int) x1, (int) y1);
}

// Without transform.
void EditView::xrect(float x0, float y0, float x1, float y1, GC gc)
{
	int x = (int) floor(x0);
	int y = (int) floor(y0);
	int w = (int)(x1 - x0);
	if (w < 0) {
		x = (int) ceil(x1);
		w = -w;
	}
	int h = (int)(y1 - y0);
	if (h < 0) {
		h = -h;
		y = (int)ceil(y1);
	}
	XDrawRectangle(Tk_Display(tk_), offscreen_, gc, x, y, w, h);
}

void EditView::line(float x0, float y0, float x1, float y1, int color)
{
	if (objType_ != NONE)
		View::line(x0, y0, x1, y1, Paint::instance()->Xor());
	else 
		View::line(x0, y0, x1, y1, color);
}

void EditView::rect(float x0, float y0, float x1, float y1, int color)
{
	if (objType_ != NONE)
		View::rect(x0, y0, x1, y1, Paint::instance()->Xor());
	else 
		View::rect(x0, y0, x1, y1, color);
}

void EditView::polygon(const float* x, const float* y, int n, int color)
{
	if (objType_ != NONE)
		View::polygon(x, y, n, Paint::instance()->Xor());
	else 
		View::polygon(x, y, n, color);
}

void EditView::fill(const float* x, const float* y, int n, int color)
{
	if (objType_ != NONE)
		View::fill(x, y, n, Paint::instance()->Xor());
	else 
		View::fill(x, y, n, color);
}

void EditView::circle(float x, float y, float r, int color)
{
	if (objType_ != NONE)
		View::circle(x, y, r, Paint::instance()->Xor());
	else 
		View::circle(x, y, r, color);
}

// Do not display any string, because no xor font gc
void EditView::string(float fx, float fy, float dim, const char* s, int anchor)
{
	if (objType_ == NONE)
		View::string(fx, fy, dim, s, anchor);
}

//----------------------------------------------------------------------
// void
// EditView::BoundingBox(BBox &bb)
//   - This copies the area in which an animation drawn may be drawn
//     into (clipping area) the destination bb.  
//----------------------------------------------------------------------
void
EditView::BoundingBox(BBox &bb) {
  double temp;
  
  // Calculate the world coordinates for the view's canvas
  matrix_.imap(0.0, 0.0, bb.xmin, bb.ymin);
  matrix_.imap(width_, height_, bb.xmax, bb.ymax);

  // Check to make sure xmin, ymin is the lower left corner
  // and xmax, ymax is the upper right
  if (bb.xmin > bb.xmax) {
    temp = bb.xmax;
    bb.xmax = bb.xmin;
    bb.xmin = temp;
  }
  if (bb.ymin > bb.ymax) {
    temp = bb.ymax;
    bb.ymax = bb.ymin;
    bb.ymin = temp;
  }
}

void
EditView::getWorldBox(BBox &bb) {
  model_->BoundingBox(bb);
}

//----------------------------------------------------------------------
// void 
// EditView::render()
//   - not sure why this is called render but it seems to take care of 
//     rubberband selections and moving a selection on the screen
//   - I believe the real "rendering" is done in model_->render()
//     which is of type NetModel.  -- mehringe@isi.edu
//
//----------------------------------------------------------------------
void
EditView::render() {
  // Here we can compute the clipping box for render
  Paint *paint = Paint::instance();
  GC gc = paint->paint_to_gc(paint->Xor());

  switch (objType_) {
    case START_RUBBERBAND:
      // draw rubber band
      xrect(rb_.xmin, rb_.ymin, rb_.xmax, rb_.ymax, gc);
      break;

    case MOVE_RUBBERBAND: 
      // erase previous rubberband
      xrect(rb_.xmin, rb_.ymin, oldx_, oldy_, gc);
      // draw new rubberband
      xrect(rb_.xmin, rb_.ymin, rb_.xmax, rb_.ymax, gc);
      break;

    case END_RUBBERBAND: 
      // erase previous rubber band
      xrect(rb_.xmin, rb_.ymin, oldx_, oldy_, gc);
      // XXX Should draw the tag?
      model_->render(this);
      objType_ = NONE;
      break;

    case START_OBJECT:
      xrect(rb_.xmin, rb_.ymin, rb_.xmax, rb_.ymax, gc);
      // xor-draw all relevant objects
      defTag_->draw(this, model_->now());
      break;

    case MOVE_OBJECT: 
      // erase old positions first.
      if ((oldx_ == rb_.xmax) && (oldy_ == rb_.ymax)) 
        return;
      defTag_->draw(this, model_->now());
      // move these objects
      defTag_->move(this, rb_.xmax - oldx_, rb_.ymax - oldy_);
      BBox bb;
      bb.clear();
      defTag_->merge(bb);
      matrix_.imap(bb.xmin, bb.ymin);
      matrix_.imap(bb.xmax, bb.ymax);
      bb.adjust();
      clip_.merge(bb);
      defTag_->draw(this, model_->now());
      break;

    case END_OBJECT:
      // Erase old positions
      defTag_->draw(this, model_->now());
      // At least we should redo scale estimation and 
      // place everything
      defTag_->move(this, rb_.xmax - oldx_, rb_.ymax - oldy_);
      model_->recalc();
      model_->render(this);
      objType_ = NONE;
      break;

    case START_LINK:
      line(link_start_x_, link_start_y_, link_end_x_, link_end_y_, 3);
      defTag_->draw(this, model_->now());
      break;

    case MOVE_LINK:
      // erase previous link
      xline(rb_.xmin, rb_.ymin, oldx_, oldy_, gc);
      // draw new rubberband
      xline(rb_.xmin, rb_.ymin, rb_.xmax, rb_.ymax, gc);
      break;

    case END_LINK:
      // erase previous link
      xline(rb_.xmin, rb_.ymin, oldx_, oldy_, gc);
      // XXX Should draw the tag?
      model_->render(this);
      objType_ = NONE;
      break;

    default:
      // redraw model
      model_->render(this);
      break;
  }
  return;
}
