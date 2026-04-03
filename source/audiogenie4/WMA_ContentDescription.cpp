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
#include "WMA_ContentDescription.h"
#include "Tools.h"

CWMA_ContentDescription::CWMA_ContentDescription(void)
{
	_objectID = WMA_CONTENT_DESCRIPTION_ID;
	_mustBuild = true;
}

CWMA_ContentDescription::CWMA_ContentDescription(CBlob *datas)
{
	_objectID = WMA_CONTENT_DESCRIPTION_ID;
	_data.Clear();
	_data.AddMemory(datas->m_pData, datas->GetLength());
	_mustBuild = false;
}

CWMA_ContentDescription::~CWMA_ContentDescription(void)
{	
}

void CWMA_ContentDescription::Empty()
{
	_data.Clear();
	Title.Empty();
	Author.Empty();
	Copyright.Empty();
	Description.Empty();
	Rating.Empty();
	_mustBuild = true;
}

bool CWMA_ContentDescription::load(FILE *Stream, size_t maxLen)
{
	//ATLTRACE(_T("ContentDescription with %i bytes\n"), maxLen);
	_data.FileRead(10, Stream);
	if (_data.GetLength() != 10 || maxLen < 10)
	{
		CTools::instance().setLastError(ERR_WMA_PARSE);
		_data.Clear();
		return false;
	}
	u16 TitleLength = _data.GetR2B(0);
	u16 AuthorLength = _data.GetR2B(2);
	u16 CopyrightLength = _data.GetR2B(4);
	u16 DescriptionLength = _data.GetR2B(6);
	u16 RatingLength = _data.GetR2B(8);

	//ATLTRACE(L"Read TitleLength=%i\n", TitleLength);
	//ATLTRACE(L"Read AuthorLength=%i\n", AuthorLength);
	//ATLTRACE(L"Read CopyrightLength=%i\n", CopyrightLength);
	//ATLTRACE(L"Read DescriptionLength=%i\n", DescriptionLength);
	//ATLTRACE(L"Read RatingLength=%i\n", RatingLength);
	_data.FileRead(TitleLength, Stream);
	_data.AddValue(0, 2);
	Title = (LPCWSTR)_data.m_pData;
	//ATLTRACE(L"Title:%s\n", Title);
	_data.FileRead(AuthorLength, Stream);
	_data.AddValue(0, 2);
	Author = (LPCWSTR)_data.m_pData;
	//ATLTRACE(L"Author:%s\n", Author);
	_data.FileRead(CopyrightLength, Stream);
	_data.AddValue(0, 2);
	Copyright = (LPCWSTR)_data.m_pData;
	//ATLTRACE(L"Copyright:%s\n", Copyright);
	_data.FileRead(DescriptionLength, Stream);
	_data.AddValue(0, 2);
	Description = (LPCWSTR)_data.m_pData;
	//ATLTRACE(L"Description:%s\n", Description);
	_data.FileRead(RatingLength, Stream);
	_data.AddValue(0, 2);
	Rating = (LPCWSTR)_data.m_pData;
	// ATLTRACE(L"Rating:%s\n", Rating);
	_mustBuild = true;
	return true;
}
void CWMA_ContentDescription::buildData()
{
	if (_mustBuild)
	{
		_data.Clear();
		u16 TitleLength = (u16)Title.GetLength() * 2; 
		u16 AuthorLength = (u16)Author.GetLength() * 2;
		u16 CopyrightLength = (u16)Copyright.GetLength() * 2;
		u16 DescriptionLength = (u16)Description.GetLength() * 2;
		u16 RatingLength = (u16)Rating.GetLength() * 2;
		
		if (TitleLength > 0)
			TitleLength+=2;
		if (AuthorLength > 0)
			AuthorLength+=2;
		if (CopyrightLength > 0)
			CopyrightLength+=2;
		if (DescriptionLength > 0)
			DescriptionLength+=2;
		if (RatingLength > 0)
			RatingLength+=2;
		_data.AddR2B(TitleLength);
		_data.AddR2B(AuthorLength);
		_data.AddR2B(CopyrightLength);
		_data.AddR2B(DescriptionLength);
		_data.AddR2B(RatingLength);

		if (TitleLength > 0)
		{
			_data.AddString(Title);
			_data.AddValue(0, 2);
		}
		if (AuthorLength > 0)
		{
			_data.AddString(Author);
			_data.AddValue(0, 2);
		}
		if (CopyrightLength > 0)
		{
			_data.AddString(Copyright);
			_data.AddValue(0, 2);
		}
		if (DescriptionLength > 0)
		{
			_data.AddString(Description);
			_data.AddValue(0, 2);
		}
		if (RatingLength > 0)
		{
			_data.AddString(Rating);
			_data.AddValue(0, 2);
		}
		_mustBuild = false;		
	}
}