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
#include "ID3F_RVAD.h"
#include "id3_framefactory.h"
#include "ID3v2.h"

/*
Relative volume adjustment (2) ID3V2.4

This is a more subjective frame than the previous ones. It allows the
user to say how much he wants to increase/decrease the volume on each
channel when the file is played. The purpose is to be able to align
all files to a reference volume, so that you don't have to change the
volume constantly. This frame may also be used to balance adjust the
audio. The volume adjustment is encoded as a fixed point decibel
value, 16 bit signed integer representing (adjustment*512), giving
+/- 64 dB with a precision of 0.001953125 dB. E.g. +2 dB is stored as
$04 00 and -2 dB is $FC 00. There may be more than one "RVA2" frame
in each tag, but only one with the same identification string.

<Header for 'Relative volume adjustment (2)', ID: "RVA2">
Identification          <text string> $00

The 'identification' string is used to identify the situation and/or
device where this adjustment should apply. The following is then
repeated for every channel

Type of channel         $xx
Volume adjustment       $xx xx
Bits representing peak  $xx
Peak volume             $xx (xx ...)


Type of channel:  $00  Other
$01  Master volume
$02  Front right
$03  Front left
$04  Back right
$05  Back left
$06  Front centre
$07  Back centre
$08  Subwoofer

Bits representing peak can be any number between 0 and 255. 0 means
that there is no peak volume field. The peak volume field is always
padded to whole bytes, setting the most significant bits to zero.

ID3v2.2 & V2.3
Relative volume adjustment

This is a more subjective function than the previous ones. It allows
the user to say how much he wants to increase/decrease the volume on
each channel while the file is played. The purpose is to be able to
align all files to a reference volume, so that you don't have to
change the volume constantly. This frame may also be used to balance
adjust the audio. If the volume peak levels are known then this could
be described with the 'Peak volume right' and 'Peak volume left'
field. If Peakvolume is not known these fields could be left zeroed
or, if no other data follows, be completely omitted. There may only
be one "RVAD" frame in each tag.

<Header for 'Relative volume adjustment', ID: "RVAD">
Increment/decrement           %00xxxxxx
Bits used for volume descr.   $xx
Relative volume change, right $xx xx (xx ...)
Relative volume change, left  $xx xx (xx ...)
Peak volume right             $xx xx (xx ...)
Peak volume left              $xx xx (xx ...)

In the increment/decrement field bit 0 is used to indicate the right
channel and bit 1 is used to indicate the left channel. 1 is
increment and 0 is decrement.

The 'bits used for volume description' field is normally $10 (16
bits) for MPEG 2 layer I, II and III [MPEG] and MPEG 2.5. This value
may not be $00. The volume is always represented with whole bytes,
padded in the beginning (highest bits) when 'bits used for volume
description' is not a multiple of eight.

This datablock is then optionally followed by a volume definition for
the left and right back channels. If this information is appended to
the frame the first two channels will be treated as front channels.
In the increment/decrement field bit 2 is used to indicate the right
back channel and bit 3 for the left back channel.

Relative volume change, right back $xx xx (xx ...)
Relative volume change, left back  $xx xx (xx ...)
Peak volume right back             $xx xx (xx ...)
Peak volume left back              $xx xx (xx ...)

If the center channel adjustment is present the following is appended
to the existing frame, after the left and right back channels. The
center channel is represented by bit 4 in the increase/decrease
field.

Relative volume change, center  $xx xx (xx ...)
Peak volume center              $xx xx (xx ...)

If the bass channel adjustment is present the following is appended
to the existing frame, after the center channel. The bass channel is
represented by bit 5 in the increase/decrease field.

Relative volume change, bass  $xx xx (xx ...)
Peak volume bass              $xx xx (xx ...)



*/

CID3F_RVAD::CID3F_RVAD(long oldID)
{
	init(EMPTY);
	_oldID = oldID;	
}

CID3F_RVAD::CID3F_RVAD(LPCWSTR identification) 	
{
	init(identification);
}

void CID3F_RVAD::init(LPCWSTR identification)
{
	_frameID = F_RVAD;
	_identification = identification;
	mustRebuild = true;
	isDecoded = true;
	_oldID = F_RVA2;
	lastTag = 99;
}

CID3F_RVAD::~CID3F_RVAD(void)
{
}

bool CID3F_RVAD::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_RVAD *target = cRVAD(frame);
	return (this->_identification.CompareNoCase(target->_identification) == 0);
}

void CID3F_RVAD::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_data.Clear();
		if (_oldID == F_RVA2) // v2.4
		{
			if (_blob.GetLength() < 2)
			{
				_identification.Empty();
			}
			else
			{
				int start = 0;
				_identification = _blob.getNextString(TEXT_ENCODED_ANSI, start);
				_data.AddBlob(_blob, start);
			}
		}
		else // V2.2 && v2.3
		{
			_identification.Empty();
			_data.AddBlob(_blob);
		}
		isDecoded = true;
	}	
}
void CID3F_RVAD::encode()
{
	if (mustRebuild || lastTag != CTools::ID3V2newTagVersion)
	{
		decode();
		_blob.Clear();
		if (CTools::ID3V2newTagVersion == TAG_VERSION_2_4) // v2.4
		{
			_blob.AddEncodedString(TEXT_ENCODED_ANSI, _identification, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
			_blob.AddBlob(_data);
			_frameID = F_RVA2;
		}
		else // v2.2 && v2.3
		{
			_blob.AddBlob(_data);
			_frameID = F_RVAD;
		}
		mustRebuild = false;
		lastTag = CTools::ID3V2newTagVersion;
	}
}

CAtlString CID3F_RVAD::getIdentification()
{
	decode();
	return _identification;
}
