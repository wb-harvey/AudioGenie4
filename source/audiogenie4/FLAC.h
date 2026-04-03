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
/* FLAC.h          for use with FLAC.cpp                                      */
/* by Stefan Toengi (c) 2003                                                  */
/* -------------------------------------------------------------------------- */
#pragma once

#include "Blob.h"
#include "Audio.h"
#include "vorbiscomment.h"
#include "FlacCover.h"
#include "atlcoll.h"

typedef unsigned __int32  u32;

static const char* FLAC_ID = "fLaC";

#define METADATA_BLOCK_STREAMINFO 0
#define METADATA_BLOCK_PADDING 1
#define METADATA_BLOCK_APPLICATION 2
#define METADATA_BLOCK_SEEKTABLE 3
#define METADATA_BLOCK_COMMENT 4
#define METADATA_BLOCK_CUESHEET 5
#define METADATA_BLOCK_PICTURE 6

struct strBlockHeader
{
  bool lastBlock;
  BYTE Type;
  int Size;
};

class CFLAC : public CAudio, public CVorbisComment
{
private:
  CBlob Daten, neu;
  CBlob BlockStreamInfo, BlockComment, BlockOther, BlockCover;
  BYTE tmpHdr[4];
  int Channels;
  long SampleRate;
  int BitsPerSample;
  long Samples;
  int minBlockSize;
  int maxBlockSize;
  long minFrameSize;
  long maxFrameSize;
  long VendorLength;
  long anzComments;
  long oldLen;
  long firstAudioPosition;
  bool mustRebuild;
  strBlockHeader BlockHeader;
  void ReadBlockHeader(FILE *Stream);
  void ReadBlock(FILE *Stream);
  void AnalyzeComment();
  void AnalyzeStreamInfo();
  void BuildComment();
  bool RebuildFile(LPCWSTR FileName);
  bool ReplaceTag(LPCWSTR FileName);
  void BuildBlockHeader(int Len, BYTE typ);
  void BuildFrame(bool withComment);
public:
  CFLAC();
  virtual ~CFLAC();
  void ResetData();
  bool ReadFromFile(FILE *Stream);
  bool SaveToFile(LPCWSTR FileName);
  bool IsValid();
  CAtlArray<CFlacCover *> covers;
  long GetBitRate();
  float GetDuration();
  float GetRatio();
  void DeletePictures();
  bool DeletePicture(short Index);
  CFlacCover* GetCover(short Index);
  bool Exists()                     { return (anzComments > 0); };
  long GetChannels()                { return Channels;      };
  long GetSampleRate()              { return SampleRate;    };
  int GetBitsPerSample()            { return BitsPerSample; };
  long GetSamples()                 { return Samples;       };
  int GetMinBlockSize()             { return minBlockSize;  };
  int GetMaxBlockSize()             { return maxBlockSize;  };
  long GetMinFrameSize()            { return minFrameSize;  };
  long GetMaxFrameSize()            { return maxFrameSize;  };  
  CAtlString GetTagVersion()        { return EMPTY;         };
  long GetFirstAudioPosition()      { return firstAudioPosition; };
  short GetPictureCount()			{ return (short)covers.GetCount();  };  
  bool replaceCover(CFlacCover *cover);
  
};
