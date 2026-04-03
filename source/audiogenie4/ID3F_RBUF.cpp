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
#include "ID3F_RBUF.h"
#include "id3_framefactory.h"

/*
Recommended buffer size

Sometimes the server from which an audio file is streamed is aware of
transmission or coding problems resulting in interruptions in the
audio stream. In these cases, the size of the buffer can be
recommended by the server using this frame. If the 'embedded info
flag' is true (1) then this indicates that an ID3 tag with the
maximum size described in 'Buffer size' may occur in the audio
stream. In such case the tag should reside between two MPEG [MPEG]
frames, if the audio is MPEG encoded. If the position of the next tag
is known, 'offset to next tag' may be used. The offset is calculated
from the end of tag in which this frame resides to the first byte of
the header in the next. This field may be omitted. Embedded tags are
generally not recommended since this could render unpredictable
behaviour from present software/hardware.

For applications like streaming audio it might be an idea to embed
tags into the audio stream though. If the clients connects to
individual connections like HTTP and there is a possibility to begin
every transmission with a tag, then this tag should include a
'recommended buffer size' frame. If the client is connected to a
arbitrary point in the stream, such as radio or multicast, then the
'recommended buffer size' frame SHOULD be included in every tag.

The 'Buffer size' should be kept to a minimum. There may only be one
"RBUF" frame in each tag.

<Header for 'Recommended buffer size', ID: "RBUF">
Buffer size               $xx xx xx
Embedded info flag        %0000000x
Offset to next tag        $xx xx xx xx
*/

CID3F_RBUF::CID3F_RBUF(void)
{
	init(0,0,0);
}

CID3F_RBUF::CID3F_RBUF(long buffer, BYTE flag, long offset) 	
{
	init(buffer, flag, offset);
}

void CID3F_RBUF::init(long buffer, BYTE flag, long offset)
{
	_frameID = F_RBUF;
	_buffer = buffer;
	_flag = flag;
	_offset = offset;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_RBUF::~CID3F_RBUF(void)
{
}

void CID3F_RBUF::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		if (_blob.GetLength() < 8)
		{
			_buffer = 0;
			_offset = 0;
			_flag = 0;
		}
		else
		{
			_buffer = (_blob.GetAt(0) << 16) + (_blob.GetAt(1) << 8) + _blob.GetAt(2);
			_flag = _blob.GetAt(3);
			_offset = _blob.Get4B(4);
		}		
		isDecoded = true;
	}	
}
void CID3F_RBUF::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddValue((_buffer >> 16 ) & 0xFF);
		_blob.AddValue((_buffer >>  8 ) & 0xFF);
		_blob.AddValue(_buffer & 0xFF);
		_blob.AddValue(_flag);
		_blob.Add4B(_offset);
		mustRebuild = false;
	}
}

long CID3F_RBUF::getBufferSize()
{
	decode();
	return _buffer;
}

BYTE CID3F_RBUF::getFlag()
{
	decode();
	return _flag;
}

long CID3F_RBUF::getOffset()
{
	decode();
	return _offset;
}
