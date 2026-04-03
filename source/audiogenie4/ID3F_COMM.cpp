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

#include "StdAfx.h"
#include "ID3F_COMM.h"
#include "id3_framefactory.h"
#include "ID3v2.h"

/*
This frame is intended for any kind of full text information that
does not fit in any other frame. It consists of a frame header
followed by encoding, language and content descriptors and is ended
with the actual comment as a text string. Newline characters are
allowed in the comment text string. There may be more than one
comment frame in each tag, but only one with the same language and
content descriptor.

<Header for 'Comment', ID: "COMM">
Text encoding          $xx
Language               $xx xx xx
Short content descrip. <text string according to encoding> $00 (00)
The actual text        <full text string according to encoding>
*/

CID3F_COMM::CID3F_COMM(void) 	
{
	init(EMPTY, EMPTY, EMPTY);
}

CID3F_COMM::CID3F_COMM(LPCWSTR newLanguage, LPCWSTR newDescription, LPCWSTR newText) 	
{
	init(newLanguage, newDescription, newText);	
}

void CID3F_COMM::init(LPCWSTR newLanguage, LPCWSTR newDescription, LPCWSTR newText) 	
{
	_frameID = F_COMM;
	_text = (newText != NULL) ? newText : EMPTY;
	_description = (newDescription != NULL) ? newDescription : EMPTY;
	_language = (newLanguage != NULL) ? newLanguage : EMPTY;
	useTextEncoding = true;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_COMM::~CID3F_COMM(void)
{

}

bool CID3F_COMM::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_COMM *target = cCOMM(frame);
	return (this->_description.CompareNoCase(target->_description) == 0 && this->_language.CompareNoCase(target->_language) == 0);
}

void CID3F_COMM::decode()
{
	if (!isDecoded)
	{	
		if (_blob.GetLength() < 4)
		{
			_language.Empty();
			_text.Empty();
			_description.Empty();
		}
		else
		{
			encodingID = _blob.GetAt(0);
			_language = _T("   ");
			_language.SetAt(0, _blob.GetAt(1));
			_language.SetAt(1, _blob.GetAt(2));
			_language.SetAt(2, _blob.GetAt(3)); 
			int start = 4;
			_description = _blob.getNextString(encodingID, start);		
			_text = _blob.getNextString(encodingID, start);
		}
		isDecoded = true;
	}	
}

void CID3F_COMM::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddValue(encodingID);
		_blob.AddFixedAnsiString(_language, 3);
		_blob.AddEncodedString(encodingID, _description, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddEncodedString(encodingID, _text, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
		mustRebuild = false;
	}
}
void CID3F_COMM::setValues(LPCWSTR newLanguage, LPCWSTR newDescription, LPCWSTR newText)
{
	isDecoded = true;
	mustRebuild = true;
	_text = newText;
	_description = newDescription;
	_language = newLanguage;
}

CAtlString CID3F_COMM::getText()
{
	decode();
	return _text;
}

CAtlString CID3F_COMM::getDescription()
{
	decode();
	return _description;
}

CAtlString CID3F_COMM::getLanguage()
{
	decode();
	return _language;
}