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
#include "ID3F_T000.h"

/*
<Header for 'Text information frame', ID: "T000" - "TZZZ", excluding "TXXX">
Text encoding                $xx
Information                  <text string according to encoding>
*/

CID3F_T000::CID3F_T000(void) 	
{
	init(0, EMPTY);	
}

CID3F_T000::CID3F_T000(unsigned int frameID) 	
{
	init(frameID, EMPTY);
}

CID3F_T000::CID3F_T000(unsigned int frameID, LPCWSTR newText) 	
{
	init(frameID, newText);	
}

void CID3F_T000::init(unsigned int frameID, LPCWSTR newText)
{
	_frameID = frameID;
	if (newText != NULL)
		_text = newText;
	else
		_text.Empty();
	useTextEncoding = true;
	mustRebuild = true;
	isDecoded = true;
}


CID3F_T000::~CID3F_T000(void)
{
	_text.Empty();
}

void CID3F_T000::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_text.Empty();
		if (_blob.GetLength() > 0)
		{
			encodingID = _blob.GetAt(0);
			if (_blob.GetLength() > 1)
			{
				int start = 1;
				_text = _blob.getNextString(encodingID, start);
			}
		}
		isDecoded = true;
	}	
}

void CID3F_T000::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddEncodedString(encodingID, _text, TEXT_WITH_ENCODING, TEXT_WITHOUT_NULLBYTES);
		mustRebuild = false;
	}
}

CAtlString CID3F_T000::getText()
{
	decode();
	return _text;
}

void CID3F_T000::print()
{
	ATLTRACE(_T("Frame:%c%c%c%c %u "), BYTE(_frameID >> 24), BYTE(_frameID >> 16), BYTE(_frameID >> 8), BYTE(_frameID), _frameID);
	decode();
	ATLTRACE(_T(" Encoding:%i Text:%s\n"), encodingID,_text);
}