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

#include "StdAfx.h"
#include "WMA_TagData.h"
#include "wma_extcontentdescription.h"
#include "id3f_apic.h"
#include "Tools.h"
#include "WMA_File.h"

CWMA_TagData::CWMA_TagData(BYTE art)
{
	_art = art;
	_loadedSize = 0;
	FieldSize = 0;
	Type = 0;
	LanguageListIndex = 0;
	StreamNumber = 0;
	FieldName.Empty();
}

CWMA_TagData::CWMA_TagData(CAtlString fieldName, CAtlString fieldValue)
{
	FieldName = fieldName;
	_art = EXTCONTENT_ART;
	_loadedSize = 0;
	Type = 0;
	LanguageListIndex = 0;
	StreamNumber = 0;
	FieldSize = (u16)FieldName.GetLength() * 2 + 2;
	setNewValue(fieldValue);
}
CWMA_TagData::~CWMA_TagData(void)
{	
}

bool CWMA_TagData::setNewPicture(BYTE *arr, u32 len, LPCWSTR Description, BYTE picType)
{
	FieldName = WM_PICTURE;
	FieldSize = (u16)FieldName.GetLength() * 2 + 2;
	Type = 1;
	LanguageListIndex = 0;
	StreamNumber = 0;
	/* WM/Picture is structured as follows
	** 1st byte is the picture type (value between 0 and 20)
	** Next 4 bytes are in LE format. They give the size of the picture data
	** Next is a LPWSTR which gives the MIME type
	** Next is a LPWSTR which gives a picture description
	** Then is the actual picture data */
	_data.Clear();
	_data.AddValue(picType);
	_data.AddR4B(int(len));
	_data.AddEncodedString(TEXT_ENCODED_UTF16, CTools::instance().ExtractMimeFromPicture(arr), false, true);
	_data.AddEncodedString(TEXT_ENCODED_UTF16, Description, false, true);
	_data.AddMemory(arr, len);
	fitArt();
	return false;
}

void CWMA_TagData::fitArt()
{
	// 4.0.4.7: an Extended Content value has a 16-bit length; a longer one (a picture just under 65500 bytes plus
	// its MIME type and description, text over 32767 characters) wrapped it and broke every descriptor after it
	// (4.0.4.6 and earlier). It goes to the Metadata Library, which has a 32-bit length.
	if (_art == EXTCONTENT_ART && _data.GetLength() > 0xFFFF)
		_art = METADATA_ART;
}

void CWMA_TagData::setNewValue(CAtlString newValue)
{
	_data.Clear();
	if (Type == 2) // BOOL (32)
	{
		_data.AddR4B((newValue.CompareNoCase(_T("TRUE")) == 0) ? 1 : 0);
		return;
	}
	else if (Type == 3) // DWORD (32)
	{
		// Kann der Wert rückstandsfrei in eine Zahl konvertiert werden ?	
		// Wenn nicht, dann speichere als Text!
		CAtlString tmp;
		tmp.Format(_T("%d"), _wtol(newValue));
		if (tmp.Compare(newValue) == 0)
		{
			_data.AddR4B(_wtol(newValue));
			return;
		}
	}
	else if (Type == 5) // WORD (16)
	{
		// Kann der Wert rückstandsfrei in eine Zahl konvertiert werden ?	
		// Wenn nicht, dann speichere als Text!
		CAtlString tmp;
		tmp.Format(_T("%d"), _wtoi(newValue));
		if (tmp.Compare(newValue) == 0)
		{
			_data.AddR2B(_wtoi(newValue));
			return;
		}
	}
	// UNICODE
	Type = 0;
	if (newValue.GetLength() > 0)
	{
		_data.AddString(newValue);
		_data.AddValue(0, 2);
	}
	fitArt();
}

