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
#include "wma_objectfactory.h"
#include "Tools.h"

TagPictureStruct CWMA_ObjectFactory::tps;
CAtlArray<CWMA_TagData *> CWMA_ObjectFactory::tagdatas;
CWMA_ObjectFactory::CWMA_ObjectFactory(void)
{		
}

CWMA_ObjectFactory::~CWMA_ObjectFactory(void)
{
	Reset();
}

void CWMA_ObjectFactory::Reset()
{
	for (size_t i = 0; i < tagdatas.GetCount(); i++)
		delete tagdatas.GetAt(i);
	tagdatas.RemoveAll();
}

CWMA_ObjectFactory& CWMA_ObjectFactory::instance()
{
	static CWMA_ObjectFactory factory;
	return factory;
}

CWMA_Object* CWMA_ObjectFactory::createObject(IID objectID) 
{
	if (IsEqualIID(objectID, WMA_FILE_PROPERTIES_ID))
	{
		return new CWMA_FileProperties();		
	}
	if (IsEqualIID(objectID, WMA_CONTENT_DESCRIPTION_ID))
	{
		return new CWMA_ContentDescription();		
	}
	if (IsEqualIID(objectID, WMA_HEADER_EXTENSION_ID))
	{
		return new CWMA_Header_Extension();		
	}
	if (IsEqualIID(objectID, WMA_EXTENDED_CONTENT_DESCRIPTION_ID))
	{
		return new CWMA_ExtContentDescription();		
	}
	if (IsEqualIID(objectID, WMA_METADATA_LIBRARY_ID))
	{
		return new CWMA_MetadataLibrary();	
	}
	if (IsEqualIID(objectID, WMA_STREAM_PROPERTIES_ID))
	{
		return new CWMA_StreamProperties();		
	}
	if (IsEqualIID(objectID, WMA_PADDING_ID))
	{
		return new CWMA_Padding();		
	}
	return new CWMA_Object(objectID);	
}

int CWMA_ObjectFactory::FindField(CAtlString FieldName)
{
	size_t counts = tagdatas.GetCount();
	for (size_t Iterator = 0; Iterator < counts; Iterator++)
	{
		if (tagdatas.GetAt(Iterator)->IsFieldName(FieldName))
			return (int)Iterator;	
	}
	return -1;
}

CAtlString CWMA_ObjectFactory::GetValue(size_t index)
{
	ATLASSERT(index < tagdatas.GetCount());
	return tagdatas.GetAt(index)->getFieldValue();
}

CAtlString CWMA_ObjectFactory::GetValue(CAtlString FieldName)
{
	int it = FindField(FieldName);
	// not found ??
	if (it == -1)
		return EMPTY;
	return tagdatas.GetAt(it)->getFieldValue();
}

void  CWMA_ObjectFactory::DeleteField(size_t index)
{
	ATLASSERT(index >= 0 && index < tagdatas.GetCount() );
	//	delete tagdatas.GetAt(index);
	tagdatas.RemoveAt(index);	
}

bool CWMA_ObjectFactory::SetValue(CAtlString FieldName, CAtlString newValue)
{
	if (FieldName.GetLength() == 0)
		return false;
	int it = FindField(FieldName);
	// Wenn neuer Wert leer ist und Feld bereits vorhanden, dann loesche das Feld aus der Liste
	// ansonsten lege kein leeres Feld an
	if (!newValue || newValue.IsEmpty())
	{
		if (it >= 0)
			DeleteField(it);				
		return true;
	}
	
	// Neuer Eintrag ?
	if (it == -1)
		tagdatas.Add(new CWMA_TagData(FieldName, newValue));
	else
		tagdatas.GetAt(it)->setNewValue(newValue);
	return true;
}

CAtlString CWMA_ObjectFactory::GetItemKeys()
{
	CAtlString result;
	/* search all stored fields for key */
	CAtlString key;
	size_t counts = tagdatas.GetCount();
	for (size_t Iterator = 0; Iterator < counts; Iterator++)
	{
		key = tagdatas.GetAt(Iterator)->getFieldName();
		// Nur aufnehmen, wenn noch nicht im result enthalten
		if (result.Find(key) < 0)
		{
			if (result.GetLength() > 0)
				result.AppendChar(',');
			result+=key;
		}
	}
	return result;
}


int CWMA_ObjectFactory::FindPicture(int index)
{
	int dummy = index;
	// Erniedrige bei jedem vorhandenen Bild die Variable pos
	size_t counts = tagdatas.GetCount();
	for (size_t Iterator = 0; Iterator < counts; Iterator++)
	{
		if (tagdatas.GetAt(Iterator)->IsPicture() )
		{
			if (--dummy == 0)
				return (int)Iterator;
		}
	}
	return -1;
}

TagPictureStruct* CWMA_ObjectFactory::ExtractPicture(CBlob* data)
{
	/* WM/Picture is structured as follows
	** 1st byte is the picture type (value between 0 and 20)
	** Next 4 bytes are in LE format. They give the size of the picture data
	** Next is a LPWSTR which gives the MIME type
	** Next is a LPWSTR which gives a picture description
	** Then is the actual picture data
	*/
	tps.Type = data->GetAt(0);
	tps.PicSize = (u32)data->GetR4B(1);
	int startPos = 5;
	tps.Mime = data->getNextString(TEXT_ENCODED_UTF16, startPos);
	tps.Description = data->getNextString(TEXT_ENCODED_UTF16, startPos);
	if (startPos + tps.PicSize > data->GetLength())
	{
		CTools::instance().setLastError(ERR_WMA_PARSE);
		tps.PicSize = 0;
	}
	tps.PicDaten = data->m_pData + startPos;
	return &tps;
}