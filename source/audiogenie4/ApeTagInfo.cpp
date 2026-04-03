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

// ApeTagInfo.cpp: Implementierung der Klasse ApeTagInfo.
// http://web.archive.org/web/20041010184518/www.personal.uni-jena.de/~pfk/mpp/sv8/apetag.html
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ApeTagInfo.h"

bool CApeTagInfo::ReadFromFile(FILE *Stream)
{
  errno = 0;
  CBlob tmp;
  tmp.FileRead(32, Stream);
  if (memcmp(tmp.m_pData, APE_ID, 8) != 0)  // must be "APETAGEX"
	  return false;
  Version = (long)tmp.GetR4B(8);            // Tag version
  Size = (long)tmp.GetR4B(12);              // Tag size including footer
  Fields = (long)tmp.GetR4B(16);            // Number of fields
  Flags = (long)tmp.GetR4B(20);             // Tag flags
  // Reserved = tmp.Get4B(24) + tmp.Get4B(28);  
  return (errno == 0);
}
void CApeTagInfo::WriteToBlob(CBlob &blob)
{
  blob.AddMemory(APE_ID, 8);
  blob.AddR4B(Version);
  blob.AddR4B(Size);
  blob.AddR4B(Fields);
  blob.AddR4B(Flags);
  blob.AddValue(0, 8);
}

void CApeTagInfo::Reset()
{
  Version = 0;
  Size = 0;
  Fields = 0;
  Flags = 0;
}
