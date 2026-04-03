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


// AAC.h: Schnittstelle für die Klasse CAAC.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Blob.h"
#include "Audio.h"

/* Header type codes */
#define AAC_HEADER_TYPE_UNKNOWN 0                                  /* Unknown */
#define AAC_HEADER_TYPE_ADIF 1                                        /* ADIF */
#define AAC_HEADER_TYPE_ADTS 2                                        /* ADTS */

/* Header type names */
static const LPCWSTR AAC_HEADER_TYPE[] =
{ UNKNOWN, _T("ADIF"), _T("ADTS") };

/* MPEG version codes */
#define AAC_MPEG_VERSION_UNKNOWN 0                                 /* Unknown */
#define AAC_MPEG_VERSION_2 1                                        /* MPEG-2 */
#define AAC_MPEG_VERSION_4 2                                        /* MPEG-4 */

/* MPEG version names */
static const LPCWSTR AAC_MPEG_VERSION[] =
{ UNKNOWN, _T("MPEG-2"), _T("MPEG-4") };

/* Profile codes */
#define AAC_PROFILE_UNKNOWN 0                                      /* Unknown */
#define AAC_PROFILE_MAIN 1                                            /* Main */
#define AAC_PROFILE_LC 2                                                /* LC */
#define AAC_PROFILE_SSR 3                                              /* SSR */
#define AAC_PROFILE_LTP 4                                              /* LTP */

/* Profile names */
static const LPCWSTR AAC_PROFILE[] =
{ UNKNOWN, _T("AAC Main"), _T("AAC LC"), _T("AAC SSR"), _T("AAC LTP") };

/* Bit rate type codes */
#define AAC_BITRATE_TYPE_UNKNOWN 0                                 /* Unknown */
#define AAC_BITRATE_TYPE_CBR 1                                         /* CBR */
#define AAC_BITRATE_TYPE_VBR 2                                         /* VBR */

/* Bit rate type names */
static const LPCWSTR AAC_BITRATE_TYPE[] =
{ UNKNOWN, _T("CBR"), _T("VBR") };

static const char *ADIF = "ADIF";

/* Sample rate values */
static long SAMPLE_RATE[] =
{ 96000, 88200, 64000, 48000, 44100, 32000,
24000, 22050, 16000, 12000, 11025, 8000, 0, 0, 0, 0 };

class CAAC : public CAudio
{ 
private:
	BYTE FHeaderTypeID;
	BYTE FMPEGVersionID;
	BYTE FProfileID;
	BYTE FChannels;
	long FSampleRate;
	long FBitRate;
	BYTE FBitRateTypeID;
	BYTE RecognizeHeaderType(FILE *Source);
	void ReadADIF(FILE *Source);
	void ReadADTS(FILE *Source);
	long ReadBits(FILE *Source, long Position, long Count);
public:
	CAAC();
	virtual ~CAAC();
	void ResetData();
	bool ReadFromFile(FILE *Stream);                       /* Load header */
	BYTE GetHeaderTypeID()   { return FHeaderTypeID; };    /* Header type code */
	CAtlString GetHeaderType();                               /* Header type name */
	BYTE GetMPEGVersionID()   { return FMPEGVersionID; };  /* MPEG version code */
	CAtlString GetFileVersion();                              /* MPEG version name */
	CAtlString GetProfile();                                  /* Profile name */
	long GetChannels()      { return FChannels; };         /* Number of channels */
	long GetSampleRate()      { return FSampleRate; };     /* Sample rate (hz) */
	long GetBitRate()         { return FBitRate/1000; };   /* Bit rate (kBit/s) */
	CAtlString GetBitRateType();                              /* Bit rate type name */
	float GetDuration();                                   /* Duration (seconds) */
	bool IsValid();                                        /* True if data valid */
};

