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
#include "stdio.h"
#include "io.h"
#include "Blob.h"
#include "Tools.h"
#include "ape.h"
#include <fcntl.h>
#include <ctype.h>
#include "sys/stat.h"
#include "share.h"
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
static long Lyrics3BlockSize(FILE *Stream); // (below)

CAPE::CAPE()
{
	FLyricsSkip = 0;
	_items.SetCount(0, 20);
}
CAPE::~CAPE()
{
	ResetData();
}

/* -------------------------------------------------------------------------- */

void CAPE::ResetData()
{ /* Reset all variables */
	for (size_t i = 0; i < _items.GetCount(); i++)
		delete _items.GetAt(i);
	_items.RemoveAll();
	TagInfo.Reset();
	CTools::APESize  = 0;
	FVersion = 0;
	Data.Clear();
}

/* -------------------------------------------------------------------------- */

bool CAPE::ReadFooter(FILE *Stream)
{
	/* Read footer data */
	FLyricsSkip = 0;
	fseek(Stream, - CTools::ID3v1Size - APE_TAG_FOOTER_SIZE, SEEK_END);
	if (TagInfo.ReadFromFile(Stream))
		return true;
	// before a Lyrics3 tag (other taggers put the APE tag there): it read as no APE tag, and a save added a second
	long lyrics = Lyrics3BlockSize(Stream);
	if (lyrics <= 0)
		return false;
	fseek(Stream, - CTools::ID3v1Size - lyrics - APE_TAG_FOOTER_SIZE, SEEK_END);
	if (!TagInfo.ReadFromFile(Stream))
		return false;
	FLyricsSkip = lyrics;
	return true;
}

/* -------------------------------------------------------------------------- */

