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
#include "wavchunk.h"

// 4.0.4.7: copies count bytes from the current position, every read and write checked
bool WAVCopyBytes(FILE *Source, FILE *Destination, unsigned __int64 count);

class CWAVDataChunk :
	public CWAVChunk
{
public:
	CWAVDataChunk(void);
	~CWAVDataChunk(void);
	bool load(FILE *Stream, u32 offset, u32 size);
	void save(CBlob *blob);
	bool save(FILE* Source, FILE *Destination);
	size_t getSize();
	unsigned __int64 getSize64() { return (unsigned __int64)_size + 8 + (_size & 1); };
	u32 getOffset() { return _offset; };
	u32 getDataSize() { return _size; };
	// the audio is copied from here when saving: the saved file's own data chunk
	void setRange(u32 offset, u32 size) { _offset = offset; _size = size; };
protected:
	u32 _size;
	u32 _offset;
};
