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
#include "ID3F_Chapter.h"
#include "id3_framefactory.h"
#include "ID3v2.h"
#include "Tools.h"

CID3F_Chapter::CID3F_Chapter(void)
{
	_frames.SetCount(0, 6);
}

CID3F_Chapter::~CID3F_Chapter(void)
{
	deleteAllFrames(); // 4.0.4.7: the sub-frames were never deleted, a leak (4.0.4.6 and earlier)
}
bool CID3F_Chapter::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	CID3F_Chapter *target = cCHAPTER(frame);
	return (this->_ID.CompareNoCase(target->_ID) == 0);
}
CAtlString CID3F_Chapter::getID()
{
	decode();
	return _ID;
}
CAtlString CID3F_Chapter::getTitle()
{
	decode();
	return _title;
}
CAtlString CID3F_Chapter::getDescription()
{
	decode();
	return _description;
}
// 4.0.4.7: decode first, and the chapter is rebuilt: a change made after a first save was not written, and a change
// to a chapter not decoded yet was overwritten by its decode (4.0.4.6 and earlier)
void CID3F_Chapter::addFrame(CID3_Frame* frame)
{
	decode();
	mustRebuild = true;
	size_t count = _frames.GetCount();
	for (size_t i = 0; i < count; i++)
	{
		if (frame->equals(_frames[i]))
		{
			delete _frames[i];
			_frames.SetAt(i, frame);
			return;
		}
	}
	_frames.Add(frame);
}

void CID3F_Chapter::deleteFrame(u32 ID)
{
	decode();
	mustRebuild = true;
	int count = (int)_frames.GetCount();
	while (--count >= 0)
	{
		if (_frames[count]->equals(ID) )
		{
			delete _frames[count];
			_frames.RemoveAt(count);			
		}
	}
}

void CID3F_Chapter::deleteAllFrames()
{
	int count = (int)_frames.GetCount();
	while (--count >= 0)
		delete _frames[count];
	_frames.RemoveAll();
	mustRebuild = true;
}

CID3_Frame* CID3F_Chapter::findFrame(u32 ID, int index)
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

void CID3F_Chapter::convertBlobToFrames()
{
	// konvertiert die Daten aus _data in CID3_Frames
	if (_data.GetLength() == 0)
		return;
	size_t start = 0;
	short flags = 0;
	bool foundTitle = false, foundDescription = false;
	u32 frameID, frameSize;
	CID3_Frame *frame;
	// 4.0.4.7: a sub-frame header needs 10 bytes and its data must fit in what is left; both were read past the data,
	// and padding (frame ID 0) was read as frames (4.0.4.6 and earlier)
	while (start + 10 <= _data.GetLength())
	{
		frameID = (u32)_data.Get4B(start);
		if (frameID == 0)
			break;
		if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_4) // SyncSafe Integer
			frameSize = (u32)_data.GetS4B(start + 4);
		else
			frameSize = (u32)_data.Get4B(start + 4);
		flags = _data.Get2B(start + 8);
		start+=10;
		// Sicherheitsabfrage auf Grösse des Frames
		if (frameSize > _data.GetLength() - start)
		{
			CTools::instance().setLastError(ERR_CHAPTER_FRAME_CORRUPT);
			start = _data.GetLength();		}
		else
		{
			frame = CID3_FrameFactory::instance().createFrame(frameID);
			frame->flags = flags;
			// 4.0.4.7: a chapter resynced as a whole: its sub-frames are not resynced again
			frame->isResynced = isResynced;
			frame->load(_data.m_pData + start, frameSize);
			frame->print();
			// 4.0.4.7: TIT2 and TIT3 stay sub-frames too. They were only copied to _title/_description, so every save
			// dropped the chapter names (encode writes _frames), and the frames leaked (4.0.4.6 and earlier).
			CID3F_T000 *text = dynamic_cast<CID3F_T000*>(frame);
			if (!foundTitle && frameID == F_TIT2 && text != NULL)
			{
				_title = text->getText();
				foundTitle = true;
			}
			else if (!foundDescription && frameID == F_TIT3 && text != NULL)
			{
				_description = text->getText();
				foundDescription = true;
			}
			_frames.Add(frame);
			start+=frameSize;
		}
	}
}

void CID3F_Chapter::convertFramesToBlob()
{
	_data.Clear();
	for (size_t i=0; i < _frames.GetCount(); i++)
		_frames[i]->storeFrame(&_data);
}

void CID3F_Chapter::setTitle(LPCWSTR title)
{
	decode();
	if (title == NULL || wcslen(title) == 0)
		deleteFrame(F_TIT2);
	else
		addFrame(new CID3F_T000(F_TIT2, title));
	_title = title;
	mustRebuild = true;
}

void CID3F_Chapter::setDescription(LPCWSTR description)
{
	decode();
	if (description == NULL || wcslen(description) == 0)
		deleteFrame(F_TIT3);
	else
		addFrame(new CID3F_T000(F_TIT3, description));	
	_description = description;
	mustRebuild = true;
}

short CID3F_Chapter::getFrameCount()
{
	decode();
	return (short)_frames.GetCount();
}
u32 CID3F_Chapter::getFrameID(u16 index)
{
	decode();
	return (index > 0 && index <= _frames.GetCount()) ? _frames[index - 1]->_frameID : 0;
}
CAtlString CID3F_Chapter::getFrameText(u16 index)
{
	decode();
	BYTE typ = getFrameTyp(index);
	// 4.0.4.7: a frame of an unknown ID (base class) was cast to the text or URL class: undefined behavior (4.0.4.6 and earlier)
	CID3_Frame *f = (typ != 0) ? _frames[index - 1] : NULL;
	switch (typ)
	{
	case 1:
		return dynamic_cast<CID3F_T000*>(f) ? cT000(f)->getText() : EMPTY;
	case 2:
		return dynamic_cast<CID3F_TXXX*>(f) ? cTXXX(f)->getText() : EMPTY;
	case 3:
		return dynamic_cast<CID3F_W000*>(f) ? cW000(f)->getURL() : EMPTY;
	case 4:
		return dynamic_cast<CID3F_WXXX*>(f) ? cWXXX(f)->getURL() : EMPTY;
	case 5:
		return dynamic_cast<CID3F_APIC*>(f) ? cAPIC(f)->getDescription() : EMPTY;
	default:
		return EMPTY;
	}
}

CAtlString CID3F_Chapter::getFrameDescription(u16 index)
{
	decode();
	BYTE typ = getFrameTyp(index);
	switch (typ)
	{
	case 2:
		return cTXXX(_frames[index - 1])->getDescription();
	case 4:
		return cWXXX(_frames[index - 1])->getDescription();
	case 5:
		return cAPIC(_frames[index - 1])->getPictureTypeAsText();
	default:
		return EMPTY;
	}
}

CID3_Frame* CID3F_Chapter::getFrame(u16 index)
{
	decode();
	if (index < 1 || index > _frames.GetCount() )
		return NULL;
	return _frames[index - 1];
}

BYTE CID3F_Chapter::getFrameTyp(u16 index)
{
	decode();
	if (index < 1 || index > _frames.GetCount() )
		return 0;
	u32 frameID = _frames[index - 1]->_frameID;
	if (frameID == F_TXXX)
		return 2;
	if (frameID == F_WXXX)
		return 4;
	if (frameID == F_APIC)
		return 5;
	BYTE firstChar = BYTE((frameID >> 24) & 0xFF);
	if (firstChar == 'T')
		return 1;
	if (firstChar == 'W')
		return 3;
	return 0;
}