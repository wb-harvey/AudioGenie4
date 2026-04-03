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
#include "stdlib.h"
#include "stdio.h"
#include "io.h"
#include <fcntl.h>
#include "lyrics.h"
#include "Blob.h"
#include "share.h"
#include "sys\stat.h"
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CLyrics::CLyrics()
{
}
CLyrics::~CLyrics()
{
}

/* -------------------------------------------------------------------------- */

void CLyrics::ResetData()
{
	/* Reset all variables */
	for (int i = 0; i < LYRICS_FRAME_COUNT; i++)
		FField[i].Empty();

	Data.Clear();
	memset(ID3v1Tag, 0, ID3V1_TAG_SIZE);
	FVersion = LYRICS_VERSION_UNKNOWN;
	FStartPosition = 0;
	FEndPosition = 0;
	CTools::LyricsSize = 0;
}

/* -------------------------------------------------------------------------- */

bool CLyrics::RemoveTag(LPCWSTR FileName)
{
	int fh;
	if (_wsopen_s(&fh, FileName, O_RDWR | _O_BINARY, _SH_DENYWR, _S_IREAD | _S_IWRITE ) != 0)
	{
		CTools::instance().setLastError(errno);
		return false;
	}

	/* Save the ID3 Tag */
	_lseek(fh, -ID3V1_TAG_SIZE, SEEK_END);
	_read(fh, ID3v1Tag, ID3V1_TAG_SIZE);
	_lseek(fh, (long)FStartPosition, SEEK_SET);
	/* write the ID3 Tag */
	_write(fh, ID3v1Tag, ID3V1_TAG_SIZE);
	_chsize_s(fh, (long)FStartPosition + ID3V1_TAG_SIZE);
	_close(fh);
	return true;
}

/* -------------------------------------------------------------------------- */

void CLyrics::SetTagItem(char ID[], long Pos, long DataSize)
{
	for (int Iterator = 0; Iterator < LYRICS_FRAME_COUNT; Iterator++)
	{
		if (memcmp(ID, FIELD_LIST[Iterator], 3) == 0)
		{
			FField[Iterator] = Data.GetStringAt(Pos, DataSize);
			break;
		}
	}
}

/* -------------------------------------------------------------------------- */

bool CLyrics::ReadHeader(FILE *Stream)
{
	FVersion = LYRICS_VERSION_UNKNOWN;
	/* Read header and get Version & Size */
	FEndPosition = CTools::FileSize - ID3V1_TAG_SIZE - 9;
	if (FEndPosition < 0)
		return false;
	fseek(Stream, FEndPosition, SEEK_SET); 
	//fseek(Stream, - (ID3V1_TAG_SIZE + 9), SEEK_END);
	//FEndPosition = ftell(Stream);
	fread(FHeader, 1, 9, Stream);
	fread(ID3v1Tag, 1, 128, Stream);
	/* check if Lyrics-Tag exists */
	if (memcmp(FHeader, LYRICS_BEGIN, 6) != 0)
		return false;
	/* check for Version */
	if (memcmp(FHeader, LYRICS_OLD, 9) == 0)
	{
		FVersion = LYRICS_VERSION_100;
		return true;
	}
	if (memcmp(FHeader, LYRICS_NEW, 9) == 0)
	{
		FVersion = LYRICS_VERSION_200;
		return true;
	}

	return false;
}

/* -------------------------------------------------------------------------- */

void CLyrics::ReadFramesOld(FILE *Stream, bool isDeleting)
{
	long StartPosition, DataSize, Iterator;
	StartPosition = FEndPosition - 5100;
	if (StartPosition < 0)
		StartPosition = 0;
	DataSize = FEndPosition - StartPosition;
	Data.Clear();
	/* Get information from Lyrics (Version 1.00) */
	fseek(Stream, (long)StartPosition, SEEK_SET);
	Data.FileRead(DataSize, Stream);
	/* Search for Begin */
	for (Iterator = 0; Iterator < (long)Data.GetLength() - 11; Iterator++)
	{
		if (memcmp(Data.m_pData + Iterator, LYRICS_BEGIN, 11) == 0)
		{
			FStartPosition = StartPosition + Iterator;

			CTools::LyricsSize = DataSize - Iterator - 11;
			if (isDeleting)
				return;
			SetTagItem("LYR", Iterator + 11, CTools::LyricsSize);
		}
	}
}

/* -------------------------------------------------------------------------- */

