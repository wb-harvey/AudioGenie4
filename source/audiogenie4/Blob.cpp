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
#include "Blob.h"
#include "Tools.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CBlob::CBlob()
{
	m_CurrentLength = 0;
	m_BufferSize = 0;
	m_pData = NULL;		
}

CBlob::CBlob(size_t pufferSize)
{
	m_CurrentLength = 0;
	m_BufferSize = 0;
	m_pData = NULL;	
	AllocNewBuffer(pufferSize);	
}

CBlob::~CBlob()
{
	Free();
}

void CBlob::Free()
{
	if (m_pData != NULL)
		delete [] m_pData;
	
	m_pData = NULL;
	m_CurrentLength = 0;
	m_BufferSize = 0;
}

void CBlob::AllocNewBuffer(size_t nLen)
{
	ATLASSERT(nLen >= 0);
	if (nLen < m_BufferSize )
		return;

	// Gebe alten Speicher frei
	Free();
	// Hole Neuen Speicher + 2 für Null-Byte
	m_BufferSize = (size_t((nLen + 4) / BLOCKSIZE) + 1) * BLOCKSIZE;
	m_pData = new BYTE[m_BufferSize + 8];
	if (m_pData == NULL)
			CTools::instance().setLastError(ERR_NOT_ENOUGH_MEMORY, nLen);
	
	m_CurrentLength = 0;
}

void CBlob::AssignCopy(size_t nSrcLen, LPCSTR lpszSrcData)
{
	ATLASSERT(nSrcLen >= 0);
	ATLASSERT(lpszSrcData != NULL);
	AllocNewBuffer(nSrcLen);
	memcpy(m_pData, lpszSrcData, nSrcLen);
	m_CurrentLength = nSrcLen;
}

void CBlob::AssignCopy(size_t nSrcLen, LPCWSTR lpszSrcData)
{
	ATLASSERT(nSrcLen >= 0);
	ATLASSERT(lpszSrcData != NULL);
	AllocNewBuffer(nSrcLen * 2);
	memcpy(m_pData, lpszSrcData, nSrcLen * 2);
	m_CurrentLength = nSrcLen * 2;
}


const CBlob& CBlob::operator=(const CBlob& stringSrc)
{
	if (m_pData != stringSrc.m_pData)
	{
		// actual copy necessary since one of the strings is locked
		AssignCopy(stringSrc.m_CurrentLength, (LPCSTR) stringSrc.m_pData);
	}
	return *this;
}

void CBlob::ConcatInPlace(size_t nSrcLen, LPCSTR lpszSrcData)
{
	// concatenating an empty string is a no-op!
	if (nSrcLen <= 0)
		return;
	ATLASSERT(lpszSrcData != NULL);
	//  -- the main routine for += operators
	// if the buffer is too small, or we have a width mis-match, just
	//   allocate a new buffer (slow but sure)
	size_t nNewLen = m_CurrentLength + nSrcLen;
	if (nNewLen >= m_BufferSize)
		GrowBuffer(nNewLen);
	// fast concatenation
	memcpy(m_pData + m_CurrentLength, lpszSrcData, nSrcLen);
	m_CurrentLength += nSrcLen;
}

void CBlob::ConcatInPlace(size_t nSrcLen, LPCWSTR lpszSrcData)
{
	ATLASSERT(nSrcLen >= 0);
	ATLASSERT(lpszSrcData != NULL);
	// concatenating an empty string is a no-op!
	if (nSrcLen <= 0)
		return;

	// if the buffer is too small, or we have a width mis-match, just
	//   allocate a new buffer (slow but sure)
	size_t nNewLen = m_CurrentLength + nSrcLen * 2;
	if (nNewLen >= m_BufferSize)
		GrowBuffer(nNewLen);
	// fast concatenation 
	memcpy(m_pData + m_CurrentLength, lpszSrcData, nSrcLen);
	m_CurrentLength += nSrcLen;  
}

BYTE CBlob::GetAt(size_t nIndex)
{
	ATLASSERT(m_pData != NULL);
	ATLASSERT(nIndex >= 0);
	ATLASSERT(nIndex < m_CurrentLength);
	return m_pData[nIndex];
}

CAtlString CBlob::GetStringAt(size_t nPos, size_t nLength)
{
	ATLASSERT(nPos >= 0 && nPos < m_CurrentLength);
	if (m_CurrentLength == 0 || nPos > m_CurrentLength)
		return EMPTY;
	if (nPos + nLength > m_CurrentLength)
		nLength = m_CurrentLength - nPos;
	CAtlString tmp((LPCSTR) (m_pData + nPos), (int)nLength);
	return tmp;
}

