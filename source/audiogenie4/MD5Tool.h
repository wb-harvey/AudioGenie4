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
#include "md5.h"

#define MAX_MD5_BUFFER 1024*16

class CMD5Tool
{
protected:
	MD5 *md5;
	MD5_CTX ctx;
	BYTE *buffer;
	CAtlString hash;
private:
	void hashIt();
public:
	CMD5Tool(void);
	~CMD5Tool(void);
	CAtlString getHashValue()	{ return hash; };
	bool calcHashFromFile(LPCWSTR FileName, long startPos, long endPos);	
};
