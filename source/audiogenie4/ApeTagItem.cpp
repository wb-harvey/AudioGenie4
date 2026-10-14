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

// ApeTagItem.cpp: Implementierung der Klasse CApeTagItem.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ApeTagItem.h"
#include "Tools.h"
// Format:  http://web.archive.org/web/20041026140532/www.personal.uni-jena.de/~pfk/mpp/sv8/apetagitem.html

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CApeTagItem::CApeTagItem()
{

}

CApeTagItem::~CApeTagItem()
{

}

bool CApeTagItem::ReadFromFile(FILE *Stream)
{
  errno = 0;
  CBlob tmp;
  tmp.FileRead(8, Stream);
  Flags = 0;
  Size = (long)tmp.GetR4B(0);   // Tag size including footer
  // (in 64 bits: a size near 2 GB wrapped the 32-bit sum negative, passed, and a 2 GB value was allocated)
  if (Size < 0 || ((__int64)ftell(Stream) + Size + CTools::ID3v1Size) > (__int64)CTools::FileSize )
	  return false;
  Flags = (long)tmp.GetR4B(4);  // Tag flags
  Key.Empty();
  // up to its NUL; the end of the file (or a key past APE's 255 characters) is a damaged item: the loop
  // never checked the read, and at the end of the file it kept the last character and spun for ever
  for (;;)
  {
    if (fread(&NextChar, 1, 1, Stream) != 1 || Key.GetLength() > 255)
      return false;
    if (NextChar == 0)
      break;
    Key+= NextChar;
  }
  Value.FileRead(Size, Stream);  
  return (errno == 0);
}

bool CApeTagItem::WriteToFile(FILE *Stream)
{
  errno = 0;
  CBlob tmp;
  tmp.AddR4B(Size);        // Tag size including footer
  tmp.AddR4B(Flags);       // Tag flags
  tmp.FileWrite(8, Stream);
  return (errno == 0);
}

void CApeTagItem::Reset()
{
  Size = 0;
  Flags = 0;
  Key.Empty();
  Value.Clear();
}

bool CApeTagItem::isBinary()
{
  return ((Flags >> 1) != 0);
}