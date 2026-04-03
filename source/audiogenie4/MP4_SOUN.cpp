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
#include "MP4_SOUN.h"
#include "mp4_atomfactory.h"

CMP4_SOUN::CMP4_SOUN(void)
{
	setFrameID(MP4_SOUN);
	version = 0;
	revision = 0;
	vendor = 0;
	channels = 0;
	sampleSize = 0;
	compressionID = 0;
	packetSize = 0;
	sampleRate = 0;
	samplesPerPacket = 0;
	bytesPerPacket = 0;
	bytesPerFrame = 0;
	bytesPerSample = 0;
}

CMP4_SOUN::~CMP4_SOUN(void)
{
}
void CMP4_SOUN::load(FILE *Stream, u32 offset, u32 size)
{
	offset;
	_blob.FileRead(size, Stream);
	if (_blob.GetLength() >= 20)
	{
		version = _blob.Get2B(0); // Normalerweise Version 0
		                          // normally version 0
		revision = _blob.Get2B(2);
		vendor = _blob.Get4B(4);
		channels = _blob.Get2B(8);
		sampleSize = _blob.Get2B(10);
		compressionID = _blob.Get2B(12);
		packetSize = _blob.Get2B(14);
		sampleRate = _blob.Get4B(16);
		if (version == 1 && _blob.GetLength() >= 36) // Version 1
		{
			samplesPerPacket = _blob.Get4B(20);
			bytesPerPacket = _blob.Get4B(24);
			bytesPerFrame = _blob.Get4B(28);
			bytesPerSample = _blob.Get4B(32);
		}
		else
		{
			samplesPerPacket = 0;
			bytesPerPacket = 0;
			bytesPerFrame = 0;
			bytesPerSample = 0;
		}
	}
}

