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
CAPE::CAPE()
{
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
	fseek(Stream, - CTools::ID3v1Size - APE_TAG_FOOTER_SIZE, SEEK_END);
	return TagInfo.ReadFromFile(Stream);  
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
	fseek(Stream, -CTools::ID3v1Size - TagInfo.Size, SEEK_END);
	/* Read all stored fields */
	for (Iterator = 0; Iterator < TagInfo.Fields; Iterator++)
	{
		CTools::instance().doEvents();
		item = new CApeTagItem();
		if (item->ReadFromFile(Stream) == false)
		{
			ResetData();
			return false;
		}
		_items.Add(item);
	}
	return true;
}

/* -------------------------------------------------------------------------- */

bool CAPE::TruncateFile(LPCWSTR FileName, int Offset)
{
	int fh;
	if( _wsopen_s(&fh,  FileName, _O_RDWR | _O_BINARY, _SH_DENYWR, _S_IREAD | _S_IWRITE )  == 0 )
	{
		long ln = _filelength(fh) - Offset;
		if (ln > 0)
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
	if (ReadFooter(Stream))
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
		fclose(Source);
		if (result)
		{
			if ( (TagInfo.Flags >> 31) != 0 )
				TagInfo.Size+= APE_TAG_HEADER_SIZE;
			// APE + ID3v1 Tag löschen!
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
	if (tmpid3v1.GetSize() > 0)
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