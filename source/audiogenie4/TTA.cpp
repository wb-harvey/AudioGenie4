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
#include "TTA.h"

// Dokumentation of the True Audio: http:\\en.true-audio.com 


CTTA::CTTA(void)
{
}


CTTA::~CTTA(void)
{
}

void CTTA::ResetData()
{
	/* Reset data */
	_header.Clear();
	_samples = 0;
	_samplerate = 0;
	_channels = 0;
	_bitspersample = 0;
}

/* -------------------------------------------------------------------------- */

bool CTTA::IsValid()
{
	if (_header.GetLength() > 26)
	{
		if (_header.GetAt(0) == 'T' && _header.GetAt(1) == 'T' && _header.GetAt(2) == 'A')
		{
			if (_header.GetAt(3) == '1' || _header.GetAt(3) == '2')
			{
				if (_samples > 0 && _channels > 0 && _samplerate > 0 && _bitspersample > 0)
					return true;
			}
		}
	}
	return false;
}

/* -------------------------------------------------------------------------- */

CAtlString CTTA::GetFileVersion()
{
	if (IsValid())
		return _header.GetStringAt(0,4);
	return L"";
}

/* -------------------------------------------------------------------------- */

CAtlString CTTA::GetChannelMode()
{
	/* Get channel mode */
	switch (_channels)
	{
	case 1:
		return MONO;
	case 2:
		return STEREO;
	default:
		return UNKNOWN;
	}  
}

/* -------------------------------------------------------------------------- */

float CTTA::GetDuration()
{
	/* Get song duration */
	if (!IsValid())
		return 0.0f;
	return (float)_samples / _samplerate;	
}

/* -------------------------------------------------------------------------- */

bool CTTA::ReadFromFile(FILE *Stream)
{
	/* Read header data */
	fseek(Stream, CTools::ID3v2Size, SEEK_SET);
	// 00-03 Format signature with major version number (ASCII, "TTA1")
	_header.FileRead(32, Stream);
	if (_header.GetAt(0) == 'T' && _header.GetAt(1) == 'T' && _header.GetAt(2) == 'A' && _header.GetAt(3) == '1')
	{
		/*
		04-05	Audio format (binary, Little Endian)
		06-07	Number of channels (binary, Little Endian)
		08-09	Bits per sample (binary, Little Endian)
		10-13	Sample rate (binary, Little Endian)
		14-17	Data length in samples (binary, Little Endian)
		18-21	CRC32 block footer (binary, Little Endian) 
		*/
		_channels = _header.GetR2B(6);
		_bitspersample = _header.GetR2B(8);
		_samplerate = _header.GetR4B(10);
		_samples = _header.GetR4B(14);
		return true;
	}
	else if (_header.GetAt(0) == 'T' && _header.GetAt(1) == 'T' && _header.GetAt(2) == 'A' && _header.GetAt(3) == '2')
	{
		/*
		04-05  Number of channels (binary, Little Endian)
		06-07 Bits per sample (binary, Little Endian)
		08-11 Sample rate (binary, Little Endian)
		12-15 Channel locations mask2 (binary, Little Endian)
		16-23 Data length in samples (binary, Little Endian, may be zero)
		24-31 Data block size, bytes (binary, Little Endian, may be zero)
		32-35 CRC32 block footer (binary, Little Endian) 
		*/
		_channels = _header.GetR2B(4);
		_bitspersample = _header.GetR2B(6);
		_samplerate = _header.GetR4B(8);
		_samples = _header.GetR4B(16);
		return true;
	}
	ResetData();
	return false;
}

