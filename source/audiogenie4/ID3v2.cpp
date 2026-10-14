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
#include "ID3v2.h"
#include "id3_framefactory.h"
#include "Tools.h"
#include "id3v1.h"
#include "io.h"


CID3V2::CID3V2(void)
{
	_frames.SetCount(0, 10);	
}

CID3V2::~CID3V2(void)
{
	ResetData();
}

short CID3V2::getEncoding(u32 FrameID)
{
	size_t counts = _frames.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		if (_frames[i]->_frameID == FrameID)
		{
			_frames[i]->decode();
			return _frames[i]->encodingID;
		}
	}
	return -1;
}

bool CID3V2::setTargetFormatAndEncoding(BYTE newFormat, BYTE newEncoding)
{
	// 4.0.4.7: out of range values are refused. Format 4 made version 5, read past the frame table and wrote a tag no
	// reader accepts; UTF-16BE (2) was allowed for v2.2/v2.3, which don't have it (4.0.4.6 and earlier).
	if (newFormat > 3 || newEncoding > 3)
	{
		CTools::instance().setLastError(ERR_ID_OUT_OF_RANGE);
		return false;
	}
	if (newFormat > 0)
		newFormat++;
	if (newFormat != TAG_VERSION_2_4 && (newEncoding == TEXT_ENCODED_UTF8 || newEncoding == TEXT_ENCODED_UTF16))
	{
		CTools::instance().setLastError(ERR_UTF8_NOT_ALLOWED); 
		return false;
	}
	// 0 = nimm default Tagformat
	if (newFormat != 0)
	{
		CTools::ID3V2newTagVersion = newFormat;
		CTools::ID3V2newTagDefaultVersion = newFormat; // also the version of tags created later
	}
	CTools::ID3V2defaultEncodingID = newEncoding;
	// The caller's choice applies to the saves of the file now loaded (until the next file is analyzed)
	CTools::ID3V2formatChosen = true;
	return true;
}

/*
* 4.0.1: the format and encoding of a save. Unless ID3V2SetFormatAndEncoding chose them after the file was analyzed,
* the tag is saved in the version it was read with (a file without a tag: the last explicit format, v2.3 by
* default) and in an encoding that can hold any text: UTF-8 for v2.4, UTF-16 with BOM for v2.2/v2.3.
*
* Up to 4.0 the format and encoding were process-wide and kept from save to save, starting at v2.3 with
* ISO-8859-1. Saving a v2.4 tag made it v2.3 and dropped its v2.4-only frames (TDRC, the date), every text frame was
* re-encoded as ISO-8859-1 (non-Latin text, e.g. Hebrew, became "?"), and a format set for one file was used for
* every later save of any file.
*/
void CID3V2::chooseSaveFormat()
{
	if (CTools::ID3V2formatChosen)
		return;
	BYTE version = CTools::ID3V2oldTagVersion;
	if (version < TAG_VERSION_2_2 || version > TAG_VERSION_2_4)
		version = CTools::ID3V2newTagDefaultVersion;
	CTools::ID3V2newTagVersion = version;
	CTools::ID3V2defaultEncodingID = (version == TAG_VERSION_2_4) ? TEXT_ENCODED_UTF8 : TEXT_ENCODED_UTF16BOM;
}

