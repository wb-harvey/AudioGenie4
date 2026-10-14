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

class CFlacCover
{
public:
	CFlacCover(void);
	CFlacCover(CBlob *data);
	CFlacCover(BYTE picType, LPCWSTR Description);
	~CFlacCover(void);
	int type;
	int width;
	int height;
	int colordepth;
	int colornumbers;
	CAtlString description;
	CBlob data;
	void calcInfos();
	long getPictureSize();
	bool getPictureFile(LPCWSTR fileName);
	long getPictureArray(BYTE *arr, size_t maxLen);
	CAtlString getPictureTypeAsText();
	bool setPictureFile(LPCWSTR fileName);
	bool setFileLink(LPCWSTR fileName);
	void setPictureArray(const BYTE *arr, u32 len);
	CAtlString getMime();
	void CFlacCover::Save(CBlob * destination);
	bool isValid() { return _valid; }; // false: a PICTURE block whose lengths run past its end
private:
	bool _valid;
	bool _isLink;
	CAtlString _mime;
	CAtlString _pictureLink;
};

