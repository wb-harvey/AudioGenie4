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
#include "ID3_FrameFactory.h"
#include "ID3v2.h"

CID3_FrameFactory::CID3_FrameFactory()
{
	anzKnownFrames = sizeof(ID3_FRAMECODES) / (MAX_EBENEN * sizeof(int));
}

CID3_FrameFactory::~CID3_FrameFactory()
{
	frameClassMap.RemoveAll();
	frameIDsMap.RemoveAll();
}

CID3_FrameFactory &CID3_FrameFactory::instance()
{
	static CID3_FrameFactory factory;
	return factory;
}

unsigned int CID3_FrameFactory::findUniqueFrameID(unsigned int oldID) 
{
	// Beim ersten Mal die Map füllen
	if (frameIDsMap.IsEmpty())
	{
		frameIDsMap.DisableAutoRehash();
		for (int row = 0; row < anzKnownFrames; row++)
		{
			for (int x = 2; x <=4; x++)
			{
				if (ID3_FRAMECODES[row][x] != F_NONE)
					frameIDsMap.SetAt(ID3_FRAMECODES[row][x], ID3_FRAMECODES[row][0]);			
			}
		}
		frameIDsMap.EnableAutoRehash();
	}
	pPair = frameIDsMap.Lookup(oldID);
	return (pPair) ? pPair->m_value : ID3_NONE;
	
	/*for (int row = 0; row < anzKnownFrames; row++)
	{
		if (ID3_FRAMECODES[row][2] == oldID || ID3_FRAMECODES[row][3] == oldID || ID3_FRAMECODES[row][4] == oldID)
			return ID3_FRAMECODES[row][0];		
	}
	return ID3_NONE; */
}

unsigned int CID3_FrameFactory::findFrameClassType(unsigned int oldID) 
{
	// Beim ersten Mal die Map füllen
	if (frameClassMap.IsEmpty())
	{
		frameClassMap.DisableAutoRehash();
		for (int row = 0; row < anzKnownFrames; row++)
		{
			for (int x = 2; x <=4; x++)
			{
				if (ID3_FRAMECODES[row][x] != F_NONE)
					frameClassMap.SetAt(ID3_FRAMECODES[row][x], ID3_FRAMECODES[row][1]);			
			}
		}
		frameClassMap.EnableAutoRehash();
	}
	pPair = frameClassMap.Lookup(oldID);
	if(pPair)
		return (pPair->m_value);
	// Nicht gefunden, vielleicht ein unbekannter Textframe (T...)
	if ( ((oldID << 24 ) & 0xFF) == 'T')
		return ID3_T000;

	// nicht gefunden
	return oldID;
	/*for (int row = 0; row < anzKnownFrames; row++)
	{
		if (ID3_FRAMECODES[row][2] == oldID || ID3_FRAMECODES[row][3] == oldID || ID3_FRAMECODES[row][4] == oldID)
			return ID3_FRAMECODES[row][1];		
	}
	return oldID; */
}
u32 CID3_FrameFactory::findTagForVersion(u32 uniqueID)
{
	for (int row = 0; row < anzKnownFrames; row++)
	{
		if (ID3_FRAMECODES[row][0] == uniqueID)
			return ID3_FRAMECODES[row][CTools::ID3V2newTagVersion];		
	}
	return F_NONE;
}

CID3_Frame* CID3_FrameFactory::createFrame(unsigned int oldID) 
{
	u32 id = findFrameClassType(oldID);
	switch (id)
	{
	case ID3_AENC: return new CID3F_AENC();
	case ID3_APIC: return new CID3F_APIC();
	case ID3_ASPI: return new CID3F_ASPI();
	case ID3_CHAP: return new CID3F_CHAP();
	case ID3_COMM: return new CID3F_COMM();
	case ID3_COMR: return new CID3F_COMR();
	case ID3_CTOC: return new CID3F_CTOC();
	case ID3_ENCR: return new CID3F_ENCR();
	case ID3_EQUA: return new CID3F_EQUA(oldID);
	case ID3_ETCO: return new CID3F_ETCO();
	case ID3_GEOB: return new CID3F_GEOB();
	case ID3_GRID: return new CID3F_GRID();
	case ID3_LINK: return new CID3F_LINK();
	case ID3_MCDI: return new CID3F_MCDI();
	case ID3_MLLT: return new CID3F_MLLT();
	case ID3_OWNE: return new CID3F_OWNE();
	case ID3_PCNT: return new CID3F_PCNT();
	case ID3_POPM: return new CID3F_POPM();
	case ID3_POSS: return new CID3F_POSS();
	case ID3_PRIV: return new CID3F_PRIV();
	case ID3_RBUF: return new CID3F_RBUF();
	case ID3_RVAD: return new CID3F_RVAD(oldID);
	case ID3_RVRB: return new CID3F_RVRB();
	case ID3_SEEK: return new CID3F_SEEK();	
	case ID3_SIGN: return new CID3F_SIGN();
	case ID3_SYLT: return new CID3F_SYLT();
	case ID3_SYTC: return new CID3F_SYTC();
	case ID3_T000: return new CID3F_T000(findUniqueFrameID(oldID));
	case ID3_TXXX: return new CID3F_TXXX();
	case ID3_UFID: return new CID3F_UFID();
	case ID3_USER: return new CID3F_USER();
	case ID3_USLT: return new CID3F_USLT();
	case ID3_W000: return new CID3F_W000(findUniqueFrameID(oldID));
	case ID3_WXXX: return new CID3F_WXXX();
	default:
		return new CID3_Frame(id);
	};

}

