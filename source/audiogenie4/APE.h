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



/* -------------------------------------------------------------------------- */
/* APE.h              for use with APE.cpp                                    */
/* by Stefan Toengi (c) 2003                                                  */
/* -------------------------------------------------------------------------- */
#pragma once

#include "Tools.h"
#include "Blob.h"
#include "ApeTagInfo.h"
#include "id3v1taginfo.h"
#include "ApeTagItem.h"
#include "resource.h"
#include "id3v1.h"
#include "atlcoll.h"

/* First version of APE tag */
#define APE_VERSION_1_0    1000
/* Second version of APE tag */
#define APE_VERSION_2_0    2000

static const LPCWSTR APE_TITLE(_T("TITLE"));
static const LPCWSTR APE_ARTIST(_T("ARTIST"));
static const LPCWSTR APE_ALBUM(_T("ALBUM"));
static const LPCWSTR APE_TRACK(_T("TRACK"));
static const LPCWSTR APE_YEAR(_T("YEAR"));
static const LPCWSTR APE_GENRE(_T("GENRE"));
static const LPCWSTR APE_COMMENT(_T("COMMENT"));
static const LPCWSTR APE_COPYRIGHT(_T("COPYRIGHT"));
static const LPCWSTR APE_COMPOSER(_T("COMPOSER"));

/* Max. number of unsupported tag fields */

class CAPE 
{
private:
	CApeTagInfo TagInfo;
	int FVersion;
	CID3V1 tmpid3v1;
	CApeTagItem* item;
	CBlob Data;
	bool ReadFooter(FILE *Stream);
	bool ReadFields(FILE *Stream);
	bool TruncateFile(LPCWSTR FileName, int Offset);
	bool SaveTag(LPCWSTR FileName);
	void BuildFooter();
	bool AddToFile(LPCWSTR FileName);
	CAtlArray<CApeTagItem *> _items;
public:
	CAPE();
	virtual ~CAPE();
	bool SetTagItem(LPCWSTR FieldName, LPCWSTR Value);
	CAtlString GetTagItem(LPCWSTR key);
	bool Exists()          { return (CTools::APESize > 0); };
	void ResetData();
	void Print();
	bool ReadFromFile(FILE *Stream);
	bool RemoveFromFile(LPCWSTR FileName, bool saveID3v1Tag);
	bool SaveToFile(LPCWSTR FileName);
	CAtlString GetTagVersion();  
	CAtlString GetAllKeys();
	long GetSize()         { return TagInfo.Size; };
	
	CAtlString GetUserDefined(CAtlString key)     { return GetTagItem(key); };
	void SetUserDefined(LPCWSTR key, LPCWSTR value) { SetTagItem(key, value); };
};


