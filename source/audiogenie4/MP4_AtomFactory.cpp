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
#include "mp4_atomfactory.h"
#include "mp4_ftyp.h"
#include "mp4_container.h"
#include "mp4_mdhd.h"
#include "mp4_soun.h"
#include "mp4_mdat.h"
#include "mp4_hdlr.h"
#include "Tools.h"
#include "id3v1taginfo.h"
#include "MP4_STCO.h"

CMP4_AtomFactory *CMP4_AtomFactory::factory = 0;
int CMP4_AtomFactory::count = 0;
long CMP4_AtomFactory::firstAudioPos = 0;
long CMP4_AtomFactory::lastAudioPos = 0;
long CMP4_AtomFactory::mediaLength = 0;
long CMP4_AtomFactory::lastOffset = 0;
CMP4_MDHD* CMP4_AtomFactory::lastMDHDAtom = NULL;
bool CMP4_AtomFactory::loadError = false;

CMP4_AtomFactory::CMP4_AtomFactory(void)
{
}

CMP4_AtomFactory::~CMP4_AtomFactory(void)
{
}

CMP4_AtomFactory *CMP4_AtomFactory::instance()
{
	if(!factory)
	{
		factory = new CMP4_AtomFactory;		
		factory->ebene = 0;	
	}
	return factory;
}

CMP4Atom* CMP4_AtomFactory::createAtom(u32 atomID) const
{
	switch (atomID)
	{
	case MP4_MOOV: return new CMP4_Container(MP4_MOOV);
	case MP4_UDTA: return new CMP4_Container(MP4_UDTA);
	case MP4_META: return new CMP4_Container(MP4_META);
	case MP4_FTYP: return new CMP4_FTYP();
	case MP4_ILST: return new CMP4_Container(MP4_ILST);
	case MP4_MDAT: return new CMP4_MDAT();
	case MP4_MDHD: 
		{
			CMP4_MDHD* mdhd = new CMP4_MDHD();
			factory->lastMDHDAtom = mdhd;
			return mdhd; 
		}
	case MP4_MDIA: return new CMP4_Container(MP4_MDIA);	
	case MP4_MINF: return new CMP4_Container(MP4_MINF);
	case MP4_COVR: return new CMP4_Container(MP4_COVR);
	case MP4_HDLR: return new CMP4_HDLR();
	case MP4_SOUN: return new CMP4_SOUN();
	case MP4_STBL: return new CMP4_Container(MP4_STBL);
	case MP4_STCO: return new CMP4_STCO();
	case MP4_CO64: return new CMP4_STCO(MP4_CO64);
	case MP4_TRAK:
		// a new track: an smhd in it must not mark the previous track's mdhd (or one already freed) as sound
		factory->lastMDHDAtom = NULL;
		return new CMP4_Container(MP4_TRAK);
	// vmhd is a plain full box (12 data bytes), not a container: read as one, its data was lost on save
	// (4.0.4.6 and earlier)
	case MP4_VMHD: return new CMP4Atom(MP4_VMHD);
	case MP4_SMHD:
		{
			if (factory->lastMDHDAtom != NULL)
				factory->lastMDHDAtom->isSoundAtom = true;
			return new CMP4Atom(MP4_SMHD);
		}
	//case MP4_ITUN: return new CMP4_Tunes(); 
	default:
		return new CMP4Atom(atomID);
	};
}

