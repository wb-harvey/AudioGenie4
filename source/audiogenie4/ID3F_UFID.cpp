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
#include "ID3F_UFID.h"
#include "id3_framefactory.h"

/* Unique file identifier
This frame's purpose is to be able to identify the audio file in a
database, that may provide more information relevant to the content.
Since standardisation of such a database is beyond this document, all
UFID frames begin with an 'owner identifier' field. It is a null-
terminated string with a URL [URL] containing an email address, or a
link to a location where an email address can be found, that belongs
to the organisation responsible for this specific database
implementation. Questions regarding the database should be sent to
the indicated email address. The URL should not be used for the
actual database queries. The string
"http://www.id3.org/dummy/ufid.html" should be used for tests. The
'Owner identifier' must be non-empty (more than just a termination).
The 'Owner identifier' is then followed by the actual identifier,
which may be up to 64 bytes. There may be more than one "UFID" frame
in a tag, but only one with the same 'Owner identifier'.

<Header for 'Unique file identifier', ID: "UFID">
Owner identifier        <text string> $00
Identifier              <up to 64 bytes binary data>
*/

CID3F_UFID::CID3F_UFID(void)
{
	init(EMPTY);
}

CID3F_UFID::CID3F_UFID(LPCWSTR owner) 	
{
	init(owner);
}

void CID3F_UFID::init(LPCWSTR owner)
{
	_frameID = F_UFID;
	_owner = owner;
	_data.Clear();
	mustRebuild = true;
	isDecoded = true;
}

CID3F_UFID::~CID3F_UFID(void)
{
}

bool CID3F_UFID::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_UFID *target = cUFID(frame);
	return (this->_owner.CompareNoCase(target->_owner) == 0);
}

void CID3F_UFID::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_data.Clear();
		if (_blob.GetLength() < 2)
		{
			_owner.Empty();
		}
		else
		{
			int start = 0;
			_owner = _blob.getNextString(TEXT_ENCODED_ANSI, start);
			_data.AddBlob(_blob, start);
		}		
		isDecoded = true;
	}	
}
void CID3F_UFID::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _owner, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddBlob(_data);
		mustRebuild = false;
	}
}

CAtlString CID3F_UFID::getOwner()
{
	decode();
	return _owner;
}