bool CID3V2::ReadHeader(FILE *Stream)
{
	// An ID3v2 tag can be detected with the following pattern:
	// $49 44 33 yy yy xx zz zz zz zz
	// Where yy is less than $FF, xx is the 'flags' byte and zz is less than $80.
	memset(buf, 0, 10);
	fseek(Stream, 0, SEEK_SET);
	fread(buf, 1, 10, Stream);	
	Size = 0;
	if (buf[0] == 'I' && buf[1] == 'D' && buf[2] == '3' && buf[3] < 0xFF && buf[4] < 0xFF)
	{
		Version = buf[3];
		Revision = buf[4];
		CTools::ID3V2Flags = buf[5];
		Size = (buf[6] << 21) + ( buf[7] << 14) + (buf[8] << 7) + buf[9] + 10;
		// 4.0.4.7: the size already includes an extended header (parseTags skips it); a v2.4 footer is not included.
		// 10 bytes were added for an extended header, and the footer was left in front of the audio (4.0.4.6 and earlier).
		if (buf[3] == TAG_VERSION_2_4 && (CTools::ID3V2Flags & 0x10) == 0x10)
			Size+= 10; // Footer present
		if (Size > CTools::FileSize)
		{
			CTools::instance().writeError(L"ID3V2 tag is corrupt, because the ID3V2 size is bigger than filesize!");
			Size = 0;			
		}
		buf[3] = 0; buf[4] = 0;
		ID = (buf[0] << 16) + (buf[1] << 8) + buf[2]; 
		return true;
	}
	return false;
}

void CID3V2::ResetData()
{
	Version = 0;
	Revision = 0;
	CTools::ID3V2Flags = 0;
	Size = 0;
	ID = 0;
	size_t cnt = _frames.GetCount();
	if (cnt > 0)
	{
		for (size_t i = 0; i < cnt; i++)
			delete _frames.GetAt(i);	
	}
	_frames.RemoveAll();
}

void CID3V2::WriteHeader(CBlob *blob, long tagSize)
{
	blob->AddValue('I');
	blob->AddValue('D');
	blob->AddValue('3');
	blob->AddValue(CTools::ID3V2newTagVersion);
	blob->AddValue(0);
	//if ((Flags & 128) == 128)
	//	blob->AddValue(128); // Flags auf 0 setzen, kein Extended Header, kein Footer, aber Unsynchronized Flag gesetzt
	//else
	blob->AddValue(0); // Flags auf 0 setzen, kein Extended Header, kein Footer, kein Unsynchronized
	blob->AddS4B(tagSize); // SyncSafe Integer	
}

void CID3V2::ReadFromFile(FILE *Stream)
{
	CTools::ID3V2oldTagVersion = 0;
	CTools::ID3V2formatChosen = false; // a new file: its own tag version applies again (chooseSaveFormat)
	size_t count = _frames.GetCount();
	for (size_t i = 0; i < count; i++)
		delete _frames.GetAt(i);
	_frames.RemoveAll();
	CTools::instance().writeDebug(_T("Analyze id3v2 tag"));
	ReadHeader(Stream);	
	oldTagSize = Size;
	CTools::ID3v2Size = Size;
	if (Size > 0 && ID == ID3V2_TAGID && Version >= TAG_VERSION_2_2 && Version <= TAG_VERSION_2_4 )
	{
		CTools::instance().writeDebug(_T("id3v2.%i tag found"), Version);
		CTools::ID3V2oldTagVersion = Version;
		CBlob data(Size);
		data.FileRead(Size - 10, Stream);
		if (Version < TAG_VERSION_2_4 && (CTools::ID3V2Flags & 0x80) == 0x80 && data.GetLength() > 0)
		{
			// 4.0.4.7: v2.2/v2.3 unsynchronisation covers the whole tag, and the frame sizes count the resynced bytes.
			// Each frame was cut at its size from the unsynchronised data, then resynced: every FF 00 before its end
			// lost a byte of the frame (4.0.4.6 and earlier).
			CBlob resynced(data.GetLength());
			const BYTE *p = data.m_pData;
			size_t n = data.GetLength();
			for (size_t i = 0; i < n; i++)
			{
				resynced.AddValue(p[i]);
				if (p[i] == 0xFF && i + 1 < n && p[i + 1] == 0x00)
					i++;
			}
			CTools::ID3V2Flags &= ~0x80; // the frames are resynced already
			parseTags(&resynced);
		}
		else
			parseTags(&data);		
	}
	else
		CTools::instance().writeDebug(_T("no id3v2 tag present"));
}

