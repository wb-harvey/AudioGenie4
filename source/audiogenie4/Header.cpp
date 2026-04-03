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

// Header.cpp: Implementierung der Klasse CHeader.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Header.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CHeader::CHeader()
{

}

CHeader::~CHeader()
{

}
// Dateiheader auf bekannte Datentypen prüfen
BYTE CHeader::ReadFromFile(FILE *Stream)
{
  // fseek(Stream, 0, SEEK_SET);
  fread(Buf, 1, 8, Stream); 
  for (int i = 0; i < MAX_FILETYPES; i++)
  {
    if ( memcmp(Buf, filetypes[i].kennung, filetypes[i].laenge) == 0)
      return filetypes[i].result;
  }
  // Noch nichts gefunden, prüfe Bytes 5 - 8
  if (Buf[4] == 0x66 && Buf[5] == 0x74 && Buf[6] == 0x79 && Buf[7] ==  0x70)  // mp4    "ftyp"
	  return AUDIO_FORMAT_MP4;
  return AUDIO_FORMAT_UNKNOWN;  
}