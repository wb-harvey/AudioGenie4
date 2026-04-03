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
#include "ID3F_TXXX.h"
#include "id3_framefactory.h"
/*
This frame is intended for one-string text information concerning the
audio file in a similar way to the other "T"-frames. The frame body
consists of a description of the string, represented as a terminated
string, followed by the actual string. There may be more than one
"TXXX" frame in each tag, but only one with the same description.

<Header for 'User defined text information frame', ID: "TXXX">
Text encoding     $xx
Description       <text string according to encoding> $00 (00)
Value             <text string according to encoding>
*/

CID3F_TXXX::CID3F_TXXX(void)
{
	init(EMPTY, EMPTY);
}

CID3F_TXXX::CID3F_TXXX(LPCWSTR description, LPCWSTR text) 	
{
	init(description, text);
}

void CID3F_TXXX::init(LPCWSTR description, LPCWSTR text)
{
	_frameID = F_TXXX;
	_description = (description != NULL) ? description : EMPTY;
	_text = (text != NULL) ? text : EMPTY;	
	useTextEncoding = true;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_TXXX::~CID3F_TXXX(void)
{
	_text.Empty();
	_description.Empty();
}

bool CID3F_TXXX::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_TXXX *target = cTXXX(frame);
	return (this->_description.CompareNoCase(target->_description) == 0);
}

void CID3F_TXXX::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_text.Empty();
		_description.Empty();
		if (_blob.GetLength() > 0)
		{
			encodingID = _blob.GetAt(0);
			if (_blob.GetLength() > 1)
			{
				int start = 1;
				_description = _blob.getNextString(encodingID, start);	
				_text = _blob.getNextString(encodingID, start);			
			}
		}
		isDecoded = true;
	}	
}
void CID3F_TXXX::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddEncodedString(encodingID, _description, TEXT_WITH_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddEncodedString(encodingID, _text, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
		mustRebuild = false;
	}
}

CAtlString CID3F_TXXX::getText()
{
	decode();
	return _text;
}

CAtlString CID3F_TXXX::getDescription()
{
	decode();
	return _description;
}