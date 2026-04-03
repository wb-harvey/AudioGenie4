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

// MPEGAudio.h: Schnittstelle für die Klasse CMPEGAudio.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Blob.h"
#include "Audio.h"

/*{ Table for bit rates */
static const unsigned short MPEG_BIT_RATE [4][4][16] =
{
	/* For MPEG 2.5 */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,    0,
	0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160,   0,
	0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160,   0,
	0,  32,  48,  56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256,   0,
	/* Reserved */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	/* For MPEG 2 */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160,   0,
	0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160,   0,
	0,  32,  48,  56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256,   0,
	/* For MPEG 1 */
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,  32,  40,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320,   0,
	0,  32,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384,   0,
	0,  32,  64,  96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448,   0
};

/* Sample rate codes */
#define MPEG_SAMPLE_RATE_LEVEL_3 0                                 /* Level 3 */
#define MPEG_SAMPLE_RATE_LEVEL_2 1                                 /* Level 2 */
#define MPEG_SAMPLE_RATE_LEVEL_1 2                                 /* Level 1 */
#define MPEG_SAMPLE_RATE_UNKNOWN 3                           /* Unknown value */

/* Table for sample rates */
static const int MPEG_SAMPLE_RATE[4][4] =
{
	11025, 12000,  8000, 0,                                   /* For MPEG 2.5 */
	0,     0,     0, 0,                                       /* Reserved */
	22050, 24000, 16000, 0,                                   /* For MPEG 2 */
	44100, 48000, 32000, 0                                    /* For MPEG 1 */

};

/* VBR header ID for Xing/FhG */
static const char *VBR_ID_XING = "Xing";                       /* Xing VBR ID */
static const char *VBR_ID_FHG  = "VBRI";                       /* FhG VBR ID */

/* MPEG version codes */
#define MPEG_VERSION_2_5     0                                    /* MPEG 2.5 */
#define MPEG_VERSION_UNKNOWN 1                             /* Unknown version */
#define MPEG_VERSION_2       2                                      /* MPEG 2 */
#define MPEG_VERSION_1       3                                      /* MPEG 1 */

/* MPEG version names */
static const LPCWSTR MPEG_VERSION[] =
{ _T("MPEG 2.5"), UNKNOWN, _T("MPEG 2.0"), _T("MPEG 1.0") };

/* MPEG layer codes */
#define MPEG_LAYER_UNKNOWN   0                               /* Unknown layer */
#define MPEG_LAYER_III       1                                   /* Layer III */
#define MPEG_LAYER_II        2                                    /* Layer II */
#define MPEG_LAYER_I         3                                     /* Layer I */

/* Channel mode codes */
#define MPEG_CM_STEREO       0                                      /* Stereo */
#define MPEG_CM_JOINT_STEREO 1                                /* Joint Stereo */
#define MPEG_CM_DUAL_CHANNEL 2                                /* Dual Channel */
#define MPEG_CM_MONO         3                                        /* Mono */
#define MPEG_CM_UNKNOWN      4                                /* Unknown mode */

/* Extension mode codes (for Joint Stereo) */
#define MPEG_CM_EXTENSION_OFF     0                /* IS and MS modes set off */
#define MPEG_CM_EXTENSION_IS      1                    /* Only IS mode set on */
#define MPEG_CM_EXTENSION_MS      2                    /* Only MS mode set on */
#define MPEG_CM_EXTENSION_ON      3                 /* IS and MS modes set on */
#define MPEG_CM_EXTENSION_UNKNOWN 4                 /* Unknown extension mode */

/* Emphasis mode codes */
#define MPEG_EMPHASIS_NONE    0                                       /* None */
#define MPEG_EMPHASIS_5015    1                                   /* 50/15 ms */
#define MPEG_EMPHASIS_UNKNOWN 2                           /* Unknown emphasis */
#define MPEG_EMPHASIS_CCIT    3                                  /* CCIT J.17 */

/* Emphasis names */
static const LPCWSTR MPEG_EMPHASIS[] =
{ _T("None"), _T("50/15 ms"), UNKNOWN, _T("CCIT J.17") };

/* Encoder codes */
#define MPEG_ENCODER_UNKNOWN  0                            /* Unknown encoder */
#define MPEG_ENCODER_XING     1                                       /* Xing */
#define MPEG_ENCODER_FHG      2                                        /* FhG */
#define MPEG_ENCODER_LAME     3                                       /* LAME */
#define MPEG_ENCODER_BLADE    4                                      /* Blade */
#define MPEG_ENCODER_GOGO     5                                       /* GoGo */
#define MPEG_ENCODER_SHINE    6                                      /* Shine */
#define MPEG_ENCODER_QDESIGN  7                                    /* QDesign */

/* Enocders */
static const LPCWSTR MPEG_ENCODER[] = {
	UNKNOWN, _T("Xing"),_T("FhG"),_T("LAME"),
	_T("Blade"), _T("GoGo"),_T("Shine"),_T("QDesign") };

	/* MPEG layer names */
	static const LPCWSTR MPEG_LAYER[] =
	{ UNKNOWN, _T("Layer 3"), _T("Layer 2"), _T("Layer 1") };

	/* Limitation constants */
