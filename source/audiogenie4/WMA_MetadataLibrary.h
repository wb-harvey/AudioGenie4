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

#pragma once
#include "wma_object.h"
#include "atlcoll.h"
#include "Tools.h"
#include "wma_tagdata.h"

const IID CDECL WMA_METADATA_LIBRARY_ID = 
{ 0x44231C94, 0x9498, 0x49D1, { 0xA1,0x41,0x1D,0x13,0x4E,0x45,0x70,0x54 } };

class CWMA_MetadataLibrary :
	public CWMA_Object
{
public:
	CWMA_MetadataLibrary(void);
	CWMA_MetadataLibrary(CBlob *datas);
	~CWMA_MetadataLibrary(void);
	bool load(FILE *Stream, size_t maxLen);	
	void buildData();
private:
	bool _mustBuild;
};
