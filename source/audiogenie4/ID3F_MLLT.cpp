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
#include "ID3F_MLLT.h"
#include "id3_framefactory.h"

/*
MPEG location lookup table

To increase performance and accuracy of jumps within a MPEG [MPEG]
audio file, frames with time codes in different locations in the file
might be useful. This ID3v2 frame includes references that the
software can use to calculate positions in the file. After the frame
header follows a descriptor of how much the 'frame counter' should be
increased for every reference. If this value is two then the first
reference points out the second frame, the 2nd reference the 4th
frame, the 3rd reference the 6th frame etc. In a similar way the
'bytes between reference' and 'milliseconds between reference' points
out bytes and milliseconds respectively.

Each reference consists of two parts; a certain number of bits, as
defined in 'bits for bytes deviation', that describes the difference
between what is said in 'bytes between reference' and the reality and
a certain number of bits, as defined in 'bits for milliseconds
deviation', that describes the difference between what is said in
'milliseconds between reference' and the reality. The number of bits
in every reference, i.e. 'bits for bytes deviation'+'bits for
milliseconds deviation', must be a multiple of four. There may only
be one "MLLT" frame in each tag.

<Header for 'Location lookup table', ID: "MLLT">
MPEG frames between reference  $xx xx
Bytes between reference        $xx xx xx
Milliseconds between reference $xx xx xx
Bits for bytes deviation       $xx
Bits for milliseconds dev.     $xx

Then for every reference the following data is included;

Deviation in bytes         %xxx....
Deviation in milliseconds  %xxx....
*/

CID3F_MLLT::CID3F_MLLT() 	
{
	init(0, 0, 0, 0, 0);	
}

CID3F_MLLT::CID3F_MLLT(long frames, long bytes, long millis, BYTE bdev, BYTE mdev)
{
	init(frames, bytes, millis, bdev, mdev);
}
void CID3F_MLLT::init(long frames, long bytes, long millis, BYTE bdev, BYTE mdev)
{
	_frameID = F_MLLT;
	_frames = frames;
	_bytes = bytes;
	_millis = millis;
	_bdev = bdev;
	_mdev = mdev;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_MLLT::~CID3F_MLLT(void)
{

}

void CID3F_MLLT::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_data.Clear();
		if (_blob.GetLength() < 10)
		{
			_frames = 0;
			_bytes = 0;
			_millis = 0;
			_bdev = 0;
			_mdev = 0;
		}
		else
		{
			_frames = _blob.Get2B(0);
			_bytes = _blob.Get3B(2);
			_millis = _blob.Get3B(5);
			_bdev = _blob.GetAt(8);
			_mdev = _blob.GetAt(9);
			_data.AddBlob(_blob, 10);
		}
		isDecoded = true;
	}	
}
void CID3F_MLLT::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.Add2B(_frames);
		_blob.Add3B(_bytes);
		_blob.Add3B(_millis);
		_blob.AddValue(_bdev);
		_blob.AddValue(_mdev);
		_blob.AddBlob(_data);
		mustRebuild = false;		
	}
}

long CID3F_MLLT::getFrames()
{
	decode();
	return _frames;
}

long CID3F_MLLT::getBytes()
{
	decode();
	return _bytes;
}

long CID3F_MLLT::getMillis()
{
	decode();
	return _millis;
}

BYTE CID3F_MLLT::getMdev()
{
	decode();
	return _mdev;
}

BYTE CID3F_MLLT::getBdev()
{
	decode();
	return _bdev;
}
