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


// ApeTagInfo.h: Schnittstelle für die Klasse ApeTagInfo.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Blob.h"

static const char *APE_ID = "APETAGEX";                                   // APE

#define APE_TAG_FOOTER_SIZE  32                                // APE tag footer
#define APE_TAG_HEADER_SIZE  32                                // APE tag header

class CApeTagInfo
{

public:
	CApeTagInfo()  { Reset(); };
	~CApeTagInfo() {};
	// Datenstruktur
	long Version;                                             // Tag version
	long Size;                                                // Tag size including footer
	long Fields;                                              // Number of fields
	long Flags;                                               // Tag flags
	// char Reserved[8];                                      // Reserved for later use
	// Methoden
	bool ReadFromFile(FILE *Stream);
	//bool WriteToFile(FILE *Stream);
	void WriteToBlob(CBlob &blob);
	void Reset();	
};

