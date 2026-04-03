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
#include "ID3F_GEOB.h"
#include "id3_framefactory.h"

/*
In this frame any type of file can be encapsulated. After the header,
'Frame size' and 'Encoding' follows 'MIME type' [MIME] represented as
as a terminated string encoded with ISO 8859-1 [ISO-8859-1]. The
filename is case sensitive and is encoded as 'Encoding'. Then follows
a content description as terminated string, encoded as 'Encoding'.
The last thing in the frame is the actual object. The first two
strings may be omitted, leaving only their terminations. MIME type is
always an ISO-8859-1 text string. There may be more than one "GEOB"
frame in each tag, but only one with the same content descriptor.

<Header for 'General encapsulated object', ID: "GEOB">
Text encoding          $xx
MIME type              <text string> $00
Filename               <text string according to encoding> $00 (00)
Content description    <text string according to encoding> $00 (00)
Encapsulated object    <binary data>
*/

CID3F_GEOB::CID3F_GEOB(void)
{
	init(EMPTY, EMPTY, EMPTY);
}

CID3F_GEOB::CID3F_GEOB(LPCWSTR mime, LPCWSTR filename , LPCWSTR description) 	
{
	init(mime, filename, description);
}

void CID3F_GEOB::init(LPCWSTR mime, LPCWSTR filename , LPCWSTR description)
{
	_frameID = F_GEOB;
	_mime = mime;
	_filename = filename;
	_description = description;
	useTextEncoding = true;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_GEOB::~CID3F_GEOB(void)
{
}

bool CID3F_GEOB::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_GEOB *target = cGEOB(frame);
	return (this->_description.CompareNoCase(target->_description) == 0);
}

void CID3F_GEOB::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_data.Clear();
		if (_blob.GetLength() < 4)
		{
			_mime.Empty();
			_filename.Empty();
			_description.Empty();		
		}
		else
		{
			encodingID = _blob.GetAt(0);
			int start = 1;
			_mime = _blob.getNextString(TEXT_ENCODED_ANSI, start);
			_filename = _blob.getNextString(encodingID, start);	
			_description = _blob.getNextString(encodingID, start);	
			_data.AddBlob(_blob, start);
		}		
		isDecoded = true;
	}	
}
void CID3F_GEOB::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddValue(encodingID);
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _mime, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddEncodedString(encodingID, _filename, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddEncodedString(encodingID, _description, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddBlob(_data);
		mustRebuild = false;		
	}
}

CAtlString CID3F_GEOB::getMime()
{
	decode();
	return _mime;
}

CAtlString CID3F_GEOB::getFilename()
{
	decode();
	return _filename;
}

CAtlString CID3F_GEOB::getDescription()
{
	decode();
	return _description;
}

