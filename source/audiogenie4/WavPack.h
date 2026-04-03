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
#include "Audio.h"
#include "Tools.h"

static const unsigned int sample_rates[] = { 6000, 8000, 9600, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 64000, 88200, 96000, 192000 };

class CWavPack : public CAudio
{
private:
	CBlob _header;
	long _samples, _flags;
	BYTE _majorversion, _minorversion; 
	public:
	CWavPack(void);
	virtual ~CWavPack();
	bool ReadFromFile(FILE *Stream);
	void ResetData();
	bool IsValid();
	CAtlString GetFileVersion();
	CAtlString GetChannelMode();
	long GetSamples()        { return _samples;     };
	long GetSampleRate();
	long GetChannels();    
	float GetDuration();
};

