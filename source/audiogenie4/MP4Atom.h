/* AudioGenie is a Library for analyzing and tagging audio files.
   Copyright (C) 2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,2011
   Free Software Foundation, Inc.
   This file is part of the AudioGenie Library.
   Contributed by Stefan Toengi <stefan@toengi.de>.

   The AudioGenie Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see <http://www.gnu.org/licenses/> 
*/

#pragma once
#include "Blob.h"
#include "atlcoll.h"

typedef unsigned __int32  u32;

class CMP4Atom
{
public:
	CMP4Atom(void);
	CMP4Atom(unsigned int frameID);
	~CMP4Atom(void);
	virtual CMP4Atom* copy();
	virtual CMP4Atom* find(CAtlString atomID);
	void init(unsigned int frameID);
	virtual long getSize();
	CBlob _blob;
	CAtlString _txtid;
	void print();
	virtual void replaceAtom(CMP4Atom* atom) { atom; } ;
	virtual void load(FILE *stream, u32 offset, u32 size);
	virtual void save(FILE *stream);
	virtual void Empty();
	virtual void remove() { };
	virtual bool removeAtom(CAtlString atomID);
	virtual void addAtom(CMP4Atom* atom)  { atom; } ;
	virtual bool removeAtom(CMP4Atom* atom);
	CAtlString getId();
	void setParent(CAtlString parent);
	void setFrameID(unsigned int frameID);
	void setParent(CMP4Atom* parentAtom) { _parentAtom = parentAtom; };
	CMP4Atom* getParent() { return _parentAtom; };
	CAtlString getPath();
	u32 getFrameID() { return _frameID; };
	u32 getDataLen();
	u32 getVersion();
	CAtlString _parent;
	CMP4Atom* _parentAtom;
	CAtlArray<CMP4Atom *> _children;
private:
	unsigned int _frameID;
};