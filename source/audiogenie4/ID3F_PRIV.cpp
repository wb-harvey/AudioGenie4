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
#include "ID3F_PRIV.h"
#include "id3_framefactory.h"


/*
Private frame
This frame is used to contain information from a software producer
that its program uses and does not fit into the other frames. The
frame consists of an 'Owner identifier' string and the binary data.
The 'Owner identifier' is a null-terminated string with a URL [URL]
containing an email address, or a link to a location where an email
address can be found, that belongs to the organisation responsible
for the frame. Questions regarding the frame should be sent to the
indicated email address. The tag may contain more than one "PRIV"
frame but only with different contents.

<Header for 'Private frame', ID: "PRIV">
Owner identifier      <text string> $00
The private data      <binary data>
*/

CID3F_PRIV::CID3F_PRIV(void)
{
	init(EMPTY);
}

CID3F_PRIV::CID3F_PRIV(LPCWSTR URL) 	
{
	init(URL);
}

void CID3F_PRIV::init(LPCWSTR URL)
{
	_frameID = F_PRIV;
	_url = URL;
	_data.Clear();
	mustRebuild = true;
	isDecoded = true;
}

CID3F_PRIV::~CID3F_PRIV(void)
{
}

bool CID3F_PRIV::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_PRIV *target = cPRIV(frame);
	return (this->_url.CompareNoCase(target->_url) == 0);
}

void CID3F_PRIV::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_data.Clear();
		if (_blob.GetLength() < 2)
		{
			_url.Empty();			
		}
		else
		{
			int start = 0;
			_url = _blob.getNextString(TEXT_ENCODED_ANSI, start);
			_data.AddBlob(_blob, start);	
		}		
		isDecoded = true;
	}	
}
void CID3F_PRIV::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _url, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddBlob(_data);
		mustRebuild = false;
	}
}

CAtlString CID3F_PRIV::getURL()
{
	decode();
	return _url;
}
