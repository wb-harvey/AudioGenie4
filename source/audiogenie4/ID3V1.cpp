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

// ID3V1.cpp: Implementierung der Klasse CID3V1. ( Kein Unicode )
// Schreibt nur ANSI
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ID3V1.h"
#include "Blob.h"
#include "id3v1taginfo.h"
#include "io.h"
#include <fcntl.h>
#include "sys/stat.h"
#include "share.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CID3V1::CID3V1()
{
}

CID3V1::~CID3V1()
{
}

void CID3V1::ResetData()
{
	id3v1tag.Reset();
	CTools::ID3v1Size = 0;
}

bool CID3V1::OpenFile(LPCWSTR FileName, bool WriteModus)
{
	CAtlString modus;
	if (WriteModus)
		modus = READ_AND_WRITE;
	else
		modus = READ_ONLY;
	id3v1tag._exists = false;			
	if ( (Stream = _wfsopen(FileName, modus, _SH_DENYNO)) != NULL)
  {
		/* Read tag */
		char ap[10];
		fseek(Stream, -131, SEEK_END);
		fread(ap, 1, 8, Stream);
		if (ap[0] == 'A' && ap[1] == 'P' && ap[2] == 'E' && ap[3] == 'T' && ap[4] == 'A'&& ap[5] == 'G' && ap[6] == 'E' && ap[7] == 'X')
		{
			id3v1tag._exists = false;			
		}
		else if(ap[3] == 'T' && ap[4] == 'A' && ap[5] == 'G')
		{
			id3v1tag._exists = true;
		}
		fseek(Stream, -ID3V1_TAG_SIZE, SEEK_END);
		return true;
	}
	CTools::instance().setLastError(errno);
	return false;
}

void CID3V1::CloseFile()
{
	if (Stream != NULL)
		fclose(Stream);
}

void CID3V1::ReadFromFile(FILE *Stream)
{
	/* Reset and load tag data from file to variable */
	//fseek(Stream, -ID3V1_TAG_SIZE, SEEK_END);
	if (id3v1tag.ReadFromFile(Stream))
	{
		CTools::ID3v1Size = ID3V1_TAG_SIZE;		
		return;
	}
	//ResetData();
}

CAtlString CID3V1::GetGenre()
{
	if (id3v1tag.Genre >= 0 && id3v1tag.Genre < MAX_MUSIC_GENRES)
		return MUSIC_GENRE[id3v1tag.Genre];
	else
		return EMPTY;
}

CAtlString CID3V1::GetGenreItem(int i)
{
	if (i >= 0 && i < MAX_MUSIC_GENRES)
		return MUSIC_GENRE[i];
	else
		return EMPTY;
}

bool CID3V1::RemoveTag(LPCWSTR FileName)
{
	int fh;
	/* Open a file */
	if (_wsopen_s(&fh, FileName, _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE) == 0)
	{
		long ln = _filelength(fh) - 128;
		if (ln > 0)
			_chsize_s(fh, ln);
		_close(fh);
		return true;
	}
	else
		CTools::instance().setLastError(errno);
	return false;
};

bool CID3V1::RemoveFromFile(LPCWSTR FileName)
{
	/* Find tag */
	bool Result = OpenFile(FileName, true);
	CloseFile();
	if (!Result)
		return false;
	/* Delete tag if loaded and tag header OK */
	if (id3v1tag.exists())
		return RemoveTag(FileName);

	CTools::instance().setLastError(ERR_TAG_NOT_EXIST);
	return false;
};

bool CID3V1::AddTag(LPCWSTR FileName)
{
	bool result = false;
	/* Write tag */
	if ( (Stream = _wfsopen(FileName, APPEND, _SH_DENYWR)) != NULL)
  {
		result = id3v1tag.WriteToFile(Stream);
		fclose(Stream);
	}
	else
		CTools::instance().setLastError(errno);
	return result;
};

bool CID3V1::SaveToFile(LPCWSTR FileName)
{
	bool Result = OpenFile(FileName, true);
	// Wenn exisitert, dann ersetzen
	if (Result && id3v1tag.exists())
	{
		fseek(Stream, -128, SEEK_END);
		Result = id3v1tag.WriteToFile(Stream);
		CloseFile();
	}
	else
	{
		CloseFile();
		Result = AddTag(FileName);
	}
	return Result;
};
