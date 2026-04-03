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
#include "ID3F_GRID.h"
#include "id3_framefactory.h"

/* Group identification registration
This frame enables grouping of otherwise unrelated frames. This can
be used when some frames are to be signed. To identify which frames
belongs to a set of frames a group identifier must be registered in
the tag with this frame. The 'Owner identifier' is a null-terminated
string with a URL [URL] containing an email address, or a link to a
location where an email address can be found, that belongs to the
organisation responsible for this grouping. Questions regarding the
grouping should be sent to the indicated email address. The 'Group
symbol' contains a value that associates the frame with this group
throughout the whole tag, in the range $80-F0. All other values are
reserved. The 'Group symbol' may optionally be followed by some group
specific data, e.g. a digital signature. There may be several "GRID"
frames in a tag but only one containing the same symbol and only one
containing the same owner identifier. The group symbol must be used
somewhere in the tag. See the description of the frame grouping flag
in the ID3v2 structure document [ID3v2-strct] for more information.

<Header for 'Group ID registration', ID: "GRID">
Owner identifier      <text string> $00
Group symbol          $xx
Group dependent data  <binary data>
*/

CID3F_GRID::CID3F_GRID(void)
{
	init(EMPTY, 0);
}

CID3F_GRID::CID3F_GRID(LPCWSTR url, BYTE symbol) 	
{
	init(url, symbol);
}

void CID3F_GRID::init(LPCWSTR url, BYTE symbol)
{
	_frameID = F_GRID;
	_url = url;
	_symbol = symbol;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_GRID::~CID3F_GRID(void)
{
}

bool CID3F_GRID::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_GRID *target = cGRID(frame);
	return (this->_url.CompareNoCase(target->_url) == 0);
}

void CID3F_GRID::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_data.Clear();
		if (_blob.GetLength() < 2)
		{
			_url.Empty();
			_symbol = 0;
		}
		else
		{
			int start = 0;
			_url = _blob.getNextString(TEXT_ENCODED_ANSI, start);
			_symbol = _blob.GetAt(start++);
			_data.AddBlob(_blob, start);
		}		
		isDecoded = true;
	}	
}
void CID3F_GRID::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _url, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddValue(_symbol);
		_blob.AddBlob(_data);		
		mustRebuild = false;	
	}
}

CAtlString CID3F_GRID::getURL()
{
	decode();
	return _url;
}

BYTE CID3F_GRID::getSymbol()
{
	decode();
	return _symbol;
}
