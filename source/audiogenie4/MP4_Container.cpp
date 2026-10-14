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

#include "StdAfx.h"
#include "MP4_container.h"
#include "mp4_atomfactory.h"
#include "Tools.h"

CMP4_Container::CMP4_Container()
{
	init(0);
}

CMP4_Container::CMP4_Container(u32 frameID)
{
	init(frameID);	
}
CMP4Atom* CMP4_Container::copy()
{
	CMP4_Container *atom;
	atom = new CMP4_Container(getFrameID());
	atom->_noFullBox = _noFullBox;
	atom->_blob.Clear();
	atom->_blob.AddBlob(_blob);
	atom->_parent = _parent;
	// Kinder auch kopieren
	// also copy children
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		atom->addAtom(_children[i]->copy());
	}
	return atom;
}

void CMP4_Container::init(u32 frameID)
{
	setFrameID(frameID);
	_noFullBox = false;
	_offset = 0;
	_size = 0;
	_children.SetCount(0, 10);
	setParent(EMPTY);
}

CMP4_Container::~CMP4_Container(void)
{
	remove();
}
void CMP4_Container::replaceAtom(CMP4Atom* atom)
{
	CAtlString path = getPath();
	CAtlString atomID = atom->getPath();
	int pathLen = path.GetLength();
	if (atomID.Left(pathLen).CompareNoCase(path) == 0)
	{
		// Rechter gesuchter Zweig bereits am Ende ?
		// right side of node we are searching in already ended?
		CAtlString node;
		if (pathLen == 0)
			node = atomID.Left(4);
		else
			node = atomID.Mid(pathLen + 1, 4);

		bool isEnde = (atomID.GetLength() - pathLen <= 5);
		size_t counts = _children.GetCount();
		for (size_t i = 0; i < counts; i++)
		{
			if (_children[i]->getId().CompareNoCase(node) == 0)
			{
				if (isEnde)
				{
					delete _children[i];
					_children[i] = atom;
					atom->setParent(this);
					return;
				}
				_children[i]->replaceAtom(atom);
				return;
			}			
		}
		// Nicht gefunden, falls am Ende neu anfügen
		// nothing found. in case we are at the end, add as new.
		if (isEnde)
		{
			_children.Add(atom);
			atom->setParent(this);
			return;
		}
		// Mitten drin, neuen Container rekursiv anlegen 
		// we are in the middle of something -> add new containter recursively
		u32 id = (node.GetAt(0) << 24) + (node.GetAt(1) << 16) + (node.GetAt(2) << 8) + node.GetAt(3);
		CMP4Atom* container = CMP4_AtomFactory::instance()->createAtom(id);
		container->setParent(getPath());
		_children.Add(container);
		container->replaceAtom(atom);
		return;
	}		
}

void CMP4_Container::addAtom(CMP4Atom* atom)
{
	CAtlString path = getPath();
	CAtlString atomID = atom->getPath();
	int pathLen = path.GetLength();
	if (atomID.Left(pathLen).CompareNoCase(path) == 0)
	{
		// Rechter gesuchter Zweig bereits am Ende ?
		// right side of node we are searching in already ended?
		CAtlString node;
		if (pathLen == 0)
			node = atomID.Left(4);
		else
			node = atomID.Mid(pathLen + 1, 4);

		bool isEnde = (atomID.GetLength() - pathLen <= 5);
		if (isEnde)
		{
			atom->setParent(this);
			_children.Add(atom);
			return;
		}
		size_t counts = _children.GetCount();
		for (size_t i = 0; i < counts; i++)
		{
			if (_children[i]->getId().CompareNoCase(node) == 0)
			{
				_children[i]->addAtom(atom);
				return;
			}
		}
		// Mitten drin, neuen Container rekursiv anlegen 
		// we are in the middle of something -> add new containter recursively
		u32 id = (node.GetAt(0) << 24) + (node.GetAt(1) << 16) + (node.GetAt(2) << 8) + node.GetAt(3);
		CMP4Atom* container = CMP4_AtomFactory::instance()->createAtom(id);
		container->setParent(getPath());
		_children.Add(container);
		container->addAtom(atom);
		return;
	}		
}

CMP4Atom* CMP4_Container::find(CAtlString atomID)
{
	CAtlString path = getPath();
	if (path.CompareNoCase(atomID) == 0)
		return (--CMP4_AtomFactory::count == 0) ? this : NULL;
	CMP4Atom* atom = NULL;
	if (atomID.Left(path.GetLength()).CompareNoCase(path) == 0)
	{
		size_t counts = _children.GetCount();
		for (size_t i = 0; i < counts; i++)
		{
			atom = _children[i]->find(atomID);
			if (atom != NULL)
				return atom;
		}
	}	
	return NULL;
}

