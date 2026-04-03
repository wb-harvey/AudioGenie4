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

#include "ID3_frame.h"
static const LPCWSTR APIC_PICTURE_TYPES[] =
{ _T("Other"), _T("32x32 pixels file icon"),
  _T("Other file icon"), _T("Cover (front)"),
  _T("Cover (back)"),  _T("Leaflet page"),
  _T("Media"), _T("Lead artist/lead performer/soloist"),
  _T("Artist/performer"), _T("Conductor"),
  _T("Band/Orchestra"), _T("Composer"),
  _T("Lyricist/text writer"), _T("Recording Location"),
  _T("During recording"), _T("During performance"),
  _T("Movie/video screen capture"), _T("A bright coloured fish"),
  _T("Illustration"), _T("Band/artist logotype"),
  _T("Publisher/Studio logotype")
};

class CID3F_APIC : public CID3_Frame
{
public:
	CID3F_APIC(void);
	CID3F_APIC(BYTE newPictureType, LPCWSTR newDescription);
	~CID3F_APIC(void);
	void init(BYTE newPictureType, LPCWSTR newDescription);
	bool equals(CID3_Frame *frame);
	void decode();
	void encode();
	CAtlString getMime();
	CAtlString getDescription();
	BYTE getPictureType();
	CAtlString getPictureTypeAsText();
	long getPictureSize();
	void setDescription(LPCWSTR newText);
	void setPictureType(BYTE newType);
	bool getPictureFile(LPCWSTR fileName);
	bool setPictureFile(LPCWSTR fileName);
	bool setFileLink(LPCWSTR fileName);
private:
	CAtlString _mime, _description, _pictureLink;
	BYTE _pictureType;
	BYTE lastTag;
	bool _isLink;
};
