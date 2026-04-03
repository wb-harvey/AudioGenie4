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
#include "ID3F_SYTC.h"
#include "id3_framefactory.h"

/*
Synchronised tempo codes

For a more accurate description of the tempo of a musical piece, this
frame might be used. After the header follows one byte describing
which time stamp format should be used. Then follows one or more
tempo codes. Each tempo code consists of one tempo part and one time
part. The tempo is in BPM described with one or two bytes. If the
first byte has the value $FF, one more byte follows, which is added
to the first giving a range from 2 - 510 BPM, since $00 and $01 is
reserved. $00 is used to describe a beat-free time period, which is
not the same as a music-free time period. $01 is used to indicate one
single beat-stroke followed by a beat-free period.

The tempo descriptor is followed by a time stamp. Every time the
tempo in the music changes, a tempo descriptor may indicate this for
the player. All tempo descriptors MUST be sorted in chronological
order. The first beat-stroke in a time-period is at the same time as
the beat description occurs. There may only be one "SYTC" frame in
each tag.

  <Header for 'Synchronised tempo codes', ID: "SYTC">
  Time stamp format$xx
  Tempo data <binary data>

Where time stamp format is:

  $01  Absolute time, 32 bit sized, using MPEG [MPEG] frames as unit
  $02  Absolute time, 32 bit sized, using milliseconds as unit

Absolute time means that every stamp contains the time from the
beginning of the file.
*/

CID3F_SYTC::CID3F_SYTC() 	
{
	init(0);	
}

CID3F_SYTC::CID3F_SYTC(BYTE format)
{
	init(format);
}
void CID3F_SYTC::init(BYTE format)
{
	_frameID = F_SYTC;
	_format = format;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_SYTC::~CID3F_SYTC(void)
{

}

void CID3F_SYTC::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_data.Clear();
		if (_blob.GetLength() < 2)
		{
			_format = 0;
		}
		else
		{
			_format = _blob.GetAt(0);
			_data.AddBlob(_blob, 1);
		}
		isDecoded = true;
	}	
}
void CID3F_SYTC::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddValue(_format);
		_blob.AddBlob(_data);
		mustRebuild = false;
	}
}

BYTE CID3F_SYTC::getFormat()
{
	decode();
	return _format;
}
