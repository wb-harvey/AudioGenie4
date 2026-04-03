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
#include "WMA_StreamProperties.h"
#include "Tools.h"

CWMA_StreamProperties::CWMA_StreamProperties(void)
{
	_objectID = WMA_STREAM_PROPERTIES_ID;
}

CWMA_StreamProperties::~CWMA_StreamProperties(void)
{
}

void CWMA_StreamProperties::Empty()
{
	CodecID = 0;
	NumberofChannels = 0;
	SamplesPerSecond = 0;
	AverageBytesPerSecond = 0;
	BlockAlignment = 0;
	BitsPerSample = 0;
	CodecSpecificDataSize = 0;
}
bool CWMA_StreamProperties::load(FILE *Stream, size_t maxLen)
{
	ATLTRACE(_T("StreamProperties with %u Bytes\n"), maxLen);
	_data.FileRead(maxLen, Stream);
	if (_data.GetLength() < 54)
	{	
		CTools::instance().setLastError(ERR_WMA_PARSE);
		_data.Clear();
		return false;
	}
	memcpy(&StreamType, _data.m_pData, 16);
	if (IsEqualIID(StreamType, WMA_AUDIO_MEDIA_ID))
	{
		CodecID = _data.GetR2B(54);
		NumberofChannels = _data.GetR2B(56);
		SamplesPerSecond = _data.GetR4B(58);
		AverageBytesPerSecond = _data.GetR4B(62);
		BlockAlignment = _data.GetR2B(66);
		BitsPerSample = _data.GetR2B(68);
		CodecSpecificDataSize = _data.GetR2B(70);
	}
	return true;
}