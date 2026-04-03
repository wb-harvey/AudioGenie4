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
#include "id3f_apic.h"

typedef unsigned __int16  u16;

#define METADATA_ART 1
#define EXTCONTENT_ART 2

class CWMA_TagData
{
public:
	CWMA_TagData(BYTE art);
	CWMA_TagData(CAtlString fieldName, CAtlString fieldValue);
	~CWMA_TagData(void);
	bool load(FILE *Stream, size_t maxLen); 
	bool IsFieldName(CAtlString tstString);
	void setNewValue(CAtlString newValue);
	void ConvertPicture();
	CBlob* getData();
	bool IsPicture();
	CAtlString getFieldValue();
	BYTE getArt() { return _art; };
	const CAtlString getFieldName();
	bool setNewPicture(BYTE *arr, u32 len, LPCWSTR Description, BYTE picType);
	void buildData(CBlob* blob);
private:
	BYTE _art;
	u16 FieldSize, Type, LanguageListIndex, StreamNumber;
	CAtlString FieldName;
	CBlob _data;
};
