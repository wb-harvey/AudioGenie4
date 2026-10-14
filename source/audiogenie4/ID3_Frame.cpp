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
#include "ID3_Frame.h"
#include "ID3v2.h"
#include "Tools.h"
#include "ID3_FrameFactory.h"

bool CID3_Frame::equals(const unsigned int ID)
{
	return (this->_frameID == ID);
}

bool CID3_Frame::equals(CID3_Frame *frame)
{
	return (this->_frameID == frame->_frameID);
}

CID3_Frame::CID3_Frame(void)
{
	init(0);
}

CID3_Frame::CID3_Frame(unsigned int frameID)
{
	init(frameID);
}

void CID3_Frame::init(unsigned int frameID)
{
	_frameID = frameID;	
	isDecoded = false;
	mustRebuild = true;
	encodingID = 255;
	useTextEncoding = false;
	flags = 0;
	isResynced = true; // cleared before load() for a frame read from a file
	isRaw = false;
}

CID3_Frame::~CID3_Frame(void)
{
	_blob.Clear();
	_data.Clear();
}

long CID3_Frame::getSize()
{
	// 4.0.4.7: an untouched compressed or encrypted frame is written as it was read (getStoreID)
	if (isRaw && !mustRebuild)
		return (long)_blob.GetLength();
	// 4.0.4.7: a frame whose class doesn't resync in decode() (an unknown frame) is resynced here: its unsync flag
	// is no longer written (storeFrame)
	if (isUnsynchronized())
	{
		decode();
		if (isUnsynchronized())
			resync();
	}
	// Prüfe auf Encoding
	if (useTextEncoding)
	{
		decode();
		// 4.0.2: only a frame that is new or was changed, or whose encoding the version being saved doesn't allow
		// (UTF-16BE and UTF-8 before v2.4), is encoded again. Every frame used to be: the text was decoded and
		// encoded in the default encoding, which kept only the first value of a v2.4 multi-value frame and turned
		// an unreadable UTF-8 frame into an empty one. An untouched frame now keeps its bytes.
		bool v24 = CTools::ID3V2newTagVersion == TAG_VERSION_2_4;
		bool allowed = encodingID == TEXT_ENCODED_ANSI || encodingID == TEXT_ENCODED_UTF16BOM ||
			(v24 && (encodingID == TEXT_ENCODED_UTF16 || encodingID == TEXT_ENCODED_UTF8));
		if ((mustRebuild || !allowed) && CTools::ID3V2defaultEncodingID != encodingID)
		{
			encodingID = CTools::ID3V2defaultEncodingID;
			mustRebuild = true;
		}
	}
	encode();
	return (long)_blob.GetLength();
}

void CID3_Frame::load(BYTE* source, long size)
{
	if (isCompressed())
		CTools::instance().setLastError(ERR_COMPRESSED_FRAME_NOT_SUPPORTED);
	// isResynced is set by the caller: false for a frame of the tag, the chapter's state for a sub-frame
	// 4.0.4.7: the bytes in front of the frame data are the group byte (v2.3 and v2.4) and the data length (v2.4).
	// The group byte was kept as text, and the encryption byte was cut although the data stays encrypted. A compressed
	// or encrypted frame can't be decoded: it is kept as read (with its prefixes) and written back unchanged in the same
	// tag version only (getStoreID). A frame shorter than its prefixes made size - extraSize negative, a huge copy
	// (4.0.4.6 and earlier); it is kept as read too.
	int extraSize = 0;
	if (isGrouped())
		extraSize+=1;
	if (isDataLenIndicator())
		extraSize+=4;
	isRaw = isCompressed() || isEncrypted() || size < extraSize;
	if (isRaw)
	{
		if (size > 0)
			_blob.AddMemory(source, size);
		if (isUnsynchronized())
			resync();
		isDecoded = true; // never decoded: its data isn't readable
		mustRebuild = false;
		return;
	}
	_blob.AddMemory(source + extraSize, size - extraSize);
	isDecoded = false;
	// falls unsynchronized gesetzt ist, muss alles beim Speichern decodiert werden, da beim Speichern das unsync bit nicht mehr gesetzt ist
	mustRebuild = isUnsynchronized();
}

