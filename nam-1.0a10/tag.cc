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
// $Header: /nfs/jade/vint/CVSROOT/nam-1/tag.cc,v 1.5 2001/04/18 00:14:16 mehringe Exp $

#ifdef WIN32
#include <windows.h>
#endif
#include <string.h>
#include <tcl.h>
#include "animation.h"
#include "tag.h"
#include "view.h"

Tag::Tag()
	: Animation(0, 0), nMbr_(0), name_(NULL)
{
	mbrHash_ = new Tcl_HashTable;
	Tcl_InitHashTable(mbrHash_, TCL_ONE_WORD_KEYS);
}

Tag::Tag(const char *name)
	: Animation(0, 0), nMbr_(0)
{
	size_t len = strlen(name);
	if (len == 0) 
		name_ = NULL;
	else {
		name_ = new char[len+1];
		strcpy(name_, name);
	}
	mbrHash_ = new Tcl_HashTable;
	Tcl_InitHashTable(mbrHash_, TCL_ONE_WORD_KEYS);
}

Tag::~Tag()
{
	remove();
	Tcl_DeleteHashTable(mbrHash_);
	delete mbrHash_;	
	if (name_ != NULL)
		delete []name_;
}

void Tag::setname(const char *name)
{
	if (name_ != NULL)
		delete name_;
	size_t len = strlen(name);
	if (len == 0) 
		name_ = NULL;
	else {
		name_ = new char[len+1];
		strcpy(name_, name);
	}
}

void Tag::add(Animation *a)
{
	int newEntry = 1;
	Tcl_HashEntry *he = 
		Tcl_CreateHashEntry(mbrHash_,(const char *)a->id(), &newEntry);
	if (newEntry && (he != NULL)) {
		// Do not handle exception he == NULL. Don't know how.
		Tcl_SetHashValue(he, (ClientData)a);
		nMbr_++;
		a->merge(bb_);
		// Let that object know about this group
		a->addTag(this); 
	}
}

void Tag::remove()
{
	// Remove this tag from all its members
	Animation *p;
	Tcl_HashEntry *he;
	Tcl_HashSearch hs;
	int i = 0;
	for (he = Tcl_FirstHashEntry(mbrHash_, &hs);
	     he != NULL;
	     he = Tcl_NextHashEntry(&hs), i++) {
		p = (Animation *) Tcl_GetHashValue(he);
		p->deleteTag(this);
		Tcl_DeleteHashEntry(he);
		nMbr_--;
	}
	bb_.clear();
}

void Tag::remove(Animation *a)
{
	Tcl_HashEntry *he = Tcl_FindHashEntry(mbrHash_, (const char *)a->id());
	if (he != NULL) {
		a->deleteTag(this);
		Tcl_DeleteHashEntry(he);
		nMbr_--;
		update_bb();
	}
}

void Tag::draw(View *v, double now) {
	// XXX don't draw anything except it's selected. Or draw 
	// 4 black dots in the 4 corners.
	v->rect(bb_.xmin, bb_.ymin, bb_.xmax, bb_.ymax, 
		Paint::instance()->thin());
}

// Used for xor-draw only
void Tag::xdraw(View *v, double now) const 
{
	// Draw every object of this group
	Tcl_HashEntry *he;
	Tcl_HashSearch hs;
	Animation *a;

	for (he = Tcl_FirstHashEntry(mbrHash_, &hs);
	     he != NULL;
	     he = Tcl_NextHashEntry(&hs)) {
		a = (Animation *) Tcl_GetHashValue(he);
		a->draw(v, now);
	}
}

void Tag::update_bb(void)
{
	Tcl_HashEntry *he;
	Tcl_HashSearch hs;
	Animation *a;

	bb_.clear();
	for (he = Tcl_FirstHashEntry(mbrHash_, &hs);
	     he != NULL;
	     he = Tcl_NextHashEntry(&hs)) {
		a = (Animation *) Tcl_GetHashValue(he);
		a->merge(bb_);
	}
}

void Tag::reset(double /*now*/)
{
}

// Move this object and update bbox
void Tag::move(EditView *v, float wdx, float wdy)
{
	Tcl_HashEntry *he;
	Tcl_HashSearch hs;
	Animation *a;

	bb_.clear();
	for (he = Tcl_FirstHashEntry(mbrHash_, &hs);
	     he != NULL;
	     he = Tcl_NextHashEntry(&hs)) {
		a = (Animation *) Tcl_GetHashValue(he);
		a->move(v, wdx, wdy);
		a->merge(bb_);
	}
	// We do separate move for bb_ because 
}

const char *Tag::info() const
{
	static char str[256];
	strcpy(str, "Tag");
	return str;
}
