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

// VorbisComment.cpp: Implementierung der Klasse CVorbisComment.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VorbisComment.h"
#include "Tools.h"
#include "io.h"

// Comments are decoded and encoded here with buffers of their own size: through the shared text buffer (256 KB)
// a longer value (a METADATA_BLOCK_PICTURE in base64) read empty, and so did one that isn't valid UTF-8; both
// were then dropped on the next save (4.0.4.6 and earlier). Invalid UTF-8 is read as Windows-1252.
static CAtlString DecodeComment(const BYTE *p, size_t n)
{
	CAtlString result;
	const BYTE *nul = (n > 0) ? (const BYTE *)memchr(p, 0, n) : NULL;
	if (nul != NULL)
		n = nul - p; // (as before: the text ends at a null byte)
	if (n == 0 || n > 0x7FFFFFFF)
		return result;
	UINT cp = CP_UTF8;
	int wn = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCSTR)p, (int)n, NULL, 0);
	if (wn <= 0)
	{
		cp = 1252;
		wn = MultiByteToWideChar(cp, 0, (LPCSTR)p, (int)n, NULL, 0);
		if (wn <= 0)
			return result;
	}
	wchar_t *w = (wchar_t *)malloc(((size_t)wn + 1) * sizeof(wchar_t));
	if (w == NULL)
	{
		CTools::instance().setLastError(ERR_NOT_ENOUGH_MEMORY, (unsigned int)((wn + 1) * sizeof(wchar_t)));
		return result;
	}
	wn = MultiByteToWideChar(cp, cp == CP_UTF8 ? MB_ERR_INVALID_CHARS : 0, (LPCSTR)p, (int)n, w, wn);
	try
	{
		if (wn > 0)
			result.SetString(w, wn);
	}
	catch (...)
	{
		result.Empty();
	}
	free(w);
	return result;
}

static void EncodeComment(CBlob &dst, const CAtlString &s)
{
	int len = s.GetLength();
	if (len <= 0)
		return;
	int n = WideCharToMultiByte(CP_UTF8, 0, s, len, NULL, 0, NULL, NULL);
	if (n <= 0)
		return;
	char *buf = (char *)malloc(n);
	if (buf == NULL)
	{
		CTools::instance().setLastError(ERR_NOT_ENOUGH_MEMORY, n);
		return;
	}
	n = WideCharToMultiByte(CP_UTF8, 0, s, len, buf, n, NULL, NULL);
	if (n > 0)
		dst.AddMemory(buf, n);
	free(buf);
}

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CVorbisComment::CVorbisComment()
{
	_items.SetCount(0, 20);	
}

CVorbisComment::~CVorbisComment()
{
	ResetData();
}

void CVorbisComment::ResetData()
{
	Fields = 0;
	VendorInfo = DEFAULT_VENDOR;
	for (size_t i = 0; i < _items.GetCount(); i++)
		delete _items.GetAt(i);
	_items.RemoveAll();	
}

void CVorbisComment::BuildVorbisComments(CBlob &Data)
{
	/* Build Comments into Blob */
	CBlob str;
	int Index;
	// Build Vorbis tag
	Fields = 0;
	content.Empty();
	//int maxLen = VendorInfo.GetLength();
	Fields = (int)_items.GetCount();
	// vendor info and number of fields
	EncodeComment(str, VendorInfo);
	Data.AddR4B((int)str.GetLength());
	Data.AddBlob(str); 
	Data.AddR4B(Fields);
	// Write tag fields
	if (Fields == 0)
		return;
	for (Index = 0; Index < Fields; Index++)
	{
		item = _items.GetAt(Index);	
		content = item->key + L"=" + item->value;
		//content = item->key;
		//content+=_T("=");
		//content+=item->value;
		str.Clear();
		EncodeComment(str, content);
		Data.AddR4B((int)str.GetLength());
		Data.AddBlob(str);		
	}
}

