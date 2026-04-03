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

const IID CDECL WMA_FILE_PROPERTIES_ID =
{ 0x8CABDCA1, 0xA947, 0x11CF, { 0x8E,0xE4,0x00,0xC0,0x0C,0x20,0x53,0x65 } };

class CWMA_FileProperties:public CWMA_Object
{
public:
	CWMA_FileProperties(void);
	~CWMA_FileProperties(void);
	bool load(FILE *Stream, size_t maxLen);	
	long getBitRate() { return MaxBitrate / 1000; };
	float getDuration() { return float(PlayDuration / 10000000.0) - float(Preroll / 1000.0 );     };
private:
	u64 PlayDuration, Preroll;
	u32 Flags, MaxBitrate;
};
