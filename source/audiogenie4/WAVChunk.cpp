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
	ATLTRACE(_T("Chunk %c%c%c%c with %i bytes at Offset %i\n"), BYTE(_chunkID >> 24), BYTE(_chunkID >> 16), BYTE(_chunkID >> 8), BYTE(_chunkID), size, offset);
	fseek(Stream, offset, SEEK_SET);
	_data.FileRead(size, Stream);
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
	if (_data.GetLength() > 0)
	{
		CBlob *blob = new CBlob(_data.GetLength() + 10);
		save(blob);
		blob->FileWrite(blob->GetLength(), Destination);
		delete blob;
	}
	return true;
}

