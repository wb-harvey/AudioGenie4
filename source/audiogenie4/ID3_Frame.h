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

#include "Blob.h"
#include "Tools.h"
typedef unsigned __int32  u32;
typedef signed __int32  i32;
typedef unsigned __int16  u16;
typedef signed __int16  i16;

class CID3_Frame
{
public:
	CID3_Frame(void);
	CID3_Frame(unsigned int frameID);
	virtual ~CID3_Frame(void);
	virtual bool equals(CID3_Frame *frame);
	virtual bool equals(const unsigned int ID);
	void init(unsigned int frameID);
	long getSize();
	CBlob _blob, _data;	
	unsigned int _frameID;
	BYTE encodingID;
	u16 flags;
	bool isDecoded;
	bool mustRebuild;
	bool useTextEncoding;
	long getData(BYTE *destination, long maxLen);
	bool setData(BYTE *source, unsigned int maxLen);
	void storeFrame(CBlob *tmp);
	CAtlString getFrameIDString();
	/* analysiert die Daten aus dem Blob */
	virtual void decode() {} ;
	/** überträgt die Daten in den Blob **/
	virtual void encode() {} ;
	void load(BYTE *source, long size);
	void save(FILE *stream);
	virtual void print();
	//void unsync();
	void resync();
	bool isUnsynchronized();
	bool isReadOnly();
	bool isGrouped();
	bool isCompressed();
	bool isEncrypted();
	bool isDataLenIndicator();	
};

