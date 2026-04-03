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

const IID CDECL WMA_HEADER_OBJECT =
{ 0x75B22630, 0x668E, 0x11CF, { 0xA6,0xD9,0x00,0xAA,0x00,0x62,0xCE,0x6C } };


class CWMA_Header :
	public CWMA_Object
{
public:
	CWMA_Header(void);
	virtual ~CWMA_Header(void);
	bool load(FILE *Stream, size_t maxLen);
	__int64 loadHeaderOnly(FILE *Stream);
	void buildData();
	void Empty();
	CWMA_Object* findObject(IID objectID);
	bool deleteObject(IID objectID);
	void insertObject(CWMA_Object* object, size_t position);
	void replaceObject(CWMA_Object* object);
	void Remove();
private:
	IID testIID;
	CAtlArray<CWMA_Object *> _children;	
};
