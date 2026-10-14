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
#include "WMA_File.h"
#include "share.h"
#include "io.h"

CWMA_File::CWMA_File(void)
{	
	_valid = false;
}

CWMA_File::~CWMA_File(void)
{
	ResetData();
}

bool CWMA_File::ReadFromFile(FILE *Stream)
{
	ResetData();
	/* Read file data */
	/* Check for existing header */
	fseek(Stream, CTools::ID3v2Size, SEEK_SET);
	if (header.load(Stream, CTools::FileSize))
	{
		_valid = true;
		DataPosition = ftell(Stream); 
		return true;
	}
	_valid = false;
	return false;
}

CAtlString CWMA_File::GetUserItem(CAtlString key)
{
	//Suche zuerst in normalen Content und dann erst in ExtContent
	CWMA_Object *obj = header.findObject(WMA_CONTENT_DESCRIPTION_ID);
	if (obj != NULL)
	{
		CWMA_ContentDescription *cd = static_cast<CWMA_ContentDescription*>(obj);
		// Probieren wir es mal damit
		if (key.CompareNoCase(WM_TITLE) == 0 || key.CompareNoCase(WM_TITLE2) == 0)
			return cd->GetTitle();
		if (key.CompareNoCase(WM_AUTHOR) == 0 || key.CompareNoCase(WM_AUTHOR2) == 0)
			return cd->GetAuthor();
		if (key.CompareNoCase(WM_DESCRIPTION) == 0 || key.CompareNoCase(WM_DESCRIPTION2) == 0)
			return cd->GetComment();
		if (key.CompareNoCase(WM_PROVIDERRATING) == 0 || key.CompareNoCase(WM_PROVIDERRATING2) == 0)
			return cd->GetRating();
		if (key.CompareNoCase(WM_COPYRIGHT) == 0 || key.CompareNoCase(WM_COPYRIGHT2) == 0)
			return cd->GetCopyright();
	}

	// Suche nun im ExtContentDescription Object, wenn es da ist
	int pos = CWMA_ObjectFactory::instance().FindField(key);
	if (pos != -1)
		return CWMA_ObjectFactory::instance().GetValue(pos);
	// Nix gefunden
	return EMPTY;
}

void CWMA_File::SetUserItem(LPCWSTR key, LPCWSTR item)
{
	// Wohin schreiben, Standard oder ExtContent?	
	CAtlString tmp(key);
	int art = 0;
	if (tmp.CompareNoCase(WM_TITLE) == 0 || tmp.CompareNoCase(WM_TITLE2) == 0)
		art = 1;
	if (tmp.CompareNoCase(WM_AUTHOR) == 0 || tmp.CompareNoCase(WM_AUTHOR2) == 0)
		art = 2;
	if (tmp.CompareNoCase(WM_DESCRIPTION) == 0 || tmp.CompareNoCase(WM_DESCRIPTION2) == 0)
		art = 3;
	if (tmp.CompareNoCase(WM_PROVIDERRATING) == 0 || tmp.CompareNoCase(WM_PROVIDERRATING2) == 0)
		art = 4;
	if (tmp.CompareNoCase(WM_COPYRIGHT) == 0 || tmp.CompareNoCase(WM_COPYRIGHT2) == 0)
		art = 5;

	if (art > 0)
	{
		// Schreibe Werte in ContentDescription
		CWMA_Object *obj = header.findObject(WMA_CONTENT_DESCRIPTION_ID);
		CWMA_ContentDescription *cd;
		if (obj == NULL)
		{
			obj = CWMA_ObjectFactory::instance().createObject(WMA_CONTENT_DESCRIPTION_ID);
			header.insertObject(obj, 0);
		}
		cd = static_cast<CWMA_ContentDescription*>(obj);
		switch (art)
		{
		case 1:
			return cd->SetTitle(item);
		case 2:
			return cd->SetAuthor(item);
		case 3:
			return cd->SetComment(item);
		case 4:
			return cd->SetRating(item);
		case 5:
			return cd->SetCopyright(item);
		}
	}
	// Schreibe in ExtContentDescription
	CWMA_ObjectFactory::instance().SetValue(key, item);

}

