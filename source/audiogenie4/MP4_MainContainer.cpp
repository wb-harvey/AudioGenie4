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

#include "StdAfx.h"
#include "MP4_MainContainer.h"
#include "mp4_atomfactory.h"
#include "mp4atom.h"

CMP4_MainContainer::CMP4_MainContainer(void)
{
	setFrameID(0);
	_parent = EMPTY;
}

CMP4_MainContainer::~CMP4_MainContainer(void)
{
	remove();
}

CMP4Atom* CMP4_MainContainer::find(CAtlString atomID, int count)
{
	CMP4_AtomFactory::count = count;
	CMP4Atom *atom = NULL;
	size_t counts = _children.GetCount(); 
	for (size_t i = 0; i < counts; i++)
	{
		atom = _children[i]->find(atomID);
		if (atom != NULL)
			return atom;
	}
	return NULL;
}

// Where the padding goes: right after moov (before mdat when a file has no moov).
static size_t PaddingIndex(CAtlArray<CMP4Atom*>& children)
{
	size_t counts = children.GetCount();
	for (size_t i = 0; i < counts; i++)
		if (children[i]->getFrameID() == MP4_MOOV)
			return i + 1;
	for (size_t i = 0; i < counts; i++)
		if (children[i]->getFrameID() == MP4_MDAT)
			return i;
	return counts;
}

void CMP4_MainContainer::removePadding()
{
	// only the free atoms right after moov: others (before moov, after mdat) keep their place,
	// so the atoms after them do too
	size_t at = PaddingIndex(_children);
	while (at < _children.GetCount() && _children[at]->getFrameID() == 'free')
	{
		_children[at]->remove();
		delete _children[at];
		_children.RemoveAt(at);
	}
}

void CMP4_MainContainer::addPadding(u32 payload)
{
	// a free atom of payload + 8 bytes, right after moov
	CMP4Atom* atom = CMP4_AtomFactory::instance()->createAtom('free');
	atom->_blob.AddValue(0, payload);
	_children.InsertAt(PaddingIndex(_children), atom);
}

void CMP4_MainContainer::save(FILE *stream)
{
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		_children[i]->save(stream);		
	}	
}

void CMP4_MainContainer::checkMetaBox()
{
// Prüft, ob eine Meta Box da ist, falls nicht wird eine Meta Box angelegt und eine hdlr Box hinzugefügt ( Pflicht-Box )
// check if a meta box is present. if not, add a new meta box. in addidtion, a new hdlr bix is created (mandatory box)
	CMP4Atom* atom;
	if (find(_T("moov.udta.meta")) == NULL)
	{
		atom = CMP4_AtomFactory::instance()->createAtom(MP4_HDLR);
		atom->setParent(_T("moov.udta.meta"));
		addAtom(atom);
	}
}