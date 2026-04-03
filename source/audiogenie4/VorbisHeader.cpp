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

// VorbisHeader.cpp: Implementierung der Klasse CVorbisHeader.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VorbisHeader.h"
#include "Blob.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CVorbisHeader::CVorbisHeader()
{
	Reset();
}

CVorbisHeader::~CVorbisHeader()
{

}
bool CVorbisHeader::ReadFromFile(FILE *Stream)
{
	errno = 0;
	fread(ID, 1, 7, Stream);                          /* Always #1 + "vorbis" */
	CBlob tmp;
    tmp.FileRead(23, Stream);
    if (tmp.GetLength() != 23)
	  return false;	
	memcpy(BitstreamVersion, tmp.m_pData, 4);         /* Bitstream version number */
	ChannelMode = tmp.GetAt(4);                       /* Number of channels */
	SampleRate = tmp.GetR4B(5);               /* Sample rate (hz) */
	BitRateMaximal = tmp.GetR4B(9);           /* Bit rate upper limit */
	BitRateNominal = tmp.GetR4B(13);          /* Nominal bit rate */
	BitRateMinimal = tmp.GetR4B(17);          /* Bit rate lower limit */
	BlockSize = tmp.GetAt(21);                        /* Coded size for small and long blocks */
	StopFlag = tmp.GetAt(22);
	return (errno == 0);
}

void CVorbisHeader::Reset()
{
	memset(ID, 0, 7);
	memset(BitstreamVersion, 0, 4);
	ChannelMode = 0;
	SampleRate = 0;
	BitRateMaximal = 0;
	BitRateNominal = 0;
	BitRateMinimal = 0;
	BlockSize = 0;
	StopFlag = 0;
}

bool CVorbisHeader::exists()
{
	return (memcmp(ID, VORBIS_PARAMETERS_ID, 7) == 0);
}
