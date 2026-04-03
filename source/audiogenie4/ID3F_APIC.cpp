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
#include "ID3F_APIC.h"
#include "id3_framefactory.h"
#include "ID3v2.h"
#include "Tools.h"
#include "io.h"


/*
This frame contains a picture directly related to the audio file.
Image format is the MIME type and subtype [MIME] for the image. In
the event that the MIME media type name is omitted, "image/" will be
implied. The "image/png" [PNG] or "image/jpeg" [JFIF] picture format
should be used when interoperability is wanted. Description is a
short description of the picture, represented as a terminated
text string. There may be several pictures attached to one file, each
in their individual "APIC" frame, but only one with the same content
descriptor. There may only be one picture with the picture type
declared as picture type $01 and $02 respectively. There is the
possibility to put only a link to the image file by using the 'MIME
type' "-->" and having a complete URL [URL] instead of picture data.
The use of linked files should however be used sparingly since there
is the risk of separation of files.

<Header for 'Attached picture', ID: "APIC">
Text encoding      $xx
MIME type          <text string> $00
Picture type       $xx
Description        <text string according to encoding> $00 (00)
Picture data       <binary data>

// v2.2:
// Text encoding      $xx
// Image format       $xx xx xx
// Picture type       $xx
// Description        <textstring> $00 (00)
// Picture data       <binary data>

Picture type:  $00  Other
$01  32x32 pixels 'file icon' (PNG only)
$02  Other file icon
$03  Cover (front)
$04  Cover (back)
$05  Leaflet page
$06  Media (e.g. label side of CD)
$07  Lead artist/lead performer/soloist
$08  Artist/performer
$09  Conductor
$0A  Band/Orchestra
$0B  Composer
$0C  Lyricist/text writer
$0D  Recording Location
$0E  During recording
$0F  During performance
$10  Movie/video screen capture
$11  A bright coloured fish
$12  Illustration
$13  Band/artist logotype
$14  Publisher/Studio logotype
*/


CID3F_APIC::CID3F_APIC(void)
{
	init(0, EMPTY);	
}

CID3F_APIC::CID3F_APIC(BYTE newPictureType, LPCWSTR newDescription)
{
	init(newPictureType, newDescription);	
}

void CID3F_APIC::init(BYTE newPictureType, LPCWSTR newDescription)
{
	_frameID = F_APIC;
	_pictureType = newPictureType;
	_description = (newDescription == NULL ) ? EMPTY : newDescription;
	_isLink = false;
	_mime.Empty();
	_data.Clear();
	useTextEncoding = true;		
	mustRebuild = true;
	isDecoded = true;
	lastTag = 99;
}

CID3F_APIC::~CID3F_APIC(void)
{
	_blob.Clear();
}

bool CID3F_APIC::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_APIC *target = cAPIC(frame);
	return (this->_description.CompareNoCase(target->_description) == 0);
}

void CID3F_APIC::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		_data.Clear();
		lastTag = CTools::ID3V2oldTagVersion;
		if (_blob.GetLength() < 4)
		{
			_mime.Empty();
			_description.Empty();
			_pictureType = 0;
			_pictureLink.Empty();
		}
		else
		{
			int start = 0;
			encodingID = _blob.GetAt(start++);
			if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_2)
			{
				_mime = _blob.GetStringAt(start, 3);
				start+=3;
			}
			else
			{
				_mime = _blob.getNextString(TEXT_ENCODED_ANSI, start);
			}
			_pictureType = _blob.GetAt(start++);
			_description = _blob.getNextString(encodingID, start);
			// nun Bilddaten
			// Falls Mime --> ist , dann Bild aus Datei lesen
			if (_mime.Compare(_T("-->")) == 0)
			{
				_isLink = true;
				_pictureLink = _blob.getNextString(TEXT_ENCODED_ANSI, start);
				FILE *tmpSource;
				if ( (tmpSource = _wfsopen(_pictureLink, READ_ONLY, _SH_DENYNO)) != NULL)
				{
					_data.FileRead(_filelength(_fileno(tmpSource)), tmpSource);
					fclose(tmpSource);
				}
				else
					CTools::instance().setLastError(ERR_IMAGEURL_NOT_FOUND, _pictureLink);				
			}
			else // Bild aus Speicher lesen
			{
				_isLink = false;
				_data.AddBlob(_blob, start);
			}
		}
		isDecoded = true;
	}	
}

