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
#include "WMA_Header.h"
#include "Tools.h"
#include "wma_objectfactory.h"

CWMA_Header::CWMA_Header(void)
{
	_objectID = WMA_HEADER_OBJECT;
	_complete = false;
}

CWMA_Header::~CWMA_Header(void)
{
	Remove();
}

void CWMA_Header::Empty()
{
	Remove();
}

__int64 CWMA_Header::loadHeaderOnly(FILE *Stream)
{
	// Ist das die richtige ID ?
	__int64 HeaderSize;
	if (fread(&testIID, 1, 16, Stream) != 16 || !IsEqualIID(testIID, WMA_HEADER_OBJECT))
		return 0;
	if (fread(&HeaderSize, 1, 8, Stream) != 8 || HeaderSize < 30)
		return 0;
	return HeaderSize;
}


bool CWMA_Header::load(FILE *Stream, size_t maxLen)
{
	
	_complete = false;
	__int64 HeaderSize = loadHeaderOnly(Stream);
	_data.FileRead(6, Stream);
	if (_data.GetLength() != 6 || HeaderSize == 0 || (unsigned __int64)HeaderSize > maxLen)
	{
		CTools::instance().setLastError(ERR_WMA_PARSE);
		_data.Clear();
		return false;
	}
	u32 anzElem = _data.GetR4B(0);
	__int64 Size;
	CWMA_Object* obj;
	// 4.0.4.7: 64-bit positions and checked reads (a header cut short repeated the last object header it read),
	// and whether every object was read whole: only then may the header be written back (4.0.4.6 and earlier)
	bool whole = true;
	__int64 Position = _ftelli64(Stream);
	for (size_t i = 0; i < anzElem; i++)
	{
		if (fread(&testIID, 1, 16, Stream) != 16 || fread(&Size, 1, 8, Stream) != 8)
		{
			CTools::instance().setLastError(ERR_WMA_PARSE);
			_data.Clear();
			return false;
		}
		Position+=24;
		Size-=24;
		if (Size < 0 || Size > HeaderSize)
		{
			CTools::instance().setLastError(ERR_WMA_PARSE);
			_data.Clear();
			return false;
		}
		obj = CWMA_ObjectFactory::instance().createObject(testIID);
		bool loaded = false;
		try
		{
			loaded = obj->load(Stream, (size_t)Size);
		}
		catch (...)
		{
			loaded = false;
		}
		if (!loaded)
			whole = false;
		_children.Add(obj);
		Position+=Size;
		_fseeki64(Stream, Position, SEEK_SET);
	}
	_complete = whole;
	return true;
}

CWMA_Object* CWMA_Header::findObject(IID objectID)
{
	size_t counts =  _children.GetCount();
	for (size_t Iterator = 0; Iterator < counts; Iterator++)
	{
		if (_children.GetAt(Iterator)->isIID(objectID) )
			return _children.GetAt(Iterator);
	}
	return NULL;
}

void CWMA_Header::replaceObject(CWMA_Object* object)
{
	bool found = false;
	size_t counts =  _children.GetCount();
	for (size_t Iterator = 0; Iterator < counts; Iterator++)
	{
		if (_children.GetAt(Iterator)->isIID(object->getIID()) )
		{
			 delete _children.GetAt(Iterator);
			_children.SetAt(Iterator, object);
			found = true;
		}
	}
	if (!found)
		_children.Add(object);
}

void  CWMA_Header::insertObject(CWMA_Object* object, size_t position)
{
	_children.InsertAt(position, object);
}

void CWMA_Header::buildData()
{
	// 4.0.4.7: count only the objects written; an empty one writes nothing and the count was too high, so players
	// read past the header (4.0.4.6 and earlier)
	CBlob objects;
	int written = 0;
	for (size_t i = 0; i < _children.GetCount(); i++)
	{
		size_t before = objects.GetLength();
		_children.GetAt(i)->save(&objects);
		if (objects.GetLength() > before)
			written++;
	}
	_data.Clear();
	_data.AddR4B(written);
	_data.AddValue(1); // reserved1
	_data.AddValue(2); // reserved2
	_data.AddBlob(objects);
}

bool CWMA_Header::deleteObject(IID objectID)
{
	size_t counts =  _children.GetCount();
	for (size_t Iterator = 0; Iterator < counts; Iterator++)
	{
		if (_children.GetAt(Iterator)->isIID(objectID) )
		{
			delete _children.GetAt(Iterator);
			_children.RemoveAt(Iterator);
			return true;
		}
	}
	return false;
}

void CWMA_Header::Remove()
{
	size_t counts =  _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		_children.GetAt(i)->Remove();
		 delete _children.GetAt(i);
	}
	_children.RemoveAll();
	_children.SetCount(0, 10);
	CWMA_ObjectFactory::instance().Reset();
}