void CVorbisComment::AnalyzeVorbisComments(FILE *Stream)
{
	long i, len = 0, Separator;
	CBlob tmpBlob;
	CAtlString temp;
	// every length is checked against what is left of the file (the vendor's wasn't: a huge one was read, 4.0.4.6
	// and earlier), and every read must be whole
	__int64 fileLength = _filelengthi64(_fileno(Stream));
	// Vendor
	tmpBlob.FileRead(4, Stream);
	if (tmpBlob.GetLength() != 4)
		return;
	len = tmpBlob.GetR4B(0);
	if (len < 0 || len > fileLength - _ftelli64(Stream))
		return;
	tmpBlob.FileRead(len, Stream);
	if ((long)tmpBlob.GetLength() != len)
		return;
	VendorInfo = DecodeComment(tmpBlob.m_pData, len);
	// weiter gehts mit Anzahl der Felder
	tmpBlob.FileRead(4, Stream);
	if (tmpBlob.GetLength() != 4)
		return;
	Fields = tmpBlob.GetR4B(0);
	if (Fields <= 0)
		return;

	for (i = 0; i < Fields ; i++)
	{
		tmpBlob.FileRead(4, Stream);
		if (tmpBlob.GetLength() != 4)
			return;
		len = tmpBlob.GetR4B(0);
		if (len < 0 || len > fileLength - _ftelli64(Stream))
			return;
		tmpBlob.FileRead(len, Stream);
		if ((long)tmpBlob.GetLength() != len)
			return;
		temp = DecodeComment(tmpBlob.m_pData, len);
		if ((Separator = temp.Find(_T("="))) > 0)
		{
			item = new structField;
			item->key = temp.Left(Separator);
			item->value = temp.Mid(Separator + 1);
			_items.Add(item);
		}
	}
}

void CVorbisComment::AnalyzeVorbisComments(CBlob &Data, size_t pos)
{
	/* The comments from a packet read whole (an Ogg comment packet can span pages); every length is checked
	   against what is left of the packet */
	size_t total = Data.GetLength();
	size_t len;
	long Separator;
	CBlob tmpBlob;
	CAtlString temp;
	// Vendor
	if (pos + 4 > total)
		return;
	len = (size_t)(unsigned long)Data.GetR4B(pos);
	pos += 4;
	if (len > total - pos)
		return;
	VendorInfo = DecodeComment(Data.m_pData + pos, len);
	pos += len;
	// Number of fields
	if (pos + 4 > total)
		return;
	Fields = Data.GetR4B(pos);
	pos += 4;
	for (long i = 0; i < Fields; i++)
	{
		if (pos + 4 > total)
			return;
		len = (size_t)(unsigned long)Data.GetR4B(pos);
		pos += 4;
		if (len > total - pos)
			return;
		temp = DecodeComment(Data.m_pData + pos, len);
		pos += len;
		if ((Separator = temp.Find(_T("="))) > 0)
		{
			item = new structField;
			item->key = temp.Left(Separator);
			item->value = temp.Mid(Separator + 1);
			_items.Add(item);
		}
	}
}

CAtlString CVorbisComment::GetUserItem(LPCWSTR key)
{
	CAtlString result;
	size_t counts = _items.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		item = _items.GetAt(i);
		if (item->key.CompareNoCase(key) == 0)
		{
			if (result.GetLength() > 0)
				result+=_T("|");
			result+=item->value;
		}
	}
	return result;
}

void CVorbisComment::SetUserItem(LPCWSTR key, LPCWSTR value)
{
	// Every entry with this key is replaced (only the first was, while GetUserItem joins them all with "|": the
	// value read back and saved grew by the other entries on every save, 4.0.4.6 and earlier). When the key had
	// several entries, the value is split again at "|" into one entry each, so a multi-value field read and saved
	// unchanged stays as it was; a key with one entry keeps a "|" in its value. An empty value removes them all.
	bool empty = (value == 0 || wcslen(value) == 0);
	CAtlString all(empty ? L"" : value);
	size_t found = 0, position = _items.GetCount();
	CAtlString name(key);
	for (size_t i = _items.GetCount(); i-- > 0; )
	{
		item = _items.GetAt(i);
		if (item->key.CompareNoCase(key) == 0)
		{
			found++;
			position = i;
			name = item->key; // (the file's spelling of the key, as before)
			delete item;
			_items.RemoveAt(i);
		}
	}
	item = NULL;
	// falls Value leer ist, verlasse Prozedur
	if (empty)
		return ;
	int start = 0;
	for (;;)
	{
		int bar = (found > 1) ? all.Find(L'|', start) : -1;
		structField *field = new structField;
		field->key = name;
		field->value = (bar < 0) ? all.Mid(start) : all.Mid(start, bar - start);
		_items.InsertAt(position++, field);
		if (bar < 0)
			break;
		start = bar + 1;
	}
}

CAtlString CVorbisComment::GetAllKeys()
{
	CAtlString result;
	/* search all stored fields for key */
	size_t counts = _items.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		if (result.GetLength() > 0)
			result+=",";
		result+=_items.GetAt(i)->key;		
	}
	return result;
} 