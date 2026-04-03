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

#include "ID3_frame.h"
#include "atlcoll.h"

class CID3F_Chapter:public CID3_Frame
{
public:
	CID3F_Chapter(void);
	~CID3F_Chapter(void);
	bool equals(CID3_Frame *frame);
	void addFrame(CID3_Frame* frame);
	void deleteFrame(u32 ID);
	void deleteAllFrames();
	short getFrameCount();
	u32 getFrameID(u16 index);
	BYTE getFrameTyp(u16 index);
	CAtlString getFrameText(u16 index);
	CAtlString getFrameDescription(u16 index);
	CID3_Frame* getFrame(u16 index);
	CID3_Frame* findFrame(u32 ID, int index = 1);
	void convertBlobToFrames();
	void convertFramesToBlob();
	CAtlString getID();
	CAtlString getTitle();
	CAtlString getDescription();
	void setTitle(LPCWSTR title);
	void setDescription(LPCWSTR description);
	bool isCTOC() { return (_frameID == 'CTOC'); };
	bool isCHAP() { return (_frameID == 'CHAP'); };
	CAtlString _title, _description, _ID;
	CAtlArray<CID3_Frame *> _frames;
};
