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
#include "WAVContainer.h"
#include "Tools.h"

CWAVContainer::CWAVContainer(void)
{
	_data.Clear();
	_chunkID = 0;
	_children.SetCount(0, 10);
}
CWAVContainer::CWAVContainer(u32 ID)
{
	_data.Clear();
	_chunkID = ID;
	_children.SetCount(0, 10);
}

CWAVContainer::~CWAVContainer(void)
{
	Remove();
}
size_t CWAVContainer::getSize()
{
	size_t size = 12;
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		size+=_children[i]->getSize();
	}
	return size;
}

CWAVChunk* CWAVContainer::find(u32 ID)
{
	CWAVChunk* chunk = NULL;
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		chunk = _children[i]->find(ID);
		if (chunk != NULL)
			return chunk;
	}
	return NULL;
}

bool CWAVContainer::load(FILE *Stream, u32 offset, u32 size)
{
	// 4.0.4.7: positions in 64 bits and the header reads checked: offset + size wrapped in 32 bits, and a stale
	// header after the end of the file repeated the last chunk (4.0.4.6 and earlier)
	unsigned __int64 endPos = (unsigned __int64)offset + size;
	CBlob header;
	// Container hat 4 extra Bytes am Anfang, nehme diese als ID
	if (size < 4 || _fseeki64(Stream, offset, SEEK_SET) != 0)
		return false;
	header.FileRead(4, Stream);
	if (header.GetLength() != 4)
		return false;
	_chunkID = header.Get4B(0);
	ATLTRACE(_T("LIST Container at Pos:%u with ID:%c%c%c%c\n"), offset, BYTE(_chunkID >> 24), BYTE(_chunkID >> 16), BYTE(_chunkID >> 8), BYTE(_chunkID));
	unsigned __int64 pos = (unsigned __int64)offset + 4;
	while (pos < endPos)
	{
		if (pos + 8 > endPos || _fseeki64(Stream, pos, SEEK_SET) != 0)
			return false;
		header.FileRead(8, Stream);
		if (header.GetLength() != 8)
			return false;
		u32 chunkID = header.Get4B(0);
		u32 dataSize = header.GetR4B(4);

		if (pos + dataSize > endPos)
			return false;

		CWAVChunk *chunk = new CWAVChunk(chunkID);
		_children.Add(chunk);
		CTools::instance().doEvents();
		unsigned __int64 nextReadPos = pos + 8;
		chunk->load(Stream, (u32)nextReadPos, dataSize);
		// the walk always moves on by at least the 8 header bytes
		pos = nextReadPos + dataSize + (dataSize & 1);
	}
	return true;
}

void CWAVContainer::save(CBlob *blob)
{
	// LIST ( 4 Bytes )
	// datasize ( 4 Bytes )
	// chunkID ( 4 Bytes )
	if (getSize() > 0)
	{
		blob->Add4B('LIST');
		blob->AddR4B((u32)getSize() - 8);
		blob->Add4B(_chunkID);
		size_t counts = _children.GetCount();
		for (size_t i = 0; i < counts; i++)
		{
			_children[i]->save(blob);		
		}
	}
}

bool CWAVContainer::save(FILE* Source, FILE *Destination)
{
	if (getSize() > 0)
	{
		CBlob blob;
		blob.Add4B('LIST');
		blob.AddR4B((u32)getSize() - 8);
		blob.Add4B(_chunkID);
		if (blob.FileWrite(blob.GetLength(), Destination) != blob.GetLength())
			return false;
		size_t counts = _children.GetCount();
		for (size_t i = 0; i < counts; i++)
		{
			if (!_children[i]->save(Source, Destination))
				return false;
		}
	}
	return true;
}


void CWAVContainer::Remove()
{
	size_t counts = _children.GetCount();
	ATLTRACE(L"WavContainer.Remove(%i)", counts);
	for (size_t i = 0; i < counts; i++)
		delete _children.GetAt(i);
	_children.RemoveAll();
	_children.SetCount(0,10);
}