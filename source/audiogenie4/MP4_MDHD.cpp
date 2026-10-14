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
#include "MP4_MDHD.h"
#include "mp4_atomfactory.h"

CMP4_MDHD::CMP4_MDHD(void)
{
	setFrameID(MP4_MDHD);
	isSoundAtom = false;
	version = 0;
	flags = creationTime = modifyTime = timeScale = 0;
	duration = 0;
	language = quality = 0;
}

CMP4_MDHD::~CMP4_MDHD(void)
{
}

void CMP4_MDHD::load(FILE *Stream, u32 offset, u32 size)
{
	offset;
	_blob.FileRead(size, Stream);
	// version 1: 64-bit times and duration. Read with the version 0 layout, the time scale and duration came
	// from the creation/modification times, and duration and sample rate read 0 (4.0.4.6 and earlier).
	if (_blob.GetLength() >= 36 && _blob.GetAt(0) == 1)
	{
		version = 1;
		flags = _blob.Get3B(1);
		creationTime = _blob.Get4B(8);	// low 32 bits
		modifyTime = _blob.Get4B(16);	// low 32 bits
		timeScale = _blob.Get4B(20);
		duration = ((unsigned __int64)(u32)_blob.Get4B(24) << 32) | (u32)_blob.Get4B(28);
		language = _blob.Get2B(32);
		quality = _blob.Get2B(34);
	}
	else if (_blob.GetLength() >= 24)
	{
		version = _blob.GetAt(0);
		flags = _blob.Get3B(1);
		creationTime = _blob.Get4B(4);
		modifyTime = _blob.Get4B(8);
		timeScale = _blob.Get4B(12);
		duration = (u32)_blob.Get4B(16);
		language = _blob.Get2B(20);
		quality = _blob.Get2B(22);
	}
}
