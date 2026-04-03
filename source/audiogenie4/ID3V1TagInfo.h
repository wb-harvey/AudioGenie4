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

// ID3V1TagInfo.h: Schnittstelle für die Klasse CID3V1TagInfo.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Blob.h"

#define MAX_MUSIC_GENRES       148        /* Max. number of music genres */
#define DEFAULT_GENRE          255            /* Index for default genre */

static const char *TAG_VERSION_1_0 = "1.0";     /* Index for ID3v1.0 tag */
static const char *TAG_VERSION_1_1 = "1.1";     /* Index for ID3v1.1 tag */

static const char *ID3V1_ID = "TAG";

/* Size constants */
#define ID3V1_TAG_SIZE      128        

static const LPCWSTR MUSIC_GENRE[] =
{
	/* Genre names */
	_T("Blues"), _T("Classic Rock"), _T("Country"), _T("Dance"), _T("Disco"), _T("Funk"), _T("Grunge"),
	_T("Hip-Hop"), _T("Jazz"), _T("Metal"), _T("New Age"), _T("Oldies"), _T("Other"), _T("Pop"), _T("R&B"),
	_T("Rap"), _T("Reggae"), _T("Rock"), _T("Techno"), _T("Industrial"), _T("Alternative"), _T("Ska"),
	_T("Death Metal"), _T("Pranks"), _T("Soundtrack"), _T("Euro-Techno"), _T("Ambient"),
	_T("Trip-Hop"), _T("Vocal"), _T("Jazz+Funk"), _T("Fusion"), _T("Trance"), _T("Classical"),
	_T("Instrumental"), _T("Acid"), _T("House"), _T("Game"), _T("Sound Clip"), _T("Gospel"), _T("Noise"),
	_T("AlternRock"), _T("Bass"), _T("Soul"), _T("Punk"), _T("Space"), _T("Meditative"),
	_T("Instrumental Pop"), _T("Instrumental Rock"), _T("Ethnic"), _T("Gothic"), _T("Darkwave"),
	_T("Techno-Industrial"), _T("Electronic"), _T("Pop-Folk"), _T("Eurodance"), _T("Dream"),
	_T("Southern Rock"), _T("Comedy"), _T("Cult"), _T("Gangsta"), _T("Top"), _T("Christian Rap"),
	_T("Pop & Funk"), _T("Jungle"), _T("Native American"), _T("Cabaret"), _T("New Wave"),
	_T("Psychedelic"), _T("Rave"), _T("Showtunes"), _T("Trailer"), _T("Lo-Fi"), _T("Tribal"),
	_T("Acid Punk"), _T("Acid Jazz"), _T("Polka"), _T("Retro"), _T("Musical"), _T("Rock & Roll"),
	_T("Hard Rock"), _T("Folk"), _T("Folk-Rock"), _T("National Folk"), _T("Swing"), _T("Fast Fusion"),
	_T("Bebob"), _T("Latin"), _T("Revival"), _T("Celtic"), _T("Bluegrass"), _T("Avantgarde"),
	_T("Gothic Rock"), _T("Progressive Rock"), _T("Psychedelic Rock"), _T("Symphonic Rock"),
	_T("Slow Rock"), _T("Big Band"), _T("Chorus"), _T("Easy Listening"), _T("Acoustic"), _T("Humour"),
	_T("Speech"), _T("Chanson"), _T("Opera"), _T("Chamber Music"), _T("Sonata"), _T("Symphony"),
	_T("Booty Bass"), _T("Primus"), _T("Porn Groove"), _T("Satire"), _T("Slow Jam"), _T("Club"),
	_T("Tango"), _T("Samba"), _T("Folklore"), _T("Ballad"), _T("Power Ballad"), _T("Rhythmic Soul"),
	_T("Freestyle"), _T("Duet"), _T("Punk Rock"), _T("Drum Solo"), _T("A capella"), _T("Euro-House"),
	_T("Dance Hall"), _T("Goa"), _T("Drum & Bass"), _T("Club-House"), _T("Hardcore"), _T("Terror"),
	_T("Indie"), _T("BritPop"), _T("Negerpunk"), _T("Polsk Punk"), _T("Beat"),
	_T("Christian Gangsta Rap"), _T("Heavy Metal"), _T("Black Metal"), _T("Crossover"),
	_T("Contemporary Christian"), _T("Christian Rock"), _T("Merengue"), _T("Salsa"),
	_T("Trash Metal"), _T("Anime"), _T("JPop"), _T("Synthpop")
};

class CID3V1TagInfo
{
private:
	CBlob *tmp;
public:
	CID3V1TagInfo();
	virtual ~CID3V1TagInfo();
	// Datenstruktur
	bool _exists;
	CAtlString Title;
	CAtlString Artist;
	CAtlString Album;
	CAtlString Comment;
	CAtlString Year;
	BYTE Genre;
	BYTE Track;
	// Methoden
	bool ReadFromFile(FILE *Stream);
	bool WriteToFile(FILE *Stream);
	void Reset();
	bool exists();
	CAtlString GetTagVersion();
	CAtlString GetTrack();
	void SetTrack(LPCWSTR newValue);
	void SetGenre(LPCWSTR newValue);
};

