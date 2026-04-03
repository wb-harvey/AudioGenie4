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
#include "WAVCARTChunk.h"
#include "Tools.h"

/*
CHAR Version[4]; // Version of the data structure
CHAR Title[64]; // ASCII title of cart audio sequence
CHAR Artist[64]; // ASCII artist or creator name
CHAR CutID[64]; // ASCII cut number identification
CHAR ClientID[64]; // ASCII client identification
CHAR Category[64]; // ASCII Category ID, PSA, NEWS, etc
CHAR Classification[64]; // ASCII Classification or auxiliary key
CHAR OutCue[64]; // ASCII out cue text
CHAR StartDate[10]; // ASCII YYYY-MM-DD
CHAR StartTime[8]; // ASCII hh:mm:ss
CHAR EndDate[10]; // ASCII YYYY-MM-DD
CHAR EndTime[8]; // ASCII hh:mm:ss
CHAR ProducerAppID[64]; // Name of vendor or application
CHAR ProducerAppVersion[64]; // Version of producer application
CHAR UserDef[64]; // User defined text
DWORD dwLevelReference // Sample value for 0 dB reference
CART_TIMER PostTimer[8]; // 8 time markers after head
CHAR Reserved[276]; // Reserved for future expansion
CHAR URL[1024]; // Uniform resource locator
CHAR TagText[];
*/
CWAVCARTChunk::CWAVCARTChunk(void)
{
	_chunkID = 'cart';
}

CWAVCARTChunk::~CWAVCARTChunk(void)
{
}
CAtlString CWAVCARTChunk::getASCIIText(BYTE nr)
{
	if (nr > WAV_CART_TAGTEXT)
	{
		CTools::instance().setLastError(ERR_ID_OUT_OF_RANGE);
		return EMPTY;
	}
	size_t startPos = CART_CODES[nr][0];
	size_t maxLen = CART_CODES[nr][1];
	if (startPos > _data.GetLength() )
		return EMPTY;
	CAtlString result;
	if (nr == WAV_CART_LEVELREFERENCE)
	{
		result.Format(_T("%u"), _data.GetR4B(startPos));
		return result;
	}
	if (nr >= WAV_CART_POSTTIMER0 && nr <= WAV_CART_POSTTIMER7)
	{
		if (_data.GetAt(startPos) == 0 || _data.GetAt(startPos+1) == 0 || _data.GetAt(startPos+2) == 0 || _data.GetAt(startPos+3) == 0)
			return EMPTY;
		result.Format(_T("%c%c%c%c:%u"), _data.GetAt(startPos), _data.GetAt(startPos+1), _data.GetAt(startPos + 2), _data.GetAt(startPos+3),_data.GetR4B(startPos+4));
		return result;
	}
	CBlob tmp;
	if (maxLen == 0)
		tmp.AddMemory(_data.m_pData + startPos, _data.GetLength() - startPos);
	else
		tmp.AddMemory(_data.m_pData + startPos, maxLen);
	int start = 0;
	result = tmp.getNextString(TEXT_ENCODED_ANSI, start);
	return result.Trim();
}

void CWAVCARTChunk::setASCIIText(CAtlString newText, BYTE nr)
{
	if (nr > WAV_CART_TAGTEXT)
	{
		CTools::instance().setLastError(ERR_ID_OUT_OF_RANGE);
		return;
	}
	size_t startPos = CART_CODES[nr][0];
	size_t maxLen = CART_CODES[nr][1];
	CBlob tmp;
	// Auf 2048 bytes auffüllen
	if (_data.GetLength() < 2048)
	{
		_data.Clear();
		_data.AddValue(0, 2048 - _data.GetLength());
	}
	if (nr == WAV_CART_LEVELREFERENCE)
	{
		tmp.AddR4B(_wtoi(newText));
		memcpy(_data.m_pData + startPos, tmp.m_pData, 4);
		return;
	}
	if (nr >= WAV_CART_POSTTIMER0 && nr <= WAV_CART_POSTTIMER7)
	{   // 4 bytes Identifier 
		// :
		// Zahl
		if (newText.GetLength() >  4 && newText.GetAt(4) == ':')
		{
			tmp.AddValue((BYTE)newText.GetAt(0));
			tmp.AddValue((BYTE)newText.GetAt(1));
			tmp.AddValue((BYTE)newText.GetAt(2));
			tmp.AddValue((BYTE)newText.GetAt(3));
			tmp.AddR4B(_wtoi(newText.Mid(5)));
		}
		else
			tmp.AddValue(0, 8);
		memcpy(_data.m_pData + startPos, tmp.m_pData, 8);
		return;
	}
	if (nr == WAV_CART_TAGTEXT)
	{
		// Alle Bytes ab Pos 2048 löschen
		tmp.AddMemory(_data.m_pData, 2048);
		if (!newText.IsEmpty())
			tmp.AddEncodedString(TEXT_ENCODED_ANSI, newText, false, false);
		_data.Clear();
		_data.AddBlob(tmp);
		return;
	}
	tmp.AddFixedAnsiString(newText, maxLen);
	memcpy(_data.m_pData + startPos, tmp.m_pData, maxLen);
}
