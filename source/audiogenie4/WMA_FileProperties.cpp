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
#include "WMA_FileProperties.h"

CWMA_FileProperties::CWMA_FileProperties(void)
{
	_objectID = WMA_FILE_PROPERTIES_ID;
}

CWMA_FileProperties::~CWMA_FileProperties(void)
{
}
bool CWMA_FileProperties::load(FILE *Stream, size_t maxLen)
{
	ATLTRACE(_T("FileProperties with %i bytes\n"), maxLen);
	_data.FileRead(80, Stream);
	if (_data.GetLength() != 80 || maxLen < 80)
	{	
		_data.Clear();
		return false;
	}
	/*	File ID	16 Bytes                      0 - 15
		File Size	8 Bytes                   16 - 23 
		Creation Date	8 Bytes               24 - 31
		Data Packets Count	8 Bytes           32 - 39
		Play Duration	8 Bytes               40 - 47
		Send Duration	8 Bytes               48 - 55
		Preroll	8 Bytes                       56 - 63
		Flags	4 Bytes                       64 - 67 
		Minimum Data Packet Size	4 Bytes   68 - 71
		Maximum Data Packet Size	4 Bytes   72 - 75
		Maximum Bitrate	4 Bytes               76 - 79 
	*/
	
	//__int64 fs = _data.GetR8B(16);
	//__int64 cn = _data.GetR8B(32);
	PlayDuration = _data.GetR8B(40);
	Preroll = _data.GetR8B(56);
	Flags = _data.GetR4B(64);
	MaxBitrate = _data.GetR4B(76);
	return true;
}