long CBlob::Get4B(size_t nIndex) // MSB Big Endian
{
	ATLASSERT((nIndex + 4) <= m_CurrentLength);
	return (m_pData[nIndex] << 24) + (m_pData[nIndex + 1] << 16) + (m_pData[nIndex + 2] << 8) + m_pData[nIndex + 3];
}

long CBlob::GetR4B(size_t nIndex) // LSB little Endian
{
	ATLASSERT((nIndex + 4) <= m_CurrentLength);		
	return *(long *)(m_pData + nIndex);  
	// return m_pData[nIndex] + (m_pData[nIndex + 1] << 8) + (m_pData[nIndex + 2] << 16) + (m_pData[nIndex + 3] << 24);
}

__int64 CBlob::GetR8B(size_t nIndex)
{
	ATLASSERT((nIndex + 8) <= m_CurrentLength);
	return *(__int64 *)(m_pData + nIndex);  
}

long CBlob::GetS4B(size_t nIndex)
{
	ATLASSERT((nIndex + 4) <= m_CurrentLength);
	return (m_pData[nIndex] << 21) + (m_pData[nIndex + 1] << 14) + (m_pData[nIndex + 2] << 7) + m_pData[nIndex + 3];
}

unsigned __int16 CBlob::Get2B(size_t nIndex)
{
	ATLASSERT((nIndex + 2) <= m_CurrentLength);
	return (m_pData[nIndex] << 8) + m_pData[nIndex + 1];
}

unsigned __int16 CBlob::GetR2B(size_t nIndex)
{
	ATLASSERT((nIndex + 2) <= m_CurrentLength);
	return *(__int16 *)(m_pData + nIndex);  
	// return m_pData[nIndex] + (m_pData[nIndex + 1] << 8);
}

unsigned __int16 CBlob::GetS2B(size_t nIndex)
{
	ATLASSERT((nIndex + 2) <= m_CurrentLength);		
	return (m_pData[nIndex] << 7) + m_pData[nIndex + 1];	
}

long CBlob::Get3B(size_t nIndex)
{
	ATLASSERT((nIndex + 3) <= m_CurrentLength);		
	return (m_pData[nIndex] << 16) + (m_pData[nIndex + 1] << 8) + m_pData[nIndex + 2];
}

void CBlob::FileRead(size_t nLen, FILE *Stream)
{
	ATLASSERT(nLen >= 0 && Stream != NULL);
	if (nLen < 1)
	{
		m_CurrentLength = 0;
		return;
	}
	AllocNewBuffer(nLen);
	m_CurrentLength = fread(m_pData, 1, nLen, Stream);
}

size_t CBlob::FileWrite(size_t nLen, FILE *Stream)
{
	ATLASSERT(nLen >= 0 && Stream != NULL);
	if (nLen > m_CurrentLength)
		nLen = m_CurrentLength;
	if (nLen <= 0)
		return 0;
	return fwrite(m_pData, 1, nLen, Stream);
}

void CBlob::AddFile(size_t nLen, FILE* Stream)
{
	ATLASSERT(nLen >= 0 && Stream != NULL);
	if (nLen < 1)
		return;
	size_t nNewLen = m_CurrentLength + nLen;
	GrowBuffer(nNewLen);
	// fast reading 
	size_t tmpLen = fread(&m_pData[m_CurrentLength], 1, nLen, Stream);
	m_CurrentLength += tmpLen;
}

void CBlob::AddMemory(const void* src, size_t nLen)
{
	ATLASSERT(nLen >= 0 && src != NULL);
	ConcatInPlace(nLen, (LPCSTR)src);
}

void CBlob::Add2B(int Value)
{
	GrowBuffer(m_CurrentLength + 2);
	m_pData[m_CurrentLength++] = (Value >> 8) & 0xFF;
	m_pData[m_CurrentLength++] = Value & 0xFF;	
}

void CBlob::AddR2B(int Value)
{
	GrowBuffer(m_CurrentLength + 2);
	memcpy(m_pData + m_CurrentLength, &Value, 2);
	m_CurrentLength+=2;
}

void CBlob::Add3B(int Value)
{
	GrowBuffer(m_CurrentLength + 3);
	m_pData[m_CurrentLength++] = (Value >> 16) & 0xFF;
	m_pData[m_CurrentLength++] = (Value >> 8) & 0xFF;
	m_pData[m_CurrentLength++] = Value & 0xFF;	
}

void CBlob::Add4B(unsigned int Value)
{
	GrowBuffer(m_CurrentLength + 4);
	m_pData[m_CurrentLength++] = (Value >> 24) & 0xFF;
	m_pData[m_CurrentLength++] = (Value >> 16) & 0xFF;
	m_pData[m_CurrentLength++] = (Value >> 8) & 0xFF;
	m_pData[m_CurrentLength++] = Value & 0xFF;		
}

