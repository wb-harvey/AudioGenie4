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
#include "ID3F_OWNE.h"
#include "id3_framefactory.h"

/*
The ownership frame might be used as a reminder of a made transaction
or, if signed, as proof. Note that the "USER" and "TOWN" frames are
good to use in conjunction with this one. The frame begins, after the
frame ID, size and encoding fields, with a 'price paid' field. The
first three characters of this field contains the currency used for
the transaction, encoded according to ISO 4217 [ISO-4217] alphabetic
currency code. Concatenated to this is the actual price paid, as a
numerical string using "." as the decimal separator. Next is an 8
character date string (YYYYMMDD) followed by a string with the name
of the seller as the last field in the frame. There may only be one
"OWNE" frame in a tag.

<Header for 'Ownership frame', ID: "OWNE">
Text encoding     $xx
Price paid        <text string> $00
Date of purch.    <text string>
Seller            <text string according to encoding>
*/

CID3F_OWNE::CID3F_OWNE(void)
{
	init(EMPTY, EMPTY, EMPTY);
}

CID3F_OWNE::CID3F_OWNE(LPCWSTR price, LPCWSTR date, LPCWSTR seller) 	
{
	init(price, date, seller);
}

void CID3F_OWNE::init(LPCWSTR price, LPCWSTR date, LPCWSTR seller)
{
	_frameID = F_OWNE;
	_price = price;
	_date = date;
	_seller = seller;
	useTextEncoding = true;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_OWNE::~CID3F_OWNE(void)
{
}

void CID3F_OWNE::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		if (_blob.GetLength() < 10)
		{
			_price.Empty();
			_date.Empty();
			_seller.Empty();
		}
		else
		{
			encodingID = _blob.GetAt(0);
			int start = 1;
			_price = _blob.getNextString(TEXT_ENCODED_ANSI, start);
			_date = _T("        ");
			_date.SetAt(0, _blob.GetAt(start++));
			_date.SetAt(1, _blob.GetAt(start++));
			_date.SetAt(2, _blob.GetAt(start++));
			_date.SetAt(3, _blob.GetAt(start++));
			_date.SetAt(4, _blob.GetAt(start++));
			_date.SetAt(5, _blob.GetAt(start++));
			_date.SetAt(6, _blob.GetAt(start++));
			_date.SetAt(7, _blob.GetAt(start++));
			_seller = _blob.getNextString(encodingID, start);	
		}		
		isDecoded = true;
	}	
}
void CID3F_OWNE::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddValue(encodingID);
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _price, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _date, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
		_blob.AddEncodedString(encodingID, _seller, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
		mustRebuild = false;		
	}
}

CAtlString CID3F_OWNE::getPrice()
{
	decode();
	return _price;
}

CAtlString CID3F_OWNE::getDate()
{
	decode();
	return _date;
}

CAtlString CID3F_OWNE::getSeller()
{
	decode();
	return _seller;
}