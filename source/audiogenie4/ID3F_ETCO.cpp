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
#include "ID3F_ETCO.h"
#include "id3_framefactory.h"

/*
Event timing codes

This frame allows synchronisation with key events in the audio. The
header is:

<Header for 'Event timing codes', ID: "ETCO">
Time stamp format    $xx

Where time stamp format is:

$01  Absolute time, 32 bit sized, using MPEG [MPEG] frames as unit
$02  Absolute time, 32 bit sized, using milliseconds as unit

Absolute time means that every stamp contains the time from the
beginning of the file.

Followed by a list of key events in the following format:

Type of event   $xx
Time stamp      $xx (xx ...)

The 'Time stamp' is set to zero if directly at the beginning of the
sound or after the previous event. All events MUST be sorted in
chronological order. The type of event is as follows:

$00  padding (has no meaning)
$01  end of initial silence
$02  intro start
$03  main part start
$04  outro start
$05  outro end
$06  verse start
$07  refrain start
$08  interlude start
$09  theme start
$0A  variation start
$0B  key change
$0C  time change
$0D  momentary unwanted noise (Snap, Crackle & Pop)
$0E  sustained noise
$0F  sustained noise end
$10  intro end
$11  main part end
$12  verse end
$13  refrain end
$14  theme end
$15  profanity
$16  profanity end

$17-$DF  reserved for future use

$E0-$EF  not predefined synch 0-F

$F0-$FC  reserved for future use

$FD  audio end (start of silence)
$FE  audio file ends
$FF  one more byte of events follows (all the following bytes with
the value $FF have the same function)

Terminating the start events such as "intro start" is OPTIONAL. The
'Not predefined synch's ($E0-EF) are for user events. You might want
to synchronise your music to something, like setting off an explosion
on-stage, activating a screensaver etc.

There may only be one "ETCO" frame in each tag.

*/

CID3F_ETCO::CID3F_ETCO() 	
{
	init(0);	
}

CID3F_ETCO::CID3F_ETCO(BYTE format)
{
	init(format);
}
void CID3F_ETCO::init(BYTE format)
{
	_frameID = F_ETCO;
	_format = format;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_ETCO::~CID3F_ETCO(void)
{

}

void CID3F_ETCO::decode()
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
void CID3F_ETCO::encode()
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

BYTE CID3F_ETCO::getFormat()
{
	decode();
	return _format;
}