#define MAX_MPEG_FRAME_LENGTH 1729                  /* Max. MPEG frame length */
#define MIN_MPEG_BIT_RATE        8                     /* Min. bit rate value */
#define MAX_MPEG_BIT_RATE      448                     /* Max. bit rate value */
#define MIN_ALLOWED_DURATION     1                /* Min. song duration value */

	/* VBR Vendor ID strings */
static const char *VENDOR_ID_LAME     = "LAME";                   /* For LAME */
static const char *VENDOR_ID_GOGO_NEW = "GOGO";                   /* For GoGo (New) */
static const char *VENDOR_ID_GOGO_OLD = "MPGE";                   /* For GoGo (Old) */

static const BYTE ZERO[6] = { 0,0,0,0,0,0 };

#define DATASIZE  3460                        /* ca MAX_MPEG_FRAME_LENGTH * 2 */
static BYTE Data[DATASIZE];

struct VBRData
{
	BYTE ID[4];                                  /* Header ID: "Xing" or "VBRI" */
	bool Found;                                  /* True if VBR header found */
	long Frames;                                 /* Total number of frames */
	long Bytes;                                  /* Total number of bytes */
	BYTE Scale;                                  /* VBR scale (1..100) */
	BYTE VendorID[8];                            /* Vendor ID (if present) */
};

	/* MPEG frame header data */
struct tagFrameData
{
	long FramePosition;                            /*Frame position in the file */
	long FrameSize;                                /*Frame size (bytes) */
	BYTE VersionID;                                /* MPEG version ID */
	BYTE LayerID;                                  /* MPEG layer ID */
	BYTE BitRateID;                                /* Bit rate ID */
	BYTE SampleRateID;                             /* Sample rate ID */
	BYTE ModeID;                                   /* Channel mode ID */
	BYTE ModeExtensionID;                          /* Mode extension ID (for Joint Stereo) */
	BYTE EmphasisID;                               /* Emphasis ID */
	bool Found;                                    /*True if frame found */
	bool ProtectionBit;                            /* True if protected by CRC */
	bool PaddingBit;                               /* True if frame padded */
	bool PrivateBit;                               /* Extra information */
	bool CopyrightBit;                             /* True if audio copyrighted */
	bool OriginalBit;                              /* True if original media */
	bool Xing;                                     /* True if Xing encoder */
};

class CMPEGAudio: public CAudio
{
private:
	CBlob vendorValues;
	CAtlString Encoder;                                                /* Encoder */
	BYTE VendorID[10];                                               /* Vendor ID (if present) */
	long StartPosition;                                              /* Position of first valid Header */
	long totalBitrate;
	float secPerFrame;
	VBRData FVBR;
	tagFrameData Frame;
	/* internal Funcions */
	long Get4B(BYTE v[]);
	bool IsFrameHeader(BYTE HeaderData[] );
	WORD GetCoefficient();
	int GetPadding();
	long GetFrameLength();
	void DecodeHeader(BYTE HeaderData[]);
	bool ValidFrameAt(long Index, BYTE Data[] );
	bool IsXing(long Index, BYTE Data[] );
	void GetXingInfo(long Index, BYTE Data[] );
	void GetFhgInfo(long Index, BYTE Data[]);
	void FindVBR(long Index, BYTE Data[]);
	void GetInternEncoder();
	BYTE GetVBRDeviation();
	BYTE GetVBREncoderID();
	BYTE GetCBREncoderID();
	BYTE GetEncoderID();
	WORD GetBitRateID();
	void FindVendorID();
	bool FindFrame();
	bool SetBit(LPCWSTR FileName, int HdrPos, BYTE BitPos, bool neu);
	long firstAudioPos, lastAudioPos;
public:
	CMPEGAudio();
	virtual ~CMPEGAudio();
	void ResetData();
	bool ReadFromFile(FILE *Stream);
	/* Getter */
	CAtlString GetFileVersion();
	CAtlString GetLayer();
	CAtlString GetChannelMode();
	CAtlString GetEmphasis();
	CAtlString GetEncoder()      { return Encoder; };
	long GetBitRate();
	long GetSampleRate();
	long GetFrames();
	long GetChannels();
	float GetDuration();
	long GetFramePosition()   { return Frame.FramePosition; };
	long GetFrameSize()       { return Frame.FrameSize; };
	bool GetProtectionBit()   { return Frame.ProtectionBit; };
	bool GetPrivateBit()      { return Frame.PrivateBit; };
	bool GetCopyrightBit()    { return Frame.CopyrightBit; };
	bool GetOriginalBit()     { return Frame.OriginalBit; };
	bool GetPaddingBit()      { return Frame.PaddingBit; };
	bool IsVBR()              { return FVBR.Found; };
	bool SetPrivateBit(LPCWSTR FileName, bool neu);
	bool SetCopyrightBit(LPCWSTR FileName, bool neu);
	bool SetOriginalBit(LPCWSTR FileName, bool neu);
	bool IsValid();
	void ReadAllFrames(FILE *Stream);
	long GetFirstAudioPosition() { return firstAudioPos; };
	long GetLastAudioPosition()  { return lastAudioPos; };
	void setLastAudioPosition(long tmp) { lastAudioPos = tmp; };
};
