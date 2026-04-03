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
	ATLASSERT(newEncoding >= 0 && newEncoding <= 3);
	ATLASSERT(newFormat >= 0 && newFormat <= 3);
	if (newFormat > 0)
		newFormat++;
	if (newFormat != TAG_VERSION_2_4 && newEncoding == 3)
	{
		CTools::instance().setLastError(ERR_UTF8_NOT_ALLOWED); 
		return false;
	}
	// 0 = nimm default Tagformat
	if (newFormat != 0)
		CTools::ID3V2newTagVersion = newFormat;
	CTools::ID3V2defaultEncodingID = newEncoding;	
	return true;
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
		if ( (CTools::ID3V2Flags & 0x40) == 0x40)
			Size+= 10; // Extended Header present
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
			if (DataPosition + headerSize + FrameSize > dataSize)
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
	
	size_t blockSize = CTools::configValues[CONFIG_ID3V2WRITEBLOCKSIZE];
	CBlob tmp(blockSize);
	/* Copy data blocks */
	if (fseek(Source, FrameOldSize, SEEK_SET) != 0) 
	{
		CTools::instance().writeError(L"ID3V2 fseek(%i) fehlgeschlagen, breche Speichern ab", FrameOldSize);
		return false;
	}
	if (data != NULL)
		data->FileWrite(data->GetLength(), Destination);
		
	// Copy data in Blocks for large files
	do
	{
		ATLTRACE(_T("write %i bytes\n"), blockSize);
		tmp.FileRead(blockSize, Source);
		CTools::instance().doEvents();
		tmp.FileWrite(tmp.GetLength(), Destination);
		CTools::instance().doEvents();		
	} while (tmp.GetLength() == blockSize);
	_flushall();
	_fcloseall();
	CTools::instance().doEventsNow();
	/* Replace old file and delete temporary file */
	if (_wremove(FileName) != 0)
	{
		// Lösche auch temp file
		_wremove(NewFileName);
		CTools::instance().setLastError(errno);
		return false;
	}
	CTools::instance().doEventsNow();
	if (_wrename(NewFileName, FileName) != 0)
	{
		CTools::instance().setLastError(errno);
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
	CID3_Frame *f = findFrame(ID);
	return (f != NULL ) ? cT000(f)->getText() : EMPTY;
}

CAtlString CID3V2::GetURL(u32 ID)
{
	if (_frames.GetCount() == 0)
		return EMPTY;
	CID3_Frame *f = findFrame(ID);
	return (f != NULL ) ? cW000(f)->getURL() : EMPTY;
}

void CID3V2::SetText(u32 ID, LPCWSTR newText)
{
	replaceFrame(new CID3F_T000(ID, newText));	
}

void CID3V2::SetURL(u32 ID, LPCWSTR newURL)
{
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
			CID3F_Chapter* result = cCHAPTER(_frames[count]);
			if (result->getID().Compare(ID) == 0)
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
		id = CID3_FrameFactory::instance().findTagForVersion(frame->_frameID);
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
	for (int i = 1; i < endPos && i < 4; i++)
		Buffer[i - 1] = (char)genre.GetAt(i);
	int zahl = atoi(Buffer);
	if (zahl >= 0 && zahl < MAX_MUSIC_GENRES)
		genre = MUSIC_GENRE[zahl];
	return genre;
}
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
	int ln = _filelength(_fileno(Source));
	CBlob dummy(ln + 1);
	dummy.FileRead(ln, Source);
	fclose(Source);
	// Blob aufteilen in Stücke, Trenner sind 0D, 0A, EOL und Leerzeichen
	int pos = 0, start = 0;
	CAtlArray<CAtlString> tokens;
	dummy.AddValue(13);
	CID3F_CTOC *toc = new CID3F_CTOC(_T("toc1"), EMPTY, EMPTY, true);
	CID3F_CHAP *chap = NULL, *lastChap = NULL;
	long lastTime = 0;
	toc->setRoot(true);
	replaceFrame(toc);
	while (pos < ln)
	{
		if (dummy.GetAt(pos) == '"') // Anführungszeichen
		{
			do // Lese bis zum nächsten Anführungszeichen oder Ende der Zeile
			{
				pos++;
			}
			while (!(dummy.GetAt(pos) == '"' || dummy.GetAt(pos) == 13 || dummy.GetAt(pos) == 10));
			if (pos - start > 0)
				tokens.Add(dummy.GetStringAt(start + 1, pos - start - 1));
			start = pos + 1;
		}
		if (dummy.GetAt(pos) == ' ') // blank	
		{
			if (pos - start > 0)
				tokens.Add(dummy.GetStringAt(start, pos - start));
			start = pos + 1;
		}
		else if (dummy.GetAt(pos) == 13 || dummy.GetAt(pos) == 10)
		{
			if (pos - start > 0)
				tokens.Add(dummy.GetStringAt(start, pos - start));
			start = pos + 1;
			// Nun habe ich eine Zeile mit allen relevanten Wörtern
			// Auswerten; falls Fehler kommt false zurück
			if (tokens.GetCount() > 1)
			{
				CAtlString key(tokens.GetAt(0));
				CAtlString tmp;
				if (key.CompareNoCase(_T("INDEX")) == 0)
				{
					// int nr = _wtoi(tokens.GetAt(1));
					tmp = tokens.GetAt(2); // Die Zeitangabe erfolgt in Minuten (mm), Sekunden (ss) und Frames (ff), wobei jede Sekunde in 75(!) Frames unterteilt wird.
					// time parsen
					int p1, p2;
					p1 = tmp.Find(':', 0);
					p2 = tmp.Find(':', p1 + 1);
					long startTime = 0;
					if (p1 != -1 && p2 != -1)
						startTime = ( _wtoi(tmp.Left(p1)) * 60000l) + (_wtoi(tmp.Mid(p1 + 1, p2 - p1 - 1)) * 1000l) + (_wtoi(tmp.Mid(p2 + 1)) * 1000l / 76l);
					lastTime = startTime;			
					if (chap != NULL)
						chap->setTimes(startTime, startTime);
					if (lastChap != NULL)
						lastChap->setTimes(lastChap->getStartTime(), lastTime );
					lastChap = chap;
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
						chap = new CID3F_CHAP(tmp, EMPTY, EMPTY);
						replaceFrame(chap);
						toc->addChildElement(tmp);										
				}				
			}
			// Zeile wieder zurücksetzen
			tokens.RemoveAll();
		}
		pos++;
	}
	return true;
}