void CID3_Frame::save(FILE *stream)
{
	encode();
	_blob.FileWrite(_blob.GetLength(), stream);	
}

void CID3_Frame::print()
{
	ATLTRACE(_T("Frame:%c%c%c%c Size:%u\n"), BYTE(_frameID >> 24), BYTE(_frameID >> 16), BYTE(_frameID >> 8), BYTE(_frameID), _blob.GetLength());
}
CAtlString CID3_Frame::getFrameIDString()
{
	CAtlString dummy;
	dummy.Format(_T("%c%c%c%c"), BYTE(_frameID >> 24), BYTE(_frameID >> 16), BYTE(_frameID >> 8), BYTE(_frameID));
	return dummy;
}
long CID3_Frame::getData(BYTE *destination, long maxLen)
{
	decode();
	long ln = (long)_data.GetLength();
	if (ln > maxLen)
	{
		CTools::instance().setLastError(ERR_NOT_ENOUGH_MEMORY);
		return 0;
	}
	if (ln > 0)
		memcpy(destination, _data.m_pData, ln);
	return ln;
}

bool CID3_Frame::setData(BYTE *source, unsigned int maxLen)
{
	decode();
	_data.Clear();
	_data.AddMemory(source, maxLen);
	mustRebuild = true;
	return true;
}

// 4.0.4.7: the ID the frame is saved with in the new tag version, F_NONE when it isn't saved. calcTagSize and
// storeFrame both use it, so the computed tag size matches the bytes written.
u32 CID3_Frame::getStoreID()
{
	u32 id = CID3_FrameFactory::instance().findTagForVersion(_frameID);
	// a compressed or encrypted frame kept as read can only be written back into a tag of its own version
	if (id != F_NONE && isRaw && !mustRebuild && CTools::ID3V2oldTagVersion != CTools::ID3V2newTagVersion)
		return F_NONE;
	return id;
}

void CID3_Frame::storeFrame(CBlob *tmp)
{
	u32 id = getStoreID();
	if (id != F_NONE)
	{
		// Ermittle die Framegrösse und berücksichtige dabei evtl. Synchronisation
		long FrameSize = getSize();
		// 4.0.4.7: the flags written match the bytes written. The data is written resynced, without group byte and data
		// length (load), so only the status flags (tag/file alter preservation, read only) are kept, moved to their
		// place in the new version. The unsync, data length, compression and encryption flags were written back as
		// read (also for v2.3 <-> v2.4, whose bits differ), describing data that was no longer there (4.0.4.6 and earlier).
		// A compressed or encrypted frame kept as read keeps its flags (same version only), less unsync.
		u16 newFlags = 0;
		if (isRaw && !mustRebuild)
			newFlags = (CTools::ID3V2oldTagVersion == TAG_VERSION_2_4) ? (u16)(flags & ~0x0002) : flags;
		else
		{
			u16 status = 0; // %abc: tag alter preservation, file alter preservation, read only
			if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_3)
				status = (flags >> 13) & 7;
			else if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_4)
				status = (flags >> 12) & 7;
			newFlags = (CTools::ID3V2newTagVersion == TAG_VERSION_2_3) ? (u16)(status << 13) : (u16)(status << 12);
		}
		switch (CTools::ID3V2newTagVersion)
		{
		case TAG_VERSION_2_2:
			{
				tmp->Add3B(id);
				tmp->Add3B(FrameSize);
				break;
			}
		case TAG_VERSION_2_3:
			{
				tmp->Add4B(id);
				tmp->Add4B(FrameSize);
				tmp->Add2B(newFlags);
				break;
			}
		case TAG_VERSION_2_4:
			{
				tmp->Add4B(id);
				tmp->AddS4B(FrameSize);
				tmp->Add2B(newFlags);
				break;
			}
		}
		tmp->AddBlob(_blob);
	}
	else
	{
		CTools::instance().writeWarning(L"frame '%s' ignored because it is not supported in id3v2.%i", getFrameIDString(), CTools::ID3V2newTagVersion);
	}
}

