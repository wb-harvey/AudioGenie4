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
#include "WMA_ExtContentDescription.h"
#include "Tools.h"
#include "wma_objectfactory.h"

CWMA_ExtContentDescription::CWMA_ExtContentDescription(void)
{
	_objectID = WMA_EXTENDED_CONTENT_DESCRIPTION_ID;
	_mustBuild = true;
}
CWMA_ExtContentDescription::CWMA_ExtContentDescription(CBlob *datas)
{
	_objectID = WMA_EXTENDED_CONTENT_DESCRIPTION_ID;
	_data.Clear();
	_data.AddMemory(datas->m_pData, datas->GetLength());
	_mustBuild = false;	
}

CWMA_ExtContentDescription::~CWMA_ExtContentDescription(void)
{
	_data.Clear();
}

bool CWMA_ExtContentDescription::load(FILE *Stream, size_t maxLen)
{
	ATLTRACE(_T("ExtContentDescription with %i bytes\n"), maxLen);
	_data.FileRead(2, Stream);
	if (_data.GetLength() != 2 || maxLen < 2)
	{
		CTools::instance().setLastError(ERR_WMA_PARSE);
		_data.Clear();
		return false;
	}
	u16 TagCount = _data.GetR2B(0);
	// 4.0.4.7: each descriptor is bounded by what is left of the object, not by the whole object (4.0.4.6 and earlier)
	size_t remaining = maxLen - 2;
	CWMA_TagData *item;
	for (size_t Iterator = 0; Iterator < TagCount; Iterator++)
	{
		item = new CWMA_TagData(EXTCONTENT_ART);
		if (!item->load(Stream, remaining) )
		{
			delete item;
			return false;
		}
		remaining -= item->getLoadedSize();
		CWMA_ObjectFactory::instance().tagdatas.Add(item);
		ATLTRACE(_T("Value:%s\n"), item->getFieldValue());
	}
	_mustBuild = true;
	return true;
}

void CWMA_ExtContentDescription::buildData()
{
	if (_mustBuild)
	{
		_data.Clear();
		// Suche alle Objekte
		CWMA_TagData *item;
		int anzahl = 0;
		size_t counts = CWMA_ObjectFactory::instance().tagdatas.GetCount();
		for (size_t Iterator = 0; Iterator < counts; Iterator++)
		{
			if (CWMA_ObjectFactory::instance().tagdatas.GetAt(Iterator)->getArt() == EXTCONTENT_ART)
				anzahl++;
		}
		if (anzahl > 0)
		{
			_data.AddR2B(anzahl);
			for (size_t Iterator = 0; Iterator < counts; Iterator++)
			{
				item = CWMA_ObjectFactory::instance().tagdatas.GetAt(Iterator);
				if (item->getArt() == EXTCONTENT_ART)
					item->buildData(&_data);		
			}
		}
		_mustBuild = false;
	}
}
