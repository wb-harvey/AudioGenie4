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
#include "Tools.h"
#include "wma_header.h"
#include "Audio.h"
#include "wma_objectfactory.h"

static const LPCWSTR WM_TITLE = _T("WM/Title");
static const LPCWSTR WM_TITLE2 = _T("Title");
static const LPCWSTR WM_AUTHOR = _T("WM/Author");
static const LPCWSTR WM_AUTHOR2 = _T("Author");
static const LPCWSTR WM_DESCRIPTION = _T("WM/Description");
static const LPCWSTR WM_DESCRIPTION2 = _T("Description");
static const LPCWSTR WM_TRACKNUMBER = _T("WM/TrackNumber");
static const LPCWSTR WM_PICTURE = _T("WM/Picture");
static const LPCWSTR WM_COPYRIGHT = _T("WM/ProviderCopyright");
static const LPCWSTR WM_COPYRIGHT2 = _T("Copyright");
static const LPCWSTR WM_PROVIDERRATING = _T("WM/ProviderRating");
static const LPCWSTR WM_PROVIDERRATING2 = _T("Rating");
static const LPCWSTR WM_ALBUMTITLE = _T("WM/AlbumTitle");
static const LPCWSTR WM_GENRE = _T("WM/Genre");
static const LPCWSTR WM_YEAR = _T("WM/Year");
static const LPCWSTR WM_COMPOSER = _T("WM/Composer");

class CWMA_File: public CAudio
{
public:
	CWMA_File(void);
	~CWMA_File(void);
	bool ReadFromFile(FILE *Stream);
	bool SaveToFile(LPCWSTR FileName);
	CAtlString GetUserItem(CAtlString key);
    void SetUserItem(LPCWSTR key, LPCWSTR item);
	TagPictureStruct* getPictureTag(int pos);
	bool DeletePictureFrame(short index);
	void DeletePictures();
	int GetPictures();
	CAtlString GetPictureDescription(int index);
	int GetPictureType(int index);
	long GetPictureSize(int index);
	bool GetPicture(int index, LPCWSTR fileName);
	size_t GetPictureArray(BYTE *arr, u32 maxLen, short Index);
	CAtlString GetPictureMime(int index);
	CAtlString GetItemKeys();
	bool SetPicture(LPCWSTR FileName, LPCWSTR Description, short Index, short PictureType);
	bool SetPictureArray(BYTE *arr, u32 maxLen, LPCWSTR Description, short index, short picType);
	bool IsVBR();
	void ResetData();
	// CAudio Member
	long GetChannels();
	float GetDuration();
	long GetSampleRate();
	long GetBitRate();
	bool IsValid();
	long GetFirstAudioPosition() { return DataPosition; };	
private:
	 CWMA_Header header;
	 long DataPosition;
	 bool _valid;
};

