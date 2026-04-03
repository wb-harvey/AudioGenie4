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

const IID CDECL WMA_HEADER_EXTENSION_ID =
{ 0x5FBF03B5, 0xA92E, 0x11CF, { 0x8E,0xE3,0x00,0xC0,0x0C,0x20,0x53,0x65 } };

class CWMA_Header_Extension :
	public CWMA_Object
{
public:
	CWMA_Header_Extension(void);
	~CWMA_Header_Extension(void);
	bool load(FILE *Stream, size_t maxLen);
	void Empty();
	CWMA_Object* findObject(IID objectID);
	bool deleteObject(IID objectID);
	void buildData();
	void replaceObject(CWMA_Object* object);
	void Remove();
private:
	CAtlArray<CWMA_Object *> _children;
	IID reserved1;
	u16 reserved2;
};