// 4.0.4.7: the size of an ID3v2 tag in front of the file, by the same rule the analysis uses (CID3V2::ReadHeader),
// read from the file being saved instead of the globals of the last analyzed file
static __int64 WMAReadID3v2Size(FILE *Stream, __int64 fileLen)
{
	BYTE buf[10];
	if (_fseeki64(Stream, 0, SEEK_SET) != 0 || fread(buf, 1, 10, Stream) != 10)
		return 0;
	if (buf[0] != 'I' || buf[1] != 'D' || buf[2] != '3' || buf[3] == 0xFF || buf[4] == 0xFF)
		return 0;
	__int64 size = ((__int64)buf[6] << 21) + (buf[7] << 14) + (buf[8] << 7) + buf[9] + 10;
	if (buf[3] == 4 && (buf[5] & 0x10) == 0x10)
		size += 10; // v2.4 footer (an extended header is inside the size)
	return (size > fileLen) ? 0 : size;
}

static bool WMACopyBytes(FILE *Source, FILE *Destination, __int64 count)
{
	static const size_t blockSize = 1024 * 1024;
	CBlob buffer;
	try
	{
		while (count > 0)
		{
			size_t want = count > (__int64)blockSize ? blockSize : (size_t)count;
			buffer.FileRead(want, Source);
			if (buffer.GetLength() != want || fwrite(buffer.m_pData, 1, want, Destination) != want)
				return false;
			count -= want;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool CWMA_File::SaveToFile(LPCWSTR FileName)
{
	// 4.0.4.7: the header is read again from this file (its own ID3v2 size and length, not the globals of the last
	// analyzed file) and nothing is written unless it was read whole; a rebuild keeps an ID3v2 tag in front, checks
	// every read and write and replaces the original in one step. Up to 4.0.4.6 a header read short was written
	// back in place, a failed temp file returned true and left the file open, _fcloseall closed every stream of
	// the process, the original was deleted before the rename, and an ID3v2 tag in front was dropped.
	FILE* Stream;
	errno = 0;
	if ( (Stream = _wfsopen(FileName, READ_AND_WRITE, _SH_DENYWR)) == NULL)
	{
		CTools::instance().setLastError(errno);
		return false;
	}

	__int64 fileLen = _filelengthi64(_fileno(Stream));
	__int64 id3v2Size = WMAReadID3v2Size(Stream, fileLen);
	_fseeki64(Stream, id3v2Size, SEEK_SET);
	__int64 oldHeaderSize = header.loadHeaderOnly(Stream);
	if (oldHeaderSize <= 0 || oldHeaderSize > 0x7FFFFFFF || id3v2Size + oldHeaderSize > fileLen)
	{
		CTools::instance().setLastError(ERR_WMA_PARSE);
		fclose(Stream);
		return false;
	}
	__int64 DataPosition = id3v2Size + oldHeaderSize;
	CWMA_Object *obj = NULL;
	// Rette die aktuellen Objekte, falls vorhanden
	CWMA_MetadataLibrary *newMetas = NULL;
	CWMA_ContentDescription *newContent = NULL;
	CWMA_ExtContentDescription *newExtContent = NULL;
	bool dropExtContent = false;
	if ((obj = header.findObject(WMA_CONTENT_DESCRIPTION_ID)) != NULL)
	{
		if (obj->getData()->GetLength() > 0)
			newContent = new CWMA_ContentDescription(obj->getData());
	}
	if ((obj = header.findObject(WMA_EXTENDED_CONTENT_DESCRIPTION_ID)) != NULL)
	{
		if (obj->getData()->GetLength() > 0)
			newExtContent = new CWMA_ExtContentDescription(obj->getData());
		else
			dropExtContent = true; // 4.0.4.7: every field cleared; reloading the header brought the old ones back
	}
	else
	{
		// Was ist, wenn ExtContent nicht exisitiert, aber neue Einträge hinzugekommen sind ?
		newExtContent = new CWMA_ExtContentDescription();
		newExtContent->buildData();
		if (newExtContent->getData()->GetLength() == 0)
		{
			// war nix neues da
			delete newExtContent;
			newExtContent = NULL;
		}
	}
	if ((obj = header.findObject(WMA_HEADER_EXTENSION_ID)) != NULL)
	{
		CWMA_Header_Extension* he = static_cast<CWMA_Header_Extension*>(obj);
		if ((obj = he->findObject(WMA_METADATA_LIBRARY_ID)) != NULL)
		{
			// Ist überhaupt was drin ?
			if (obj->getData()->GetLength() > 0)
				newMetas = new CWMA_MetadataLibrary(obj->getData());

		}
		else
		{
			// Was ist, wenn metadata nicht existiert, aber neue Einträge hinzugekommen sind ?
			newMetas = new CWMA_MetadataLibrary();
			newMetas->buildData();
			if (newMetas->getData()->GetLength() == 0)
			{
				delete newMetas;
				newMetas = NULL;				
			}
		}
	}

	ResetData();
	_fseeki64(Stream, id3v2Size, SEEK_SET);
	__int64 maxHeader = fileLen - id3v2Size;
	if (maxHeader > 0x7FFFFFFF)
		maxHeader = 0x7FFFFFFF;
	if (!header.load(Stream, (size_t)maxHeader) || !header.isComplete())
	{
		delete newMetas;
		delete newContent;
		delete newExtContent;
		CTools::instance().setLastError(ERR_WMA_PARSE);
		fclose(Stream);
		return false;
	}
	// Alle Paddings rausschmeissen
	size_t oldPadding = 0;
	while ((obj = header.findObject(WMA_PADDING_ID)) != NULL)
	{
		oldPadding += obj->getDataSize();
		header.deleteObject(WMA_PADDING_ID);
	}
	obj = header.findObject(WMA_HEADER_EXTENSION_ID);
	if (obj != NULL)
	{
		CWMA_Header_Extension* he = static_cast<CWMA_Header_Extension*>(obj);
		while ((obj = he->findObject(WMA_PADDING_ID)) != NULL)
		{
			oldPadding += obj->getDataSize();
			he->deleteObject(WMA_PADDING_ID);
		}
		if (newMetas != NULL)
		{
			he->replaceObject(newMetas);
		}
		else
			he->deleteObject(WMA_METADATA_LIBRARY_ID);
	}
	else
		delete newMetas; // (nowhere to put it; it leaked)
	// Neue Elemente ersetzen
	if (newContent != NULL)
		header.replaceObject(newContent);
	if (newExtContent != NULL)
		header.replaceObject(newExtContent);
	else if (dropExtContent)
		header.deleteObject(WMA_EXTENDED_CONTENT_DESCRIPTION_ID);

	size_t blockSize = CTools::configValues[CONFIG_ID3V2WRITEBLOCKSIZE];
	CBlob newHeader(blockSize);
	header.save(&newHeader);
	CWMA_Padding *newPadding = NULL;
	long paddingBlockSize = CTools::configValues[CONFIG_WMAPADDINGSIZE];
	if ((__int64)newHeader.GetLength() + 26 > oldHeaderSize) // 24 Bytes brauch das Padding-Object für sich und mind. 1 Bytes Padding
	{   
		ATLTRACE(L"rebuild file...\n");
		// Rebuild
		// Padding erwünscht ?
		if (blockSize > 0)
		{
			newPadding = new CWMA_Padding(paddingBlockSize);
			header.replaceObject(newPadding);
		}
		else
			header.deleteObject(WMA_PADDING_ID);

		newHeader.Clear();
		header.save(&newHeader);
		{
			// 4.0.4.9: the File Properties object's File Size (offset 40 in the object) is the new ASF size; it kept
			// the old one after a rebuild (4.0.4.8 and earlier)
			static const BYTE FILE_PROPERTIES_GUID[16] = { 0xA1, 0xDC, 0xAB, 0x8C, 0x47, 0xA9, 0xCF, 0x11, 0x8E, 0xE4, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65 };
			size_t len = newHeader.GetLength();
			for (size_t pos = 30; pos + 48 <= len; pos++)
			{
				if (memcmp(newHeader.m_pData + pos, FILE_PROPERTIES_GUID, 16) == 0)
				{
					unsigned __int64 asfSize = (unsigned __int64)(fileLen - DataPosition) + len;
					for (int b = 0; b < 8; b++)
						newHeader.m_pData[pos + 40 + b] = (BYTE)(asfSize >> (8 * b));
					break;
				}
			}
		}
		FILE *Destination;
		CAtlString NewFileName(FileName);
		NewFileName+=TILDE;
		if ( (Destination = _wfsopen(NewFileName, WRITE_ONLY, _SH_DENYWR)) == NULL)
		{
			CTools::instance().setLastError(errno);
			fclose(Stream);
			return false;
		}
		// The ID3v2 tag in front, the new header, then the rest of the file
		bool copied = true;
		if (id3v2Size > 0)
			copied = (_fseeki64(Stream, 0, SEEK_SET) == 0 && WMACopyBytes(Stream, Destination, id3v2Size));
		if (copied && newHeader.FileWrite(newHeader.GetLength(), Destination) != newHeader.GetLength())
			copied = false;
		if (copied)
			copied = (_fseeki64(Stream, DataPosition, SEEK_SET) == 0 && WMACopyBytes(Stream, Destination, fileLen - DataPosition));
		// Only this save's own streams: _fcloseall closed every stream the process had open
		if (fflush(Destination) != 0 || ferror(Destination))
			copied = false;
		if (fclose(Destination) != 0)
			copied = false;
		fclose(Stream);
		if (!copied)
		{
			_wremove(NewFileName); // the original is untouched
			CTools::instance().setLastError(errno != 0 ? errno : EIO);
			return false;
		}
		CTools::instance().doEventsNow();
		/* Replace the original with the copy in one step: it is either the old file or the new one, never missing */
		if (!MoveFileExW(NewFileName, FileName, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
		{
			DWORD err = GetLastError();
			_wremove(NewFileName);
			CTools::instance().setLastError(err == ERROR_ACCESS_DENIED || err == ERROR_SHARING_VIOLATION ? EACCES : EIO);
			return false;
		}
	}
	else
	{
		ATLTRACE(L"rewrite file...\n");
		// Passendes Padding einfügen
		u32 newPaddingSize = (u32)(oldHeaderSize - newHeader.GetLength() - 24);
		if (newPaddingSize > 0)
		{
			newPadding = new CWMA_Padding(newPaddingSize);
			header.replaceObject(newPadding);
		}
		newHeader.Clear();
		header.save(&newHeader);
		bool written = (_fseeki64(Stream, id3v2Size, SEEK_SET) == 0
			&& newHeader.FileWrite(newHeader.GetLength(), Stream) == newHeader.GetLength());
		if (fflush(Stream) != 0 || ferror(Stream))
			written = false;
		if (fclose(Stream) != 0)
			written = false;
		if (!written)
		{
			CTools::instance().setLastError(errno != 0 ? errno : EIO);
			return false;
		}
	}
	return true;
}

bool CWMA_File::DeletePictureFrame(short index)
{
	int found = CWMA_ObjectFactory::instance().FindPicture(index);
	if (found != -1)
	{
		CWMA_ObjectFactory::instance().DeleteField(found);
		return true;
	}
	return false;
}

void CWMA_File::DeletePictures()
{	
	for (int index = (int)CWMA_ObjectFactory::instance().tagdatas.GetCount() - 1; index >= 0; index--)
	{
		if (CWMA_ObjectFactory::instance().tagdatas.GetAt(index)->IsPicture() )
			CWMA_ObjectFactory::instance().DeleteField(index);
	}
}

int CWMA_File::GetPictures()
{
	int count = 0;
	size_t counts = CWMA_ObjectFactory::instance().tagdatas.GetCount();
	for (size_t i = 0; i < counts; i++)
	{	
		if (CWMA_ObjectFactory::instance().tagdatas.GetAt(i)->IsPicture() )
			count++;
	}
	return count;
}

CAtlString CWMA_File::GetPictureDescription(int index)
{
	TagPictureStruct *tps = getPictureTag(index);
	return (tps == NULL) ? EMPTY : tps->Description;
}

TagPictureStruct* CWMA_File::getPictureTag(int index)
{
	int result = -1;
	result = CWMA_ObjectFactory::instance().FindPicture(index);
	if (result != -1)
	{
		CBlob *dt = CWMA_ObjectFactory::instance().tagdatas[result]->getData();
		return CWMA_ObjectFactory::instance().ExtractPicture(dt);			
	}	
	return NULL;
}

int CWMA_File::GetPictureType(int index)
{
	TagPictureStruct *tps = getPictureTag(index);
	return (tps == NULL) ? 0 : tps->Type;
}

long CWMA_File::GetPictureSize(int index)
{
	TagPictureStruct *tps = getPictureTag(index);
	return (tps == NULL) ? 0 : tps->PicSize;
}

bool CWMA_File::GetPicture(int index, LPCWSTR fileName)
{
	TagPictureStruct *tps = getPictureTag(index);
	if (tps == NULL)
		return false;
	FILE *Stream;
	ATLTRACE(_T("open %s WRITE_ONLY\n"), fileName);
	if ( (Stream = _wfsopen(fileName, WRITE_ONLY, _SH_DENYNO)) != NULL)
	{
		fwrite(tps->PicDaten, 1, tps->PicSize, Stream);
		fflush(Stream);
		fclose(Stream);
		return true;
	}
	CTools::instance().setLastError(errno);
	return false;
}

size_t CWMA_File::GetPictureArray(BYTE *arr, u32 maxLen, short index)
{
	TagPictureStruct *tps = getPictureTag(index);
	if (tps == NULL)
		return 0;
	if (tps->PicSize > maxLen)
	{
		CTools::instance().setLastError(ERR_NOT_ENOUGH_MEMORY);
		return 0;
	}
	memcpy(arr, tps->PicDaten, tps->PicSize);
	return tps->PicSize;	
}
CAtlString CWMA_File::GetPictureMime(int index)
{
	TagPictureStruct *tps = getPictureTag(index);
	return (tps == NULL) ? EMPTY : tps->Mime;
}

CAtlString CWMA_File::GetItemKeys()
{
	return CWMA_ObjectFactory::instance().GetItemKeys();
}

bool CWMA_File::SetPicture(LPCWSTR FileName, LPCWSTR Description, short Index, short PictureType)
{
	if (FileName == NULL || wcslen(FileName) == 0)
	{
		CTools::instance().setLastError(errno);
		return false;
	}
	FILE *Stream;
	long ln = 0;
	if ( (Stream = _wfsopen(FileName, READ_ONLY, _SH_DENYNO)) != NULL)
	{
		ln = _filelength(_fileno(Stream));
		CBlob tmp(ln);
		tmp.FileRead(ln, Stream);
		fclose(Stream);

		return SetPictureArray(tmp.m_pData, ln, Description, Index, PictureType);
	}
	CTools::instance().setLastError(errno);
	return false;
}
bool CWMA_File::SetPictureArray(BYTE *arr, u32 len, LPCWSTR Description, short index, short picType)
{
	if (index < 1)
	{
		CTools::instance().setLastError(ERR_ID_OUT_OF_RANGE);
		return false;
	}
	if (len < 4)
	{
		CTools::instance().setLastError(ERR_PICTUREARRAY_TOO_SMALL);
		return false;
	}
	// Neues Bild anlegen, wenn kleiner als 65500 in ExtContentDescription, sonst in MetadataLibrary
	CWMA_TagData *item;
	if (len < 65500)
		item = new CWMA_TagData(EXTCONTENT_ART);
	else
		item = new CWMA_TagData(METADATA_ART);
	item->setNewPicture(arr, len, Description, (BYTE)picType);

	int it = CWMA_ObjectFactory::instance().FindPicture(index);
	if (it != -1) // Bestehendes Bild ersetzen
	{
		delete CWMA_ObjectFactory::instance().tagdatas.GetAt(it);
		CWMA_ObjectFactory::instance().tagdatas.SetAt(it, item);
	}
	else
	{
		CWMA_ObjectFactory::instance().tagdatas.Add(item);
	}
	return true;
}

void CWMA_File::ResetData()
{
	header.Remove();
	DataPosition = 0;	
}

bool CWMA_File::IsVBR()
{
	int pos = CWMA_ObjectFactory::instance().FindField(_T("IsVBR"));
	if (pos != -1)
		return (CWMA_ObjectFactory::instance().GetValue(pos).Compare(_T("true")) == 0);
	return false;
}

long CWMA_File::GetBitRate()
{
	CWMA_Object *obj = header.findObject(WMA_FILE_PROPERTIES_ID);
	if (obj == NULL)
		return 0;
	CWMA_FileProperties *fd = static_cast<CWMA_FileProperties*>(obj);
	return fd->getBitRate();
}

long CWMA_File::GetChannels()
{
	CWMA_Object *obj = header.findObject(WMA_STREAM_PROPERTIES_ID);
	if (obj == NULL)
		return 0;
	CWMA_StreamProperties *sp = static_cast<CWMA_StreamProperties*>(obj);
	return sp->getNumberOfChannels();	
}

float CWMA_File::GetDuration()
{
	CWMA_Object *obj = header.findObject(WMA_FILE_PROPERTIES_ID);
	if (obj == NULL)
		return 0;
	CWMA_FileProperties *fd = static_cast<CWMA_FileProperties*>(obj);
	return fd->getDuration();	
}

long CWMA_File::GetSampleRate()
{ 
	CWMA_Object *obj = header.findObject(WMA_STREAM_PROPERTIES_ID);
	if (obj == NULL)
		return 0;
	CWMA_StreamProperties *sp = static_cast<CWMA_StreamProperties*>(obj);
	return sp->getSamplesPerSecond();
}

bool CWMA_File::IsValid()
{
	return _valid;
}
