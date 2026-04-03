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
#include "WMA_MetadataLibrary.h"
#include "wma_objectfactory.h"

CWMA_MetadataLibrary::CWMA_MetadataLibrary(void)
{
	_objectID = WMA_METADATA_LIBRARY_ID;
	_mustBuild = true;
}
CWMA_MetadataLibrary::CWMA_MetadataLibrary(CBlob *datas)
{
	_objectID = WMA_METADATA_LIBRARY_ID;
	_data.Clear();
	_data.AddMemory(datas->m_pData, datas->GetLength());
	_mustBuild = false;
	_objectID = WMA_METADATA_LIBRARY_ID;
}

CWMA_MetadataLibrary::~CWMA_MetadataLibrary(void)
{	
}


bool CWMA_MetadataLibrary::load(FILE *Stream, size_t maxLen)
{
	ATLTRACE(_T("Metadata Library with %i bytes\n"), maxLen);
	_data.FileRead(2, Stream);
	u16 TagCount = _data.GetR2B(0);
	CWMA_TagData *item;
	for (size_t Iterator = 0; Iterator < TagCount; Iterator++)
	{
		item = new CWMA_TagData(METADATA_ART);
		if (!item->load(Stream, maxLen) )
		{
			delete item;
			return false;
		}
		CWMA_ObjectFactory::instance().tagdatas.Add(item);
		ATLTRACE(_T("Value:%s\n"), item->getFieldValue());
	}
	_mustBuild = true;
	return true;
}

void CWMA_MetadataLibrary::buildData()
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
			if (CWMA_ObjectFactory::instance().tagdatas.GetAt(Iterator)->getArt() == METADATA_ART)
				anzahl++;
		}
		if (anzahl > 0)
		{
			_data.AddR2B(anzahl);
			for (size_t Iterator = 0; Iterator < counts; Iterator++)
			{
				item = CWMA_ObjectFactory::instance().tagdatas.GetAt(Iterator);
				if (item->getArt() == METADATA_ART)
					item->buildData(&_data);		
			}
		}
		_mustBuild = false;		
	}
}