bool CWMA_TagData::load(FILE *Stream, size_t maxLen)
{
	// 4.0.4.7: maxLen is what is left of the object. The name and value sizes come from the file and are checked
	// against it, and every read must return all its bytes; a Metadata Library value size of up to 4 GB was read
	// as it was (an overflow on x86, bad_alloc out of the DLL) (4.0.4.6 and earlier)
	_data.Clear();
	_loadedSize = 0;
	u32	DataSize = 0;
	size_t headerSize;
	try
	{
		if (_art == EXTCONTENT_ART)
		{
			_data.FileRead(2, Stream);
			if (_data.GetLength() != 2 || maxLen < 2)
				goto corrupt;
			FieldSize = _data.GetR2B(0);
			if ((size_t)FieldSize + 6 > maxLen)
				goto corrupt;
			/* Read field name */
			_data.FileRead(FieldSize, Stream);
			if (_data.GetLength() != FieldSize)
				goto corrupt;
			FieldName = _data.ConvertToUnicodeString(TEXT_ENCODED_UTF16);
			/* Read value data type */
			_data.FileRead(4, Stream);
			if (_data.GetLength() != 4)
				goto corrupt;
			Type = _data.GetR2B(0);
			DataSize = (u32)_data.GetR2B(2);
			headerSize = (size_t)FieldSize + 6;
		}
		else if (_art == METADATA_ART)
		{
			_data.FileRead(12, Stream);
			if (_data.GetLength() != 12 || maxLen < 12)
				goto corrupt;
			LanguageListIndex = _data.GetR2B(0);
			StreamNumber = _data.GetR2B(2);
			FieldSize = _data.GetR2B(4);
			Type = _data.GetR2B(6);
			DataSize = _data.GetR4B(8);
			if ((size_t)FieldSize + 12 > maxLen)
				goto corrupt;
			/* Read field name */
			_data.FileRead(FieldSize, Stream);
			if (_data.GetLength() != FieldSize)
				goto corrupt;
			FieldName = _data.ConvertToUnicodeString(TEXT_ENCODED_UTF16);
			headerSize = (size_t)FieldSize + 12;
		}
		else
		{
			ATLASSERT(_T("Ungültige TagData Art!"));
			return false;
		}
		if (DataSize > maxLen - headerSize)
			goto corrupt;
		ATLTRACE(_T("   FieldName: %s with %u bytes "), FieldName, DataSize);
		_data.FileRead(DataSize, Stream);
		if (_data.GetLength() != DataSize)
			goto corrupt;
		_loadedSize = headerSize + DataSize;
		return true;
	}
	catch (...)
	{
	}
corrupt:
	CTools::instance().setLastError(ERR_WMA_PARSE);
	_data.Clear();
	return false;
}

CBlob* CWMA_TagData::getData()
{
	return &_data;	
}

void CWMA_TagData::buildData(CBlob* blob)
{
	// 4.0.4.7: the name size of the name written (the size read from the file was written again, and the following
	// descriptors were out of step when the name in the file had other padding) (4.0.4.6 and earlier)
	FieldSize = (u16)(FieldName.GetLength() * 2 + 2);
	if (_art == METADATA_ART)
	{
		blob->AddR2B(LanguageListIndex);
		blob->AddR2B(StreamNumber);
		blob->AddR2B(FieldSize);
		blob->AddR2B(Type);
		blob->AddR4B((int)_data.GetLength());
		blob->AddEncodedString(TEXT_ENCODED_UTF16, FieldName, false, true);
	}
	else if (_art == EXTCONTENT_ART)
	{
		blob->AddR2B(FieldSize);
		blob->AddEncodedString(TEXT_ENCODED_UTF16, FieldName, false, true);
		blob->AddR2B(Type);
		blob->AddR2B((int)_data.GetLength());
	}
	blob->AddBlob(_data);

}
bool CWMA_TagData::IsFieldName(CAtlString tstString)
{
	return (FieldName.CompareNoCase(tstString) == 0);
}

bool CWMA_TagData::IsPicture()
{
	return (FieldName.CompareNoCase(WM_PICTURE) == 0);
}

const CAtlString CWMA_TagData::getFieldName()
{
	return FieldName;
}

CAtlString CWMA_TagData::getFieldValue()
{
	// Datatypes , 0=Unicode, 1=Byte-Array, 2=Bool(32), 3=DWord(32), 4=QWord(64), 5=Word(16), 6=GUID
	if (Type == 0) // UNICODE
		return _data.ConvertToUnicodeString(TEXT_ENCODED_UTF16);

	if (Type == 2)  // BOOL
		return (_data.GetR4B(0) != 0) ? _T("true") : _T("false");

	CAtlString FieldValue;
	if (Type == 3) // DWORD (32)
		FieldValue.Format(_T("%u"), _data.GetR4B(0));
	else if (Type == 4) // QWORD (64)
		FieldValue.Format(_T("%I64u"), _data.GetR8B(0)); // 4.0.4.7: %u took half of the 64-bit value
	else if (Type == 5) // WORD (16)
		FieldValue.Format(_T("%u"), _data.GetR2B(0));
	else if (Type == 6) // GUID
		FieldValue.Format(_T("%.08X-%.04X-%.04X-%.02X%.02X-%.02X%.02X%.02X%.02X%.02X%.02X"), _data.GetR4B(0), (u16)_data.GetR2B(4), (u16)_data.GetR2B(6), _data.GetAt(8), _data.GetAt(9), _data.GetAt(10), _data.GetAt(11), _data.GetAt(12), _data.GetAt(13), _data.GetAt(14), _data.GetAt(15));
	return FieldValue;
}