void CID3F_APIC::encode()
{
	if (mustRebuild || lastTag != CTools::ID3V2newTagVersion)
	{
		decode();
		_blob.Clear();
		_blob.AddValue(encodingID);
		if (CTools::ID3V2newTagVersion == TAG_VERSION_2_2)
		{
			if (_isLink)
				_mime = MIME_LINK;
			else
				_mime = CTools::instance().ExtractSmallMimeFromPicture(_data.m_pData);
			_blob.AddFixedAnsiString(_mime, 3);
		}
		else
		{
			if (_isLink)
				_mime = MIME_LINK;
			else
				_mime = CTools::instance().ExtractMimeFromPicture(_data.m_pData);
			_blob.AddEncodedString(TEXT_ENCODED_ANSI, _mime, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);		
		}
		_blob.AddValue(_pictureType);
		_blob.AddEncodedString(encodingID, _description, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		if (_isLink)
			_blob.AddEncodedString(TEXT_ENCODED_ANSI, _pictureLink, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
		else
			_blob.AddBlob(_data);
		mustRebuild = false;
		lastTag = CTools::ID3V2newTagVersion;
		//if (isUnsynchronized())
		//	unsync();
	}
}

CAtlString CID3F_APIC::getMime()
{
	decode();
	return (_isLink) ?  MIME_LINK + _pictureLink : _mime;	
}

CAtlString CID3F_APIC::getDescription()
{
	decode();
	return _description;
}

BYTE CID3F_APIC::getPictureType()
{
	decode();
	return _pictureType;
}

long CID3F_APIC::getPictureSize()
{
	decode();
	return (long)_data.GetLength();
}

CAtlString CID3F_APIC::getPictureTypeAsText()
{
	decode();
	if (_pictureType >= 0 && _pictureType <= 20)
		return APIC_PICTURE_TYPES[_pictureType];

	CAtlString dummy;	
	dummy.Format(_T("unknown PictureType %i"), _pictureType);	
	return dummy;
}

void CID3F_APIC::setDescription(LPCWSTR newText)
{
	decode();
	if (_description.Compare(newText) != 0)
	{
		_description = newText;
		mustRebuild = true;
	}
}

void CID3F_APIC::setPictureType(BYTE newType)
{
	decode();
	if (newType != _pictureType)
	{
		_pictureType = newType;
		mustRebuild = true;
	}
}

bool CID3F_APIC::getPictureFile(LPCWSTR fileName)
{
	decode();
	FILE *Stream;
	ATLTRACE(_T("open %s WRITE_ONLY\n"), fileName);
	if ( (Stream = _wfsopen(fileName, WRITE_ONLY, _SH_DENYNO)) != NULL)
	{
		_data.FileWrite(_data.GetLength(), Stream);	  
		fflush(Stream);
		fclose(Stream);
		return true;
	}
	CTools::instance().setLastError(errno);
	return false;

}

bool CID3F_APIC::setPictureFile(LPCWSTR fileName)
{
	decode();
	FILE *Stream;
	ATLTRACE(_T("open %s READ_ONLY\n"), fileName);
	_data.Clear();
	_isLink = false;
	_pictureLink.Empty();
	if ( (Stream = _wfsopen(fileName, READ_ONLY, _SH_DENYNO)) != NULL)
	{
		_data.FileRead(_filelength(_fileno(Stream)), Stream); 
		fclose(Stream);
		// MIME neu bilden
		if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_2)
			_mime = CTools::instance().ExtractSmallMimeFromPicture(_data.m_pData);
		else
			_mime = CTools::instance().ExtractMimeFromPicture(_data.m_pData);
		return true;
	}
	CTools::instance().setLastError(errno);
	return false;
}

bool CID3F_APIC::setFileLink(LPCWSTR fileName)
{
	decode();
	FILE *Stream;
	_data.Clear();
	_isLink = true;
	_pictureLink.Empty();
	if (fileName != NULL && wcslen(fileName) > 0)
	{
		_pictureLink = fileName;
		_mime = MIME_LINK;
		if ( (Stream = _wfsopen(fileName, READ_ONLY, _SH_DENYNO)) != NULL)
		{
			_data.FileRead(_filelength(_fileno(Stream)), Stream); 
			fclose(Stream);
		}
		return true;
	}	
	CTools::instance().setLastError(ERR_INVALID_FILENAME);
	return false;
}
