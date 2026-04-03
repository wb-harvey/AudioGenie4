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
#include "ID3F_W000.h"

/*
With these frames dynamic data such as webpages with touring
information, price information or plain ordinary news can be added to
the tag. There may only be one URL [URL] link frame of its kind in an
tag, except when stated otherwise in the frame description. If the
text string is followed by a string termination, all the following
information should be ignored and not be displayed. All URL link
frame identifiers begins with "W". Only URL link frame identifiers
begins with "W", except for "WXXX". All URL link frames have the
following format:

<Header for 'URL link frame', ID: "W000" - "WZZZ", excluding "WXXX" described in 4.3.2.>
URL              <text string>
URLs always in ISO-8859-1

*/
CID3F_W000::CID3F_W000(void)
{
	init(0, EMPTY);		
}

CID3F_W000::CID3F_W000(unsigned int frameID) 	
{
	init(frameID, EMPTY);	
}

CID3F_W000::CID3F_W000(unsigned int frameID, LPCWSTR newUrl) 
{
	init(frameID, newUrl);
	isDecoded = true;
}

void CID3F_W000::init(unsigned int frameID, LPCWSTR newUrl)
{
	_frameID = frameID;
	_url = (newUrl != NULL) ? newUrl : EMPTY;	
	mustRebuild = true;
	isDecoded = true;
}


CID3F_W000::~CID3F_W000(void)
{

}

void CID3F_W000::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		int start = 0;
		_url = _blob.getNextString(TEXT_ENCODED_ANSI, start);
		isDecoded = true;
	}	
}
void CID3F_W000::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _url, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
		mustRebuild = false;
	}
}

void CID3F_W000::setURL(LPCWSTR newURL)
{
	isDecoded = true;
	mustRebuild = true;
	_url = newURL;
}

CAtlString CID3F_W000::getURL()
{
	decode();
	return _url;
}
