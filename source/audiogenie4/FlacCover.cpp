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
#include "FlacCover.h"
#include "Tools.h"
#include "share.h"
#include "io.h"
#include "ID3F_APIC.h"

/** 
<32>   	 The picture type according to the ID3v2 APIC frame:
Others are reserved and should not be used. There may only be one each of picture type 1 and 2 in a file.
<32> 	The length of the MIME type string in bytes.
<n*8> 	The MIME type string, in printable ASCII characters 0x20-0x7e. The MIME type may also be --> to signify that the data part is a URL of the picture instead of the picture data itself.
<32> 	The length of the description string in bytes.
<n*8> 	The description of the picture, in UTF-8.
<32> 	The width of the picture in pixels.
<32> 	The height of the picture in pixels.
<32> 	The color depth of the picture in bits-per-pixel.
<32> 	For indexed-color pictures (e.g. GIF), the number of colors used, or 0 for non-indexed pictures.
<32> 	The length of the picture data in bytes.
<n*8> 	The binary picture data.  
*/

CFlacCover::CFlacCover(void)
{
	type = 0;
	width = 0;
	height = 0;
	colordepth = 0;
	colornumbers = 0;
	_isLink = false;
}

CFlacCover::CFlacCover(CBlob *tmpData)
{
	type = tmpData->Get4B(0);
	size_t ln = tmpData->Get4B(4); // Length
	int start = 8;
	int dummy = 0;
	_mime = tmpData->GetStringAt(start, ln); // ASCII Mime
	start+=(int)ln;
	ln = tmpData->Get4B(start); // Length
	start+=4;
	data.Clear();
	data.AddMemory(tmpData->m_pData + start, ln);
	description = data.getNextString(TEXT_ENCODED_UTF8, dummy);
	start+=(int)ln;
	width = tmpData->Get4B(start); // width
	height = tmpData->Get4B(start + 4); // height
	colordepth = tmpData->Get4B(start + 8); // color depth
	colornumbers = tmpData->Get4B(start + 12); // numbers of colors
	ln = tmpData->Get4B(start + 16); //picture length
	data.Clear();
	start+=20;
	// Falls Mime --> ist , dann Bild aus Datei lesen
	if (_mime.Compare(_T("-->")) == 0)
	{
		_isLink = true;
		_pictureLink = tmpData->getNextString(TEXT_ENCODED_ANSI, start);
		FILE *tmpSource;
		if ( (tmpSource = _wfsopen(_pictureLink, READ_ONLY, _SH_DENYNO)) != NULL)
		{
			data.FileRead(_filelength(_fileno(tmpSource)), tmpSource);
			fclose(tmpSource);
		}
		else
			CTools::instance().setLastError(ERR_IMAGEURL_NOT_FOUND, _pictureLink);

	}
	else
	{
		_isLink = false;
		data.AddMemory(tmpData->m_pData + start, ln);
	}
}

CFlacCover::CFlacCover(BYTE picType, LPCWSTR Description)
{
	type = picType;
	description = Description;
	width = 0;
	height = 0;
	colordepth = 0;
	colornumbers = 0;
}


CFlacCover::~CFlacCover(void)
{

}

bool CFlacCover::getPictureFile(LPCWSTR fileName)
{
	FILE *Stream;
	ATLTRACE(_T("open %s WRITE_ONLY\n"), fileName);
	if ( (Stream = _wfsopen(fileName, WRITE_ONLY, _SH_DENYNO)) != NULL)
	{
		data.FileWrite(data.GetLength(), Stream);	  
		fflush(Stream);
		fclose(Stream);
		return true;
	}
	CTools::instance().setLastError(errno);
	return false;
}

CAtlString CFlacCover::getMime()
{
	return (_isLink) ?  MIME_LINK + _pictureLink : _mime;	
}

long CFlacCover::getPictureArray(BYTE *arr, size_t maxLen)
{
	size_t ln = data.GetLength();
	if (ln > maxLen)
	{
		CTools::instance().setLastError(ERR_NOT_ENOUGH_MEMORY);
		return -1;
	}
	if (ln > 0)
		memcpy(arr, data.m_pData, ln);
	return (long)ln;
}

long CFlacCover::getPictureSize()
{
	return (long)data.GetLength();
}

CAtlString CFlacCover::getPictureTypeAsText()
{
	if (type >= 0 && type <= 20)
		return APIC_PICTURE_TYPES[type];

	CAtlString dummy;	
	dummy.Format(_T("unknown PictureType %i"), type);	
	return dummy;
}

