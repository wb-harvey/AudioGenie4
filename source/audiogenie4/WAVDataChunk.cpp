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
#include "WAVDataChunk.h"
#include "Tools.h"

bool WAVCopyBytes(FILE *Source, FILE *Destination, unsigned __int64 count)
{
	long cfg = CTools::configValues[CONFIG_ID3V2WRITEBLOCKSIZE];
	size_t blockSize = (cfg > 0) ? (size_t)cfg : 65536;
	CBlob buffer;
	try
	{
		while (count > 0)
		{
			size_t want = (count > (unsigned __int64)blockSize) ? blockSize : (size_t)count;
			buffer.FileRead(want, Source);
			if (buffer.GetLength() != want || fwrite(buffer.m_pData, 1, want, Destination) != want)
				return false;
			CTools::instance().doEvents();
			count -= want;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

CWAVDataChunk::CWAVDataChunk(void)
{
	_chunkID = 'data';
	_size = 0;
	_offset = 0;
}

CWAVDataChunk::~CWAVDataChunk(void)
{
}

size_t CWAVDataChunk::getSize()
{
	// 4.0.4.7: an empty data chunk is still written (it used to be dropped, leaving a file without one)
	if (_size % 2 == 1)
		return (size_t)_size + 9;
	return (size_t)_size + 8;
}

bool CWAVDataChunk::load(FILE *Stream, u32 offset, u32 size)
{
	Stream;
	ATLTRACE(_T("Data Chunk with %i bytes\n"), size);
	_offset = offset;
	_size = size;
	return true;
}

void CWAVDataChunk::save(CBlob *blob)
{
	blob->Add4B(_chunkID);
	blob->AddR4B(_size);
}

bool CWAVDataChunk::save(FILE* Source, FILE *Destination)
{
	// 4.0.4.7: checked reads and writes, 64-bit seek and count; a data chunk of 2 GB or more was a negative long and
	// no audio was copied (4.0.4.6 and earlier)
	CBlob blob;
	blob.Add4B('data');
	blob.AddR4B(_size);
	if (blob.FileWrite(8, Destination) != 8)
		return false;
	if (_size > 0)
	{
		if (_fseeki64(Source, _offset, SEEK_SET) != 0)
			return false;
		if (!WAVCopyBytes(Source, Destination, _size))
			return false;
		if (_size % 2 == 1 && fputc(0, Destination) == EOF)
			return false;
	}
	return true;
}