//	To be used when reading an ID3v2-tag
//	Transforms all FF 00 sequences into FF
void CID3_Frame::resync()
{
	BYTE *dest = new BYTE[_blob.GetLength()];
	BYTE *src = _blob.m_pData;
	BYTE *end = src + _blob.GetLength();
	int ln = 0;
	while (src < end)
	{
		dest[ln++] = *src;
		src+=(src[0] == 0xFF && (src + 1 < end) && src[1] == 0) ? 2 : 1;
	}
	_blob.Clear();
	_blob.AddMemory(dest, ln);
	delete [] dest;
	mustRebuild = true;
	isResynced = true; // 4.0.4.7: never resynced twice (a second pass turned FF 00 00 of the data into FF)
}

//	To be used when writing an ID3v2-tag
//	Transforms:
//	11111111 111xxxxx -> 11111111 00000000 111xxxxx
//	11111111 00000000 -> 11111111 00000000 00000000 
//	11111111 <EOF> -> 11111111 00000000 <EOF>

/*void CID3_Frame::unsync()
{
	BYTE *dest = new BYTE[_blob.GetLength() * 2];
	BYTE *src = _blob.m_pData;
	BYTE *end = src + _blob.GetLength();
	int ln = 0;
	while (src < end)
	{
		dest[ln++] = *src;
		if (src[0] == 0xFF && ((src+1 == end) || (src[1] == 0) || src[1] >= 0xE0))
			dest[ln++] = 0;
		src++;
	}
	_blob.Clear();
	_blob.AddMemory(dest, ln);
	delete [] dest;	
} */

/*
id3v2.3: %abc00000 %ijk00000
a - Tag alter preservation
b - File alter preservation
c - Read only
i - Compression
j - Encryption
k - Grouping identity

id3v2.4: %0abc0000 %0h00kmnp
a - Tag alter preservation
b - File alter preservation
c - Read only
h - Grouping identity
k - Compression
m - Encryption
n - Unsynchronisation
p - Data length indicator
*/

bool CID3_Frame::isUnsynchronized()
{ 
	if (isResynced) // 4.0.4.7: already resynced, or not read from a file
		return false;
	if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_3) // Flags wirken global auf alle Frames bei v2.3
		return ( (CTools::ID3V2Flags & 128) == 128);
	if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_4 && (CTools::ID3V2Flags & 128) == 128) // Falls Unsynchronized bei v2.4 eingeschaltet ist, prüfe die Frame flags
		return ((flags & 2) == 2);
	return 0; // bei id3v2.2 gibt es keine Flags	
};

bool CID3_Frame::isReadOnly() 
{ 
	if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_3) // Flags wirken global auf alle Frames bei v2.3
		return ((flags & 8192) == 8192); 
	if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_4) 
		return ((flags & 4096) == 4096); 
	return 0; // bei id3v2.2 gibt es keine Flags	
};   
bool CID3_Frame::isGrouped() 
{ 
	if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_3) // Flags wirken global auf alle Frames bei v2.3
		return ((flags & 32) == 32); 
	if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_4) 
		return ((flags & 64) == 64); 
	return 0; // bei id3v2.2 gibt es keine Flags
};
bool CID3_Frame::isCompressed() 
{ 
	if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_3) // Flags wirken global auf alle Frames bei v2.3
		return ((flags & 128) == 128); 
	if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_4) 
		return ((flags & 8) == 8); 
	return 0; // bei id3v2.2 gibt es keine Flags
}; 
bool CID3_Frame::isEncrypted() 
{ 
	if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_3) // Flags wirken global auf alle Frames bei v2.3
		return ((flags & 64) == 64);
	if (CTools::ID3V2oldTagVersion == TAG_VERSION_2_4) 
		return ((flags & 4) == 4); 
	return 0; // bei id3v2.2 gibt es keine Flags	
};
bool CID3_Frame::isDataLenIndicator() // nur bei v2.4
{ 
	return (CTools::ID3V2oldTagVersion == TAG_VERSION_2_4) ? ((flags & 1) == 1) : 0;	
};