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

#include "StdAfx.h"
#include "ID3F_PCNT.h"
#include "id3_framefactory.h"

/*
This is simply a counter of the number of times a file has been
played. The value is increased by one every time the file begins to
play. There may only be one "PCNT" frame in each tag. When the
counter reaches all one's, one byte is inserted in front of the
counter thus making the counter eight bits bigger.  The counter must
be at least 32-bits long to begin with.

<Header for 'Play counter', ID: "PCNT">
Counter        $xx xx xx xx (xx ...)
*/

CID3F_PCNT::CID3F_PCNT(void)
{
	_frameID = F_PCNT;
	_counter = 0;	
}

CID3F_PCNT::CID3F_PCNT(u32 newCounter)
{
	_frameID = F_PCNT;
	_counter = newCounter;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_PCNT::~CID3F_PCNT(void)
{
}


void CID3F_PCNT::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		if (_blob.GetLength() < 4)
		{
			_counter = 0;
		}
		else
		{
			_counter = _blob.Get4B(0);
		}		
		isDecoded = true;
	}	
}
void CID3F_PCNT::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.Add4B(_counter);
		mustRebuild = false;		
	}
}
void CID3F_PCNT::setCounter(u32 newCounter)
{
	if (_counter != newCounter)
	{
		_counter = newCounter;
		isDecoded = true;
		mustRebuild = true;	
	}
}

void CID3F_PCNT::increaseCounter()
{
	decode();
	_counter++;
	mustRebuild = true;
}

u32 CID3F_PCNT::getCounter()
{
	decode();
	return _counter;
}


