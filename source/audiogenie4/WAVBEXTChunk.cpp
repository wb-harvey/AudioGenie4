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
#include "WAVBEXTChunk.h"
#include "Tools.h"

/* 
CHAR Description[256];  ASCII : Description of the sound sequence
CHAR Originator[32];  ASCII : Name of the originator
CHAR OriginatorReference[32];  ASCII : Reference of the originator
CHAR OriginationDate[10];  ASCII : yyyy-mm-dd 
CHAR OriginationTime[8];  ASCII : hh-mm-ss
DWORD TimeReferenceLow;  First sample count since midnight low word 
DWORD TimeReferenceHigh;  First sample count since midnight, high word 
WORD Version;  Version of the BWF; unsigned binary number 
BYTE UMID_0  Binary byte 0 of SMPTE UMID 
....
BYTE UMID_63  Binary byte 63 of SMPTE UMID 
BYTE Reserved[190] ;  190 bytes, reserved for future use, set to NULL
CHAR CodingHistory[];  ASCII :  History coding  
*/


CWAVBEXTChunk::CWAVBEXTChunk(void)
{
	_chunkID = 'bext';
}


CWAVBEXTChunk::~CWAVBEXTChunk(void)
{
}

CAtlString CWAVBEXTChunk::getASCIIText(BYTE nr)
{
	if (nr > WAV_BEXT_CODINGHISTORY)
	{
		CTools::instance().setLastError(ERR_ID_OUT_OF_RANGE);
		return EMPTY;
	}
	size_t startPos = BEXT_CODES[nr][0];
	size_t maxLen = BEXT_CODES[nr][1];
	if (startPos > _data.GetLength() )
		return EMPTY;
	CAtlString result;
	if (nr == WAV_BEXT_TIMEREFERENCE)
	{
		result.Format(_T("%I64u"), _data.GetR8B(startPos));
		return result;
	}
	if (nr == WAV_BEXT_VERSION)
	{
		result.Format(_T("%u"), _data.GetR2B(startPos));
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

void CWAVBEXTChunk::setASCIIText(CAtlString newText, BYTE nr)
{
	if (nr > WAV_BEXT_CODINGHISTORY)
	{
		CTools::instance().setLastError(ERR_ID_OUT_OF_RANGE);
		return;
	}
	size_t startPos = BEXT_CODES[nr][0];
	size_t maxLen = BEXT_CODES[nr][1];
	CBlob tmp;
	// Auf 602 bytes auffüllen
	if (_data.GetLength() < 602)
	{
		_data.Clear();
		_data.AddValue(0, 602 - _data.GetLength());
	}
	if (nr == WAV_BEXT_TIMEREFERENCE)
	{
		tmp.AddR8B(_wtoi64(newText));
		memcpy(_data.m_pData + startPos, tmp.m_pData, 8);
		return;
	}
	if (nr == WAV_BEXT_VERSION)
	{
		tmp.AddR2B(_wtoi(newText));
		memcpy(_data.m_pData + startPos, tmp.m_pData, 2);
		return;
	}
	if (nr == WAV_BEXT_CODINGHISTORY )
	{
		// Alle Bytes ab Pos 602 löschen
		tmp.AddMemory(_data.m_pData, startPos);
		if (!newText.IsEmpty())
			tmp.AddEncodedString(TEXT_ENCODED_ANSI, newText, false, false);
		_data.Clear();
		_data.AddBlob(tmp);
		return;
	}
	tmp.AddFixedAnsiString(newText, maxLen);
	memcpy(_data.m_pData + startPos, tmp.m_pData, maxLen);
}
