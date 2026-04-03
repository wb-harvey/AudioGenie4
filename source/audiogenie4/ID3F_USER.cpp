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
#include "ID3F_USER.h"
#include "id3_framefactory.h"

/*
Terms of use frame

This frame contains a brief language of the terms of use and
ownership of the file. More detailed information concerning the legal
terms might be available through the "WCOP" frame. Newlines are
allowed in the text. There may be more than one 'Terms of use' frame
in a tag, but only one with the same 'Language'.

<Header for 'Terms of use frame', ID: "USER">
Text encoding        $xx
Language             $xx xx xx
The actual text      <text string according to encoding>
*/

CID3F_USER::CID3F_USER(void)
{
	init(EMPTY, EMPTY);
}

CID3F_USER::CID3F_USER(LPCWSTR text, LPCWSTR language) 	
{
	init(text, language);
}

void CID3F_USER::init(LPCWSTR text, LPCWSTR language)
{
	_frameID = F_USER;
	_text = (text != NULL) ? text : EMPTY;
	_language = (language != NULL) ? language : EMPTY;
	useTextEncoding = true;
	mustRebuild = true;
	isDecoded = true;
}

void CID3F_USER::setValues(LPCWSTR newText, LPCWSTR newLanguage)
{
	isDecoded = true;
	mustRebuild = true;
	_language = (newLanguage != NULL) ? newLanguage : EMPTY;
	_text = (newText != NULL) ? newText : EMPTY;
}

CID3F_USER::~CID3F_USER(void)
{
}

bool CID3F_USER::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_USER *target = cUSER(frame);
	return (this->_language.CompareNoCase(target->_language) == 0);
}

void CID3F_USER::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		if (_blob.GetLength() < 2)
		{
			_text.Empty();
			_language.Empty();
		}
		else
		{
			int start = 0;
			encodingID = _blob.GetAt(start++);
			_language = _T("   ");
			_language.SetAt(0, _blob.GetAt(start++));
			_language.SetAt(1, _blob.GetAt(start++));
			_language.SetAt(2, _blob.GetAt(start++));
			_text = _blob.getNextString(encodingID, start);	
		}		
		isDecoded = true;
	}	
}
void CID3F_USER::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddValue(encodingID);
		_blob.AddFixedAnsiString(_language, 3);
		_blob.AddEncodedString(encodingID, _text, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
		mustRebuild = false;
	}
}

CAtlString CID3F_USER::getText()
{
	decode();
	return _text;
}

CAtlString CID3F_USER::getLanguage()
{
	decode();
	return _language;
}