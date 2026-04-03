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
#include "wma_object.h"

const IID CDECL WMA_STREAM_PROPERTIES_ID =
{ 0xB7DC0791, 0xA9B7, 0x11CF, { 0x8E,0xE6,0x00,0xC0,0x0C,0x20,0x53,0x65 } };

const IID CDECL WMA_AUDIO_MEDIA_ID =
{ 0xF8699E40, 0x5B4D, 0x11CF, { 0xA8,0xFD,0x00,0x80,0x5F,0x5C,0x44,0x2B } };

class CWMA_StreamProperties :
	public CWMA_Object
{
public:
	CWMA_StreamProperties(void);
	~CWMA_StreamProperties(void);
	bool load(FILE *Stream, size_t maxLen);
	void Empty();
	u16 CodecID;
	u16 getNumberOfChannels() { return NumberofChannels; };
	u32 getSamplesPerSecond() { return SamplesPerSecond; };
	u32 getAverageBytesPerSecond() { return AverageBytesPerSecond; };
private:
	u16 NumberofChannels;
	u32 SamplesPerSecond;
	u32 AverageBytesPerSecond;
	u16 BlockAlignment;
	u16 BitsPerSample;
	u16 CodecSpecificDataSize;
	GUID StreamType;
};
