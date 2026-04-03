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

#include "StdAfx.h"
#include "ID3F_SEEK.h"
#include "id3_framefactory.h"

/*
Seek frame

This frame indicates where other tags in a file/stream can be found.
The 'minimum offset to next tag' is calculated from the end of this
tag to the beginning of the next. There may only be one 'seek frame'
in a tag.

<Header for 'Seek frame', ID: "SEEK">
Minimum offset to next tag       $xx xx xx xx
*/

CID3F_SEEK::CID3F_SEEK() 	
{
	init(0);	
}

CID3F_SEEK::CID3F_SEEK(long offset)
{
	init(offset);
}
void CID3F_SEEK::init(long offset)
{
	_frameID = F_SEEK;
	_offset = offset;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_SEEK::~CID3F_SEEK(void)
{

}

void CID3F_SEEK::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		if (_blob.GetLength() < 4)
		{
			_offset = 0;
		}
		else
		{
			_offset = _blob.Get4B(0);			
		}
		isDecoded = true;
	}	
}
void CID3F_SEEK::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.Add4B(_offset);
		mustRebuild = false;
	}
}

long CID3F_SEEK::getOffset()
{
	decode();
	return _offset;
}