void CID3V2::parseTags(CBlob* data)
{
	u32 DataPosition = 0;
	int headerSize = ( Version == TAG_VERSION_2_2) ? 6 : 10;
	u32 dataSize = (u32)data->GetLength();
	// 4.0.4.7: the frames start after the extended header and end before a v2.4 footer. The extended header was
	// parsed as frames (4.0.4.6 and earlier).
	if (Version == TAG_VERSION_2_4 && (CTools::ID3V2Flags & 0x10) == 0x10)
		dataSize = (dataSize > 10) ? dataSize - 10 : 0;
	if (Version >= TAG_VERSION_2_3 && (CTools::ID3V2Flags & 0x40) == 0x40 && dataSize >= 4)
	{
		// v2.3: size without its own 4 bytes; v2.4: syncsafe size of the whole extended header
		u32 extSize = (Version == TAG_VERSION_2_3) ? (u32)data->Get4B(0) : (u32)data->GetS4B(0);
		if (Version == TAG_VERSION_2_3 && extSize < dataSize)
			extSize+= 4;
		DataPosition = (extSize > dataSize) ? dataSize : extSize;
	}
	while ((DataPosition + headerSize) < dataSize)
	{
		ATLTRACE(_T("Reading at Pos:%d \n"), DataPosition);
		if (Version == TAG_VERSION_2_2)
		{
			FrameID = data->Get3B(DataPosition);
			FrameSize = data->Get3B(DataPosition + 3);
			FrameFlags = 0;
			isValid = ( ALLOWED[data->GetAt(DataPosition)] && ALLOWED[data->GetAt(DataPosition+1)] && ALLOWED[data->GetAt(DataPosition+2)]);			
		}
		else
		{
			FrameID = data->Get4B(DataPosition);
			//fread(buf, 1, 10, Stream);
			if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_4) // SyncSafe Integer
				FrameSize = data->GetS4B(DataPosition + 4);
			else // Unsync Integer
				FrameSize = data->Get4B(DataPosition + 4);
			FrameFlags = data->Get2B(DataPosition + 8);
			// is Data Length Indicator set?

			isValid = ( ALLOWED[data->GetAt(DataPosition)] && ALLOWED[data->GetAt(DataPosition+1)] && ALLOWED[data->GetAt(DataPosition+2)] && ALLOWED[data->GetAt(DataPosition+3)] );			
		}
		if (FrameID == 0) // 4 mal 0, Paddingbereich
			break;
		// Falls ungültiger Frame ab nächstem Byte analysieren  
		if (!isValid)
		{
			DataPosition++;
		}
		else
		{
			ATLTRACE(_T("Size:%i "), FrameSize);
			// 4.0.4.7: compared without adding, a v2.3 size of FFFFFFFF wrapped the sum and the frame was read past the
			// tag (4.0.4.6 and earlier)
			if (FrameSize > dataSize - DataPosition - headerSize)
			{
				
				CTools::instance().setLastError(ERR_FRAME_CORRUPT, BYTE(FrameID >> 24), BYTE(FrameID >> 16), BYTE(FrameID >> 8), BYTE(FrameID), DataPosition + headerSize);
				DataPosition = dataSize;
			}
			else
			{
				if (FrameSize > oldTagSize)
					FrameSize = oldTagSize;
				CID3_Frame *frame = CID3_FrameFactory::instance().createFrame(FrameID);
				frame->flags = FrameFlags;
				frame->isResynced = (Version < TAG_VERSION_2_4); // 4.0.4.7: v2.2/v2.3 unsync was undone for the whole tag above
				frame->load(data->m_pData + DataPosition + headerSize, FrameSize);
#ifdef DEBUG
				frame->print();
#endif
				_frames.Add(frame);
				DataPosition+= headerSize + FrameSize;
			}
		}		
	}
}

bool CID3V2::SaveToFile(LPCWSTR FileName)
{
	chooseSaveFormat(); // before the frames are sized and written in that format and encoding
	FILE *Stream;
	/* Check for existing tag */
	ATLTRACE(_T("open %s READ_ONLY\n"), FileName); 
	if ( (Stream = _wfsopen(FileName, READ_ONLY, _SH_DENYNO)) != NULL)
	{
		ReadHeader(Stream);
		oldTagSize = Size;
		fclose(Stream);
		return SaveTag(FileName);
	}
	CTools::instance().setLastError(errno);
	return false;
}


