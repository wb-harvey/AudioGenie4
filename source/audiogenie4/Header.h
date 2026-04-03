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

// Header.h: Schnittstelle für die Klasse CHeader.
//
//////////////////////////////////////////////////////////////////////
#pragma once

struct structft
{
	BYTE kennung[4];
	BYTE laenge;
	BYTE result;
};

// Audio-Formate
static enum AUDIO_FORMAT {
	AUDIO_FORMAT_UNKNOWN=0,
	AUDIO_FORMAT_MPEG,
	AUDIO_FORMAT_WMA,
	AUDIO_FORMAT_MONKEY,
	AUDIO_FORMAT_FLAC,
	AUDIO_FORMAT_WAV,
	AUDIO_FORMAT_OGGVORBIS,
	AUDIO_FORMAT_MPP,
	AUDIO_FORMAT_AAC,
	AUDIO_FORMAT_MP4,
	AUDIO_FORMAT_TTA,	
	AUDIO_FORMAT_WAVPACK,
	AUDIO_FORMAT_ID3V2,
	AUDIO_FORMAT_INVALID=99
};
#define MAX_FILETYPES 30

static const structft filetypes[MAX_FILETYPES] = { 
	{ 0x30, 0x26, 0xB2, 0x75,  4, AUDIO_FORMAT_WMA       },  // wma   48, 38, 178, 117
	{ 0x49, 0x44, 0x33, 0x00,  3, AUDIO_FORMAT_ID3V2     },  // id3    "id3"
	{ 0x52, 0x49, 0x46, 0x46,  4, AUDIO_FORMAT_WAV       },  // wav    "RIFF"
	{ 0x66, 0x4C, 0x61, 0x43,  4, AUDIO_FORMAT_FLAC      },  // flac   "fLaC"
	{ 0x4F, 0x67, 0x67, 0x53,  4, AUDIO_FORMAT_OGGVORBIS },  // ogg    "OggS"
	{ 0x4D, 0x41, 0x43, 0x20,  4, AUDIO_FORMAT_MONKEY    },  // monkey "MAC "
	{ 0x4D, 0x50, 0x2B, 0x00,  3, AUDIO_FORMAT_MPP       },  // mpp    "MP+"
	{ 0x41, 0x44, 0x49, 0x46,  4, AUDIO_FORMAT_AAC       },  // aac    "ADIF" 
	{ 0x54, 0x54, 0x41, 0x31,  4, AUDIO_FORMAT_TTA       },  // tta    "TTA1" 
	{ 0x54, 0x54, 0x41, 0x32,  4, AUDIO_FORMAT_TTA       },  // tta    "TTA2" 
	{ 0x77, 0x76, 0x70, 0x6B,  4, AUDIO_FORMAT_WAVPACK   },  // wavpack "wvpk"
	{ 0xFF, 0x4B, 0x45, 0x59,  4, AUDIO_FORMAT_INVALID   },  // keyboard.sys 	
	{ 0x89, 0x50, 0x4E, 0x47,  4, AUDIO_FORMAT_INVALID   },  // png
	{ 0xD0, 0xCF, 0x11, 0xE0,  4, AUDIO_FORMAT_INVALID   },  // word/excel
	{ 0x55, 0x52, 0x4C, 0x20,  4, AUDIO_FORMAT_INVALID   },  // ie cache
	{ 0x21, 0x42, 0x44, 0x4E,  4, AUDIO_FORMAT_INVALID   },  // outlook
	{ 0xCF, 0xAD, 0x12, 0xFE,  4, AUDIO_FORMAT_INVALID   },  // outlook express
	{ 0x25, 0x40, 0x44, 0x46,  4, AUDIO_FORMAT_INVALID   },  // pdf
	{ 0x49, 0x49, 0x2A, 0x00,  4, AUDIO_FORMAT_INVALID   },  // tif
	{ 0x49, 0x54, 0x53, 0x46,  4, AUDIO_FORMAT_INVALID   },  // chm
	{ 0x50, 0x4B, 0x03, 0x04,  4, AUDIO_FORMAT_INVALID   },  // jar, zip
	{ 0x52, 0x61, 0x72, 0x21,  4, AUDIO_FORMAT_INVALID   },  // rar
	{ 0xD0, 0xCF, 0x11, 0xE0,  4, AUDIO_FORMAT_INVALID   },  // msi
	{ 0x3F, 0x5F, 0x03, 0x00,  4, AUDIO_FORMAT_INVALID   },  // hlp
	{ 0x47, 0x49, 0x46, 0x38,  4, AUDIO_FORMAT_INVALID   },  // gif
	{ 0x4D, 0x53, 0x43, 0x46,  4, AUDIO_FORMAT_INVALID   },  // cab
	{ 0xCA, 0xFE, 0xBA, 0xBE,  4, AUDIO_FORMAT_INVALID   },  // class ( Java )
	{ 0x4D, 0x5A, 0x00, 0x00,  2, AUDIO_FORMAT_INVALID   },  // exe, dll, drv, ocx, scr
	{ 0x42, 0x4D, 0x00, 0x00,  2, AUDIO_FORMAT_INVALID   },  // bmp
	{ 0xFF, 0xD8, 0x00, 0x00,  2, AUDIO_FORMAT_INVALID   }   // jpg
};

class CHeader  
{
private:
	BYTE Buf[16];
public:
	CHeader();
	virtual ~CHeader();
	BYTE ReadFromFile(FILE *Stream);
};

