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
#include "WMA_Header_Extension.h"
#include "wma_objectfactory.h"
#include "Tools.h"

CWMA_Header_Extension::CWMA_Header_Extension(void)
{
	_objectID = WMA_HEADER_EXTENSION_ID;	
}

CWMA_Header_Extension::~CWMA_Header_Extension(void)
{
	Remove();
}
void CWMA_Header_Extension::Empty()
{
	Remove();
}

bool CWMA_Header_Extension::load(FILE *Stream, size_t maxLen)
{
	IID testIID;
	_data.FileRead(22, Stream);
	if (_data.GetLength() != 22)
	{
		CTools::instance().setLastError(ERR_WMA_PARSE);
		_data.Clear();
		return false;
	}

	memcpy(&reserved1.Data1, _data.m_pData, 16);
	reserved2 = _data.GetR2B(16); // should be 6
	__int64 totalSize = _data.GetR4B(18);
	__int64 Size;
	CWMA_Object* obj;
	while (totalSize > 0)
	{
		ATLTRACE(_T(" Item in Extended Header at Pos:%i "), ftell(Stream));
		fread(&testIID, 1, 16, Stream);
		fread(&Size, 1, 8, Stream);
		totalSize-= Size;
		Size-=24;			
		if (Size < 0 || Size > maxLen)
		{
			CTools::instance().setLastError(ERR_WMA_PARSE);
			_data.Clear();
			return false;
		}
		obj = CWMA_ObjectFactory::instance().createObject(testIID);
		obj->load(Stream, (size_t)Size);
		_children.Add(obj);
	}
	return true;

}

CWMA_Object* CWMA_Header_Extension::findObject(IID objectID)
{
	size_t counts =  _children.GetCount();
	for (size_t Iterator = 0; Iterator < counts; Iterator++)
	{
		if (_children.GetAt(Iterator)->isIID(objectID) )
			return _children.GetAt(Iterator);
	}
	return NULL;
}

void CWMA_Header_Extension::replaceObject(CWMA_Object* object)
{
	bool found = false;
	size_t counts = _children.GetCount();
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


bool CWMA_Header_Extension::deleteObject(IID objectID)
{
	size_t counts = _children.GetCount();
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

void CWMA_Header_Extension::buildData()
{
	_data.Clear();
	_data.AddMemory(&reserved1.Data1, 16);
	_data.AddR2B(reserved2);	
	CBlob tmp;
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		_children.GetAt(i)->save(&tmp);
	}
	_data.AddR4B((int)tmp.GetLength());
	_data.AddBlob(tmp);
}

void CWMA_Header_Extension::Remove()
{
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		delete _children.GetAt(i);
	}
	_children.RemoveAll();
	_children.SetCount(0, 10);
}