void CLyrics::ReadFramesNew(FILE *Stream,bool isDeleting)
{
	long Transferred, DataSize, Iterator;
	char Buffer[10];
	memset(Buffer, 0, 10);
	Data.Clear();
	Transferred = 0;
	/* Get information from Lyrics (Version 2.00) */
	fseek(Stream, (long)FEndPosition - 6, SEEK_SET);
	Transferred = (long)fread(Buffer, 1, 6, Stream);
	/* Convert from Char into Size */
	DataSize = atoi(Buffer);
	if (DataSize <= 0 || (FEndPosition - DataSize <= 0))
		return;

	CTools::LyricsSize = DataSize;
	FStartPosition = FEndPosition - 6 - DataSize;  
	if (isDeleting)
		return;
	fseek(Stream, (long)FStartPosition, SEEK_SET);
	Data.FileRead(DataSize, Stream);
	if (memcmp(Data.m_pData, LYRICS_BEGIN, 11) == 0)
	{
		Iterator = 11;
		while (Iterator < (long)Data.GetLength())
		{
			memcpy(Buffer, Data.m_pData + Iterator, 8);
			Transferred = atoi(Buffer + 3);
			SetTagItem(Buffer, Iterator + 8, Transferred);
			Iterator += Transferred + 8;
		}
	}
}

/* -------------------------------------------------------------------------- */

void CLyrics::ReadFromFile(FILE *Stream)
{
	/* Process data if loaded and header valid */
	if (ReadHeader(Stream))
	{
		/* Get information from frames if version supported */
		if (FVersion == LYRICS_VERSION_200)
			ReadFramesNew(Stream, false);
		else
			ReadFramesOld(Stream, false);
		return;
	}
	ResetData();
}

/* -------------------------------------------------------------------------- */

bool CLyrics::RemoveFromFile(LPCWSTR FileName)
{
	bool Result = false;
	FILE *Stream;
	if ( (Stream = _wfsopen(FileName, READ_ONLY, _SH_DENYNO)) != NULL)
	{ Result = ReadHeader(Stream);
	if (FVersion == LYRICS_VERSION_200)
		ReadFramesNew(Stream, true);
	if (FVersion == LYRICS_VERSION_100)
		ReadFramesOld(Stream, true);
	fclose(Stream);
	if (!Result) 
	{
		CTools::instance().setLastError(ERR_TAG_NOT_EXIST);
		return false; 
	}
	return RemoveTag(FileName);    
	}
	CTools::instance().setLastError(errno);
	return false;
}

/* -------------------------------------------------------------------------- */

bool CLyrics::SaveTag(LPCWSTR FileName)
{
	long Iterator, FrameSize;
	char Buffer[16];
	Data.Clear();
	FILE *Stream;
	Data.AddMemory(LYRICS_BEGIN, 11);
	for (Iterator = 0; Iterator < LYRICS_FRAME_COUNT; Iterator++)
	{
		FrameSize = FField[Iterator].GetLength();
		if (FrameSize > 0)
		{
			Data.AddMemory(FIELD_LIST[Iterator], 3);
			sprintf_s(Buffer, 16, "%05i", FrameSize);
			//sprintf(Buffer, "%05i", FrameSize);
			Data.AddMemory(&Buffer, 5);
			Data.AddEncodedString(TEXT_ENCODED_ANSI, FField[Iterator], TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
		}
	}
	sprintf_s(Buffer, 16, "%06iLYRICS200", Data.GetLength());
	//sprintf(Buffer, "%06iLYRICS200", Data.GetLength());
	Data.AddMemory(Buffer, 15);
	CTools::LyricsSize = (int)Data.GetLength() - 15;

	FVersion = LYRICS_VERSION_200;
	if ( (Stream = _wfsopen(FileName, READ_AND_WRITE, _SH_DENYWR)) != NULL)
	{
		fseek(Stream, -ID3V1_TAG_SIZE, SEEK_END);
		Data.FileWrite(Data.GetLength(), Stream);
		fwrite(ID3v1Tag, 1, ID3V1_TAG_SIZE, Stream); // ID3v1Tag wurde schon gefüllt
		fflush(Stream);
		fclose(Stream);
		return true;
	}
	else
		CTools::instance().setLastError(errno);
	return false;
}

/* -------------------------------------------------------------------------- */

bool CLyrics::SaveToFile(LPCWSTR FileName)
{
	/* Prepare tag record */
	FILE *Stream;
	if ( (Stream = _wfsopen(FileName, READ_ONLY, _SH_DENYNO)) != NULL)
	{
		ReadHeader(Stream);
		fclose(Stream);
		/*  ID3v1-Tag must exist! */
		if (memcmp(ID3v1Tag, ID3V1_ID, 3) != 0)
		{
			CTools::instance().setLastError(ERR_V1TAG_MISSING);
			return false;
		}
		/* Delete old tag */
		RemoveFromFile(FileName);
		/* Write new tag */
		return SaveTag(FileName);
	}
	else
		CTools::instance().setLastError(errno);
	return false;
}

CAtlString CLyrics::GetTagVersion()
{
	_TCHAR Buf[16]; 
	if (FVersion == LYRICS_VERSION_UNKNOWN)
		return EMPTY;
	_stprintf_s(Buf, 16, _T("%i.00"), FVersion);
	return CAtlString(Buf);
}

