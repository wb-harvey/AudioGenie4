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
#include "Blob.h"
typedef unsigned __int32  u32;
typedef signed __int32  i32;
typedef unsigned __int16  u16;
typedef signed __int16  i16;
typedef unsigned __int64 u64;

class CWMA_Object
{
public:
	CWMA_Object(void);
	CWMA_Object(IID newID);
	virtual ~CWMA_Object();
	virtual size_t getDataSize();
	virtual bool isIID(IID otherID);
	virtual bool load(FILE *Stream, size_t maxLen);
	virtual void save(CBlob *blob);
	virtual void buildData();
	virtual void Remove();
	IID getIID() { return _objectID; };
	CBlob* getData() { buildData(); return &_data; };
protected:
	IID _objectID;
	CBlob _data;	
};


