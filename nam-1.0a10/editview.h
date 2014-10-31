//
// Copyright (c) 1997 by the University of Southern California
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
// $Header: /nfs/jade/vint/CVSROOT/nam-1/editview.h,v 1.13 2001/05/25 02:26:38 mehringe Exp $

#ifndef nam_EditView_h
#define nam_EditView_h

extern "C" {
#include <tcl.h>
#include <tk.h>
}

#include "tkcompat.h"
#include "enetmodel.h"
#include "transform.h"
#include "view.h"
#include "netview.h"
#include "bbox.h"
#include "tag.h"
#include "trafficsource.h"

class NetModel;
struct TraceEvent;
class Tcl;
class Paint;

class EditView : public NetView {
public:
	EditView(const char* name, NetModel *m);
 	EditView(const char* name, NetModel *m, int height, int width);
	virtual ~EditView();

	static int command(ClientData, Tcl_Interp*, int argc, char **argv);
	static void DeleteCmdProc(ClientData);

	virtual void draw();
        virtual void draw(double current_time);
	virtual void render();
	virtual void BoundingBox(BBox &bb);
	virtual void getWorldBox(BBox & world_boundary);

	int cmdSetPoint(float, float, int);
	int cmdMoveTo(float, float);
	int cmdReleasePoint(float, float);
	int cmdRemoveSel(float, float);
	int cmdaddNode(float, float);
	int cmdaddLink(float, float);
	int cmdaddAgent(float, float, char*);
	int cmdaddAgentLinker(float, float);
  int cmdaddTrafficSource(float cx, float cy, char * type);
	int cmdgetCurrentObj(float, float);
	int cmdgetObjProperty(float, float);
	int cmdsetNodeProperty(int, char*,int);
  int cmdsetAgentProperty(int, char*,int);
	int cmdsetLinkProperty(int,int, char*,int);
  int cmdsetTrafficSourceProperty(int id, char * value, char * variable);
	int cmdDeleteObj(float, float);

	void moveNode(Node *n) const {
		model_->moveNode(n);
	}

	virtual void line(float x0, float y0, float x1, float y1, int color);
	virtual void rect(float x0, float y0, float x1, float y1, int color);
	virtual void polygon(const float* x, const float* y, int n, int color);
	virtual void fill(const float* x, const float* y, int n, int color);
	virtual void circle(float x, float y, float r, int color);
  virtual void string(float fx, float fy, float dim, const char* s, 
                      int anchor);
	void view_mode() {
		defTag_->remove();
		draw();
	}

protected:
	// xor-draw rectangle
	void xrect(float x0, float y0, float x1, float y1, GC gc);
	void xline(float x0, float y0, float x1, float y1, GC gc);

	enum EditType { START_RUBBERBAND, MOVE_RUBBERBAND, END_RUBBERBAND,
			START_OBJECT, MOVE_OBJECT, END_OBJECT, NONE,
			START_LINK, MOVE_LINK, END_LINK };

	inline void startRubberBand(float cx, float cy) {
		// Nothing's here. Set rubber band.
		objType_ = START_RUBBERBAND;
		rb_.xmin = rb_.xmax = cx;
		rb_.ymin = rb_.ymax = cy;
		clip_ = rb_;
	}

  inline void startSetObject(Animation *p, float cx, float cy) {
		// Find an object, add it to group
		objType_ = START_OBJECT;
    
		// Add it into defTag_
		if (p != defTag_)
			model_->tagObject(defTag_, p);

		rb_.xmin = rb_.xmax = cx;
		rb_.ymin = rb_.ymax = cy;
		clip_.clear();
    defTag_->merge(clip_);

		// for the bbox
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
		objType_ = START_OBJECT;
	}

	// Grouping/interactive stuff
	Tag * defTag_;
	Animation *curObj_;
	float cx_, cy_; 	// current point
	EditType objType_;	// current selection type
	BBox rb_;		// rubber band rectangle
	float oldx_, oldy_;	// old rubber band position, used for erasion
  float link_start_x_, link_start_y_;           // link origin
  float link_end_x_, link_end_y_;           // link origin
};

#endif
