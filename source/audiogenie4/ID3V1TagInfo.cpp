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

// ID3V1TagInfo.cpp: Implementierung der Klasse CID3V1TagInfo.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ID3V1TagInfo.h"
#include "Tools.h"
#include "io.h"

/*
Blob
0      3     evtl APE 
3 	  3 	Kennung „TAG“ zur Kennzeichnung eines ID3v1-Blocks
6 	 30 	Songtitel
36 	 30 	Künstler/Interpret
66 	 30 	Album
96 	  4 	Erscheinungsjahr
100 	 30 	Beliebiger Kommentar
130   1 	Genre


*/



// A field of exactly maxLen bytes: as many whole characters as fit in the ANSI code page, padded with zeros. The
// first maxLen characters were taken, which on a DBCS code page (Chinese, Japanese, Korean) are up to twice as many
// bytes: the fields after it moved and the record was cut at 128 bytes, losing the genre (4.0.4.6 and earlier).
static void AddFixedAnsi(CBlob *dst, const CAtlString &source, int maxLen)
{
  char buf[256];
  int n = 0;
  int k = min(source.GetLength(), maxLen);
  for (; k > 0; k--)
  {
    if (k < source.GetLength() && IS_LOW_SURROGATE(source[k]))
      continue; // (not between the halves of a surrogate pair)
    n = WideCharToMultiByte(CP_ACP, 0, source, k, buf, sizeof(buf), NULL, NULL);
    if (n > 0 && n <= maxLen)
      break;
    n = 0;
  }
  if (n > 0)
    dst->AddMemory(buf, n);
  dst->AddValue(0, maxLen - n);
}

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CID3V1TagInfo::CID3V1TagInfo()
{
  tmp = new CBlob(132);
  Reset();
}

CID3V1TagInfo::~CID3V1TagInfo()
{
	delete tmp;
}

bool CID3V1TagInfo::ReadFromFile(FILE *Stream)
{
  errno = 0;
  _exists = false;
  // Zuerst prüfe ob TAG nicht evtl auf APETAGEX zeigt (BUG 27.08.06)
  fseek(Stream, -131, SEEK_END);
  tmp->FileRead(131, Stream);
  if ( (tmp->GetLength() != 131)
	  || (memcmp(tmp->m_pData, "APETAGEX", 8) == 0) // (a wide _T() literal never matched, 4.0.4.6 and earlier)
	  || (tmp->GetAt(3) != 'T' || tmp->GetAt(4) != 'A' || tmp->GetAt(5) != 'G') )
	  return false;
  
  /*  Alt
  CAtlString st = tmp->GetStringAt(6, 124);
  Title = st.Mid(0, 30).TrimRight();
  Artist = st.Mid(30, 30).TrimRight();
  Album = st.Mid(60, 30).TrimRight();
  Year = st.Mid(90, 4).TrimRight(); 
  Comment = st.Mid(94, 30).TrimRight(); 
  */
  // 4.0.4.9: each field up to its first NUL, in the code page it was written in (GetAnsiStringAt); the NUL padding
  // was part of the text, and a v1.1 comment ended with the track number (4.0.4.8 and earlier)
  Title = tmp->GetAnsiStringAt(6, 30).TrimRight();
  Artist = tmp->GetAnsiStringAt(36, 30).TrimRight();
  Album = tmp->GetAnsiStringAt(66, 30).TrimRight();
  Year = tmp->GetAnsiStringAt(96, 4).TrimRight();
  if ((tmp->GetAt(128) == 0 && tmp->GetAt(129) != 0) ||
      (tmp->GetAt(128) == 32 && tmp->GetAt(129) != 32))
    Track = tmp->GetAt(129);
  else
    Track = 0;
  Comment = tmp->GetAnsiStringAt(100, (Track != 0) ? 28 : 30).TrimRight();

  Genre = tmp->GetAt(130);
  _exists = true;
  return true;
}

bool CID3V1TagInfo::WriteToFile(FILE *Stream)
{
  errno = 0;
  tmp->Clear();
  tmp->AddMemory(ID3V1_ID, 3);
  AddFixedAnsi(tmp, Title ,30);
  AddFixedAnsi(tmp, Artist, 30);
  AddFixedAnsi(tmp, Album, 30);
  AddFixedAnsi(tmp, Year, 4);
  if (Track > 0)
  {
    AddFixedAnsi(tmp, Comment, 28);
    tmp->AddNullByte();
    tmp->AddValue(Track);    
  }
  else
    AddFixedAnsi(tmp, Comment, 30);
  tmp->AddValue(Genre);
  tmp->FileWrite(ID3V1_TAG_SIZE, Stream);
  return (errno == 0);
}

void CID3V1TagInfo::Reset()
{
  _exists = false;
  Album.Empty();
  Artist.Empty();
  Comment.Empty();
  Title.Empty();
  Year.Empty();
  Track = 0;
  Genre = DEFAULT_GENRE;
}

bool CID3V1TagInfo::exists()
{
  return _exists;
}

CAtlString CID3V1TagInfo::GetTagVersion()
{
  if (!_exists)
    return EMPTY;
  if (Track > 0)
    return CAtlString(TAG_VERSION_1_1);
  else
    return CAtlString(TAG_VERSION_1_0);
}

CAtlString CID3V1TagInfo::GetTrack()
{
  if (Track == 0 || Track > 255)
    return EMPTY;
  CAtlString result;
  result.Format(_T("%i"), Track);
  return result;
}

void CID3V1TagInfo::SetTrack(LPCWSTR newValue)
{
  Track = (BYTE)_wtoi(newValue);
  if (Track < 0 || Track > 255) Track = 0;
}

void CID3V1TagInfo::SetGenre(LPCWSTR newValue)
{
  CAtlString vgl = newValue;
  Genre = DEFAULT_GENRE;
  for (int i=0; i < MAX_MUSIC_GENRES; i++)
    if (vgl.CompareNoCase(MUSIC_GENRE[i]) == 0)
      Genre = (BYTE)i;
}
