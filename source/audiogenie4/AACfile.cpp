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

#include "stdafx.h"
#include "aac.h"
#include "io.h"
#include "Blob.h"
#include <fcntl.h>
#include "resource.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CAAC::CAAC()
{

}

CAAC::~CAAC()
{
}

/* ********************* Auxiliary functions & procedures ******************** */

long CAAC::ReadBits(FILE *Source, long Position, long Count)
{
	unsigned long Result = 0;
	BYTE Buffer[6];
	/* Read a number of bits from file at the given position */
	fseek(Source, Position / 8, SEEK_SET);
	fread(Buffer, 1, 4, Source);
	Result = Buffer[0] * 0x1000000 +
		Buffer[1] * 0x10000 +
		Buffer[2] * 0x100 +
		Buffer[3];
	return (Result << (Position % 8)) >> (32 - Count);
}

/* ********************** Private functions & procedures ********************* */

void CAAC::ResetData()
{
	/* Reset all variables */
	FHeaderTypeID = AAC_HEADER_TYPE_UNKNOWN;
	FMPEGVersionID = AAC_MPEG_VERSION_UNKNOWN;
	FProfileID = AAC_PROFILE_UNKNOWN;
	FChannels = 0;
	FSampleRate = 0;
	FBitRate = 0;
	FBitRateTypeID = AAC_BITRATE_TYPE_UNKNOWN;
}

/* --------------------------------------------------------------------------- */

CAtlString CAAC::GetHeaderType()
{
	/* Get header type name */
	return AAC_HEADER_TYPE[FHeaderTypeID];
}

/* --------------------------------------------------------------------------- */

CAtlString CAAC::GetFileVersion()
{
	/* Get MPEG version name */
	return AAC_MPEG_VERSION[FMPEGVersionID];
}

/* --------------------------------------------------------------------------- */

CAtlString CAAC::GetProfile()
{
	/* Get profile name */
	return AAC_PROFILE[FProfileID];
}

/* --------------------------------------------------------------------------- */

CAtlString CAAC::GetBitRateType()
{
	/* Get bit rate type name */
	return AAC_BITRATE_TYPE[FBitRateTypeID];
}

/* --------------------------------------------------------------------------- */

float CAAC::GetDuration()
{
	/* Calculate duration time */
	if (FBitRate == 0)
		return 0.0f;
	return 8.0f * (CTools::FileSize - CTools::ID3v2Size) / FBitRate;
}

/* --------------------------------------------------------------------------- */

bool CAAC::IsValid()
{
	/* Check for file correctness */
	return (FHeaderTypeID != AAC_HEADER_TYPE_UNKNOWN &&
		FChannels > 0 &&
		FSampleRate > 0 &&
		FBitRate > 0);
}

/* --------------------------------------------------------------------------- */

BYTE CAAC::RecognizeHeaderType(FILE *Source)
{
	BYTE Header[6];
	/* Get header type of the file */
	fseek(Source, CTools::ID3v2Size, SEEK_SET);
	fread(Header, 1, 4, Source);
	if (memcmp(Header, ADIF, 4) == 0)
		return AAC_HEADER_TYPE_ADIF;
	if ((Header[0] == 0xFF) && ((Header[1] & 0xF0) == 0xF0))
		return AAC_HEADER_TYPE_ADTS;
	return AAC_HEADER_TYPE_UNKNOWN;
}

/* --------------------------------------------------------------------------- */

void CAAC::ReadADIF(FILE *Source)
{
	long Position;
	/* Read ADIF header data */
	Position = CTools::ID3v2Size * 8 + 32;
	if (ReadBits(Source, Position, 1) == 0)
		Position += 3l;
	else
		Position += 75l;
	if (ReadBits(Source, Position, 1) == 0)
		FBitRateTypeID = AAC_BITRATE_TYPE_CBR;
	else
		FBitRateTypeID = AAC_BITRATE_TYPE_VBR;
	Position++;
	FBitRate = ReadBits(Source, Position, 23);
	if (FBitRateTypeID == AAC_BITRATE_TYPE_CBR)
		Position += 51l;
	else
		Position += 31l;
	FMPEGVersionID = AAC_MPEG_VERSION_4;
	FProfileID = (BYTE) (ReadBits(Source, Position, 2) + 1);
	Position += 2l;
	FSampleRate = SAMPLE_RATE[ReadBits(Source, Position, 4)];
	Position += 4l;
	FChannels += (BYTE) (ReadBits(Source, Position, 4l));
	Position += 4l;
	FChannels += (BYTE) (ReadBits(Source, Position, 4l));
	Position += 4l;
	FChannels += (BYTE) (ReadBits(Source, Position, 4l));
	Position += 4l;
	FChannels += (BYTE) (ReadBits(Source, Position, 2l));  
}

/* --------------------------------------------------------------------------- */

void CAAC::ReadADTS(FILE *Source)
{
	long Frames, TotalSize, Position;
	/* Read ADTS header data */
	Frames = 0;
	TotalSize = 0;
	size_t totalBitrate = 0;
	do
	{
		Position = (CTools::ID3v2Size + TotalSize) * 8l;
		if (ReadBits(Source, Position, 12) != 0xFFF)
			break;
		Frames++;
		Position += 12l;
		if (ReadBits(Source, Position, 1) == 0)
			FMPEGVersionID = AAC_MPEG_VERSION_4;
		else
			FMPEGVersionID = AAC_MPEG_VERSION_2;
		Position += 4l;
		FProfileID = (BYTE) (ReadBits(Source, Position, 2) + 1);
		Position += 2l;
		FSampleRate = SAMPLE_RATE[ReadBits(Source, Position, 4)];
		Position += 5l;
		FChannels = (BYTE) ReadBits(Source, Position, 3);
		//if (FMPEGVersionID == AAC_MPEG_VERSION_4)
		//	Position += 9l;
		//else
		Position += 7l;
		long sz = ReadBits(Source, Position, 13);
		if (sz == 0)
		{
			CTools::instance().setLastError(ERR_WRONG_FORMAT);
			break;
		}
		TotalSize += sz;
		totalBitrate+= size_t((sz / 128.0 * FSampleRate) / 1024.0);
		Position += 13l;
		if (ReadBits(Source, Position, 11) == 0x7FF)
			FBitRateTypeID = AAC_BITRATE_TYPE_VBR;
		else
		{
			FBitRateTypeID = AAC_BITRATE_TYPE_CBR;
			//break;
		}   
	}
	while (CTools::FileSize > (CTools::ID3v2Size + TotalSize));
	if (Frames > 0)
		FBitRate = long(totalBitrate / Frames * 1024.0 + 0.5);	
		//FBitRate = long(8.0 * TotalSize / 1024.0 * FSampleRate / Frames + 0.5 );
	
}

/* --------------------------------------------------------------------------- */

bool CAAC::ReadFromFile(FILE *Source)
{
	/* Read data from file */
	FHeaderTypeID = RecognizeHeaderType(Source);
	/* Read header data */
	if (FHeaderTypeID == AAC_HEADER_TYPE_ADIF)
	{
		ReadADIF(Source);
		return true;
	}
	if (FHeaderTypeID == AAC_HEADER_TYPE_ADTS)
	{
		ReadADTS(Source);
		return IsValid(); // ADTS sieht fast aus wie MPEG, also noch validieren
	}
	ResetData();
	return false; // Kein AAC File
}

