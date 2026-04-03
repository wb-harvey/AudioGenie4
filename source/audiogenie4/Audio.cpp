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

// Audio.cpp: Implementierung der Klasse CAudio.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Audio.h"


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CAudio::CAudio()
{
}

CAudio::~CAudio()
{

}
// Default-Implementierung
long CAudio::GetBitRate()
{
  float Duration = GetDuration();
  if (Duration > 0)
    return (long)((CTools::FileSize - CTools::ID3v1Size - CTools::ID3v2Size - CTools::LyricsSize - CTools::APESize ) / 125 / Duration + 0.5) ;
  else
    return 0;
}

// Default-Implementierung
CAtlString CAudio::GetChannelMode()
{
  switch (GetChannels())
  {
    case 1: return MONO;
    case 2: return STEREO;
    default: return UNKNOWN;
  }
}