CAtlString CMP4_AtomFactory::getText(CMP4Atom* atom)
{
	if (atom->_blob.GetLength() <= 16)
		return EMPTY;
	// 4 bytes length
	// 4 bytes 'data' 
	// 4 bytes version/flags = byte hex version + 24-bit hex flags
	// (current version = 0 ; contains text flag = 0x000001)
	// 4 bytes reserved = 32-bit value set to zero
	size_t ln = atom->_blob.GetLength() - 16;
	// long data = atom->_blob.Get4B(4);
	long version = atom->getVersion();
	// Version: 1=text, 21=uint8 0=uint8
	if (version == 1)
	{
		CBlob tmp;
		tmp.AddMemory(atom->_blob.m_pData + 16, ln);
		return tmp.ConvertToUnicodeString(TEXT_ENCODED_UTF8);
	}

	if (version == 0 || version == 21) // Binary Data
	{
		CAtlString result;
		// a big-endian unsigned integer (type 21): only the low byte of 2 was read, 4 came back signed and 8
		// was read as a track pair (4.0.4.6 and earlier)
		if (version == 21 && (ln == 2 || ln == 4 || ln == 8) && atom->getFrameID() != 'gnre')
		{
			unsigned __int64 value = 0;
			for (size_t i = 0; i < ln; i++)
				value = (value << 8) | atom->_blob.m_pData[16 + i];
			result.Format(_T("%I64u"), value);
			return result;
		}
		if (ln == 1)
			result.Format(_T("%i"), (BYTE)atom->_blob.m_pData[16]);
		else if (ln == 2 && atom->getFrameID() == 'gnre')
		{
			BYTE genreID = (BYTE)atom->_blob.m_pData[17];
			if (genreID > 0 && genreID <= MAX_MUSIC_GENRES)
				result = MUSIC_GENRE[genreID - 1];
		}
		else if (ln == 2)
			result.Format(_T("%i"), atom->_blob.Get2B(16));
		else if (ln == 4)
			result.Format(_T("%i"), atom->_blob.Get4B(16)); // (BYTE)atom->_blob.m_pData[19]);
		else if (ln ==6 || ln == 8)
		{
			// track/disc number and count are 16-bit: above 255 only the low byte was read (4.0.4.6 and earlier)
			if (atom->_blob.Get2B(20) >  0)
				result.Format(_T("%i/%i"), atom->_blob.Get2B(18), atom->_blob.Get2B(20));
			else
				result.Format(_T("%i"), atom->_blob.Get2B(18));
		}
		else  // unbekannte Länge, was nun ?
					// unknown length -> what can we do?
		{
			CBlob tmp;
			tmp.AddMemory(atom->_blob.m_pData + 16, ln);
			result = tmp.ConvertToUnicodeString(TEXT_ENCODED_UTF8);
		}		
		return result;
	}

	// Hoppla, unbekanntes Format
	// ooops, unknown format
	CTools::instance().setLastError(ERR_ATOM_IS_NO_TEXTCLASS);
	return EMPTY;
}

// The width of an integer item (type 21), 0 for any other item. Written as UTF-8 text (type 1), players did not
// read these, and tmpo kept only its low byte (4.0.4.6 and earlier).
static int IntegerItemSize(u32 id)
{
	switch (id)
	{
	case 'cpil': case 'pgap': case 'pcst': case 'stik': case 'rtng': case 'hdvd': case 'shwm': case 'akID':
		return 1;
	case 'tmpo':
		return 2;
	case 'tvsn': case 'tves': case 'cnID': case 'atID': case 'geID': case 'sfID': case 'cmID':
		return 4;
	case 'plID':
		return 8;
	}
	return 0;
}

// a 16-bit number for a track or disc pair
static int PairValue(CAtlString text)
{
	int value = _wtoi(text);
	return (value < 0) ? 0 : (value > 0xFFFF) ? 0xFFFF : value;
}

