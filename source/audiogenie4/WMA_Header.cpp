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
	fread(&testIID, 1, 16, Stream);
	if (!IsEqualIID(testIID, WMA_HEADER_OBJECT))
		return 0;
	fread(&HeaderSize, 1, 8, Stream);
	return HeaderSize;
}


bool CWMA_Header::load(FILE *Stream, size_t maxLen)
{
	
	__int64 HeaderSize = loadHeaderOnly(Stream);
	_data.FileRead(6, Stream);
	if (_data.GetLength() != 6 || HeaderSize == 0 || HeaderSize > maxLen)
	{
		CTools::instance().setLastError(ERR_WMA_PARSE);
		_data.Clear();
		return false;
	}
	u32 anzElem = _data.GetR4B(0);
	__int64 Size;
	CWMA_Object* obj;
	long Position = 0;
	Position = ftell(Stream);
	for (size_t i = 0; i < anzElem; i++)
	{
		ATLTRACE(_T(" at Pos:%i "), ftell(Stream));
		fread(&testIID, 1, 16, Stream);
		fread(&Size, 1, 8, Stream);
		Position+=24;
		Size-=24;
		if (Size < 0 || Size > HeaderSize)
		{
			CTools::instance().setLastError(ERR_WMA_PARSE);
			_data.Clear();
			return false;
		}
		obj = CWMA_ObjectFactory::instance().createObject(testIID);
		obj->load(Stream, (long)Size);
		_children.Add(obj);
		Position+=(long)Size;
		fseek(Stream, Position, SEEK_SET);
	}
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
	_data.Clear();
	_data.AddR4B((int)_children.GetCount());
	_data.AddValue(1); // reserved1
	_data.AddValue(2); // reserved2
	for (size_t i = 0; i < _children.GetCount(); i++)
		_children.GetAt(i)->save(&_data);		
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
