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
#include "OGGVORBIS.h"
#include "Blob.h"
#include "io.h"
#include <fcntl.h>
#include "OggHeader.h"
#include "vorbisheader.h"
#include <share.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COggVorbis::COggVorbis()
{
}

COggVorbis::~COggVorbis()
{
}

/* -------------------------------------------------------------------------- */



/* ********************* Auxiliary functions & procedures ******************** */


/* --------------------------------------------------------------------------- */

void COggVorbis::ReadTag(FILE *Source)
{
	AnalyzeVorbisComments(Source);
	TagEndPos = ftell(Source);
	firstAudioPos = TagEndPos + 28; // 28 = geschätzter Wert
}

/* --------------------------------------------------------------------------- */

int COggVorbis::GetSamples(FILE *Source)
{
	int Index, DataIndex, Iterator;
	char Data[252];
	COGGHeader Header;
	/* Get total number of samples */
	for (Index = 1; Index < 50; Index++)
	{
		DataIndex = CTools::FileSize - (251 - 10) * Index - 10;
		fseek(Source, DataIndex, SEEK_SET);
		fread(Data, 1, 251, Source);
		/* Get number of PCM samples from last Ogg packet header */
		for (Iterator = (251 - 10); Iterator > 0; Iterator--)
		{
			if (memcmp(&Data[Iterator], OGG_PAGE_ID, 4) == 0)
			{
				fseek(Source, DataIndex + Iterator, SEEK_SET);
				Header.ReadFromFile(Source);
				return (long) Header.AbsolutePosition;
			}
		}
	}
	return 0;
}

/* --------------------------------------------------------------------------- */

bool COggVorbis::GetInfo(FILE *Source, bool withComments)
{
	/* Get info from file */
	fseek(Source, CTools::ID3v2Size, SEEK_SET);
	if (FPage.ReadFromFile(Source))
	{
		fseek(Source, CTools::ID3v2Size + FPage.Segments + 27, SEEK_SET);
		/* Read Vorbis parameter header */
		Parameters.ReadFromFile(Source);
		if (memcmp(Parameters.ID, VORBIS_PARAMETERS_ID, 7) == 0)
		{
			SPagePos = ftell(Source);
			SPage.ReadFromFile(Source);
			fseek(Source, SPagePos + SPage.Segments + 27,
				SEEK_SET);
			fread(Tag.ID, 1, 7, Source);
			// Read Vorbis tag
			if (memcmp(&Tag.ID, VORBIS_TAG_ID, 7) == 0 && withComments)
				ReadTag(Source);
			// Get total number of samples
			Samples = GetSamples(Source);
			return true;
		}
	}
	return false;
}

/* -------------------------------------------------------------------------- */

void COggVorbis::BuildTag()
{
	Data.Clear();
	Data.AddMemory(VORBIS_TAG_ID, 7);
	BuildVorbisComments(Data);
}  

/* -------------------------------------------------------------------------- */

void COggVorbis::SetLacingValues()
{
	long Index, Position, Value;
	BYTE Buffer[256];
	// Set new lacing values for the second Ogg page
	Position = 0;
	Value = 0;
	for (Index = SPage.Segments - 1; Index >= 0; Index--)
	{
		if (SPage.LacingValues[Index] < 0xFF)
		{
			Position = Index;
			Value = 0;
		}
		Value += SPage.LacingValues[Index];		
	}
	Value = Value +	(long)Data.GetLength() - (TagEndPos - SPagePos - SPage.Segments - 27);
	if (Value < 0)
		Value = 0;
	// Change lacing values at the beginning
	for (Index = 0; Index < (Value / 0xFF); Index++)
		Buffer[Index] = 0xFF;
	Buffer[Index] = BYTE(Value % 0xFF);
	if (Position + 1 < SPage.Segments)
	{
		for (Index = Position + 1; Index < SPage.Segments; Index++)
		{
			Buffer[Index - Position + (Value / 0xFF) ] = SPage.LacingValues[Index];
		}
	}
	SPage.Segments = (BYTE) (SPage.Segments - (BYTE)Position + (BYTE)(Value / 0xFF) );
	for (Index = 0; Index < SPage.Segments ; Index++)
	{
		SPage.LacingValues[Index] = Buffer[Index];
	}
}

/* -------------------------------------------------------------------------- */

unsigned long COggVorbis::CalculateCRC(unsigned long CRC, BYTE Data[], long Size)
{
	long Index;
	// Calculate CRC through data
	for (Index = 0; Index < Size; Index++)
		CRC = (CRC << 8) XOR CRC_TABLE[((CRC >> 24) &0xff ) XOR Data[Index]];		
	
	return CRC;
}

/* -------------------------------------------------------------------------- */

void COggVorbis::SetCRC(FILE *Destination)
{
	int Index;
	unsigned long Value;
	BYTE Data[512];
	// Calculate and set checksum for Vorbis tag
	Value = 0;
	SPage.CopyToChar(Data);
	// memcpy(&Data, &SPage, SPage.Segments + 27);
	Value = CalculateCRC(Value, Data, SPage.Segments + 27);
	fseek(Destination, SPagePos + SPage.Segments + 27, SEEK_SET);
	for (Index = 0; Index < SPage.Segments; Index++)
	{
		BYTE id = SPage.LacingValues[Index];
		if (id > 0)
		{
			fread(Data, 1, id, Destination);
			Value = CalculateCRC(Value, Data, id);
		}
	}
	fseek(Destination, SPagePos + 22, SEEK_SET);
	fwrite(&Value, 1, sizeof(Value), Destination);
}

/* -------------------------------------------------------------------------- */

