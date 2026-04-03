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
#include "ID3F_USLT.h"
#include "id3_framefactory.h"

/*
This frame contains the lyrics of the song or a text transcription of
other vocal activities. The head includes an encoding descriptor and
a content descriptor. The body consists of the actual text. The
'Content descriptor' is a terminated string. If no descriptor is
entered, 'Content descriptor' is $00 (00) only. Newline characters
are allowed in the text. There may be more than one 'Unsynchronised
lyrics/text transcription' frame in each tag, but only one with the
same language and content descriptor.

<Header for 'Unsynchronised lyrics/text transcription', ID: "USLT">
Text encoding        $xx
Language             $xx xx xx
Content descriptor   <text string according to encoding> $00 (00)
Lyrics/text          <full text string according to encoding>
*/

CID3F_USLT::CID3F_USLT() 	
{
	init(EMPTY, EMPTY, EMPTY);
}

CID3F_USLT::CID3F_USLT(LPCWSTR newLanguage, LPCWSTR newDescription, LPCWSTR newText) 	
{
	init(newLanguage, newDescription, newText);
}

void CID3F_USLT::init(LPCWSTR newLanguage, LPCWSTR newDescription, LPCWSTR newText) 	
{
	_frameID = F_USLT;
	_language = (newLanguage != NULL) ? newLanguage : EMPTY;
	_description = (newDescription != NULL) ? newDescription : EMPTY;
	_text = (newText != NULL) ? newText : EMPTY;
	useTextEncoding = true;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_USLT::~CID3F_USLT(void)
{

}

bool CID3F_USLT::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_USLT *target = cUSLT(frame);
	return (this->_description.CompareNoCase(target->_description) == 0 && this->_language.CompareNoCase(target->_language) == 0);
}

void CID3F_USLT::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		if (_blob.GetLength() < 4)
		{
			_language.Empty();
			_text.Empty();
			_description.Empty();
		}
		else
		{
			encodingID = _blob.GetAt(0);
			_language = _blob.GetStringAt(1, 3);			
			int start = 4;
			_description = _blob.getNextString(encodingID, start);
			_text = _blob.getNextString(encodingID, start);
		}
		isDecoded = true;
	}	
}
void CID3F_USLT::encode()
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
void CID3F_USLT::setValues(LPCWSTR newLanguage, LPCWSTR newDescription, LPCWSTR newText)
{
	isDecoded = true;
	mustRebuild = true;
	_language = (newLanguage != NULL) ? newLanguage : EMPTY;
	_description = (newDescription != NULL) ? newDescription : EMPTY;
	_text = (newText != NULL) ? newText : EMPTY;
}

CAtlString CID3F_USLT::getText()
{
	decode();
	return _text;
}

CAtlString CID3F_USLT::getDescription()
{
	decode();
	return _description;
}

CAtlString CID3F_USLT::getLanguage()
{
	decode();
	return _language;
}