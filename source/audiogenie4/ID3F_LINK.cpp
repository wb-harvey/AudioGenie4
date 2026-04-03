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
#include "ID3F_LINK.h"
#include "id3_framefactory.h"

/* Linked information

To keep information duplication as low as possible this frame may be
used to link information from another ID3v2 tag that might reside in
another audio file or alone in a binary file. It is RECOMMENDED that
this method is only used when the files are stored on a CD-ROM or
other circumstances when the risk of file separation is low. The
frame contains a frame identifier, which is the frame that should be
linked into this tag, a URL [URL] field, where a reference to the
file where the frame is given, and additional ID data, if needed.
Data should be retrieved from the first tag found in the file to
which this link points. There may be more than one "LINK" frame in a
tag, but only one with the same contents. A linked frame is to be
considered as part of the tag and has the same restrictions as if it
was a physical part of the tag (i.e. only one "RVRB" frame allowed,
whether it's linked or not).

<Header for 'Linked information', ID: "LINK">
Frame identifier        $xx xx xx xx
URL                     <text string> $00
ID and additional data  <text string(s)>

Frames that may be linked and need no additional data are "ASPI",
"ETCO", "EQU2", "MCID", "MLLT", "OWNE", "RVA2", "RVRB", "SYTC", the
text information frames and the URL link frames.

The "AENC", "APIC", "GEOB" and "TXXX" frames may be linked with
the content descriptor as additional ID data.

The "USER" frame may be linked with the language field as additional
ID data.

The "PRIV" frame may be linked with the owner identifier as
additional ID data.

The "COMM", "SYLT" and "USLT" frames may be linked with three bytes
of language descriptor directly followed by a content descriptor as
additional ID data.
*/

CID3F_LINK::CID3F_LINK(void)
{
	init(0, EMPTY, EMPTY);
}

CID3F_LINK::CID3F_LINK(long identifier, LPCWSTR URL, LPCWSTR additional) 	
{
	init(identifier, URL, additional);
}

void CID3F_LINK::init(long identifier, LPCWSTR URL, LPCWSTR additional)
{
	_frameID = F_LINK;
	_identifier = identifier;
	_additional = additional;
	_URL = URL;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_LINK::~CID3F_LINK(void)
{
}

bool CID3F_LINK::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_LINK *target = cLINK(frame);
	return ((this->_identifier == target->_identifier) && (this->_URL.CompareNoCase(target->_URL) == 0));
}

void CID3F_LINK::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		if (_blob.GetLength() < 5)
		{
			_identifier = 0;
			_URL.Empty();
			_additional.Empty();
		}
		else
		{
			_identifier = _blob.Get4B(0);
			int start = 4;
			_URL = _blob.getNextString(TEXT_ENCODED_ANSI, start);
			_additional = _blob.getNextString(TEXT_ENCODED_ANSI, start);
		}		
		isDecoded = true;
	}	
}
void CID3F_LINK::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.Add4B(_identifier);
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _URL, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _additional, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
		mustRebuild = false;	
	}
}

CAtlString CID3F_LINK::getURL()
{
	decode();
	return _URL;
}

CAtlString CID3F_LINK::getAdditional()
{
	decode();
	return _additional;
}

long CID3F_LINK::getIdentifier()
{
	decode();
	return _identifier;
}