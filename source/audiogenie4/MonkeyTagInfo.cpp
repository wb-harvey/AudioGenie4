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

// MonkeyTagInfo.cpp: Implementierung der Klasse CMonkeyTagInfo.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MonkeyTagInfo.h"
#include "Blob.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CMonkeyTagInfo::CMonkeyTagInfo()
{
	Reset();
}

CMonkeyTagInfo::~CMonkeyTagInfo()
{

}

bool CMonkeyTagInfo::ReadFromFile(FILE *Stream)
{
	errno = 0;
	CBlob tmp;
	fread(ID, 1, 4, Stream);                                                /* Always "MAC " */
	tmp.FileRead(2, Stream);
	VersionID = tmp.GetR2B(0);                                             /* Version number * 1000 (3.91 = 3910) */
	if (VersionID < 3980)
	{ 
		// 34 bytes
		tmp.FileRead(34, Stream);
		if (tmp.GetLength() != 34)
			return false;
		CompressionID = tmp.GetR2B(0);                                      /* Compression level code */
		Flags = tmp.GetR2B(2);                                              /* Any format flags */
		Channels = tmp.GetR2B(4);                                           /* Number of channels */
		SampleRate = tmp.GetR4B(6);                                         /* Sample rate (hz) */   
		HeaderBytes = tmp.GetR4B(10);                                       /* Header length (without header ID) */
		TerminatingBytes = tmp.GetR4B(14);                                  /* Extended data */
		Frames = tmp.GetR4B(18);                                            /* Number of frames in the file */
		FinalSamples = tmp.GetR4B(22) ;                                     /* Number of samples in the final frame */
		PeakLevel = tmp.GetR4B(26);                                         /* Peak level (if stored) */
		SeekElements = tmp.GetR4B(30);
	}
	else
	{
		// 70 bytes
		fseek(Stream, 2, SEEK_CUR);
		tmp.FileRead(68, Stream);
		if (tmp.GetLength() != 68)
			return false;
		// APE_DESCRIPTOR 
		DescriptorBytes = tmp.GetR4B(0);
		HeaderBytes = tmp.GetR4B(4);
		SeekTableBytes = tmp.GetR4B(8);
		HeaderDataBytes = tmp.GetR4B(12);
		APEFrameDataBytes = tmp.GetR4B(16);
		APEFrameDataBytesHigh = tmp.GetR4B(20);
		TerminatingDataBytes = tmp.GetR4B(24);
		memcpy(FileMD5, tmp.m_pData + 28, 16);    // fread(FileMD5, 1, 16, Stream); 
		// APE_HEADER
		CompressionID = tmp.GetR2B(44);
		FormatFlags = tmp.GetR2B(46);
		BlocksPerFrame = tmp.GetR4B(48);
		FinalFrameBlocks = tmp.GetR4B(52);
		Frames = tmp.GetR4B(56);
		BitsPerSample = tmp.GetR2B(60);
		Channels = tmp.GetR2B(62);
		SampleRate = tmp.GetR4B(64);
	}  
	return (errno == 0);
}
void CMonkeyTagInfo::Reset()
{
	memset(ID,0, 4);
	VersionID = 0;
	CompressionID = 0;
	Flags = 0;
	Channels = 0;
	SampleRate = 0;
	HeaderBytes = 0;
	TerminatingBytes = 0;
	Frames = 0;
	FinalSamples = 0;
	PeakLevel = 0;
	SeekElements = 0;
}
bool CMonkeyTagInfo::exists()
{
	return (memcmp(ID, MONKEY_TAG_ID, 4) == 0);
}