bool CAPE::SetTagItem(LPCWSTR FieldName, LPCWSTR Value)
{
	
	/* Set tag item if supported field found */
	for (size_t Iterator = 0; Iterator < _items.GetCount(); Iterator++)
	{
		item = _items[Iterator];
		if (item->Key.CompareNoCase(FieldName) == 0)
		{
			// Falls Wert leer ist, lösche Schlüssel
			if (Value == 0 || wcslen(Value) == 0)
			{
				delete item;
				_items.RemoveAt(Iterator);
				return true;
			}
			item->Value.Clear();
			item->Flags = 0x00;
			if (TagInfo.Version > APE_VERSION_1_0)
				item->Value.AddEncodedString(TEXT_ENCODED_UTF8, Value, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
			else
				item->Value.AddString(Value);
			return true;
		}
	}
	/* not found, make a new entry if new Value != blank */
	if (Value == 0 || wcslen(Value) == 0)
		return true;

	item = new CApeTagItem();
	item->Key = FieldName;
	item->Value.Clear();
	item->Flags = 0x00;
	if (TagInfo.Version == 0)
		TagInfo.Version = APE_VERSION_2_0;

	if (TagInfo.Version > APE_VERSION_1_0)
		item->Value.AddEncodedString(TEXT_ENCODED_UTF8, Value, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
	// TagItem[newEntry].Value.AddAsUTF8(Value);
	else
		item->Value.AddEncodedString(TEXT_ENCODED_ANSI, Value, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
	_items.Add(item);
	return true;
}

/* -------------------------------------------------------------------------- */

CAtlString CAPE::GetTagItem(LPCWSTR FieldName)
{
	/* search all stored fields for key */
	for (size_t Iterator = 0; Iterator < _items.GetCount(); Iterator++)
	{
		item = _items.GetAt(Iterator);
		if ( item->Key.CompareNoCase(FieldName) == 0 && !item->isBinary() )
		{
			if (TagInfo.Version > APE_VERSION_1_0)
				return item->Value.ConvertToUnicodeString(TEXT_ENCODED_UTF8);
			else
				return item->Value.ConvertToUnicodeString(TEXT_ENCODED_ANSI);
		}
	}
	return EMPTY;
}

/* -------------------------------------------------------------------------- */

bool CAPE::ReadFields(FILE *Stream)
{
	CAtlString FieldName;
	long Iterator;
	fseek(Stream, -CTools::ID3v1Size - FLyricsSkip - TagInfo.Size, SEEK_END);
	/* Read all stored fields */
	for (Iterator = 0; Iterator < TagInfo.Fields; Iterator++)
	{
		CTools::instance().doEvents();
		item = new CApeTagItem();
		if (item->ReadFromFile(Stream) == false)
		{
			delete item; // (it isn't in _items yet: ResetData doesn't free it)
			item = NULL;
			ResetData();
			return false;
		}
		_items.Add(item);
	}
	return true;
}

/* -------------------------------------------------------------------------- */

// The size of a Lyrics3 tag (v1 or v2) right before the file's ID3v1 tag, its "LYRICSBEGIN" to its
// "LYRICSEND"/"LYRICS200"; 0 when there is none.
static long Lyrics3BlockSize(FILE *Stream)
{
	if (CTools::ID3v1Size != ID3V1_TAG_SIZE)
		return 0; // (a Lyrics3 tag is always followed by an ID3v1 tag)
	__int64 end = _filelengthi64(_fileno(Stream)) - ID3V1_TAG_SIZE;
	if (end < 9 + 11)
		return 0;
	char tail[9];
	if (_fseeki64(Stream, end - 9, SEEK_SET) != 0 || fread(tail, 1, 9, Stream) != 9)
		return 0;
	__int64 start = -1;
	if (memcmp(tail, "LYRICS200", 9) == 0)
	{
		char digits[7] = { 0 };
		if (end < 15 + 11 || _fseeki64(Stream, end - 15, SEEK_SET) != 0 || fread(digits, 1, 6, Stream) != 6)
			return 0;
		for (int i = 0; i < 6; i++)
			if (digits[i] < '0' || digits[i] > '9')
				return 0;
		start = end - 15 - atol(digits);
	}
	else if (memcmp(tail, "LYRICSEND", 9) == 0)
	{
		// version 1 has no size: its LYRICSBEGIN is in the 5100 bytes before
		__int64 from = end - 9 - 5100;
		if (from < 0)
			from = 0;
		int n = (int)(end - 9 - from);
		char *buf = (char *)malloc(n);
		if (buf == NULL)
			return 0;
		if (_fseeki64(Stream, from, SEEK_SET) == 0 && (int)fread(buf, 1, n, Stream) == n)
			for (int i = n - 11; i >= 0 && start < 0; i--)
				if (memcmp(buf + i, "LYRICSBEGIN", 11) == 0)
					start = from + i;
		free(buf);
	}
	if (start < 0)
		return 0;
	char begin[11];
	if (_fseeki64(Stream, start, SEEK_SET) != 0 || fread(begin, 1, 11, Stream) != 11 || memcmp(begin, "LYRICSBEGIN", 11) != 0)
		return 0;
	return (long)(end - start);
}

/* -------------------------------------------------------------------------- */

// Removes an APE tag of apeSize bytes that sits before a Lyrics3 tag of FLyricsSkip bytes: the Lyrics3 tag is
// moved down over it and the ID3v1 tag after it is cut (SaveTag / RemoveFromFile write that back).
bool CAPE::CutBeforeLyrics(LPCWSTR FileName, long apeSize)
{
	int fh;
	if (_wsopen_s(&fh, FileName, _O_RDWR | _O_BINARY, _SH_DENYWR, _S_IREAD | _S_IWRITE) != 0)
	{
		CTools::instance().setLastError(errno);
		return false;
	}
	__int64 len = _filelengthi64(fh);
	__int64 lyricsStart = len - tmpid3v1.GetSize() - FLyricsSkip;
	__int64 apeStart = lyricsStart - apeSize;
	char *buf = (apeStart > 0 && FLyricsSkip > 0) ? (char *)malloc(FLyricsSkip) : NULL;
	bool ok = buf != NULL &&
		_lseeki64(fh, lyricsStart, SEEK_SET) == lyricsStart && _read(fh, buf, FLyricsSkip) == FLyricsSkip &&
		_lseeki64(fh, apeStart, SEEK_SET) == apeStart && _write(fh, buf, FLyricsSkip) == FLyricsSkip &&
		_chsize_s(fh, apeStart + FLyricsSkip) == 0;
	if (buf != NULL)
		free(buf);
	_close(fh);
	if (!ok)
		CTools::instance().setLastError(ERR_INVALID_FORMAT);
	return ok;
}

/* -------------------------------------------------------------------------- */

bool CAPE::TruncateFile(LPCWSTR FileName, int Offset)
{
	int fh;
	if( _wsopen_s(&fh,  FileName, _O_RDWR | _O_BINARY, _SH_DENYWR, _S_IREAD | _S_IWRITE )  == 0 )
	{
		long ln = _filelength(fh) - Offset;
		if (Offset < 0 || ln <= 0)
		{
			// the tags' sizes reach past the start of the file (a damaged APE footer): nothing is cut, and the save
			// stops (it went on, and the new tags were written after the old ones)
			_close( fh );
			CTools::instance().setLastError(ERR_INVALID_FORMAT);
			return false;
		}
		_chsize_s(fh, ln);
		_close( fh );
		return true;
	}
	CTools::instance().setLastError(errno);
	return false;
}

/* -------------------------------------------------------------------------- */

bool CAPE::AddToFile(LPCWSTR FileName)
{ /* Add tag data to file */
	FILE * Stream;
	if ( (Stream = _wfsopen(FileName, APPEND, _SH_DENYWR)) != NULL)
	{
		Data.FileWrite(Data.GetLength(), Stream);
		fflush(Stream);
		fclose(Stream);
		return true;
	}
	CTools::instance().setLastError(errno);
	return false;
}

/* -------------------------------------------------------------------------- */

void CAPE::BuildFooter()
{
	CBlob tmpBlob;
	size_t Iterator;
	/* Build tag footer */
	TagInfo.Reset();
	TagInfo.Version = APE_VERSION_2_0;
	TagInfo.Size = APE_TAG_FOOTER_SIZE;
	for (Iterator = 0; Iterator < _items.GetCount(); Iterator++)
	{
		item = _items.GetAt(Iterator);
		TagInfo.Size+=(long)item->Value.GetLength();
		TagInfo.Size+=(long)item->Key.GetLength();
		TagInfo.Size+= 9; // 4 + 4 + 1		

	}
	TagInfo.Fields = (long)_items.GetCount();
}


/* -------------------------------------------------------------------------- */

bool CAPE::SaveTag(LPCWSTR FileName)
{
	size_t Iterator;
	long ValueSize, Flags;
	/* Build and write tag fields and footer to stream */
	Data.Clear();
	BuildFooter();
	/* Flags auf Footer Anfang setzen */
	TagInfo.Flags = (0xA0 << 24);
	TagInfo.WriteToBlob(Data);
	Flags = 0;
	CBlob tmpBlob;
	for (Iterator = 0; Iterator < _items.GetCount(); Iterator++)
	{
		item = _items.GetAt(Iterator);
		ValueSize = (long)item->Value.GetLength();
		Data.AddR4B(ValueSize);
		Data.AddR4B(item->Flags); // Flag = 0;
		Data.AddEncodedString(TEXT_ENCODED_ANSI, item->Key, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
		Data.AddValue(0);
		Data.AddBlob(item->Value);		
	}
	/* Flags auf Footer Ende setzen */
	TagInfo.Flags = (0x80 << 24);
	TagInfo.WriteToBlob(Data);
	/* Add created tag to file */
	bool result = AddToFile(FileName);
	// ID3v1 Tag evtl berücksichtigen
	if (tmpid3v1.GetSize() > 0)
		tmpid3v1.SaveToFile(FileName);
	return result;
}

/* -------------------------------------------------------------------------- */

bool CAPE::ReadFromFile(FILE *Stream)
{
	/* Process data if loaded and footer is valid */
	// a footer whose size can't be: below its own 32 bytes (2 GB or more reads negative), or longer than the
	// file with the ID3v1 tag (the fields were then read from wherever the failed seek left the stream)
	if (ReadFooter(Stream) && TagInfo.Size >= 32 && (__int64)TagInfo.Size + CTools::ID3v1Size + FLyricsSkip <= (__int64)CTools::FileSize)
	{
		CTools::APESize = TagInfo.Size;
		FVersion = TagInfo.Version;
		/* Get information from fields */
		return ReadFields(Stream);    
	}
	ResetData();
	return false;
}

/* -------------------------------------------------------------------------- */

bool CAPE::RemoveFromFile(LPCWSTR FileName, bool saveID3v1Tag)
{ /* Remove tag from file if found */
	FILE *Source;
	if ( (Source = _wfsopen(FileName, READ_ONLY, _SH_DENYWR)) != NULL)
	{
		// ID3v1 Tag lesen und merken
		tmpid3v1.ReadFromFile(Source);

		bool result = ReadFooter(Source);
		__int64 fileLength = _filelengthi64(_fileno(Source));
		fclose(Source);
		if (result)
		{
			// A damaged footer: a size below the footer's own 32 bytes (a size of 2 GB or more reads negative), or
			// one that, with the header and the ID3v1 tag, reaches the start of the file. The cut went past the
			// start (4.0.4.4 refused that), or, negative, grew the file with zeros; nothing is cut and the save stops.
			// (The footer's own size, before the header is added: a size of 8 with the header passed as 40.)
			bool damaged = TagInfo.Size < 32;
			if ( (TagInfo.Flags >> 31) != 0 )
				TagInfo.Size+= APE_TAG_HEADER_SIZE;
			if (damaged || TagInfo.Size < 32 || (__int64)TagInfo.Size + tmpid3v1.GetSize() + FLyricsSkip >= fileLength)
			{
				CTools::instance().setLastError(ERR_INVALID_FORMAT);
				TagInfo.Reset();
				return false;
			}
			// APE + ID3v1 Tag löschen!
			// (a tag before a Lyrics3 tag: cut out from between, the Lyrics3 tag moved down over it; the ID3v1 tag goes
			// with the cut as below, and is written back after the new APE tag)
			if (FLyricsSkip > 0)
				result = CutBeforeLyrics(FileName, TagInfo.Size);
			else
				result = TruncateFile(FileName, tmpid3v1.GetSize() + TagInfo.Size);
			// ID3v1-Tag wieder speichern falls vorhanden und erwünscht
			if (saveID3v1Tag && tmpid3v1.GetSize() > 0)
				tmpid3v1.SaveToFile(FileName);
			TagInfo.Reset();
			return result;
		}
		else 
		{ 
			CTools::instance().setLastError(ERR_TAG_NOT_EXIST);
			TagInfo.Reset();
			return false;
		}
	}
	CTools::instance().setLastError(errno);
	return false;
}

/* -------------------------------------------------------------------------- */

bool CAPE::SaveToFile(LPCWSTR FileName)
{
	/* Delete old tag if exists and write new tag */
	CTools::instance().setLastError(0);
	// APE und ID3v1 Tag löschen
	bool result = RemoveFromFile(FileName, false);
	// Tag existiert nicht ist in dem Fall kein Fehler
	if (result == false && CTools::instance().getLastError() != ERR_TAG_NOT_EXIST)
		return false;
	CTools::instance().setLastError(0);
	// With no APE tag, RemoveFromFile cut nothing: the ID3v1 tag is cut here (SaveTag puts it back after the new
	// APE tag). When it removed an APE tag it had already cut the ID3v1 tag with it, and cutting 128 more bytes
	// took them off the end of the audio, on every save (4.0.4.1 and earlier).
	if (!result && tmpid3v1.GetSize() > 0)
		TruncateFile(FileName, 128);
	return SaveTag(FileName);  
}

/* -------------------------------------------------------------------------- */

CAtlString CAPE::GetTagVersion()
{
	if (FVersion == 0)
		return EMPTY;
	CAtlString tmp;
	tmp.Format(_T("%i"), FVersion);
	return tmp;	
}

/* -------------------------------------------------------------------------- */

CAtlString CAPE::GetAllKeys()
{
	CAtlString result;
	/* search all stored fields for key */
	for (size_t Iterator = 0; Iterator < _items.GetCount(); Iterator++)
	{
		item = _items.GetAt(Iterator);
		if ( !item->isBinary() )
		{ 
			if (result.GetLength() > 0)
				result+=_T(",");
			result+=item->Key;     
		}
	}
	return result;
}