bool COggVorbis::RebuildFile(LPCWSTR FileName)
{
	FILE *Source;
	FILE *Destination;
	CAtlString NewFileName(FileName);
	CBlob tmp;
	/* Rebuild the file with the new Vorbis tag */
	if ( (Source = _wfsopen(FileName, READ_ONLY, _SH_DENYNO)) == NULL)
	{
		CTools::instance().setLastError(errno);
		return false;
	}

	/* Create file streams */
	// Read and Write permissons MUSS!
	NewFileName+=TILDE;
	if ( (Destination = _wfsopen(NewFileName, READ_AND_WRITENEW, _SH_DENYWR)) == NULL)
	{
		CTools::instance().setLastError(errno);
		fclose(Source);
		return false;
	};
	// Copy data blocks
	//Destination.CopyFrom(Source, SPagePos);
	tmp.FileRead(SPagePos, Source);
	tmp.FileWrite(tmp.GetLength(), Destination);
	//Destination.Write(SPage, SPage.Segments + 27);
	SPage.WriteToFile(Destination);
	// Destination.CopyFrom(Tag, 0); // Alles kopieren
	Data.FileWrite(Data.GetLength(), Destination);
	// Source.Seek(TagEndPos, soFrombeginning);
	fseek(Source, TagEndPos, SEEK_SET);
	// Destination.CopyFrom(Source, Source.Size - TagEndPos);
	tmp.FileRead(CTools::FileSize - TagEndPos, Source);
	tmp.FileWrite(tmp.GetLength(), Destination);
	SetCRC(Destination);
	_flushall();
	_fcloseall();
	CTools::instance().doEventsNow();
	// Replace old file and delete temporary file
	if (_wremove(FileName) != 0)
	{
		// Lösche auch temp file
		_wremove(NewFileName);
		CTools::instance().setLastError(errno);
		return false;
	}
	CTools::instance().doEventsNow();
	if (_wrename(NewFileName, FileName) != 0)
	{
		CTools::instance().setLastError(errno);
		return false;
	}
	return true;
}

/* -------------------------------------------------------------------------- */

void COggVorbis::ResetData()
{
	/* Reset variables */
	FPage.Reset();
	SPage.Reset();
	LPage.Reset();
	Parameters.Reset();
	Samples = 0;                                   /* Total number of samples */
	SPagePos = 0;                                  /* Position of second Ogg page */
	TagEndPos = 0;                                 /* Tag end position */
	FChannelModeID = 0;
	FSampleRate = 0;
	FBitRateNominal = 0;
	FSamples = 0;
	Data.Clear();
	firstAudioPos = 0;
	CVorbisComment::ResetData();
}

/* --------------------------------------------------------------------------- */

float COggVorbis::FGetDuration()
{
	/* Calculate duration time */
	if (FSamples > 0)
		if (FSampleRate > 0)
			return (float) (FSamples / FSampleRate);
		else
			return 0.0f;
	else if (FBitRateNominal > 0 && FChannelModeID > 0)
		return (float) (CTools::FileSize - CTools::ID3v2Size) /
		FBitRateNominal /
		FChannelModeID /
		125.0f * 2.0f;
	else
		return 0.0f;
}

/* --------------------------------------------------------------------------- */

int COggVorbis::FGetBitRate()
{
	/* Calculate average bit rate */
	float Duration = FGetDuration();
	if (Duration > 0)
		return (int) ((CTools::FileSize - CTools::ID3v2Size) / Duration / 125);
	else
		return 0;
}

/* --------------------------------------------------------------------------- */

bool COggVorbis::FIsValid()
{
	/* Check for file correctness */
	return ((FChannelModeID == VORBIS_CM_MONO ||
		FChannelModeID == VORBIS_CM_STEREO) &&
		FSampleRate >
		0 &&
		FGetDuration() >
		0.1f &&
		FGetBitRate() >
		0);
}

/* --------------------------------------------------------------------------- */

bool COggVorbis::ReadFromFile(FILE *Stream)
{
	/* Read data from file */
	if (GetInfo(Stream, true))
	{
		/* Fill variables */
		FChannelModeID = Parameters.ChannelMode;
		FSampleRate = Parameters.SampleRate;
		FBitRateNominal = int(Parameters.BitRateNominal / 1000);
		FSamples = Samples;
		return true;
	}
	return false;
}

/* --------------------------------------------------------------------------- */

bool COggVorbis::SaveTag(LPCWSTR FileName)
{
	// Save Vorbis tag
	bool Result = false;
	FILE *Source;
	if ( (Source = _wfsopen(FileName, READ_ONLY, _SH_DENYWR)) != NULL)
	{
		Result = GetInfo(Source, false);
		fclose(Source);
		if (Result)
		{
			FChannelModeID = Parameters.ChannelMode;
			FSampleRate = Parameters.SampleRate;
			FBitRateNominal = int(Parameters.BitRateNominal / 1000);
			FSamples = Samples;
			/* Prepare tag data and save to file */
			BuildTag();
			SPage.Checksum = 0;
			SetLacingValues();
			Result = RebuildFile(FileName);
		}
	}
	else
		CTools::instance().setLastError(errno);

	return Result;
}

/* -------------------------------------------------------------------------- */

bool COggVorbis::RemoveTag(LPCWSTR FileName)
{
	// Clear Vorbis tag excepts Vendor
	Data.Clear();
	CVorbisComment::ResetData();
	CVorbisComment::BuildVorbisComments(Data);
	return SaveTag(FileName);
}

/* -------------------------------------------------------------------------- */

CAtlString COggVorbis::GetChannelMode()
{ 
	switch (FChannelModeID)
	{
	case VORBIS_CM_MONO:
		return MONO;
	case VORBIS_CM_STEREO:
		return STEREO;
	case VORBIS_CM_MULTICHANNEL:
		return MULTICHANNEL;
	default:
		return UNKNOWN;
	}
};
