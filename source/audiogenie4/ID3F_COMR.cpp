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
#include "ID3F_COMR.h"
#include "id3_framefactory.h"
#include "Tools.h"

/*
This frame enables several competing offers in the same tag by
bundling all needed information. That makes this frame rather complex
but it's an easier solution than if one tries to achieve the same
result with several frames. The frame begins, after the frame ID,
size and encoding fields, with a price string field. A price is
constructed by one three character currency code, encoded according
to ISO 4217 [ISO-4217] alphabetic currency code, followed by a
numerical value where "." is used as decimal separator. In the price
string several prices may be concatenated, separated by a "/"
character, but there may only be one currency of each type.

The price string is followed by an 8 character date string in the
format YYYYMMDD, describing for how long the price is valid. After
that is a contact URL, with which the user can contact the seller,
followed by a one byte 'received as' field. It describes how the
audio is delivered when bought according to the following list:

$00  Other
$01  Standard CD album with other songs
$02  Compressed audio on CD
$03  File over the Internet
$04  Stream over the Internet
$05  As note sheets
$06  As note sheets in a book with other sheets
$07  Music on other media
$08  Non-musical merchandise

Next follows a terminated string with the name of the seller followed
by a terminated string with a short description of the product. The
last thing is the ability to include a company logotype. The first of
them is the 'Picture MIME type' field containing information about
which picture format is used. In the event that the MIME media type
name is omitted, "image/" will be implied. Currently only "image/png"
and "image/jpeg" are allowed. This format string is followed by the
binary picture data. This two last fields may be omitted if no
picture is attached. There may be more than one 'commercial frame' in
a tag, but no two may be identical.

<Header for 'Commercial frame', ID: "COMR">
Text encoding      $xx
Price string       <text string> $00
Valid until        <text string>
Contact URL        <text string> $00
Received as        $xx
Name of seller     <text string according to encoding> $00 (00)
Description        <text string according to encoding> $00 (00)
Picture MIME type  <string> $00
Seller logo        <binary data>

*/

CID3F_COMR::CID3F_COMR(void)
{
	init(EMPTY, EMPTY, EMPTY, 0, EMPTY, EMPTY);
}

CID3F_COMR::CID3F_COMR(LPCWSTR price, LPCWSTR validUntil, LPCWSTR contactUrl , BYTE receivedAs , LPCWSTR seller, LPCWSTR description) 	
{
	init(price, validUntil, contactUrl, receivedAs, seller, description);
}

void CID3F_COMR::init(LPCWSTR price, LPCWSTR validUntil, LPCWSTR contactUrl , BYTE receivedAs , LPCWSTR seller, LPCWSTR description)
{
	_frameID = F_COMR;
	_price = price;
	_validUntil = validUntil;
	_contactUrl = contactUrl;
	_receivedAs = receivedAs;
	_seller = seller;
	_description = description;
	_mime.Empty();
	_data.Clear();
	useTextEncoding = true;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_COMR::~CID3F_COMR(void)
{
}

bool CID3F_COMR::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_COMR *target = cCOMR(frame);
	return (this->_description.CompareNoCase(target->_description) == 0
		&&  this->_price.CompareNoCase(target->_price) == 0
		&& this->_validUntil.CompareNoCase(target->_validUntil) == 0
		&& this->_contactUrl.CompareNoCase(target->_contactUrl) == 0
		&& this->_seller.CompareNoCase(target->_seller) == 0);
}

void CID3F_COMR::decode()
{
	if (!isDecoded)
	{	
		_data.Clear();
		if (_blob.GetLength() < 10)
		{
			_price.Empty();
			_validUntil.Empty();
			_contactUrl.Empty();
			_receivedAs = 0;
			_seller.Empty();
			_description.Empty();
			_mime.Empty();			
		}
		else
		{
			encodingID = _blob.GetAt(0);
			int start = 1;
			_price = _blob.getNextString(TEXT_ENCODED_ANSI, start);
			_validUntil = _T("        ");
			_validUntil.SetAt(0, _blob.GetAt(start++)); 
			_validUntil.SetAt(1, _blob.GetAt(start++));
			_validUntil.SetAt(2, _blob.GetAt(start++));
			_validUntil.SetAt(3, _blob.GetAt(start++));
			_validUntil.SetAt(4, _blob.GetAt(start++));
			_validUntil.SetAt(5, _blob.GetAt(start++));
			_validUntil.SetAt(6, _blob.GetAt(start++));
			_validUntil.SetAt(7, _blob.GetAt(start++));
			_contactUrl = _blob.getNextString(TEXT_ENCODED_ANSI, start);	
			_receivedAs = _blob.GetAt(start++);
			_seller = _blob.getNextString(encodingID, start);
			_description = _blob.getNextString(encodingID, start);
			_mime = _blob.getNextString(TEXT_ENCODED_ANSI, start);
			_data.AddBlob(_blob, start);
		}		
		isDecoded = true;
	}	
}
void CID3F_COMR::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddValue(encodingID);
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _price, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);		
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _validUntil, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);		
		_blob.AddEncodedString(TEXT_ENCODED_ANSI, _contactUrl, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);		
		_blob.AddValue(_receivedAs);
		_blob.AddEncodedString(encodingID, _seller, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		_blob.AddEncodedString(encodingID, _description, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		if (_data.GetLength() > 0)
		{		
			_blob.AddEncodedString(TEXT_ENCODED_ANSI, _mime, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);		
			_blob.AddBlob(_data);
		}		
		mustRebuild = false;
	}
}

CAtlString CID3F_COMR::getPrice()
{
	decode();
	return _price;
}

CAtlString CID3F_COMR::getValidUntil()
{
	decode();
	return _validUntil;
}

CAtlString CID3F_COMR::getContactUrl()
{
	decode();
	return _contactUrl;
}

BYTE CID3F_COMR::getReceivedAs()
{
	decode();
	return _receivedAs;
}

CAtlString CID3F_COMR::getSellerName()
{
	decode();
	return _seller;
}

CAtlString CID3F_COMR::getDescription()
{
	decode();
	return _description;
}

CAtlString CID3F_COMR::getPictureMime()
{
	decode();
	return _mime;
}

void CID3F_COMR::setPicture(BYTE *arr, u32 length)
{
	setData(arr, length);
	if (length >= 4)
		_mime = CTools::instance().ExtractMimeFromPicture(arr);
	else
		_mime.Empty();
}


