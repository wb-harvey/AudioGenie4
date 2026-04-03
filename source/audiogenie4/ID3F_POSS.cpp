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
#include "ID3F_POSS.h"
#include "id3_framefactory.h"

/*
Position synchronisation frame

This frame delivers information to the listener of how far into the
audio stream he picked up; in effect, it states the time offset from
the first frame in the stream. The frame layout is:

<Head for 'Position synchronisation', ID: "POSS">
Time stamp format         $xx
Position                  $xx (xx ...)

Where time stamp format is:

$01  Absolute time, 32 bit sized, using MPEG frames as unit
$02  Absolute time, 32 bit sized, using milliseconds as unit

and position is where in the audio the listener starts to receive,
i.e. the beginning of the next frame. If this frame is used in the
beginning of a file the value is always 0. There may only be one
"POSS" frame in each tag.
*/

CID3F_POSS::CID3F_POSS() 	
{
	init(0, 0);	
}

CID3F_POSS::CID3F_POSS(BYTE format, long position)
{
	init(format, position);
}
void CID3F_POSS::init(BYTE format, long position)
{
	_frameID = F_POSS;
	_format = format;
	_position = position;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_POSS::~CID3F_POSS(void)
{

}

void CID3F_POSS::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		if (_blob.GetLength() < 5)
		{
			_format = 0;
			_position = 0;
		}
		else
		{
			_format = _blob.GetAt(0);
			_position = _blob.Get4B(1);			
		}
		isDecoded = true;
	}	
}
void CID3F_POSS::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddValue(_format);
		_blob.Add4B(_position);
		mustRebuild = false;
	}
}

BYTE CID3F_POSS::getFormat()
{
	decode();
	return _format;
}

long CID3F_POSS::getPosition()
{
	decode();
	return _position;
}