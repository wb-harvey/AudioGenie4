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
#include <share.h>
#include "Blob.h"
#include "ID3_frame.h"
#include "ID3_FrameFactory.h"
#include "atlcoll.h"

#define TAG_VERSION_2_2  2                    /* Code for ID3v2.2.x tag */
#define TAG_VERSION_2_3  3                    /* Code for ID3v2.3.x tag */
#define TAG_VERSION_2_4  4                    /* Code for ID3v2.4.x tag */

static const long ID3V2_TAGID = 'ID3';

static const bool ALLOWED[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

class CID3V2
{
public:
	CID3V2(void);
	virtual ~CID3V2(void);
	BYTE buf[16];
	u32 ID, FrameID;
	u32 FrameSize, Size, oldTagSize;
	u16 FrameFlags;
	//static BYTE oldTagVersion, newTagVersion, defaultEncodingID, Flags;
	BYTE Revision, Version;	
	bool ReadHeader(FILE *Stream);
	void ReadFromFile(FILE *Stream);
	void WriteHeader(CBlob *blob, long tagSize);
	bool SaveTag(LPCWSTR FileName);
	bool ReplaceTag(LPCWSTR FileName, CBlob* data);
	bool RebuildFile(LPCWSTR FileName, CBlob* data);
	bool setTargetFormatAndEncoding(BYTE newFormat, BYTE newEncoding);
	bool replaceFrame(CID3_Frame *frame);
	bool RemoveFromFile(LPCWSTR FileName);
	bool SaveToFile(LPCWSTR FileName);
	CID3_Frame* findFrame(u32 ID, int nr = 1);	
	CID3_Frame* findFrame(CAtlString ID);
	CID3_Frame* findNextFrame(u32 ID, size_t& index);
	short countFrame(u32 ID);
	bool deleteFrame(u32 ID, int index = 1);
	short deleteAllFrames(u32 ID);
	short deleteAllFrames(CAtlString ID);
	CAtlString GetText(u32 ID);
	CAtlString GetURL(u32 ID);
	CAtlString GetGenre();
	void SetText(u32 ID, LPCWSTR newText);
	void SetURL(u32 ID, LPCWSTR newURL);
	void storeFrames(CBlob *blob);
	CAtlString getAllFrameIDs();
	u32 calcTagSize();
	void parseTags(CBlob* data);
	bool isValid;
	void ResetData();
	bool parseCueFile(LPCWSTR FileName);
	short getEncoding(u32 FrameID);
	CAtlArray<CID3_Frame *> _frames;
};
