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
#include "MP4_STCO.h"
#include "mp4_atomfactory.h"

CMP4_STCO::CMP4_STCO(void)
{
	setFrameID(MP4_STCO);
}

CMP4_STCO::CMP4_STCO(u32 frameID)
{
	setFrameID(frameID);
}

CMP4_STCO::~CMP4_STCO(void)
{
}

void CMP4_STCO::move(long offset, FILE* Destination)
{
	if (_blob.GetLength() > 8 && offset != 0)
	{
		u32 count = _blob.Get4B(4);
		u32 lfd, entry;
		bool wide = getFrameID() == MP4_CO64;
		if (_blob.GetLength() < 8 + (size_t)count * (wide ? 8 : 4))
			return;
		CBlob tmp(count*(wide ? 8 : 4)+8);
		tmp.AddMemory(_blob.m_pData, 8);
		for (lfd = 0; lfd < count; lfd++)
		{
			if (wide)
			{
				unsigned __int64 v = ((unsigned __int64)(u32)_blob.Get4B(lfd*8 + 8) << 32) | (u32)_blob.Get4B(lfd*8 + 12);
				v += offset;
				tmp.Add4B((u32)(v >> 32));
				tmp.Add4B((u32)v);
				continue;
			}
			entry = _blob.Get4B(lfd*4 + 8) + offset;
			tmp.Add4B(entry);
		}
		_blob.Clear();
		_blob.AddBlob(tmp);
		fseek(Destination, _position, SEEK_SET);
		CMP4Atom::save(Destination);	
	}	
}

void CMP4_STCO::save(FILE *Destination)
{
	// Falls Quellpos und Zielpos identisch sind und gleiche Datei ist, dann mache nix
	// in case source and target pos are identical and in the same file, do nothing.
	_position = ftell(Destination);
	CMP4Atom::save(Destination);
}