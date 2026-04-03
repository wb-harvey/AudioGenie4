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
#include "ID3F_ENCR.h"
#include "id3_framefactory.h"

/*
To identify with which method a frame has been encrypted the
encryption method must be registered in the tag with this frame. The
'Owner identifier' is a null-terminated string with a URL [URL]
containing an email address, or a link to a location where an email
address can be found, that belongs to the organisation responsible
for this specific encryption method. Questions regarding the
encryption method should be sent to the indicated email address. The
'Method symbol' contains a value that is associated with this method
throughout the whole tag, in the range $80-F0. All other values are
reserved. The 'Method symbol' may optionally be followed by
encryption specific data. There may be several "ENCR" frames in a tag
but only one containing the same symbol and only one containing the
same owner identifier. The method must be used somewhere in the tag.
See the description of the frame encryption flag in the ID3v2
structure document [ID3v2-strct] for more information.

<Header for 'Encryption method registration', ID: "ENCR">
Owner identifier    <text string> $00
Method symbol       $xx
Encryption data     <binary data>

*/

CID3F_ENCR::CID3F_ENCR(void)
{
	init(EMPTY, 0);
}

CID3F_ENCR::CID3F_ENCR(LPCWSTR owner, BYTE symbol) 	
{
	init(owner, symbol);
}

void CID3F_ENCR::init(LPCWSTR owner, BYTE symbol)
{
	_frameID = F_ENCR;
	_owner = owner;
	_symbol = symbol;
	_data.Clear();
	mustRebuild = true;
	isDecoded = true;
}

CID3F_ENCR::~CID3F_ENCR(void)
{
}

bool CID3F_ENCR::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_ENCR *target = cENCR(frame);
	return (this->_owner.CompareNoCase(target->_owner) == 0
		&& this->_symbol == target->_symbol);
}

void CID3F_ENCR::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_data.Clear();
		if (_blob.GetLength() < 2)
		{
			_owner.Empty();
			_symbol = 0;			
		}
		else
		{
			int start = 0;
			_owner = _blob.getNextString(TEXT_ENCODED_ANSI, start);
			_symbol = _blob.GetAt(start++);
			_data.AddBlob(_blob, start);			
		}		
		isDecoded = true;
	}	
}
void CID3F_ENCR::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _owner, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddValue(_symbol);
		_blob.AddBlob(_data);
		mustRebuild = false;		
	}
}

CAtlString CID3F_ENCR::getOwner()
{
	decode();
	return _owner;
}

BYTE CID3F_ENCR::getSymbol()
{
	decode();
	return _symbol;
}

