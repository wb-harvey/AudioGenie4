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
#include "ID3F_CTOC.h"
#include "id3_framefactory.h"
/* http://id3.org/id3v2-chapters-1.0
Table of contents frame

The purpose of "CTOC" frames is to allow a table of contents to be defined. In the simplest case, a single "CTOC" frame can be used to provide a flat (single-level) table of contents. However, multiple "CTOC" frames can also be used to define a hierarchical (multi-level) table of contents.

There may be more than one frame of this type in a tag but each must have an Element ID that is unique with respect to any other "CTOC" or "CHAP" frame in the tag.

Each "CTOC" frame represents one level or element of a table of contents by providing a list of Child Element IDs. These match the Element IDs of other "CHAP" and "CTOC" frames in the tag.

<ID3v2.3 or ID3v2.4 frame header, ID: "CTOC">   (10 bytes) 
Element ID      <text string> $00
Flags           %000000ab
Entry count     $xx  (8-bit unsigned int)
<Child Element ID list>
<Optional embedded sub-frames>

The Element ID uniquely identifies the frame. It is not intended to be human readable and should not be presented to the end-user.

Flag a - Top-level bit
This is set to 1 to identify the top-level "CTOC" frame. This frame is the root of the Table of Contents tree and is not a child of any other "CTOC" frame. Only one "CTOC" frame in an ID3v2 tag can have this bit set to 1. In all other "CTOC" frames this bit shall be set to 0. 
Flag b - Ordered bit
This should be set to 1 if the entries in the Child Element ID list are ordered or set to 0 if they not are ordered. This provides a hint as to whether the elements should be played as a continuous ordered sequence or played individually. The Entry count is the number of entries in the Child Element ID list that follows and must be greater than zero. Each entry in the list consists of:

Child Element ID            <text string> $00 

The last entry in the child Element ID list is followed by a sequence of optional frames that are embedded within the "CTOC" frame and which describe this element of the table of contents (e.g. a "TIT2" frame representing the name of the element) or provide related material such as URLs and images. These sub-frames are contained within the bounds of the "CTOC" frame as signalled by the size field in the "CTOC" frame header.

If a parser does not recognise "CTOC" frames it can skip them using the size field in the frame header. When it does this it will skip any embedded sub-frames carried within the frame. 

Figure 2 shows an example of a "CTOC" frame which references a sequence of chapters. It contains a single "TIT2" sub-frame which provides a name for this element of the table of contents; "Part 1". 

*/

CID3F_CTOC::CID3F_CTOC(void)
{
	init(EMPTY, EMPTY, EMPTY, false );
}

CID3F_CTOC::CID3F_CTOC(LPCWSTR ID, LPCWSTR Title, LPCWSTR Description, bool isOrdered) 	
{
	init(ID, Title, Description, isOrdered);
}

void CID3F_CTOC::init(LPCWSTR ID, LPCWSTR Title, LPCWSTR Description, bool isOrdered)
{
	_frameID = F_CTOC;
	_ID = ID;
	_title = Title;
	_description = Description;
	_isOrdered = isOrdered;	
	useTextEncoding = true;
	_elements.SetCount(0, 6);
	isDecoded = true;
	setTitle(_title);
	setDescription(_description);
	mustRebuild = true;
}

CID3F_CTOC::~CID3F_CTOC(void)
{	
	_elements.RemoveAll();
}

void CID3F_CTOC::decode()
{
	if (!isDecoded)
	{	
		_data.Clear();
		_elements.RemoveAll();
		if (_blob.GetLength() < 3)
		{
			_title.Empty();
			_description.Empty();
			_flags = 0;
			_count = 0;
		}
		else
		{
			int start = 0;
			_ID = _blob.getNextString(TEXT_ENCODED_ANSI, start);
			_flags = _blob.GetAt(start++);
			_count = _blob.GetAt(start++);
			_isOrdered = ((_flags & 1) == 1);
			_isRoot = ((_flags & 2) == 2);
			int anz = 0;
			CAtlString tst;
			while (anz++ < _count)
				_elements.Add(_blob.getNextString(TEXT_ENCODED_ANSI, start));
			_data.AddBlob(_blob, start);
			convertBlobToFrames();
			_data.Clear();			
		}		
		isDecoded = true;
	}	
}
void CID3F_CTOC::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _ID, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_flags = 0;
		if (_isOrdered)
			_flags |= 1;
		if (_isRoot)
			_flags |= 2;
		_blob.AddValue(_flags);
		_blob.AddValue((BYTE)_elements.GetCount());
		for (size_t i = 0; i < _elements.GetCount(); i++)
			_blob.AddEncodedString(TEXT_ENCODED_ANSI, _elements[i], TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		convertFramesToBlob();
		_blob.AddBlob(_data);
		mustRebuild = false;
	}
}

bool CID3F_CTOC::isRoot()
{
	decode();
	return _isRoot;
}
void CID3F_CTOC::setRoot(bool status)
{
	decode();
	if (_isRoot != status)
	{
		_isRoot = status;
		mustRebuild = true;
	}
}

BYTE CID3F_CTOC::getNumberOfEntries()
{
	decode();
	return (BYTE)_elements.GetCount();
}

CAtlString CID3F_CTOC::getChildElementID(u16 pos)
{
	decode();
	return (pos > 0 && pos <= _elements.GetCount() ) ? _elements.GetAt(pos - 1) : EMPTY;
}

void CID3F_CTOC::addChildElement(CAtlString id)
{
	_elements.Add(id);
}

short CID3F_CTOC::deleteChildElement(CAtlString ID)
{
	decode();
	int count = (int)_elements.GetCount();
	short result = 0;
	while (--count >= 0)
	{
		if (_elements[count].Compare(ID) == 0)
		{		
			_elements.RemoveAt(count);
			result++;
		}		
	}
	return result;	
}

bool CID3F_CTOC::isOrdered()
{
	decode();
	return _isOrdered;
}

void CID3F_CTOC::setOrdered(bool newStatus)
{
	decode();
	if (newStatus != _isOrdered)
	{
		_isOrdered = newStatus;
		mustRebuild = true;
	}
}