void CBlob::AddR4B(unsigned int Value)
{
	GrowBuffer(m_CurrentLength + 4);
	memcpy(m_pData + m_CurrentLength, &Value, 4);
	m_CurrentLength+=4;
	/*m_pData[m_CurrentLength++] = Value & 0xFF;
	m_pData[m_CurrentLength++] = (Value >> 8) & 0xFF;
	m_pData[m_CurrentLength++] = (Value >> 16) & 0xFF;
	m_pData[m_CurrentLength++] = (Value >> 24) & 0xFF; */
}

void CBlob::AddR8B(__int64 Value)
{
	GrowBuffer(m_CurrentLength + 8);
	memcpy(m_pData + m_CurrentLength, &Value, 8);
	m_CurrentLength+=8;
}

void CBlob::AddS4B(int Value)
{
	GrowBuffer(m_CurrentLength + 4);
	m_pData[m_CurrentLength++] = (Value >> 21) & 0x7F;
	m_pData[m_CurrentLength++] = (Value >> 14) & 0x7F;
	m_pData[m_CurrentLength++] = (Value >> 7) & 0x7F;
	m_pData[m_CurrentLength++] = Value & 0x7F;
}

void CBlob::AddValue(BYTE ch)
{
	AddValue(ch, 1);
}

void CBlob::AddNullByte()
{
	AddValue(0, 1);
}

void CBlob::AddValue(BYTE ch, size_t nRepeat)
{
	ATLASSERT(nRepeat >= 0);
	if (nRepeat <= 0)
		return;
	size_t nNewLen = m_CurrentLength + nRepeat;
	GrowBuffer(nNewLen);
	memset(&m_pData[m_CurrentLength], ch, nRepeat);
	m_CurrentLength += nRepeat;  
}

void CBlob::GrowBuffer(size_t newLen)
{
	ATLASSERT(newLen >= 0);
	// Buffer is big enough ?
	if (newLen < m_BufferSize)
		return;

	// we have to grow the buffer
	size_t tmpSize = (size_t((newLen + 1) / BLOCKSIZE) + 1) * BLOCKSIZE;
	BYTE* tmpData = new BYTE[tmpSize + 8];
	if (tmpData == NULL)
	{ 
		ATLTRACE(_T("Can't allocate %u bytes...\n"), tmpSize);
		CTools::instance().setLastError(ERR_NOT_ENOUGH_MEMORY, tmpSize + 8);
	}
	if (m_pData != NULL)
	{
		memcpy(tmpData, m_pData, m_CurrentLength);
		delete [] m_pData;
	}
	m_BufferSize = tmpSize;
	m_pData = tmpData;  
}

void CBlob::AddBlob(const CBlob& blob, size_t start)
{
	if (start > blob.m_CurrentLength)
		return;
	if (blob.m_CurrentLength > 0)
		ConcatInPlace(blob.m_CurrentLength - start, (LPCSTR) blob.m_pData + start);
}

void CBlob::AddString(const LPCSTR string)
{
	ATLASSERT(string != NULL);
	ConcatInPlace(strlen(string), string);
}

/* Fügt einen Unicode-String zum Blob hinzu */
void CBlob::AddString(const LPCWSTR string)
{
	ATLASSERT(string != NULL);
	ConcatInPlace(wcslen(string) * 2, string);
}

