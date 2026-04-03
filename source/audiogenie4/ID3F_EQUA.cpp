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
#include "ID3F_EQUA.h"
#include "id3_framefactory.h"
#include "ID3v2.h"

/* Equalisation (2) V2.4

This is another subjective, alignment frame. It allows the user to
predefine an equalisation curve within the audio file. There may be
more than one "EQUA" frame in each tag, but only one with the same
identification string.

<Header of 'Equalisation (2)', ID: "EQUA">
Interpolation method  $xx
Identification        <text string> $00

The 'interpolation method' describes which method is preferred when
an interpolation between the adjustment point that follows. The
following methods are currently defined:

$00  Band
No interpolation is made. A jump from one adjustment level to
another occurs in the middle between two adjustment points.
$01  Linear
Interpolation between adjustment points is linear.

The 'identification' string is used to identify the situation and/or
device where this adjustment should apply. The following is then
repeated for every adjustment point

Frequency          $xx xx
Volume adjustment  $xx xx

The frequency is stored in units of 1/2 Hz, giving it a range from 0
to 32767 Hz.

The volume adjustment is encoded as a fixed point decibel value, 16
bit signed integer representing (adjustment*512), giving +/- 64 dB
with a precision of 0.001953125 dB. E.g. +2 dB is stored as $04 00
and -2 dB is $FC 00.

Adjustment points should be ordered by frequency and one frequency
should only be described once in the frame.
// V2.2 && v2.3 ------------------------------------------------------
<Header of 'Equalisation', ID: "EQUA">
Adjustment bits    $xx

The 'adjustment bits' field defines the number of bits used for
representation of the adjustment. This is normally $10 (16 bits) for
MPEG 2 layer I, II and III [MPEG] and MPEG 2.5. This value may not be
$00.

This is followed by 2 bytes + ('adjustment bits' rounded up to the
nearest byte) for every equalisation band in the following format,
giving a frequency range of 0 - 32767Hz:

Increment/decrement   %x (MSB of the Frequency)
Frequency             (lower 15 bits)
Adjustment            $xx (xx ...)

The increment/decrement bit is 1 for increment and 0 for decrement.
The equalisation bands should be ordered increasingly with reference
to frequency. All frequencies don't have to be declared. The
equalisation curve in the reading software should be interpolated
between the values in this frame. Three equal adjustments for three
subsequent frequencies. A frequency should only be described once in
the frame.

*/

CID3F_EQUA::CID3F_EQUA(long oldID) 	
{
	init(0, EMPTY);	
	_oldID = oldID;	
}

CID3F_EQUA::CID3F_EQUA(BYTE format, LPCWSTR identification)
{
	init(format, identification);
}
void CID3F_EQUA::init(BYTE format, LPCWSTR identification)
{
	_frameID = F_EQUA;
	_format = format;
	_identification = identification;
	_bits = 16;
	mustRebuild = true;
	isDecoded = true;
	_oldID = F_EQU2;
	lastTag = 99;
}

CID3F_EQUA::~CID3F_EQUA(void)
{

}

bool CID3F_EQUA::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_EQUA *target = cEQUA(frame);
	return (this->_identification.CompareNoCase(target->_identification) == 0);
}

void CID3F_EQUA::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_data.Clear();
		lastTag = CTools::ID3V2oldTagVersion;
		if (_oldID == F_EQU2) // v2.4
		{
			_bits = 16;
			int start = 1;
			if (_blob.GetLength() < 6)
			{
				_format = 0;
				_identification.Empty();
			}
			else
			{
				_format = _blob.GetAt(0);
				_identification = _blob.getNextString(TEXT_ENCODED_ANSI, start);
				_data.AddBlob(_blob, start);	
			}
		}
		else // V2.2 && v2.3
		{
			_identification.Empty();
			_format = 0;				
			if (_blob.GetLength() < 6)
			{
				_bits = 0;
			}
			else
			{
				_bits = _blob.GetAt(0);
				_data.AddBlob(_blob, 1);	
			}
		}		
		isDecoded = true;
	}	

}
void CID3F_EQUA::encode()
{
	if (mustRebuild || lastTag != CTools::ID3V2newTagVersion)
	{
		decode();
		_blob.Clear();
		if (CTools::ID3V2newTagVersion == TAG_VERSION_2_4) // v2.4
		{
			_blob.AddValue(_format);
			_blob.AddEncodedString(TEXT_ENCODED_ANSI, _identification, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
			_frameID = F_EQU2;
		}
		else // v2.2 && v2.3
		{
			_blob.AddValue(_bits);
			_frameID = F_EQUA;
		}
		_blob.AddBlob(_data);
		mustRebuild = false;
		lastTag = CTools::ID3V2newTagVersion;		
	}
}

BYTE CID3F_EQUA::getFormat()
{
	decode();
	return _format;
}

BYTE CID3F_EQUA::getAdjustmentBits()
{
	decode();
	return _bits;
}

CAtlString CID3F_EQUA::getIdentification()
{
	decode();
	return _identification;
}