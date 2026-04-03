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
#include "wma_object.h"
#include "atlcoll.h"
#include "wma_fileproperties.h"
#include "wma_contentdescription.h"
#include "wma_header_extension.h"
#include "wma_extcontentdescription.h"
#include "wma_metadatalibrary.h"
#include "wma_tagdata.h"
#include "wma_padding.h"
#include "wma_streamproperties.h"

struct TagPictureStruct
{
	CAtlString Mime;
	CAtlString Description;
	BYTE* PicDaten;
	u32 PicSize;
	BYTE Type;
};

class CWMA_ObjectFactory
{
public:
	static CWMA_ObjectFactory &instance();
	static CWMA_Object *createObject(IID objectID) ;
	static void Reset();
	static CAtlArray<CWMA_TagData *> tagdatas;
	static TagPictureStruct* ExtractPicture(CBlob* data);
	static int FindField(CAtlString FieldName);
	static CAtlString GetValue(size_t index);
	static CAtlString GetValue(CAtlString FieldName);
	static void DeleteField(size_t index);
	static bool SetValue(CAtlString FieldName, CAtlString newValue);
	static CAtlString GetItemKeys();
	static int FindPicture(int index);
protected:
	/*!
	* Constructs a factory.  Because this is a singleton this method is
	* protected, but may be used for subclasses.
	*/
	CWMA_ObjectFactory();
	virtual ~CWMA_ObjectFactory();	
	
private:
	CWMA_ObjectFactory(const CWMA_ObjectFactory &);
	CWMA_ObjectFactory &operator=(const CWMA_ObjectFactory &);
	static TagPictureStruct tps;	
};