CAtlString CBlob::getNextString(BYTE encoding, int& startPos)
{
	ATLASSERT(encoding >= 0 && encoding <= 3);
	if (m_CurrentLength == 0 || (size_t)startPos > m_CurrentLength)
		return EMPTY;
	m_pData[m_CurrentLength] = 0;
	m_pData[m_CurrentLength + 1] = 0;

	// find next nullbyte
	size_t endPos = startPos;
	if (encoding == TEXT_ENCODED_UTF16BOM || encoding == TEXT_ENCODED_UTF16)
	{
		while (endPos < m_CurrentLength && (m_pData[endPos] + m_pData[endPos + 1] != 0) ) 
			endPos+=2;
		endPos+=2;
	}
	else
	{
		while (endPos < m_CurrentLength && m_pData[endPos] != 0) 
			endPos++;
		endPos++;
	}
	//
	//int width = (encoding == TEXT_ENCODED_UTF16BOM || encoding == TEXT_ENCODED_UTF16) ? 2 : 1;
	//while (endPos < m_CurrentLength && memcmp(m_pData + endPos, null, width) != 0)
	//	endPos+=width;
	// Setze Endpos auf nächsten Wert
	//endPos+=width;
	if (endPos > m_CurrentLength)
		endPos = m_CurrentLength;
	size_t size = endPos - startPos;
	size_t maxBuffer = (size_t)CTools::instance().configValues[CONFIG_MAXTEXTBUFFER];
	CAtlString result;
	int i, j = 0;
	wchar_t *po = CTools::instance().wcTextPuffer;
	memset(po, 0, min(size * 2 + 32, maxBuffer));
	switch (encoding) // Encoding ID
	{
	case TEXT_ENCODED_ANSI: // ANSI bzw ISO-8859-1
		if (MultiByteToWideChar(CP_ACP, 0, (const char*)(m_pData + startPos), -1, po, (int)maxBuffer) > 0)
			result = CAtlString(po);
		else
		{
			result.Empty();
			CTools::instance().setLastError(ERR_TEXTCONVERT, L"ANSI");
		}
		break;
	case TEXT_ENCODED_UTF16BOM: // UTF-16 with BOM
		if (m_pData[startPos] == 0xFE && m_pData[startPos + 1] == 0xFF) // Big Endian, Bytes vertauschen
		{
			for (i = startPos + 2; i < (int)size; i+=2)
				po[j++] = ((m_pData[i] << 8) + m_pData[i + 1]); 
		}
		else if (m_pData[startPos] == 0xFF && m_pData[startPos + 1] == 0xFE ) // little endian
		{
			memcpy(po, m_pData + startPos + 2, size);   
		}
		else // unknown BOM or corrupt
		{
			result.Empty();
			break;
		}
		result = CAtlString(po);
		break;    
	case TEXT_ENCODED_UTF16: // UTF-16 without BOM
		memcpy(po, m_pData + startPos, size);   
		result = CAtlString(po);
		break;
	case TEXT_ENCODED_UTF8:   // UTF-8 
		if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (const char*)(m_pData + startPos), -1, po, (int)maxBuffer) > 0)
			result = CAtlString(po);
		else
		{
			CTools::instance().setLastError(ERR_TEXTCONVERT, L"UTF-8");
			result.Empty();
		}
		break;
	}
	startPos = (int)endPos;
	return result;
}

// konvertiert aus einem BYTE-Array beliebigen Formates einen Unicode-String
// funktioniert nur mit UNICODE-Übersetzung
// encoding: gibt die Kodierung an
// rest sind Daten ( 0-terminiert )
CAtlString CBlob::ConvertToUnicodeString( BYTE encoding)
{
	if (m_CurrentLength < 1)
		return EMPTY;
	int pos = 0;
	return getNextString(encoding, pos);
}

void CBlob::AddFixedAnsiString(const CAtlString source, size_t maxLen)
{
	CAtlString tmp = source.Left((int)maxLen);
	size_t nLen = tmp.GetLength();
	AddEncodedString(TEXT_ENCODED_ANSI, tmp, false, false);
	if (nLen < maxLen)
		AddValue(0,maxLen - nLen);
}

/**
fügt einen String je nach Kodierung hinzu
**/
void CBlob::AddEncodedString(BYTE encoding, const CAtlString source, bool withEncodingByte, bool withNullBytes)
{
	if (withEncodingByte)
		AddValue(encoding);
	// Falls String leer ist, dann führe keine Konvertierung durch
	if (source.GetLength() == 0)
	{
		if (withNullBytes)
		{
			if (encoding == TEXT_ENCODED_UTF16BOM || encoding == TEXT_ENCODED_UTF16)
				AddValue(0, 2);
			else
				AddNullByte();
		}
		return;
	}

	int maxBuffer = CTools::instance().getConfigValue(CONFIG_MAXTEXTBUFFER);
	char* buf = CTools::instance().cTextPuffer;
	memset(buf, 0, maxBuffer);
	size_t Size;

	switch (encoding) // Encoding ID
	{
	case TEXT_ENCODED_ANSI: // ANSI bzw ISO-8859-1
		Size = WideCharToMultiByte(CP_ACP,0, source, source.GetLength(), buf, maxBuffer, 0, 0);
		if (Size > 0)
			ConcatInPlace(Size, buf);
		else
			CTools::instance().setLastError(ERR_TEXTCONVERT, L"ANSI");
		if (withNullBytes)
			AddNullByte();
		break;
	case TEXT_ENCODED_UTF16BOM: // UTF-16 with BOM
		AddValue(0xFF);
		AddValue(0xFE);
		ConcatInPlace(wcslen(source) * 2, source);
		if (withNullBytes)
			AddValue(0, 2);		
		break;
	case TEXT_ENCODED_UTF16: // UTF-16 without BOM
		ConcatInPlace(wcslen(source) * 2, source);	
		if (withNullBytes)
			AddValue(0, 2);
		break;
	case TEXT_ENCODED_UTF8:   // UTF-8 
		Size = WideCharToMultiByte(CP_UTF8,0, source, source.GetLength(), buf, maxBuffer, 0, 0);
		if (Size > 0)
			ConcatInPlace(Size, buf);
		else
			CTools::instance().setLastError(ERR_TEXTCONVERT, L"UTF-8");			
		if (withNullBytes)
			AddNullByte();
		break;
	}
}
