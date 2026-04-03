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


// Audio.h: Schnittstelle für die Klasse CAudio.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Tools.h"

class CAudio
{
public:
	CAudio();
	virtual ~CAudio();
	// Virtuelle Methoden mit default-Werten
	virtual long GetChannels()       { return 0; };
	virtual float GetDuration()      { return 0; };
	virtual long GetSampleRate()     { return 0; };
	virtual long GetBitRate();
	virtual CAtlString GetChannelMode();
	virtual CAtlString GetFileVersion() { return L"";   };
	virtual bool IsValid()           { return false;     };
	virtual long GetFirstAudioPosition() { return CTools::ID3v2Size; };
	virtual long GetLastAudioPosition()  { return CTools::FileSize - 1; };
};


