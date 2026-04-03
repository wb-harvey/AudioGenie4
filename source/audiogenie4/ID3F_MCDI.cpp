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
#include "ID3F_MCDI.h"
#include "id3_framefactory.h"

/*
Music CD identifier

This frame is intended for music that comes from a CD, so that the CD
can be identified in databases such as the CDDB [CDDB]. The frame
consists of a binary dump of the Table Of Contents, TOC, from the CD,
which is a header of 4 bytes and then 8 bytes/track on the CD plus 8
bytes for the 'lead out', making a maximum of 804 bytes. The offset
to the beginning of every track on the CD should be described with a
four bytes absolute CD-frame address per track, and not with absolute
time. When this frame is used the presence of a valid "TRCK" frame is
REQUIRED, even if the CD's only got one track. It is recommended that
this frame is always added to tags originating from CDs. There may
only be one "MCDI" frame in each tag.

<Header for 'Music CD identifier', ID: "MCDI">
CD TOC                <binary data>
*/

CID3F_MCDI::CID3F_MCDI() 	
{
	_frameID = F_MCDI;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_MCDI::~CID3F_MCDI(void)
{

}

void CID3F_MCDI::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_data.Clear();
		_data.AddBlob(_blob);
		isDecoded = true;
	}	
}

void CID3F_MCDI::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddBlob(_data);
		mustRebuild = false;		
	}
}
