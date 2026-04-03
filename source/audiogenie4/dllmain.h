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

// dllmain.h : Deklaration der Modulklasse.
#include "Tools.h"
#include "ID3_Frame.h"
#include "id3f_apic.h"
#include "AAC.h"
#include "APE.h"
#include "Flac.h"
#include "ID3V1.h"
#include "ID3v2.h"
#include "Lyrics.h"
#include "Monkey.h"
#include "MPEGAudio.h"
#include "MPEGPlus.h"
#include "OggVorbis.h"
#include "WAVFile.h"
#include "WMA_File.h"
#include "mp4.h"
#include "Audio.h"
#include "header.h"
#include "md5tool.h"
#include "TTA.h"
#include "WavPack.h"

#define b2s(b)				(( b == 0 ) ? (0) : (-1))
#define getValidPointer(p)     ((p == 0 || wcslen(p) == 0) ? (EMPTY) : (p) )

static CAudio defaultaudio;
static CAAC aac;
static CAPE ape;
static CFLAC flac;
static CID3V1 id3v1;
static CID3V2 id3v2;
static CLyrics lyrics;
static CMonkey monkey;
static CMP4 mp4;
static CMPEGAudio mpeg;
static CMPEGPlus mpp;
static COggVorbis ogg;
static CWAVFile wav;
static CWMA_File wma;
static CMD5Tool md5Tool;
static CHeader header;
static CTTA tta;
static CWavPack wavpack;
static CAudio *audio; // Pointer auf die abgeleiteten Klassen
static CID3_Frame *id3frame;
static CAtlString endung, lastFile;
static CAtlString Title, Artist, Album, Comment, Genre, Track, Year, Composer;
static short Format;
static bool disablePopup;
static void ClearAllTags();

const GUID LIBID_AudioGenie4Lib = {0x6108400A,0xD5CA,0x46E4,{0x95,0x0F,0x91,0x5F,0x3A,0xC1,0x23,0x41}};
class CAudioGenie4Module : public CAtlDllModuleT< CAudioGenie4Module >
{
public :
	DECLARE_LIBID(LIBID_AudioGenie4Lib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_AudioGenie4, "{D9CE3DCF-67F5-4173-BF39-780CA989FC05}")
private:
	
};

extern class CAudioGenie4Module _AtlModule;


