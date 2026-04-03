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

#pragma once

#include "StdAfx.h"
#include "wma_object.h"
#include "Tools.h"


CWMA_Object::CWMA_Object(void)
{
	_data.Clear();
	memset(&_objectID, 0, 16);
}

CWMA_Object::CWMA_Object(IID newID)
{
	_data.Clear();
	_objectID = newID;
}

CWMA_Object::~CWMA_Object()
{
}

void CWMA_Object::Remove()
{
	_data.Clear();
}

bool CWMA_Object::isIID(IID otherID)
{
	return (IsEqualIID(_objectID, otherID) != 0);	
}

size_t CWMA_Object::getDataSize()
{
	buildData();
	return _data.GetLength();
}

bool CWMA_Object::load(FILE *Stream, size_t dataSize)
{
	ATLTRACE(_T("WMA_Object with %i bytes\n"), dataSize);
	_data.FileRead(dataSize, Stream);
	return (_data.GetLength() == dataSize);
}

void CWMA_Object::save(CBlob *blob)
{
	buildData();
	if (_data.GetLength() > 0)
	{
		blob->AddMemory(&_objectID, 16);
		blob->AddR8B(_data.GetLength() + 24);
		blob->AddBlob(_data);
	}
}

void CWMA_Object::buildData()
{
	// Nix tun, da die Daten unverändert rausgeschrieben werden
}
