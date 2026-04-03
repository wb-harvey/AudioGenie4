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

// MonkeyTagInfo.h: Schnittstelle für die Klasse CMonkeyTagInfo.
//
//////////////////////////////////////////////////////////////////////
#pragma once

static const char *MONKEY_TAG_ID = "MAC ";

class CMonkeyTagInfo
{
public:
  CMonkeyTagInfo();
  virtual ~CMonkeyTagInfo();
  //
  char ID[4];                                                /* Always "MAC " */
  WORD VersionID;                                            /* Version number * 1000 (3.91 = 3910) */
  WORD CompressionID;                                        /* Compression level code */
  WORD Flags;                                                /* Any format flags */
  WORD Channels;                                             /* Number of channels */
  long SampleRate;                                           /* Sample rate (hz) */
  long HeaderBytes;                                          /* Header length (without header ID) */
  long TerminatingBytes;                                     /* Extended data */
  long Frames;                                               /* Number of frames in the file */
  long FinalSamples;                                         /* Number of samples in the final frame */
  long PeakLevel;                                            /* Peak level (if stored) */
  long SeekElements;                                         /* Number of seek elements (if stored) */
  // newer Versions:
  WORD FormatFlags;
  long DescriptorBytes;
	long SeekTableBytes;
	long HeaderDataBytes;
	long APEFrameDataBytes;
	long APEFrameDataBytesHigh;
	long TerminatingDataBytes;
	char FileMD5[16];
  long BlocksPerFrame;
	long FinalFrameBlocks;
  WORD BitsPerSample;
  //
  bool ReadFromFile(FILE *Stream);
  void Reset();
  bool exists();
};
