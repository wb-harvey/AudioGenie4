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
#include "ID3F_ASPI.h"
#include "id3_framefactory.h"

/* Audio seek point index

Audio files with variable bit rates are intrinsically difficult to
deal with in the case of seeking within the file. The ASPI frame
makes seeking easier by providing a list a seek points within the
audio file. The seek points are a fractional offset within the audio
data, providing a starting point from which to find an appropriate
point to start decoding. The presence of an ASPI frame requires the
existence of a TLEN frame, indicating the duration of the file in
milliseconds. There may only be one 'audio seek point index' frame in
a tag.

<Header for 'Seek Point Index', ID: "ASPI">
Indexed data start (S)         $xx xx xx xx
Indexed data length (L)        $xx xx xx xx
Number of index points (N)     $xx xx
Bits per index point (b)       $xx

Then for every index point the following data is included;

Fraction at index (Fi)          $xx (xx)

'Indexed data start' is a byte offset from the beginning of the file.
'Indexed data length' is the byte length of the audio data being
indexed. 'Number of index points' is the number of index points, as
the name implies. The recommended number is 100. 'Bits per index
point' is 8 or 16, depending on the chosen precision. 8 bits works
well for short files (less than 5 minutes of audio), while 16 bits is
advantageous for long files. 'Fraction at index' is the numerator of
the fraction representing a relative position in the data. The
denominator is 2 to the power of b.

Here are the algorithms to be used in the calculation. The known data
must be the offset of the start of the indexed data (S), the offset
of the end of the indexed data (E), the number of index points (N),
the offset at index i (Oi). We calculate the fraction at index i
(Fi).

Oi is the offset of the frame whose start is soonest after the point
for which the time offset is (i/N * duration).

The frame data should be calculated as follows:

Fi = Oi/L * 2^b    (rounded down to the nearest integer)

Offset calculation should be calculated as follows from data in the
frame:

Oi = (Fi/2^b)*L    (rounded up to the nearest integer)

*/

CID3F_ASPI::CID3F_ASPI() 	
{
	init(0, 0, 0, 0);	
}

CID3F_ASPI::CID3F_ASPI(long start, long len, short number, BYTE bpi)
{
	init(start, len, number, bpi);
}

void CID3F_ASPI::init(long start, long len, short number, BYTE bpi)
{
	_frameID = F_ASPI;
	_start = start;
	_len = len;
	_number = number;
	_bpi = bpi;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_ASPI::~CID3F_ASPI(void)
{

}

void CID3F_ASPI::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_data.Clear();
		if (_blob.GetLength() < 11)
		{
			_start = 0;
			_len = 0;
			_number = 0;
			_bpi = 0;
		}
		else
		{
			_start = (long)_blob.Get4B(0);
			_len = (long)_blob.Get4B(4);
			_number = _blob.Get2B(8);
			_bpi = _blob.GetAt(10);
			_data.AddBlob(_blob, 11);
		}
		isDecoded = true;
	}	
}
void CID3F_ASPI::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.Add4B(_start);
		_blob.Add4B(_len);
		_blob.Add2B(_number);
		_blob.AddValue(_bpi);
		_blob.AddBlob(_data);
		mustRebuild = false;		
	}
}

long CID3F_ASPI::getStart()
{
	decode();
	return _start;
}

long CID3F_ASPI::getLength()
{
	decode();
	return _len;
}
short CID3F_ASPI::getNumber()
{
	decode();
	return _number;
}
BYTE CID3F_ASPI::getBpi()
{
	decode();
	return _bpi;
}

