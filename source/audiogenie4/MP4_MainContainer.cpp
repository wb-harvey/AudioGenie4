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

void CMP4_MainContainer::adjustPadding(u32 size)
{
	// Ist Padding erwünscht ? Wenn nein, dann lösche das Padding-Atom
	// Padding desired? if not, delete padding atom
	if (size <= 0)
	{
		this->removeAtom(L"free");
		return;
	}
	// Suche Padding Atom, wenn da ist, passe es an
	// ansonsten lege ein neues Padding Atom an
	
	// search for padding atom. if found, adopt it.
	// if not, create new paddint atom
	CMP4Atom* atom = find(_T("free"));
	if (atom != NULL)
	{
		atom->_blob.AddValue(0, size);
		return;
	}
	// kein Padding da, lege neues an
	// no padding atom found, create new
	
	// Suche mdat Container und lege davor das Padding
	// search for mdat container and add padding prior to this
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		atom = _children[i]->find(_T("mdat"));
		if (atom != NULL)
		{
			atom = CMP4_AtomFactory::instance()->createAtom('free');
			atom->_blob.AddValue(0, size);
			_children.InsertAt(i, atom);
			return;
		}
	}
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