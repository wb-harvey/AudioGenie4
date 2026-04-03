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

/* -------------------------------------------------------------------------- */
/* Monkey.h          for use with Monkey.cpp                                  */
/* by Stefan Toengi (c) 2003                                                  */
/* -------------------------------------------------------------------------- */
#pragma once

#include "Blob.h"
#include "MonkeyTagInfo.h"
#include "Audio.h"

/* Compression level codes */
#define MONKEY_COMPRESSION_FAST        1000                    /* Fast (poor) */
#define MONKEY_COMPRESSION_NORMAL      2000                  /* Normal (good) */
#define MONKEY_COMPRESSION_HIGH        3000               /* High (very good) */
#define MONKEY_COMPRESSION_EXTRA_HIGH  4000              /* Extra high (best) */

/* Compression level names */
static const LPCWSTR MONKEY_COMPRESSION[] =
{ UNKNOWN, _T("Fast"), _T("Normal"), _T("High"), _T("Extra High") };

/* Format flags */
#define MONKEY_FLAG_8_BIT            1                         /* Audio 8-bit */
#define MONKEY_FLAG_CRC              2           /* New CRC32 error detection */
#define MONKEY_FLAG_PEAK_LEVEL       4                   /* Peak level stored */
#define MONKEY_FLAG_24_BIT           8                        /* Audio 24-bit */
#define MONKEY_FLAG_SEEK_ELEMENTS   16      /* Number of seek elements stored */
#define MONKEY_FLAG_WAV_NOT_STORED  32               /* WAV header not stored */

class CMonkey: public CAudio
{
private:
	CMonkeyTagInfo Header;

public:
	CMonkey();
	virtual ~CMonkey();
	bool ReadFromFile(FILE *Stream);
	void ResetData();
	bool IsValid();
	CAtlString GetFileVersion();
	CAtlString GetCompression();
	BYTE  GetBits();
	CAtlString GetChannelMode();
	float GetPeak();
	long GetSamplesPerFrame();
	long GetBitRate();
	long GetSamples();
	long GetSampleRate()     { return Header.SampleRate; };
	long GetChannels()       { return Header.Channels;   };
	long GetFrames()         { return Header.Frames;     };
	float GetDuration();
	float GetCompressionRatio();
};