void CMP4_Container::load(FILE *Stream, u32 offset, u32 size)
{
	_size = size;
	_offset = offset;
	u32 endPos = _offset + _size;
	CMP4_AtomFactory *factory = CMP4_AtomFactory::instance();
	// an atom inside another one (not a top-level one): stopping early there drops the rest of its parent on save
	bool nested = factory->ebene > 0;
	// nesting is capped: hostile nesting recursed until the stack overflowed (4.0.4.6 and earlier)
	if (factory->ebene > 2 * MP4_MAX_NESTING)
	{
		CMP4_AtomFactory::loadError = true;
		return;
	}
	if (getFrameID() == MP4_META)
	{
		// A QuickTime-style meta has no version/flags: its hdlr starts right after the header. Skipping 4 bytes
		// there misparsed it, and saving wrote the contents back damaged (4.0.4.6 and earlier).
		_noFullBox = false;
		if (_size >= 8 && fseek(Stream, _offset, SEEK_SET) == 0)
		{
			CBlob peek(8);
			peek.FileRead(8, Stream);
			if (peek.GetLength() == 8 && (u32)peek.Get4B(4) == MP4_HDLR)
				_noFullBox = true;
		}
		if (!_noFullBox)
			_offset+=4;
	}
	CAtlString parent(getPath());
	CBlob header(8);
	long nextReadPos = 0;
	u32 atomSize = 0;
	u32 atomID = 0;
	u32 dataSize = 0;
	CMP4Atom *atom;
	while (_offset < endPos)
	{
		// fewer than 8 bytes left is no atom (QuickTime ends udta with 4 zero bytes)
		if (endPos - _offset < 8)
			return;
		if (fseek(Stream, _offset, SEEK_SET) != 0)
		{
			CMP4_AtomFactory::loadError = true;
			return;
		}
		header.FileRead(8, Stream);
		if (header.GetLength() < 8)
		{
			if (nested)
				CMP4_AtomFactory::loadError = true;
			return;
		}
		atomSize = header.Get4B(0);
		atomID = header.Get4B(4);
		if (atomSize == 0 && atomID == 0) // zero fill: the end
			return;
		if (atomSize == 1) // Extended Size in 64 bit isn't supported yet
		{
			CTools::instance().setLastError(ERR_FRAME_TOO_BIG);
			CMP4_AtomFactory::loadError = true;
			return;
		}
		// size 0: the atom runs to the end of its parent (it ran 8 bytes past the end of the file)
		if (atomSize == 0)
			atomSize = endPos - _offset;
		// a size of 2 to 7 wrapped dataSize around to about 4 GB (4.0.4.6 and earlier)
		if (atomSize < 8 || atomSize > endPos - _offset || atomID == 0)
		{
			if (nested)
				CMP4_AtomFactory::loadError = true;
			return;
		}
		dataSize = atomSize - 8;
		atom = factory->createAtom(atomID);
		CMP4_AtomFactory::instance()->ebene+=2;
		_children.Add(atom);
		atom->setParent(parent);
#ifdef DEBUG
		atom->print();
#endif
		ATLTRACE(_T(" offset: %u  size:%u\n"), _offset, atomSize );
		CTools::instance().doEvents();
		nextReadPos = _offset + atomSize - dataSize;
		atom->load(Stream, nextReadPos, dataSize);
		CMP4_AtomFactory::instance()->ebene-=2;
		_offset+=atomSize;
	}	
}

long CMP4_Container::getSize()
{
	long size = 8;
	if (getFrameID() == MP4_META && !_noFullBox)
		size+=4;

	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		size+=_children[i]->getSize();
	}
	return size;
}

void CMP4_Container::remove()
{
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		_children[i]->remove();
		delete _children[i];
	}
	_children.RemoveAll();
}

bool CMP4_Container::removeAtom(CAtlString atomID)
{
	CAtlString path = getPath();
	if (path.CompareNoCase(atomID) == 0)
	{
		// mich selbst löschen
		remove();
		return true;
	}
	if (atomID.Left(path.GetLength()).CompareNoCase(path) == 0)
	{
		size_t counts = _children.GetCount();
		for (size_t i = 0; i < counts; i++)
		{
			if (_children[i]->removeAtom(atomID))
			{
				delete _children[i];
				_children.RemoveAt(i);
				return false;
			}
		}
	}	
	return false;
}

bool CMP4_Container::removeAtom(CMP4Atom* atom)
{
	CAtlString path = getPath();
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		if (_children[i]->removeAtom(atom))
		{
			delete _children[i];
			_children.RemoveAt(i);				
			return false;
		}
	}
	return false;
}
void CMP4_Container::save(FILE *stream)
{
	long frameSize = getSize();
	CBlob tmp(8);
	tmp.Add4B(frameSize);
	tmp.Add4B(getFrameID());
	if (getFrameID() == MP4_META && !_noFullBox)
		tmp.Add4B(0);
	tmp.FileWrite(tmp.GetLength(), stream);
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		_children[i]->save(stream);		
	}	
}