bool CFlacCover::setPictureFile(LPCWSTR fileName)
{
	FILE *Stream;
	ATLTRACE(_T("open %s READ_ONLY\n"), fileName);
	data.Clear();
	_isLink = false;
	_pictureLink.Empty();
	if ( (Stream = _wfsopen(fileName, READ_ONLY, _SH_DENYNO)) != NULL)
	{
		data.FileRead(_filelength(_fileno(Stream)), Stream); 
		fclose(Stream);
		// MIME neu bilden
		_mime = CTools::instance().ExtractMimeFromPicture(data.m_pData);
		calcInfos();
		return true;
	}
	CTools::instance().setLastError(errno);
	return false;
}

bool CFlacCover::setFileLink(LPCWSTR fileName)
{
	FILE *Stream;
	data.Clear();
	_isLink = true;
	_pictureLink.Empty();
	if (fileName != NULL && wcslen(fileName) > 0)
	{
		_pictureLink = fileName;
		_mime = MIME_LINK;
		if ( (Stream = _wfsopen(fileName, READ_ONLY, _SH_DENYNO)) != NULL)
		{
			data.FileRead(_filelength(_fileno(Stream)), Stream); 
			fclose(Stream);
		}
		calcInfos();
		return true;
	}	
	CTools::instance().setLastError(ERR_INVALID_FILENAME);
	return false;
}

void CFlacCover::calcInfos()
{
	width = 0;
	height = 0;
	colordepth = 24;
	colornumbers = 0;
	// ermittle aus Bild die Grösse und Color depth
	if (data.GetLength() > 4)
	{
		int format = CTools::instance().CalcMimeFromPicture(data.m_pData);
		switch (format)
		{
		case IMAGE_JPG: // ok
			{
				//Retrieve the block length of the first block since the first block will not contain the size of file
				size_t i = 4;
				size_t block_length = data.Get2B(i);
				while (i < data.GetLength()) 
				{
					i+=block_length;               //Increase the file index to get to the next block
					if(i >= data.GetLength()) 
						break;   //Check to protect against segmentation faults
					if(data.GetAt(i) != 0xFF) 
						break;   //Check that we are truly at the start of another block
					if(data.GetAt(i + 1) == 0xC0) 
					{    //0xFFC0 is the "Start of frame" marker which contains the file size
						//The structure of the 0xFFC0 block is quite simple [0xFFC0][ushort length][uchar precision][ushort x][ushort y]
						height = data.Get2B(i+5);
						width = data.Get2B(i+7);
						colordepth = data.GetAt(i + 9) * 8;
						break;
					}
					else
					{
						i+=2;                              //Skip the block marker
						block_length = data.Get2B(i);   //Go to the next block
					}
				}
			}
			break;	//If this point is reached then no size was found
		case IMAGE_GIF:
			if (data.GetLength() >= 10 )
			{
				height = data.GetR2B(6);
				width = data.GetR2B(8);	
			}
			break;
		case IMAGE_PNG:
			if(data.GetLength() >= 29 )
			{
				height = data.Get4B(16);
				width = data.Get4B(20);	
			}
			break;
		case IMAGE_TIFF:
			if (data.GetLength() > 8)
			{
				long offset = data.GetR4B(4);
				if (offset > 8 && offset < (long)data.GetLength())
				{
					short anz = data.GetR2B(offset);
					offset+=2;
					while (anz > 0 && offset < (long)data.GetLength())
					{
						short tagcode = data.GetR2B(offset);
						// short tagtype = data.GetR2B(offset + 2);
						if (tagcode == 256)
							width = data.GetR2B(offset + 8);
						else if (tagcode == 257)
							height = data.GetR2B(offset + 8);
						//else if (tagcode == 258)
						//	colordepth = data.GetR2B(offset + 8); 
						offset+=12;
						anz--;
					}
				}
			}
			break;
		case IMAGE_BMP: // ok
			if(data.GetLength() >= 29 )
			{
				height = data.GetR4B(18);
				width = data.GetR4B(22);
				colordepth = data.GetR2B(28);
			}
			break;

		}
	}
}

void CFlacCover::Save(CBlob * destination)
{
	CBlob tmp;
	destination->Add4B(type);
	tmp.AddEncodedString(TEXT_ENCODED_ANSI, _mime, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
	destination->Add4B((unsigned int)tmp.GetLength());
	destination->AddBlob(tmp);
	tmp.Clear();
	tmp.AddEncodedString(TEXT_ENCODED_UTF8, description, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
	destination->Add4B((unsigned int)tmp.GetLength());
	destination->AddBlob(tmp);
	destination->Add4B(width);
	destination->Add4B(height);
	destination->Add4B(colordepth);
	destination->Add4B(colornumbers);
	if (_isLink)
	{
		tmp.Clear();
		tmp.AddEncodedString(TEXT_ENCODED_UTF8, _pictureLink, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
		destination->Add4B((unsigned int)tmp.GetLength());
		destination->AddBlob(tmp);
	}
	else
	{
		destination->Add4B((unsigned int)data.GetLength());
		destination->AddBlob(data);
	}
}