bool CID3V2::SaveTag(LPCWSTR FileName)
{
	u32 tagSize = calcTagSize();
	bool needRebuild = false;
	long paddingSize = 0;
	long paddingBlockSize = CTools::configValues[CONFIG_ID3V2PADDINGSIZE];
	u32 optimalSize = tagSize;
	if (paddingBlockSize > 0)
		optimalSize = ((u32)(tagSize / paddingBlockSize) + 1) * paddingBlockSize;
	
	if (tagSize > oldTagSize || optimalSize < oldTagSize || paddingBlockSize == 0)
	{
		long newTagSize = optimalSize;
		paddingSize = newTagSize - tagSize;
		tagSize = newTagSize;
		needRebuild = true;
	}
	else
	{
		paddingSize = oldTagSize - tagSize;
		tagSize = oldTagSize;
		needRebuild = false;
	}
	CBlob *newData = new CBlob(tagSize);
	WriteHeader(newData, tagSize - 10);
	storeFrames(newData);
	if (paddingSize > 0)
		newData->AddValue(0, paddingSize);
	bool result;
	if (needRebuild)
		result = RebuildFile(FileName, newData);
	else
		result = ReplaceTag(FileName, newData);
	delete newData;
	return result;
}

bool CID3V2::ReplaceTag(LPCWSTR FileName, CBlob* data)
{
	FILE *Stream;	
	CTools::instance().writeDebug(_T("Replace id3v2 Tag")); 
	if ( (Stream = _wfsopen(FileName, READ_AND_WRITE, _SH_DENYWR)) != NULL)
	{
		data->FileWrite(data->GetLength(), Stream);
		fflush(Stream);
		fclose(Stream);
		return true;
	}
	CTools::instance().setLastError(errno);
	return false;
}

