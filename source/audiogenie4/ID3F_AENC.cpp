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
#include "ID3F_AENC.h"
#include "id3_framefactory.h"
// Fertig
/* 
This frame indicates if the actual audio stream is encrypted, and by
whom. Since standardisation of such encryption scheme is beyond this
document, all "AENC" frames begin with a terminated string with a
URL containing an email address, or a link to a location where an
email address can be found, that belongs to the organisation
responsible for this specific encrypted audio file. Questions
regarding the encrypted audio should be sent to the email address
specified. If a $00 is found directly after the 'Frame size' and the
audio file indeed is encrypted, the whole file may be considered
useless.

After the 'Owner identifier', a pointer to an unencrypted part of the
audio can be specified. The 'Preview start' and 'Preview length' is
described in frames. If no part is unencrypted, these fields should
be left zeroed. After the 'preview length' field follows optionally a
data block required for decryption of the audio. There may be more
than one "AENC" frames in a tag, but only one with the same 'Owner
identifier'.

<Header for 'Audio encryption', ID: "AENC">
Owner identifier   <text string> $00
Preview start      $xx xx
Preview length     $xx xx
Encryption info    <binary data>
*/

CID3F_AENC::CID3F_AENC(void)
{
	init(EMPTY, 0, 0);	
}

CID3F_AENC::CID3F_AENC(LPCWSTR identifier, unsigned __int32 start, unsigned __int32 length) 	
{
	init(identifier, start, length);	
}

void CID3F_AENC::init(LPCWSTR identifier, unsigned __int32 start, unsigned __int32 length)
{
	_frameID = F_AENC;
	_identifier = identifier;
	_start = start;
	_length = length;
	_data.Clear();
	mustRebuild = true;
	isDecoded = true;
}

CID3F_AENC::~CID3F_AENC(void)
{
}

bool CID3F_AENC::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	CID3F_AENC *target = cAENC(frame);
	return (this->_identifier.CompareNoCase(target->_identifier) == 0);
}

void CID3F_AENC::decode()
{
	if (!isDecoded)
	{
		if (isUnsynchronized())
			resync();
		_data.Clear();
		if (_blob.GetLength() < 6)
		{
			_identifier.Empty();
			_start = 0;
			_length = 0;
			
		}
		else
		{
			int st = 0;
			_identifier = _blob.getNextString(TEXT_ENCODED_ANSI, st);
			_start = (unsigned __int32) _blob.Get2B(st);
			st+=2;
			_length = (unsigned __int32) _blob.Get2B(st);
			st+=2;
			_data.AddBlob(_blob, st);			
		}		
		isDecoded = true;
	}	
}
void CID3F_AENC::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _identifier, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.Add2B(_start);
		_blob.Add2B(_length);
		_blob.AddBlob(_data);
		mustRebuild = false;		
	}
}

CAtlString CID3F_AENC::getIdentifier()
{
	decode();
	return _identifier;
}

u32 CID3F_AENC::getStart()
{
	decode();
	return _start;
}

u32 CID3F_AENC::getLength()
{
	decode();
	return _length;
}
