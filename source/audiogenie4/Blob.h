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


/* -------------------------------------------------------------------------- */
/* Blob.h          for use with String-like BYTE-Fields                       */
/* by Stefan Toengi (c) 2003                                                  */
/* -------------------------------------------------------------------------- */
#pragma once

#define BLOCKSIZE 256
static const LPCWSTR EMPTY(_T(""));
static const LPCWSTR UNKNOWN(_T("Unknown"));
static const LPCWSTR MONO(_T("Mono"));
static const LPCWSTR STEREO(_T("Stereo"));
static const LPCWSTR JOINTSTEREO(_T("Joint Stereo"));
static const LPCWSTR DUALCHANNEL(_T("Dual Channel"));
static const LPCWSTR MULTICHANNEL(_T("Multi Channel"));
static const LPCWSTR READ_ONLY(_T("rb"));
static const LPCWSTR WRITE_ONLY(_T("wb"));
static const LPCWSTR READ_AND_WRITE(_T("r+b"));
static const LPCWSTR READ_AND_WRITENEW(_T("w+b"));
static const LPCWSTR APPEND(_T("ab"));
static const LPCWSTR TILDE(_T("~"));

static const int TEXT_ENCODED_ANSI = 0;     // ISO-8859-1 bzw. ANSI
static const int TEXT_ENCODED_UTF16BOM = 1; // UTF-16 with BOM
static const int TEXT_ENCODED_UTF16 = 2;    // UTF-16 without BOM
static const int TEXT_ENCODED_UTF8 = 3;     // UTF-8
static const bool TEXT_WITH_ENCODING = true;
static const bool TEXT_WITHOUT_ENCODING = false;
static const bool TEXT_WITH_NULLBYTES = true;
static const bool TEXT_WITHOUT_NULLBYTES = false;

class CBlob
{
private:
	size_t m_CurrentLength;
	size_t m_BufferSize;
	void Free();
	void AllocNewBuffer(size_t nLen);
	// Funktionen für ANSI Zeichen
	void AssignCopy(size_t nSrcLen, LPCSTR lpszSrcData);
	void ConcatInPlace(size_t nSrcLen, LPCSTR lpszSrcData);
	// Funktionen für Unicode Zeichen
	void AssignCopy(size_t nSrcLen, LPCWSTR lpszSrcData);
	void ConcatInPlace(size_t nSrcLen, LPCWSTR lpszSrcData);
	void GrowBuffer(size_t newLen);
public:
	BYTE *m_pData;
	CBlob();
	CBlob(size_t pufferSize);
	virtual ~CBlob();
	const CBlob& operator=(const CBlob& stringSrc);
	// ANSI - Methoden
	void AddString(const LPCSTR string);
	// Unicode - Methoden
	void AddString(const LPCWSTR string);
	// Allgemeine Methoden
	void AddMemory(const void *src, size_t nLen);
	void Add2B(int value);
	void Add3B(int value);
	void Add4B(unsigned int value);
	void AddR8B(__int64 value);
	void AddR4B(unsigned int value);
	void AddR2B(int value);
	void AddS4B(int value);
	void AddValue(BYTE ch);
	void AddValue(BYTE ch, size_t nRepeat);
	void AddNullByte();
	void AddBlob(const CBlob& blob, size_t start = 0);
	void AddFile(size_t nLen, FILE *Stream);
	BYTE GetAt(size_t nIndex);
	long Get4B(size_t nIndex);
	long GetR4B(size_t nIndex);
	unsigned __int16 Get2B(size_t nIndex);
	unsigned __int16 GetR2B(size_t nIndex);
	__int64 GetR8B(size_t nIndex);
	long Get3B(size_t nIndex);
	long GetS4B(size_t nIndex);
	unsigned __int16 GetS2B(size_t nIndex);
	
	CAtlString GetStringAt(size_t nPos, size_t nLength);
	void Clear() { Free(); };
	size_t GetLength() { return m_CurrentLength; };
	void FileRead(size_t nLen, FILE *Stream);
	size_t FileWrite(size_t nLen, FILE *Stream);
	bool isEmpty()    { return (m_CurrentLength == 0); };
	bool isNotEmpty() { return (m_CurrentLength > 0); };
	CAtlString ConvertToUnicodeString(BYTE code);
	void AddFixedAnsiString(const CAtlString source, size_t maxLen);
	void AddEncodedString(BYTE encoding, const CAtlString source, bool withEncodingByte = true, bool withNullByte = true);
	CAtlString getNextString(BYTE encoding, int& startPos);
};
