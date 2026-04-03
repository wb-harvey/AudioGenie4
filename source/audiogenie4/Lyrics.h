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

/* -------------------------------------------------------------------------- */
/* Lyrics.h          for use with Lyrics.cpp                                  */
/* by Stefan Toengi (c) 2003                                                  */
/* -------------------------------------------------------------------------- */
#pragma once

#include "Blob.h"
#include "id3v1taginfo.h"
#include "resource.h"
#include "Tools.h"

static const char *LYRICS_BEGIN = "LYRICSBEGIN";
static const char *LYRICS_OLD   = "LYRICSEND";
static const char *LYRICS_NEW   = "LYRICS200";

#define LYRICS_VERSION_UNKNOWN 0
#define LYRICS_VERSION_100     1
#define LYRICS_VERSION_200     2

#define LYRICS_FRAME_COUNT     9

static const char *FIELD_LIST[] = {
	"IND","LYR","INF","AUT","EAL","EAR","ETT","IMG","GRE" };

	class CLyrics
	{
	private:
		long FStartPosition;
		long FEndPosition;
		CAtlString FField[LYRICS_FRAME_COUNT];
		CBlob Data;
		BYTE ID3v1Tag[ID3V1_TAG_SIZE];
		BYTE FHeader[12];/* 9 Bytes Version 3 Bytes 'TAG' */
		BYTE FVersion;
		bool RemoveTag(LPCWSTR FileName);
		void SetTagItem(char ID[], long Pos, long DataSize);
		bool ReadHeader(FILE *Stream);
		void ReadFramesOld(FILE *Stream, bool isDeleting);
		void ReadFramesNew(FILE *Stream, bool isDeleting);
		bool SaveTag(LPCWSTR FileName);
	public:
		CLyrics();
		virtual ~CLyrics();
		void ReadFromFile(FILE *Stream);
		bool RemoveFromFile(LPCWSTR FileName);
		bool SaveToFile(LPCWSTR FileName);
		CAtlString GetTagVersion();
		bool Exists()                        { return (CTools::LyricsSize > 0); };
		long GetSize()                       { return CTools::LyricsSize;};
		void ResetData();
		CAtlString GetIndication()              { return FField[0]; };
		CAtlString GetLyrics()                  { return FField[1]; };
		CAtlString GetInformation()             { return FField[2]; };
		CAtlString GetAuthor()                  { return FField[3]; };
		CAtlString GetAlbum()                   { return FField[4]; };
		CAtlString GetArtist()                  { return FField[5]; };
		CAtlString GetTitle()                   { return FField[6]; };
		CAtlString GetImageLink()               { return FField[7]; };
		CAtlString GetGenre()                   { return FField[8]; };
		void SetIndication(LPCWSTR NewText)  { FField[0] = NewText;   };
		void SetLyrics(LPCWSTR NewText)      { FField[1] = NewText;   };
		void SetInformation(LPCWSTR NewText) { FField[2] = NewText;   };
		void SetAuthor(LPCWSTR NewText)      { FField[3] = NewText;   };
		void SetAlbum(LPCWSTR NewText)       { FField[4] = NewText;   };
		void SetArtist(LPCWSTR NewText)      { FField[5] = NewText;   };
		void SetTitle(LPCWSTR NewText)       { FField[6] = NewText;   };
		void SetImageLink(LPCWSTR NewText)   { FField[7] = NewText;   };
		void SetGenre(LPCWSTR NewText)       { FField[8] = NewText;   };
		long GetStartPosition()              { return FStartPosition; };
	};



