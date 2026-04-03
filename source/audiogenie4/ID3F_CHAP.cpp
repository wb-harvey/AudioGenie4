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
#include "ID3F_CHAP.h"
#include "id3_framefactory.h"
/* http://id3.org/id3v2-chapters-1.0
The purpose of this frame is to describe a single chapter within an audio file. There may be more than one frame of this type in a tag but each must have an Element ID that is unique with respect to any other "CHAP" frame or "CTOC" frame in the tag.

<ID3v2.3 or ID3v2.4 frame header, ID: "CHAP">           (10 bytes)
Element ID      <text string> $00
Start time      $xx xx xx xx
End time        $xx xx xx xx
Start offset    $xx xx xx xx
End offset      $xx xx xx xx
<Optional embedded sub-frames>

The Element ID uniquely identifies the frame. It is not intended to be human readable and should not be presented to the end user.

The Start and End times are a count in milliseconds from the beginning of the file to the start and end of the chapter respectively.

The Start offset is a zero-based count of bytes from the beginning of the file to the first byte of the first audio frame in the chapter. If these bytes are all set to 0xFF then the value should be ignored and the start time value should be utilized.

The End offset is a zero-based count of bytes from the beginning of the file to the first byte of the audio frame following the end of the chapter. If these bytes are all set to 0xFF then the value should be ignored and the end time value should be utilized.

There then follows a sequence of optional frames that are embedded within the "CHAP" frame and which describe the content of the chapter (e.g. a "TIT2" frame representing the chapter name) or provide related material such as URLs and images. These sub-frames are contained within the bounds of the "CHAP" frame as signalled by the size field in the "CHAP" frame header. If a parser does not recognise "CHAP" frames it can skip them using the size field in the frame header. When it does this it will skip any embedded sub-frames carried within the frame.

Figure 1 shows an example of a "CHAP" frame containing two embedded sub-frames. The first is a "TIT2" sub-frame providing the chapter name; "Chapter 1 - Loomings". The second is a "TIT3" sub-frame providing a description of the chapter; "Anticipation of the hunt". 

*/

CID3F_CHAP::CID3F_CHAP(void)
{
	init(EMPTY, EMPTY, EMPTY);	
}

CID3F_CHAP::CID3F_CHAP(LPCWSTR ID, LPCWSTR title, LPCWSTR description ) 	
{
	init(ID, title, description);	
}

void CID3F_CHAP::init(LPCWSTR ID, LPCWSTR title, LPCWSTR description )
{
	_frameID = F_CHAP;
	_ID = ID;
	_title = title;
	_description = description;
	useTextEncoding = true;
	_startTime = 0;
	_endTime = 0;
	_startOffset = 0xFFFFFFFF;
	_endOffset = 0xFFFFFFFF;
	_frames.SetCount(0, 10);
	isDecoded = true;
	setTitle(_title);
	setDescription(_description);
	mustRebuild = true;	
}

CID3F_CHAP::~CID3F_CHAP(void)
{
	_frames.RemoveAll();
}

void CID3F_CHAP::decode()
{
	if (!isDecoded)
	{	
		_data.Clear();
		if (_blob.GetLength() < 16)
		{
			_title.Empty();
			_description.Empty();
			_startTime = 0;
			_endTime = 0;
			_startOffset = 0xFFFFFFFF;
			_endOffset = 0xFFFFFFFF;
		}
		else
		{
			int start = 0;
			_ID = _blob.getNextString(TEXT_ENCODED_ANSI, start);
			_startTime = (u32)_blob.Get4B(start);
			_endTime = (u32)_blob.Get4B(start + 4);
			_startOffset = (u32)_blob.Get4B(start + 8);
			_endOffset = (u32)_blob.Get4B(start + 12);
			_data.AddBlob(_blob, start + 16);
			convertBlobToFrames();
			_data.Clear();		
		}		
		isDecoded = true;
	}	
}
void CID3F_CHAP::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _ID, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.Add4B(_startTime);
		_blob.Add4B(_endTime);
		_blob.Add4B(_startOffset);
		_blob.Add4B(_endOffset);
		convertFramesToBlob();
		_blob.AddBlob(_data);
		mustRebuild = false;
	}
}
void CID3F_CHAP::setTimes(u32 startTime, u32 endTime)
{
	decode();
	if (_startTime != startTime)
	{
		_startTime = startTime;
		_startOffset = 0xFFFFFFFF;	
		mustRebuild = true;
	}
	if (_endTime != endTime)
	{
		_endTime = endTime;
		_endOffset = 0xFFFFFFFF;	
		mustRebuild = true;
	}	
}

u32 CID3F_CHAP::getStartTime()
{
	decode();
	return _startTime;
}
u32 CID3F_CHAP::getEndTime()
{
	decode();
	return _endTime;
}
/*u32 CID3F_CHAP::getStartOffset()
{
	decode();
	return _startOffset;
}
u32 CID3F_CHAP::getEndOffset()
{
	decode();
	return _endOffset;
}*/