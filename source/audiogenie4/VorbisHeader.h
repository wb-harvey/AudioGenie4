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

// VorbisHeader.h: Schnittstelle für die Klasse CVorbisHeader.
//
//////////////////////////////////////////////////////////////////////
#pragma once

/* Vorbis parameter frame ID */
static const char VORBIS_PARAMETERS_ID[] = { 1 , 'v','o','r','b','i','s' };

/* Vorbis tag frame ID */
static const char VORBIS_TAG_ID[] = { 3, 'v','o','r','b','i','s' };

class CVorbisHeader
{
public:
	CVorbisHeader();
	virtual ~CVorbisHeader();
	//
	char ID[7];                                        /* Always #1 + "vorbis" */
	BYTE BitstreamVersion[4];                          /* Bitstream version number */
	BYTE ChannelMode;                                  /* Number of channels */
	int SampleRate;                                    /* Sample rate (hz) */
	int BitRateMaximal;                                /* Bit rate upper limit */
	int BitRateNominal;                                /* Nominal bit rate */
	int BitRateMinimal;                                /* Bit rate lower limit */
	BYTE BlockSize;                                    /* Coded size for small and long blocks */
	BYTE StopFlag;                                     /* Always 1 */
	//
	bool ReadFromFile(FILE *Stream);
	void Reset();
	bool exists();
};

