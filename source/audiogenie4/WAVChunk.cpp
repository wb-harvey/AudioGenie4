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
#include "WAVChunk.h"
#include <io.h>

CWAVChunk::CWAVChunk(void)
{
	_data.Clear();
	_chunkID = 0;
}

CWAVChunk::CWAVChunk(u32 ID)
{
	_data.Clear();
	_chunkID = ID;
}
CWAVChunk::~CWAVChunk(void)
{
}
void CWAVChunk::Remove()
{
	_data.Clear();
}
size_t CWAVChunk::getSize()
{
	if (_data.GetLength() == 0)
		return 0;
	size_t size = _data.GetLength() + 8;
	if (_data.GetLength() % 2 == 1)
		size++;
	return size;
	// return _data.GetLength() + 8;
}

CWAVChunk* CWAVChunk::find(u32 ID)
{
	return (ID == _chunkID) ? this : NULL;
}

bool CWAVChunk::load(FILE *Stream, u32 offset, u32 size)
{
	ATLTRACE(_T("Chunk %c%c%c%c with %i bytes at Offset %u\n"), BYTE(_chunkID >> 24), BYTE(_chunkID >> 16), BYTE(_chunkID >> 8), BYTE(_chunkID), size, offset);
	_data.Clear();
	// 4.0.4.7: the size comes from the file; only read what the file holds, and a failed allocation is a failed
	// load, not an exception out of the DLL (4.0.4.6 and earlier: fseek with a long, unbounded read)
	__int64 fileLen = _filelengthi64(_fileno(Stream));
	if (fileLen < 0 || (unsigned __int64)offset + size > (unsigned __int64)fileLen)
		return false;
	if (_fseeki64(Stream, offset, SEEK_SET) != 0)
		return false;
	try
	{
		_data.FileRead(size, Stream);
	}
	catch (...)
	{
		_data.Clear();
		return false;
	}
	return (_data.GetLength() == size);
}

void CWAVChunk::save(CBlob *blob)
{
	if (_data.GetLength() > 0)
	{
		blob->Add4B(_chunkID);
		blob->AddR4B((int)_data.GetLength());
		blob->AddBlob(_data);
		if (_data.GetLength() % 2 == 1)
			blob->AddNullByte();
	}

}

bool CWAVChunk::save(FILE* Source, FILE *Destination)
{
	Source;
	bool result = true;
	if (_data.GetLength() > 0)
	{
		CBlob *blob = new CBlob(_data.GetLength() + 10);
		save(blob);
		result = (blob->FileWrite(blob->GetLength(), Destination) == blob->GetLength());
		delete blob;
	}
	return result;
}

/* -------------------------------------------------------------------------- */

// 4.0.4.7: INFO, DISP, bext and cart text has no encoding marker and was always written in the PC's code page, so
// Hebrew became "?" on a Windows-1252 PC (4.0.4.6 and earlier). When the PC's code page can't hold the text and
// every non-ASCII character is Hebrew, it is written as Windows-1255, which the reader (CBlob ansiCodePage)
// recognises; anything else is written as before.
static bool WAVUseHebrewCodePage(const CAtlString &text)
{
	if (GetACP() == CP_UTF8)
		return false;
	bool nonAscii = false;
	for (int i = 0; i < text.GetLength(); i++)
	{
		wchar_t c = text.GetAt(i);
		if (c < 0x80)
			continue;
		if (c < 0x0591 || c > 0x05F4)
			return false;
		nonAscii = true;
	}
	if (!nonAscii)
		return false;
	BOOL usedDefault = FALSE;
	if (WideCharToMultiByte(CP_ACP, 0, text, text.GetLength(), NULL, 0, NULL, &usedDefault) <= 0 || !usedDefault)
		return false;
	usedDefault = FALSE;
	return (WideCharToMultiByte(1255, 0, text, text.GetLength(), NULL, 0, NULL, &usedDefault) > 0 && !usedDefault);
}

static bool WAVAddHebrewString(CBlob *blob, const CAtlString &text)
{
	int len = WideCharToMultiByte(1255, 0, text, text.GetLength(), NULL, 0, NULL, NULL);
	if (len <= 0)
		return false;
	CHeapPtr<char> buf;
	if (!buf.Allocate(len))
		return false;
	if (WideCharToMultiByte(1255, 0, text, text.GetLength(), buf, len, NULL, NULL) != len)
		return false;
	blob->AddMemory(buf, len);
	return true;
}

void WAVAddAnsiString(CBlob *blob, const CAtlString &text, bool withNullByte)
{
	if (WAVUseHebrewCodePage(text) && WAVAddHebrewString(blob, text))
	{
		if (withNullByte)
			blob->AddNullByte();
		return;
	}
	blob->AddEncodedString(TEXT_ENCODED_ANSI, text, false, withNullByte);
}

void WAVAddFixedAnsiString(CBlob *blob, const CAtlString &text, size_t maxLen)
{
	CAtlString tmp = text.Left((int)maxLen);
	if (WAVUseHebrewCodePage(tmp))
	{
		CBlob encoded;
		if (WAVAddHebrewString(&encoded, tmp))
		{
			// Windows-1255 is one byte per character, so it fits the field like the PC's code page did
			size_t n = min(encoded.GetLength(), maxLen);
			blob->AddMemory(encoded.m_pData, n);
			if (n < maxLen)
				blob->AddValue(0, maxLen - n);
			return;
		}
	}
	blob->AddFixedAnsiString(text, maxLen);
}

