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
#include "WAVDISPChunk.h"
#include "Tools.h"

/*A DISP chunk contains easily rendered and displayable objects associated
with an instance of a more complex object in a RIFF form (e.g. sound file, 
AVI movie).
     
A DISP chunk is defined as follows:
     
<DISP_ck>  ->  DISP( <type> <data> )
     
<type> is a DWORD (32 bit unsigned quantity in Intel format) that
identifies <data> as one of the standard Windows clipboard formats 
(CF_METAFILE, CF_DIB, CF_TEXT, etc.) as defined in windows.h.
     
The DISP chunk should be used as a direct child of the RIFF chunk so that
any RIFF aware application can find it.  There can be multiple DISP chunks 
with each containing different types of displayable data, but all 
representative of the same object.  The DISP chunks should be stored
in the file in order of preference (just as in the clipboard). */

/* CF_LINK = &HBF00
       CF_TEXT = 1
       CF_BITMAP = 2
       CF_METAFILE = 3
       CF_DIB = 8
       CF_PALETTE = 9 */

CWAVDISPChunk::CWAVDISPChunk(void)
{
	_chunkID = 'DISP';
	_type = CF_TEXT;
	_data.Clear();
}


CWAVDISPChunk::~CWAVDISPChunk(void)
{
}

bool CWAVDISPChunk::load(FILE *Stream, u32 offset, u32 size)
{
	ATLTRACE(_T("DISP chunk with %i bytes at Offset %u\n"), size, offset);
	fseek(Stream, offset, SEEK_SET);
	_data.FileRead(size, Stream);
	_type = _data.GetR4B(0);
	return true;
}

CAtlString CWAVDISPChunk::getText()
{
	if (_type == CF_TEXT && _data.GetLength() > 4)
	{
		int start = 4;
		return _data.getNextString(TEXT_ENCODED_ANSI, start);
	}
	return EMPTY;
}

void CWAVDISPChunk::setText(LPCWSTR text)
{
	_data.Clear();
	if (wcslen(text) > 0)
	{
		_data.AddR4B(CF_TEXT);
		_data.AddEncodedString(TEXT_ENCODED_ANSI, text, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
	}
}


