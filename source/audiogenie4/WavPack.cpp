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
#include "WavPack.h"

// Doku unter : http:\\www.wavpack.com\file_format.txt

CWavPack::CWavPack(void)
{
}


CWavPack::~CWavPack(void)
{
}

void CWavPack::ResetData()
{
	/* Reset data */
	_header.Clear();
	_samples = 0;
	_majorversion = 0;
	_minorversion = 0;
	_flags;	
}

/* -------------------------------------------------------------------------- */

bool CWavPack::IsValid()
{
	if (_header.GetLength() >= 32)
	{
		if (_header.GetAt(0) == 'w' && _header.GetAt(1) == 'v' && _header.GetAt(2) == 'p' && _header.GetAt(3) == 'k')
		{
			if (GetSampleRate() > 0)
			{
				return true;
			}
		}
	}
	return false;
}

/* -------------------------------------------------------------------------- */

CAtlString CWavPack::GetFileVersion()
{
	if (IsValid())
	{
		CAtlString tmp;
		tmp.Format(L"wavpack v%i.%i", _majorversion, _minorversion);
		return tmp;
	}
	return L"";
}

/* -------------------------------------------------------------------------- */

CAtlString CWavPack::GetChannelMode()
{
	/* Get channel mode */
	return (_flags & 4) ? MONO : STEREO;	  
}

/* -------------------------------------------------------------------------- */

float CWavPack::GetDuration()
{
	/* Get song duration */
	if (!IsValid())
		return 0.0f;
	long sampleRate = GetSampleRate();
	return sampleRate > 0 ? (float)(_samples / sampleRate ) : 0.0f;
}

long CWavPack::GetSampleRate()
{
	return (_flags > 0) ? sample_rates[(_flags & (0xfL << 23)) >> 23] : 0;
}

long CWavPack::GetChannels()
{
	return (_flags & 4) ? 1 : 2;
}

/* -------------------------------------------------------------------------- */

bool CWavPack::ReadFromFile(FILE *Stream)
{
	/* Read header data */
	fseek(Stream, CTools::ID3v2Size, SEEK_SET);
	_header.FileRead(32, Stream);
	if (_header.GetAt(0) == 'w' && _header.GetAt(1) == 'v' && _header.GetAt(2) == 'p' && _header.GetAt(3) == 'k')
	{
		/*
		0 char ckID [4];              // "wvpk"
		4 uint32_t ckSize;            // size of entire block (minus 8, of course)
		8 uint16_t version;           // 0x402 to 0x410 are currently valid for decode
		10 uchar track_no;             // track number (0 if not used, like now)
		11 uchar index_no;             // track sub-index (0 if not used, like now)
		12 uint32_t total_samples;     // total samples for entire file, but this is
		// only valid if block_index == 0 and a value of
		// -1 indicates unknown length
		16 uint32_t block_index;       // index of first sample in block relative to
		// beginning of file (normally this would start
		// at 0 for the first block)
		20 uint32_t block_samples;     // number of samples in this block (0 = no audio)
		24 uint32_t flags;             // various flags for id and decoding
		28 uint32_t crc;               // crc for actual decoded data */
		_majorversion = _header.GetAt(9);
		_minorversion = _header.GetAt(8);
		_samples = _header.GetR4B(12);
		_flags = _header.GetR4B(24);
		 //long bitsPerSample = ((_flags & 3) + 1) * 8 - ((_flags & (0x1fL << 13)) >> 13);
		 // long bytes = _samples * bitsPerSample / 8;
		return true;
	}
	ResetData();
	return false;
}

