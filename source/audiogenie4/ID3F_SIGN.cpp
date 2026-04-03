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
#include "ID3F_SIGN.h"
#include "id3_framefactory.h"

/*
Signature frame

This frame enables a group of frames, grouped with the 'Group
identification registration', to be signed. Although signatures can
reside inside the registration frame, it might be desired to store
the signature elsewhere, e.g. in watermarks. There may be more than
one 'signature frame' in a tag, but no two may be identical.

<Header for 'Signature frame', ID: "SIGN">
Group symbol      $xx
Signature         <binary data>
*/

CID3F_SIGN::CID3F_SIGN(void)
{
	init(0);
}

CID3F_SIGN::CID3F_SIGN(BYTE Symbol) 	
{
	init(Symbol);
}

void CID3F_SIGN::init(BYTE Symbol)
{
	_frameID = F_SIGN;
	_symbol = Symbol;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_SIGN::~CID3F_SIGN(void)
{
}

bool CID3F_SIGN::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_SIGN *target = cSIGN(frame);
	return (this->_symbol == target->_symbol);
}

void CID3F_SIGN::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_data.Clear();
		if (_blob.GetLength() < 2)
		{
			_symbol = 0;			
		}
		else
		{
			_symbol = _blob.GetAt(0);
			_data.AddBlob(_blob, 1);
		}		
		isDecoded = true;
	}	
}
void CID3F_SIGN::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddValue(_symbol);
		_blob.AddBlob(_data);
		mustRebuild = false;
	}
}

BYTE CID3F_SIGN::getSymbol()
{
	decode();
	return _symbol;
}

