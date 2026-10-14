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
#include <limits.h>

// whether n chars are all '0'-'9' (atoi took a sign or spaces, and a negative size)
static bool AllDigits(const char *s, int n)
{
	for (int i = 0; i < n; i++)
		if (s[i] < '0' || s[i] > '9')
			return false;
	return true;
}
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

	// Only cut at a checked "LYRICSBEGIN" before the file's ID3v1 tag: a start of 0 or a stale one wrote the
	// ID3v1 tag at that offset and cut the file to 128 bytes (4.0.4.6 and earlier)
	__int64 len = _filelengthi64(fh);
	__int64 start = FStartPosition;
	char begin[11];
	bool ok = start > 0 && start + 11 + ID3V1_TAG_SIZE <= len &&
		/* Save the ID3 Tag */
		_lseeki64(fh, len - ID3V1_TAG_SIZE, SEEK_SET) == len - ID3V1_TAG_SIZE &&
		_read(fh, ID3v1Tag, ID3V1_TAG_SIZE) == ID3V1_TAG_SIZE && memcmp(ID3v1Tag, ID3V1_ID, 3) == 0 &&
		_lseeki64(fh, start, SEEK_SET) == start && _read(fh, begin, 11) == 11 && memcmp(begin, LYRICS_BEGIN, 11) == 0 &&
		/* write the ID3 Tag */
		_lseeki64(fh, start, SEEK_SET) == start && _write(fh, ID3v1Tag, ID3V1_TAG_SIZE) == ID3V1_TAG_SIZE &&
		_chsize_s(fh, start + ID3V1_TAG_SIZE) == 0;
	_close(fh);
	if (!ok)
		CTools::instance().setLastError(ERR_INVALID_FORMAT);
	return ok;
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
	memset(FHeader, 0, sizeof(FHeader));
	memset(ID3v1Tag, 0, ID3V1_TAG_SIZE);
	/* Read header and get Version & Size */
	// the stream's own length: CTools::FileSize is the length at the last analyze, stale after a save (4.0.4.6 and earlier)
	__int64 len = _filelengthi64(_fileno(Stream));
	if (len >= ID3V1_TAG_SIZE && (_fseeki64(Stream, len - ID3V1_TAG_SIZE, SEEK_SET) != 0 ||
		fread(ID3v1Tag, 1, ID3V1_TAG_SIZE, Stream) != ID3V1_TAG_SIZE))
		memset(ID3v1Tag, 0, ID3V1_TAG_SIZE);
	if (len < ID3V1_TAG_SIZE + 9 + 11 || len - ID3V1_TAG_SIZE - 9 > LONG_MAX)
		return false;
	FEndPosition = (long)(len - ID3V1_TAG_SIZE - 9);
	if (_fseeki64(Stream, FEndPosition, SEEK_SET) != 0 || fread(FHeader, 1, 9, Stream) != 9)
		return false;
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
	FStartPosition = -1; // (no tag found: RemoveTag cuts nothing)
	StartPosition = FEndPosition - 5100;
	if (StartPosition < 0)
		StartPosition = 0;
	DataSize = FEndPosition - StartPosition;
	Data.Clear();
	/* Get information from Lyrics (Version 1.00) */
	if (DataSize < 11 || _fseeki64(Stream, StartPosition, SEEK_SET) != 0)
		return;
	Data.FileRead(DataSize, Stream);
	if ((long)Data.GetLength() != DataSize)
		return;
	/* Search for Begin (the last one before LYRICSEND) */
	for (Iterator = DataSize - 11; Iterator >= 0; Iterator--)
	{
		if (memcmp(Data.m_pData + Iterator, LYRICS_BEGIN, 11) == 0)
		{
			FStartPosition = StartPosition + Iterator;

			CTools::LyricsSize = DataSize - Iterator - 11;
			if (isDeleting)
				return;
			SetTagItem("LYR", Iterator + 11, CTools::LyricsSize);
			return;
		}
	}
}

/* -------------------------------------------------------------------------- */

