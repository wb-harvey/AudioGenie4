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

// VorbisComment.h: Schnittstelle für die Klasse CVorbisComment.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Blob.h"
#include "atlcoll.h"

static const LPCWSTR DEFAULT_VENDOR = _T("Xiph.Org libVorbis I 20020717");
static const LPCWSTR VORBIS_VENDORINFO = _T("VENDORINFO");
static const LPCWSTR VORBIS_TITLE = _T("TITLE"); 
static const LPCWSTR VORBIS_ARTIST = _T("ARTIST"); 
static const LPCWSTR VORBIS_ALBUM = _T("ALBUM"); 
static const LPCWSTR VORBIS_TRACKNUMBER = _T("TRACKNUMBER");
static const LPCWSTR VORBIS_DATE = _T("DATE"); 
static const LPCWSTR VORBIS_GENRE = _T("GENRE"); 
static const LPCWSTR VORBIS_COMMENT = _T("COMMENT"); 
static const LPCWSTR VORBIS_PERFORMER = _T("PERFORMER"); 
static const LPCWSTR VORBIS_DESCRIPTION = _T("DESCRIPTION");
static const LPCWSTR VORBIS_VERSION = _T("VERSION"); 
static const LPCWSTR VORBIS_COPYRIGHT = _T("COPYRIGHT");
static const LPCWSTR VORBIS_LICENSE = _T("LICENSE");
static const LPCWSTR VORBIS_ORGANIZATION = _T("ORGANIZATION"); 
static const LPCWSTR VORBIS_LOCATION = _T("LOCATION");
static const LPCWSTR VORBIS_CONTACT =_T("CONTACT"); 
static const LPCWSTR VORBIS_ISRC = _T("ISRC");
static const LPCWSTR VORBIS_COMPOSER = _T("COMPOSER");

struct structField
{
	CAtlString key;
	CAtlString value;
};

class CVorbisComment  
{
private:
	CAtlString content, VendorInfo;
	CAtlArray<structField *> _items;
	structField* item;
	int Fields;	
public:
	CVorbisComment();
	virtual ~CVorbisComment();
	void ResetData();
	void BuildVorbisComments(CBlob &Data);
	void AnalyzeVorbisComments(FILE * Stream);
	CAtlString GetUserItem(LPCWSTR key);
	void SetUserItem(LPCWSTR key, LPCWSTR value);
	CAtlString GetAllKeys();
	// Getter
	CAtlString GetVendor()       { return VendorInfo;  };
	// Setter
	void SetVendor(LPCWSTR Value)       { VendorInfo = Value; };
};


