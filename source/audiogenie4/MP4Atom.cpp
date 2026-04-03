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
#include "MP4Atom.h"
#include "Tools.h"
#include "mp4_atomfactory.h"

CMP4Atom::CMP4Atom(void)
{
	init(0);
}

CMP4Atom::CMP4Atom(unsigned int frameID)
{
	init(frameID);
}

CMP4Atom* CMP4Atom::copy()
{
	CMP4Atom *atom =  new CMP4Atom(_frameID);
	atom->_blob.Clear();
	atom->_blob.AddBlob(_blob);
	atom->_parent = _parent;
	atom->_txtid = _txtid;
	return atom;
}

CMP4Atom::~CMP4Atom(void)
{
	remove();
}

CMP4Atom* CMP4Atom::find(CAtlString atomID)
{
	CAtlString pfad = getPath();
	if (_frameID == MP4_ITUN && atomID.Left(24).Compare(_T("moov.udta.meta.ilst.----")) == 0)
	{
		if (atomID.Mid(25).Compare(CMP4_AtomFactory::instance()->getiTuneFrame(this)) == 0)
			return this;			
	}
	if (pfad.CompareNoCase(atomID) != 0)
		return NULL;

	// Gefunden, aber auch der richtige Index ?
	// count wird am Anfang auf 1 bis n gesetzt
	// dann bei jedem Treffer reduziert und wenn 0 ist, wurde der richtige Eintrag gefunden
	
	// found, but also the right index?
	// cound is set in the beginning from 1..n
	// will be reduced on each matching until it is 0. IN this case, the correct entry is found.

	return (--CMP4_AtomFactory::count == 0) ? this : NULL;
}

bool CMP4Atom::removeAtom(CAtlString atomID)
{
	return (getPath().CompareNoCase(atomID) != 0) ?  false : true;
}

bool CMP4Atom::removeAtom(CMP4Atom* atom)
{
	return (this == atom);
}

void CMP4Atom::init(unsigned int frameID)
{
	_parent.Empty();
	setFrameID(frameID);		
}

void CMP4Atom::print()
{
	for (int i = 0; i < CMP4_AtomFactory::instance()->ebene;i++)
		ATLTRACE(_T(" "));
	ATLTRACE(_T("Atom:%s"), getPath());	
}

CAtlString CMP4Atom::getId()
{
	return _txtid;
}
long CMP4Atom::getSize()
{
	return (long)_blob.GetLength() + 8;
}

void CMP4Atom::load(FILE *Stream, u32 offset, u32 size)
{
	offset;
	//fseek(Stream, offset, SEEK_SET); wird nicht benötigt // not necessary
	_blob.FileRead(size, Stream);
}

void CMP4Atom::save(FILE *stream)
{
	long frameSize = getSize();
	CBlob tmp(8);
	tmp.Add4B(frameSize);
	tmp.Add4B(_frameID);
	tmp.FileWrite(8, stream);
	_blob.FileWrite(_blob.GetLength(), stream);	
	CTools::instance().doEvents();	
}

void CMP4Atom::Empty()
{
	_blob.Clear();
}

void CMP4Atom::setParent(CAtlString parent)
{
	_parent = parent;
}
CAtlString CMP4Atom::getPath()
{
	if (_parent.GetLength() > 0)
	{
		return _parent + L"." + getId();
	}
	return getId();
}
u32 CMP4Atom::getDataLen()
{
	return (_blob.GetLength() >= 4) ? _blob.Get4B(0) : 0;
}
u32 CMP4Atom::getVersion()
{
	return (_blob.GetLength() >= 12) ? _blob.Get4B(8) : 0;
}
void CMP4Atom::setFrameID(unsigned int frameID)
{
	_frameID = frameID;	
	if (_frameID == 0)
		_txtid = L"";
	else
		_txtid.Format(_T("%c%c%c%c"), BYTE(_frameID >> 24), BYTE(_frameID >> 16), BYTE(_frameID >> 8), BYTE(_frameID)); 
}