// false (and the atom left as it was) for an item this can't write: SetILSTText then replaced the item with an
// empty atom, wiping it, and an empty covr was then read as a picture container and crashed (4.0.4.6 and earlier)
bool CMP4_AtomFactory::setText(CMP4Atom *atom, CAtlString newText)
{
	u32 id = atom->getFrameID();
	CBlob tmp;
	BYTE type = 1;
	if (id == 'trkn' || id == 'gnre')
	{
		CTools::instance().writeError(_T("don't use MP4SetTextFrameW with trkn frame or gnre frame. Use MP4SetTrackW or MP4SetGenreW instead!"));
		return false;
	}
	if (id == 'covr')
	{
		CTools::instance().writeError(_T("don't use MP4SetTextFrameW with 'covr' frame. Use MP4AddPictureArrayW or MP4AddPictureFileW instead!"));
		return false;
	}
	int intSize = IntegerItemSize(id);
	if (intSize > 0) // big-endian, intSize bytes
	{
		type = 21;
		unsigned __int64 value = (unsigned __int64)_wtoi64(newText);
		for (int i = intSize - 1; i >= 0; i--)
			tmp.AddValue((BYTE)(value >> (i * 8)));
	}
	else if (id == 'disk') // 6 Byte x/y
	{
		type = 0;
		int pos = newText.Find('/');
		tmp.AddValue(0, 2);
		if (pos == -1) // not found
		{
			tmp.Add2B(PairValue(newText));
			tmp.AddValue(0, 2);
		}
		else
		{
			tmp.Add2B(PairValue(newText.Left(pos)));
			tmp.Add2B(PairValue(newText.Mid(pos + 1)));
		}
	}
	else if (id == 'purl' || id == 'egid')
	{
		type = 0;
		tmp.AddEncodedString(TEXT_ENCODED_UTF8, newText, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
	}
	else
	{
		type = 1;
		tmp.AddEncodedString(TEXT_ENCODED_UTF8, newText, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
	}
		
	buildData(atom,(int)tmp.GetLength(), type);
	atom->_blob.AddMemory(tmp.m_pData, tmp.GetLength());
	return true;
}

// number and count are 16-bit (they were written as one byte each, 4.0.4.6 and earlier)
void CMP4_AtomFactory::setTrack(CMP4Atom *atom, int von, int bis)
{
	buildData(atom, 8, 0);
	atom->_blob.AddValue(0, 2);
	atom->_blob.Add2B(von);
	atom->_blob.Add2B(bis > 0 ? bis : 0);
	atom->_blob.AddValue(0, 2);
}

void CMP4_AtomFactory::buildData(CMP4Atom *atom, int length, BYTE flag)
{
	atom->_blob.Clear();
	atom->_blob.Add4B(16 + length);
	atom->_blob.Add4B('data');
	atom->_blob.Add4B(flag);
	atom->_blob.Add4B(0);
}
void CMP4_AtomFactory::setItuneText(CMP4Atom *atom, CAtlString frame, CAtlString newText)
{
	CBlob tmp;
	atom->_blob.Clear();
	atom->_blob.Add4B(28);
	atom->_blob.Add4B('mean');
	atom->_blob.Add4B(0); // Reserved
	atom->_blob.Add4B('com.');
	atom->_blob.Add4B('appl');
	atom->_blob.Add4B('e.iT');
	atom->_blob.Add4B('unes');
	// Atom "name"
	tmp.Clear();
	tmp.AddEncodedString(TEXT_ENCODED_UTF8, frame, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
	atom->_blob.Add4B(tmp.GetLength() + 12);
	atom->_blob.Add4B('name');
	atom->_blob.Add4B(0); // Reserved
	atom->_blob.AddBlob(tmp);
	// Atom "data"
	tmp.Clear();
	tmp.AddEncodedString(TEXT_ENCODED_UTF8, newText, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
	atom->_blob.Add4B(tmp.GetLength() + 16);
	atom->_blob.Add4B('data');
	atom->_blob.Add4B(1); // Version Text
	atom->_blob.Add4B(0); // Reserved
	atom->_blob.AddBlob(tmp);
}
// The text of a freeform ("----") atom's 'name' or 'data' box, found by walking its boxes: the name was read at
// byte 40 and the data 15 bytes after it, right only when the 'mean' box is exactly "com.apple.iTunes" (28
// bytes); another naming authority (or a mean written another way) read garbage (4.0.4.2 and earlier).
static CAtlString freeformText(CMP4Atom *atom, u32 boxType)
{
	CBlob &b = atom->_blob;
	size_t len = b.GetLength();
	size_t pos = 0;
	while (pos + 12 <= len)
	{
		u32 size = (u32)b.Get4B(pos);
		u32 type = (u32)b.Get4B(pos + 4);
		// (size > len - pos, not pos + size > len: a huge size wrapped the sum around in the 32-bit build, and
		// the walk went back to the start for ever on a damaged atom)
		if (size < 12 || size > len - pos)
			break;
		if (type == boxType)
		{
			// 'mean', 'name': size, type, version/flags; 'data': size, type, type indicator, locale
			size_t textStart = pos + (boxType == 'data' ? 16 : 12);
			if (textStart >= pos + size)
				return EMPTY;
			int n = (int)(pos + size - textStart);
			const char *utf8 = (const char *)(b.m_pData + textStart);
			// up to the first NUL, as before 4.0.4.3 (a tagger may end the text with one)
			const char *nul = (const char *)memchr(utf8, 0, n);
			if (nul != NULL)
				n = (int)(nul - utf8);
			if (n <= 0)
				return EMPTY;
			int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8, n, NULL, 0);
			if (wlen <= 0)
				return EMPTY;
			CAtlString text;
			MultiByteToWideChar(CP_UTF8, 0, utf8, n, text.GetBuffer(wlen), wlen);
			text.ReleaseBuffer(wlen);
			return text;
		}
		pos += size;
	}
	return EMPTY;
}

CAtlString CMP4_AtomFactory::getiTuneText(CMP4Atom *atom)
{
	return freeformText(atom, 'data');
}
CAtlString CMP4_AtomFactory::getiTuneFrame(CMP4Atom *atom)
{
	return freeformText(atom, 'name');
}
CAtlString CMP4_AtomFactory::getiTuneMean(CMP4Atom *atom)
{
	return freeformText(atom, 'mean');
}