void CLyrics::ReadFramesNew(FILE *Stream,bool isDeleting)
{
	long Transferred, DataSize, Iterator, Length;
	char Buffer[10];
	memset(Buffer, 0, 10);
	Data.Clear();
	Transferred = 0;
	FStartPosition = -1; // (no tag found: RemoveTag cuts nothing)
	/* Get information from Lyrics (Version 2.00) */
	if (FEndPosition < 6 + 11 || _fseeki64(Stream, FEndPosition - 6, SEEK_SET) != 0 || fread(Buffer, 1, 6, Stream) != 6)
		return;
	/* Convert from Char into Size */
	if (!AllDigits(Buffer, 6))
		return;
	DataSize = atoi(Buffer);
	if (DataSize < 11 || DataSize > FEndPosition - 6)
		return;

	// the tag must start with LYRICSBEGIN where its size says (also when deleting: RemoveTag cuts there)
	if (_fseeki64(Stream, FEndPosition - 6 - DataSize, SEEK_SET) != 0)
		return;
	Data.FileRead(DataSize, Stream);
	Length = (long)Data.GetLength();
	if (Length != DataSize || memcmp(Data.m_pData, LYRICS_BEGIN, 11) != 0)
		return;
	CTools::LyricsSize = DataSize;
	FStartPosition = FEndPosition - 6 - DataSize;
	if (isDeleting)
		return;
	Iterator = 11;
	// each field: 3-byte ID, 5-digit size, data; a size that isn't digits or runs past the tag ends the walk
	// (a negative one looped forever or read before the buffer, 4.0.4.6 and earlier)
	while (Iterator + 8 <= Length)
	{
		memcpy(Buffer, Data.m_pData + Iterator, 8);
		Buffer[8] = 0;
		if (!AllDigits(Buffer + 3, 5))
			break;
		Transferred = atoi(Buffer + 3);
		if (Transferred < 0 || Transferred > Length - Iterator - 8)
			break;
		SetTagItem(Buffer, Iterator + 8, Transferred);
		Iterator += Transferred + 8;
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
		if (FStartPosition >= 0)
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
	{ FStartPosition = -1;
	Result = ReadHeader(Stream);
	if (FVersion == LYRICS_VERSION_200)
		ReadFramesNew(Stream, true);
	if (FVersion == LYRICS_VERSION_100)
		ReadFramesOld(Stream, true);
	fclose(Stream);
	if (!Result || FStartPosition <= 0) // (no LYRICSBEGIN found: nothing to cut)
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
		// the size field is the field's bytes, not its characters (a DBCS field put a wrong size); at most 5 digits
		CBlob field;
		field.AddEncodedString(TEXT_ENCODED_ANSI, FField[Iterator], TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
		FrameSize = (long)field.GetLength();
		if (FrameSize > 0 && FrameSize <= 99999)
		{
			Data.AddMemory(FIELD_LIST[Iterator], 3);
			sprintf_s(Buffer, 16, "%05i", FrameSize);
			//sprintf(Buffer, "%05i", FrameSize);
			Data.AddMemory(&Buffer, 5);
			Data.AddBlob(field);
		}
	}
	if (Data.GetLength() > 999999)
	{
		CTools::instance().setLastError(ERR_FRAME_TOO_BIG);
		return false;
	}
	sprintf_s(Buffer, 16, "%06iLYRICS200", (int)Data.GetLength());
	//sprintf(Buffer, "%06iLYRICS200", Data.GetLength());
	Data.AddMemory(Buffer, 15);

	FVersion = LYRICS_VERSION_200;
	if ( (Stream = _wfsopen(FileName, READ_AND_WRITE, _SH_DENYWR)) != NULL)
	{
		bool ok = _fseeki64(Stream, -ID3V1_TAG_SIZE, SEEK_END) == 0 &&
			Data.FileWrite(Data.GetLength(), Stream) == Data.GetLength() &&
			fwrite(ID3v1Tag, 1, ID3V1_TAG_SIZE, Stream) == (size_t)ID3V1_TAG_SIZE && // ID3v1Tag wurde schon gefuellt
			fflush(Stream) == 0 && !ferror(Stream);
		if (fclose(Stream) != 0)
			ok = false;
		if (!ok)
		{
			CTools::instance().setLastError(ERR_INVALID_FORMAT);
			return false;
		}
		CTools::LyricsSize = (int)Data.GetLength() - 15;
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
		// (one that is there but couldn't be cut stops the save: the new tag went in after it, a second one)
		if (!RemoveFromFile(FileName) && CTools::instance().getLastError() != ERR_TAG_NOT_EXIST)
			return false;
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

