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

const IID CDECL WMA_STREAM_BITRATE_PROPERTIES_ID =
{ 0x7BF875CE, 0x468D, 0x11D1, { 0x8D,0x82,0x00,0x60,0x97,0xC9,0xA2,0xB2 } };

class CWMA_StreamBitrateProperties :
	public CWMA_Object
{
public:
	CWMA_StreamBitrateProperties(void);
	~CWMA_StreamBitrateProperties(void);
	bool load(FILE *Stream, size_t maxLen);
	void Empty();
	u16 Flags;
    u32 AverageBitrate;
};
