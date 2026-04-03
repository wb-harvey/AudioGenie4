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
#include "ID3F_WXXX.h"
#include "id3_framefactory.h"

/*
This frame is intended for URL [URL] links concerning the audio file
in a similar way to the other "W"-frames. The frame body consists
of a description of the string, represented as a terminated string,
followed by the actual URL. The URL is always encoded with ISO-8859-1
[ISO-8859-1]. There may be more than one "WXXX" frame in each tag,
but only one with the same description.

<Header for 'User defined URL link frame', ID: "WXXX">
Text encoding     $xx
Description       <text string according to encoding> $00 (00)
URL               <text string>
*/
CID3F_WXXX::CID3F_WXXX(void) 	
{
	init(EMPTY, EMPTY);	
}

CID3F_WXXX::CID3F_WXXX(LPCWSTR newDescription, LPCWSTR newUrl) 	
{
	init(newDescription, newUrl);
}

void CID3F_WXXX::init(LPCWSTR newDescription, LPCWSTR newUrl)
{
	_frameID = F_WXXX;
	_url = (newUrl != NULL) ? newUrl : EMPTY;
	_description = (newDescription != NULL) ? newDescription : EMPTY;
	useTextEncoding = true;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_WXXX::~CID3F_WXXX(void)
{

}
bool CID3F_WXXX::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_WXXX *target = cWXXX(frame);
	return (this->_description.CompareNoCase(target->_description) == 0);
}

void CID3F_WXXX::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		if (_blob.GetLength() < 2)
		{
			_url.Empty();
			_description.Empty();
		}
		else
		{
			encodingID = _blob.GetAt(0);
			int start = 1;
			_description = _blob.getNextString(encodingID, start);
			_url = _blob.getNextString(TEXT_ENCODED_ANSI, start);
		}		
	}	
}
void CID3F_WXXX::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddEncodedString(encodingID, _description, TEXT_WITH_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _url, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
		mustRebuild = false;
	}
}
void CID3F_WXXX::setValues(LPCWSTR newDescription, LPCWSTR newURL)
{
	isDecoded = true;
	mustRebuild = true;
	_url = (newURL != NULL) ? newURL : EMPTY;
	_description = (newDescription != NULL) ? newDescription : EMPTY;
}

CAtlString CID3F_WXXX::getURL()
{
	decode();
	return _url;
}

CAtlString CID3F_WXXX::getDescription()
{
	decode();
	return _description;
}