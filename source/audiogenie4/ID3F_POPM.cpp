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
#include "ID3F_POPM.h"
#include "id3_framefactory.h"

/*
The purpose of this frame is to specify how good an audio file is.
Many interesting applications could be found to this frame such as a
playlist that features better audio files more often than others or
it could be used to profile a person's taste and find other 'good'
files by comparing people's profiles. The frame contains the email
address to the user, one rating byte and a four byte play counter,
intended to be increased with one for every time the file is played.
The email is a terminated string. The rating is 1-255 where 1 is
worst and 255 is best. 0 is unknown. If no personal counter is wanted
it may be omitted. When the counter reaches all one's, one byte is
inserted in front of the counter thus making the counter eight bits
bigger in the same away as the play counter ("PCNT"). There may be
more than one "POPM" frame in each tag, but only one with the same
email address.

<Header for 'Popularimeter', ID: "POPM">
Email to user   <text string> $00
Rating          $xx
Counter         $xx xx xx xx (xx ...)

*/

CID3F_POPM::CID3F_POPM(void)
{
	init(EMPTY, 0, 0);
}

CID3F_POPM::CID3F_POPM(LPCWSTR email, BYTE rating, u32 counter) 	
{
	init(email, rating, counter);
}

void CID3F_POPM::init(LPCWSTR email, BYTE rating, u32 counter)
{
	_frameID = F_POPM;
	_email = (email != NULL) ? email : EMPTY;
	_rating = rating;
	_counter = counter;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_POPM::~CID3F_POPM(void)
{
}

bool CID3F_POPM::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_POPM *target = cPOPM(frame);
	return (this->_email.CompareNoCase(target->_email) == 0);
}

void CID3F_POPM::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		if (_blob.GetLength() < 2)
		{
			_email.Empty();
			_rating = 0;
			_counter = 0;
		}
		else
		{
			int start = 0;
			_email = _blob.getNextString(TEXT_ENCODED_ANSI, start);
			_rating = _blob.GetAt(start++);
			_counter = _blob.Get4B(start);
		}		
		isDecoded = true;
	}	
}

void CID3F_POPM::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _email, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddValue(_rating);
		_blob.Add4B(_counter);
		mustRebuild = false;
	}
}

CAtlString CID3F_POPM::getEmail()
{
	decode();
	return _email;
}

BYTE CID3F_POPM::getRating()
{
	decode();
	return _rating;
}

u32 CID3F_POPM::getCounter()
{
	decode();
	return _counter;
}

void CID3F_POPM::setValues(LPCWSTR email, BYTE rating, u32 counter)
{
	init(email, rating, counter);
	isDecoded = true;
	mustRebuild = true;
}

void CID3F_POPM::increaseCounter()
{
	decode();
	_counter++;
	mustRebuild = true;
}