bool CID3V2::RebuildFile(LPCWSTR FileName, CBlob* data)
{
	FILE *Source;
	FILE *Destination;
	CAtlString NewFileName(FileName);
	long FrameOldSize = 0;
	CTools::instance().writeDebug(_T("Rebuild id3v2 tag")); 
	if ( (Source = _wfsopen(FileName, READ_ONLY, _SH_DENYNO)) == NULL)
	{
		CTools::instance().setLastError(errno);
		return false;
	}

	/* Rebuild file with old file data and new tag data (optional) */
	ReadHeader(Source);
	FrameOldSize = Size;
	if (data == NULL && FrameOldSize == 0)
	{
		CTools::instance().setLastError(ERR_TAG_NOT_EXIST);
		fclose(Source);
		return false;
	}

	/* Create file streams */
	NewFileName+=TILDE;
	if ( (Destination = _wfsopen(NewFileName, WRITE_ONLY, _SH_DENYWR)) == NULL)
	{
		CTools::instance().setLastError(errno);
		fclose(Source);
		return false;
	};

	// 4.0.2: every read and write is checked, and the copy must hold all of the audio before it replaces the
	// original. A read or write error (disk full, a network share dropping) used to end the copy early as if the
	// file had ended, and the original was then deleted and the truncated copy renamed over it; when that rename
	// failed, the audio was left only in "name~".
	bool copied = true;
	__int64 audioBytes = 0;
	if (_fseeki64(Source, 0, SEEK_END) != 0 || (audioBytes = _ftelli64(Source)) < 0)
		copied = false;
	audioBytes -= FrameOldSize;
	if (copied && (audioBytes < 0 || _fseeki64(Source, FrameOldSize, SEEK_SET) != 0))
	{
		CTools::instance().writeError(L"ID3V2 fseek(%i) fehlgeschlagen, breche Speichern ab", FrameOldSize);
		copied = false;
	}
	if (copied && data != NULL && data->GetLength() > 0 && data->FileWrite(data->GetLength(), Destination) != data->GetLength())
		copied = false;

	// Copy data in Blocks for large files
	size_t blockSize = CTools::configValues[CONFIG_ID3V2WRITEBLOCKSIZE];
	CBlob tmp(blockSize);
	__int64 copiedBytes = 0;
	while (copied)
	{
		tmp.FileRead(blockSize, Source);
		if (ferror(Source)) { copied = false; break; }
		size_t got = tmp.GetLength();
		if (got > 0 && tmp.FileWrite(got, Destination) != got) { copied = false; break; }
		copiedBytes += got;
		if (got < blockSize) break; // end of the file
	}
	if (copied && (copiedBytes != audioBytes || ferror(Source)))
		copied = false;
	// Only this save's own streams: _fcloseall closed every stream the process had open
	if (fflush(Destination) != 0 || ferror(Destination))
		copied = false;
	if (fclose(Destination) != 0)
		copied = false;
	fclose(Source);

	if (!copied)
	{
		_wremove(NewFileName); // the original is untouched
		CTools::instance().setLastError(errno != 0 ? errno : EIO);
		return false;
	}

	/* Replace the original with the copy in one step: it is either the old file or the new one, never missing */
	if (!MoveFileExW(NewFileName, FileName, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
	{
		DWORD err = GetLastError();
		_wremove(NewFileName);
		CTools::instance().setLastError(err == ERROR_ACCESS_DENIED || err == ERROR_SHARING_VIOLATION ? EACCES : EIO);
		return false;
	}
	return true;
}

bool CID3V2::RemoveFromFile(LPCWSTR FileName)
{
	CTools::instance().writeDebug(_T("try to remove id3v2 tag")); 
	return RebuildFile(FileName, NULL);
}

CAtlString CID3V2::GetText(u32 ID)
{
	if (_frames.GetCount() == 0)
		return EMPTY;
	// 4.0.4.7: only a text frame has a text. Any frame was cast to one, so the ID of e.g. PCNT, APIC or TXXX read
	// garbage or crashed (4.0.4.6 and earlier).
	CID3F_T000 *f = dynamic_cast<CID3F_T000*>(findFrame(ID));
	return (f != NULL ) ? f->getText() : EMPTY;
}

CAtlString CID3V2::GetURL(u32 ID)
{
	if (_frames.GetCount() == 0)
		return EMPTY;
	CID3F_W000 *f = dynamic_cast<CID3F_W000*>(findFrame(ID));
	return (f != NULL ) ? f->getURL() : EMPTY;
}

// 4.0.4.7: an empty text deletes the frame. AUDIOSaveChangesW sets every field, so a file without e.g. an album got an
// empty TALB, and an empty UTF-16 frame was written without BOM, which readers reject (4.0.4.6 and earlier).
// An ID that isn't a text frame is refused: e.g. APIC was replaced by a text frame.
void CID3V2::SetText(u32 ID, LPCWSTR newText)
{
	if (CID3_FrameFactory::instance().findFrameClassType(ID) != ID3_T000)
	{
		CTools::instance().setLastError(ERR_INVALID_FORMAT);
		return;
	}
	if (newText == NULL || wcslen(newText) == 0)
		deleteAllFrames(ID);
	else
		replaceFrame(new CID3F_T000(ID, newText));
}

void CID3V2::SetURL(u32 ID, LPCWSTR newURL)
{
	if (CID3_FrameFactory::instance().findFrameClassType(ID) != ID3_W000)
	{
		CTools::instance().setLastError(ERR_INVALID_FORMAT);
		return;
	}
	if (newURL == NULL || wcslen(newURL) == 0)
		deleteAllFrames(ID);
	else
		replaceFrame(new CID3F_W000(ID, newURL));
}

CAtlString CID3V2::getAllFrameIDs()
{
	CAtlString result;
	CSimpleArray<unsigned int> ids;
	size_t count = _frames.GetCount();
	u32 frame;
	for (size_t i = 0; i < count; i++)
	{
		frame = _frames[i]->_frameID;
		if (ids.Find(frame) == -1)
		{
			ids.Add(frame);
			if (result.GetLength() > 0)
				result.Append(_T(","));
			result.Append(_frames[i]->getFrameIDString());
		}		
	}
	return result;
}

CID3_Frame* CID3V2::findFrame(u32 ID, int index)
{
	int found = 0;
	size_t count = _frames.GetCount();
	for (size_t i = 0; i < count; i++)
	{
		if (_frames[i]->equals(ID) )
		{		
			found++;
			if (found == index)
				return _frames[i];
		}
	}
	return NULL;
}

CID3_Frame* CID3V2::findFrame(CAtlString id)
{
	size_t count = _frames.GetCount();
	for (size_t i = 0; i < count; i++)
	{
		if (_frames[i]->equals(F_CTOC))
		{		
			CID3F_CTOC* result = cCTOC(_frames[i]);
			if (result->getID().Compare(id) == 0)
				return result;
		}
		if (_frames[i]->equals(F_CHAP))
		{		
			CID3F_CHAP* result = cCHAP(_frames[i]);
			if (result->getID().Compare(id) == 0)
				return result;
		}
	}
	return NULL;
}

CID3_Frame* CID3V2::findNextFrame(u32 ID, size_t& index)
{
	size_t count = _frames.GetCount();
	while (index < count)
	{
		if (_frames[index]->equals(ID) )
			return _frames[index++];
		index++;
	}
	return NULL;
}

short CID3V2::deleteAllFrames(u32 ID)
{
	int count = (int) _frames.GetCount();
	short result = 0;
	while (--count >= 0)
	{
		if (_frames[count]->equals(ID) )
		{
			delete _frames[count];
			_frames.RemoveAt(count);
			result++;
		}
	}
	return result;
}

short CID3V2::deleteAllFrames(CAtlString ID)
{
	int count = (int)_frames.GetCount();
	short result = 0;
	while (--count >= 0)
	{
		if (_frames[count]->equals(F_CTOC) || _frames[count]->equals(F_CHAP))
		{		
			// 4.0.4.7: was named result too, hiding the counter, so 0 was always returned (4.0.4.6 and earlier)
			CID3F_Chapter* chapter = cCHAPTER(_frames[count]);
			if (chapter->getID().Compare(ID) == 0)
			{
				delete _frames[count];
				_frames.RemoveAt(count);
				result++;
			}
		}		
	}
	return result;
}


bool CID3V2::deleteFrame(u32 ID, int index)
{
	int found = 0;
	size_t count = _frames.GetCount();
	for (size_t i = 0; i < count; i++)
	{
		if (_frames[i]->equals(ID) )
		{		
			found++;
			if (found == index)
			{
				delete _frames[i];
				_frames.RemoveAt(i);	
				return true;
			}
		}
	}	
	return false;
}
short CID3V2::countFrame(u32 ID)
{
	short result = 0;
	size_t count = _frames.GetCount();
	for (size_t i = 0; i < count; i++)
	{
		if (_frames[i]->equals(ID) )
			result++;
	}
	return result;
}

bool CID3V2::replaceFrame(CID3_Frame *frame)
{
	size_t count = _frames.GetCount();
	for (size_t i = 0; i < count; i++)
	{
		if (frame->equals(_frames[i]))
		{
			delete _frames[i];
			_frames.SetAt(i, frame);
			return true;
		}
	}
	_frames.Add(frame);
	return false;
}

void CID3V2::storeFrames(CBlob *blob)
{
	size_t counts = _frames.GetCount();
	for (size_t i = 0; i < counts; i++)
		_frames[i]->storeFrame(blob);
}

u32 CID3V2::calcTagSize()
{
	long wide = CTools::ID3V2newTagVersion == TAG_VERSION_2_2 ? 6 : 10;
	/* Calculate new tag size (without padding) */
	u32 TagSize = 10;
	CID3_Frame* frame;
	u32 id;
	size_t count = _frames.GetCount();
	for (size_t i = 0; i < count; i++)
	{
		frame = _frames[i];
		id = frame->getStoreID(); // 4.0.4.7: the same test as storeFrame
		if (id != F_NONE)
			TagSize+= frame->getSize() + wide;		
	}
	return TagSize;
}

CAtlString CID3V2::GetGenre()
{
	CAtlString genre = GetText(F_TCON);
	// Sonderfall: Falls Zahl in Klammern, Genre aus ID3V1 TagInfo holen
	if (genre.GetLength() == 0 || genre.GetAt(0) != '(' )
		return genre;
	char Buffer[4];
	memset(Buffer, 0, 4);
	int endPos = genre.Find(')', 1);
	// 4.0.4.7: only a number of 1-3 digits is a genre index. "(RX)", "(CR)" and "(Live)" read as 0, "Blues" (4.0.4.6 and earlier).
	if (endPos < 2 || endPos > 4)
		return genre;
	for (int i = 1; i < endPos && i < 4; i++)
	{
		if (genre.GetAt(i) < '0' || genre.GetAt(i) > '9')
			return genre;
		Buffer[i - 1] = (char)genre.GetAt(i);
	}
	int zahl = atoi(Buffer);
	if (zahl >= 0 && zahl < MAX_MUSIC_GENRES)
		genre = MUSIC_GENRE[zahl];
	return genre;
}
// 4.0.4.7: a token of the cue file in its code page (UTF-8 or the PC's ANSI code page)
static CAtlString cueToken(CBlob& data, size_t pos, size_t len, UINT codePage)
{
	if (len == 0 || pos + len > data.GetLength())
		return EMPTY;
	int n = MultiByteToWideChar(codePage, 0, (const char*)(data.m_pData + pos), (int)len, NULL, 0);
	if (n <= 0)
		return EMPTY;
	CAtlString result;
	MultiByteToWideChar(codePage, 0, (const char*)(data.m_pData + pos), (int)len, result.GetBuffer(n), n);
	result.ReleaseBuffer(n);
	return result;
}

// 4.0.4.7: an exception (a short INDEX line made tokens.GetAt(2) throw) no longer leaves the DLL, which ended the
// calling .NET program; frame times are 1/75 s (was divided by 76); a UTF-8 cue file is read as UTF-8 (was read in
// the PC's code page); a TRACK number given twice no longer leaves lastChap pointing to the deleted chapter
// (4.0.4.6 and earlier)
bool CID3V2::parseCueFile(LPCWSTR FileName)
{
	FILE *Source;
	CTools::instance().writeDebug(_T("parse cue file")); 
	if ( (Source = _wfsopen(FileName, READ_ONLY, _SH_DENYNO)) == NULL)
	{
		CTools::instance().setLastError(errno);
		return false;
	}
	// Alles in Blob einlesen
	__int64 fileLength = _filelengthi64(_fileno(Source));
	if (fileLength < 0 || fileLength > 16 * 1024 * 1024) // a cue sheet is a small text file
	{
		fclose(Source);
		CTools::instance().setLastError(ERR_INVALID_FORMAT);
		return false;
	}
	try
	{
		CBlob dummy((size_t)fileLength + 1);
		dummy.FileRead((size_t)fileLength, Source);
		fclose(Source);
		Source = NULL;
		int ln = (int)dummy.GetLength();
		// UTF-8 (with or without BOM) when the bytes are valid UTF-8, otherwise the PC's code page
		int pos = 0, start = 0;
		if (ln >= 3 && dummy.GetAt(0) == 0xEF && dummy.GetAt(1) == 0xBB && dummy.GetAt(2) == 0xBF)
			pos = start = 3;
		UINT codePage = CP_ACP;
		if (ln - pos > 0 && MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (const char*)(dummy.m_pData + pos), ln - pos, NULL, 0) > 0)
			codePage = CP_UTF8;
		// Blob aufteilen in Stuecke, Trenner sind 0D, 0A, EOL und Leerzeichen
		CAtlArray<CAtlString> tokens;
		dummy.AddValue(13);
		CID3F_CTOC *toc = new CID3F_CTOC(_T("toc1"), EMPTY, EMPTY, true);
		CID3F_CHAP *chap = NULL, *lastChap = NULL;
		long lastTime = 0;
		toc->setRoot(true);
		replaceFrame(toc);
		while (pos < ln)
		{
			if (dummy.GetAt(pos) == '"') // Anfuehrungszeichen
			{
				do // Lese bis zum naechsten Anfuehrungszeichen oder Ende der Zeile
				{
					pos++;
				}
				while (!(dummy.GetAt(pos) == '"' || dummy.GetAt(pos) == 13 || dummy.GetAt(pos) == 10));
				if (pos - start > 0)
					tokens.Add(cueToken(dummy, start + 1, pos - start - 1, codePage));
				start = pos + 1;
			}
			if (dummy.GetAt(pos) == ' ') // blank	
			{
				if (pos - start > 0)
					tokens.Add(cueToken(dummy, start, pos - start, codePage));
				start = pos + 1;
			}
			else if (dummy.GetAt(pos) == 13 || dummy.GetAt(pos) == 10)
			{
				if (pos - start > 0)
					tokens.Add(cueToken(dummy, start, pos - start, codePage));
				start = pos + 1;
				// Nun habe ich eine Zeile mit allen relevanten Woertern
				// Auswerten; falls Fehler kommt false zurueck
				if (tokens.GetCount() > 1)
				{
					CAtlString key(tokens.GetAt(0));
					CAtlString tmp;
					if (key.CompareNoCase(_T("INDEX")) == 0)
					{
						if (tokens.GetCount() > 2)
						{
							// int nr = _wtoi(tokens.GetAt(1));
							tmp = tokens.GetAt(2); // Die Zeitangabe erfolgt in Minuten (mm), Sekunden (ss) und Frames (ff), wobei jede Sekunde in 75(!) Frames unterteilt wird.
							// time parsen
							int p1, p2;
							p1 = tmp.Find(':', 0);
							p2 = tmp.Find(':', p1 + 1);
							long startTime = 0;
							if (p1 != -1 && p2 != -1)
								startTime = ( _wtoi(tmp.Left(p1)) * 60000l) + (_wtoi(tmp.Mid(p1 + 1, p2 - p1 - 1)) * 1000l) + (_wtoi(tmp.Mid(p2 + 1)) * 1000l / 75l);
							lastTime = startTime;			
							if (chap != NULL)
								chap->setTimes(startTime, startTime);
							if (lastChap != NULL && lastChap != chap)
								lastChap->setTimes(lastChap->getStartTime(), lastTime );
							lastChap = chap;
						}
					}
					else if (key.CompareNoCase(_T("PERFORMER")) == 0)
					{
						if (chap != NULL)
							chap->addFrame(new CID3F_T000(F_TPE1, tokens.GetAt(1)));
						else
							toc->addFrame(new CID3F_T000(F_TPE1, tokens.GetAt(1)));
					}
					else if (key.CompareNoCase(_T("TITLE")) == 0)
					{
						if (chap != NULL )
							chap->setTitle(tokens.GetAt(1));
						else
							toc->setTitle(tokens.GetAt(1));

					}
					else if (key.CompareNoCase(_T("TRACK")) == 0)
					{
							tmp.Format(_T("ch%i"), _wtoi(tokens.GetAt(1)));
							// a TRACK number given twice: replaceFrame deletes the earlier chapter
							CID3_Frame *old = findFrame(tmp);
							if (old != NULL && old == lastChap)
								lastChap = NULL;
							chap = new CID3F_CHAP(tmp, EMPTY, EMPTY);
							replaceFrame(chap);
							toc->deleteChildElement(tmp); // listed once
							toc->addChildElement(tmp);										
					}				
				}
				// Zeile wieder zuruecksetzen
				tokens.RemoveAll();
			}
			pos++;
		}
	}
	catch (...)
	{
		if (Source != NULL)
			fclose(Source);
		CTools::instance().setLastError(ERR_INVALID_FORMAT);
		return false;
	}
	return true;
}
