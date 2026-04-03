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

#include "stdafx.h"
#include "MPEGPlus.h"
#include "io.h"
#include <fcntl.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CMPEGPlus::CMPEGPlus()
{
}

CMPEGPlus::~CMPEGPlus()
{
}

/* -------------------------------------------------------------------------- */

bool CMPEGPlus::ReadHeader(FILE *Stream)
{
  fseek(Stream, CTools::ID3v2Size, SEEK_SET);
  /* Read header and get file size */
  fread(Header.ByteArray, 1, 12, Stream);
  /* if transfer is not complete */
  if (memcmp(Header.ByteArray, "MP+", 3) == 0)
  {
    memcpy(&Header.IntegerArray, &Header.ByteArray, 12);
    return true;
  }
  return false;
}

/* -------------------------------------------------------------------------- */

BYTE CMPEGPlus::GetStreamVersion()
{
  /* Get MPEGplus stream version */
  if (Header.IntegerArray[0] == STREAM_VERSION_7_ID)
    return 7;
  if (Header.IntegerArray[0] == STREAM_VERSION_71_ID)
    return 71;
  switch ((Header.ByteArray[1] % 32) / 2)
  {
    case  3: return 4;
    case  7: return 5;
    case 11: return 6;
  }
  return 0;
}

/* -------------------------------------------------------------------------- */

long CMPEGPlus::GetSampleRate()
{
  switch (Header.ByteArray[10] & 3)
  {
    case 0: return 44100l;
    case 1: return 48000l;
    case 2: return 37800l;
    case 3: return 32000l;
  }
  return 0l;
}
/* -------------------------------------------------------------------------- */

BYTE CMPEGPlus::GetChannelModeID()
{
  /* Get channel mode for stream version 7 */
  BYTE ver= GetStreamVersion();
  if (ver == 7 || ver == 71)
  {
    if ((Header.ByteArray[11] % 128) < 64)
      return MPP_CM_STEREO;
    else
      return MPP_CM_JOINT_STEREO;
  }
  else
  {
    /* Get channel mode for stream version 4-6 */
    if ((Header.ByteArray[2] % 128) == 0)
      return MPP_CM_STEREO;
    else
      return MPP_CM_JOINT_STEREO;
  }
}

/* -------------------------------------------------------------------------- */

long CMPEGPlus::GetFrameCount()
{
  /* Get frame count */
  BYTE Version = GetStreamVersion();
  if (Version == 4)
    return Header.IntegerArray[1] >> 16;
  if (Version == 5 || Version == 6 || Version == 7 || Version == 71)
    return Header.IntegerArray[1];
  return 0;
}

/* -------------------------------------------------------------------------- */

int CMPEGPlus::GetBitRateIntern()
{
  /*{ Try to get bit rate */
  BYTE Version = GetStreamVersion();
  if (Version == 4 || Version == 5)
    return (int)(Header.IntegerArray[0] >> 23);

  return 0;
}

/* -------------------------------------------------------------------------- */

BYTE CMPEGPlus::GetProfileID()
{
  BYTE ver = GetStreamVersion();
  /* Get MPEGplus profile (exists for stream version 7 only) */
  if (ver == 7 || ver == 71)
  {
    switch ( (Header.ByteArray[10] & 0xF0) >> 4)
    {
      case  1: return MPP_PROFILE_EXPERIMENTAL;
      case  5: return MPP_PROFILE_QUALITY0;
      case  6: return MPP_PROFILE_QUALITY1;
      case  7: return MPP_PROFILE_TELEPHONE;
      case  8: return MPP_PROFILE_THUMB;
      case  9: return MPP_PROFILE_RADIO;
      case 10: return MPP_PROFILE_STANDARD;
      case 11: return MPP_PROFILE_XTREME;
      case 12: return MPP_PROFILE_INSANE;
      case 13: return MPP_PROFILE_BRAINDEAD;
      case 14: return MPP_PROFILE_QUALITY9;
      case 15: return MPP_PROFILE_QUALITY10;
    }
  }
  return MPP_PROFILE_UNKNOWN;
}

/* -------------------------------------------------------------------------- */

void CMPEGPlus::ResetData()
{
  FValid = false;
  FChannelModeID = 0;
  FFrameCount = 0;
  FBitRate = 0;
  FStreamVersion = 0;
  FSampleRate = 0;
  FProfileID = MPP_PROFILE_UNKNOWN;
  memset(&Header, 0, sizeof(Header));
}

/* -------------------------------------------------------------------------- */

CAtlString CMPEGPlus::GetChannelMode()
{
  switch (FChannelModeID)
  {
    case MPP_CM_STEREO:
      return STEREO;
    case MPP_CM_JOINT_STEREO:
      return JOINTSTEREO;
    default:
      return UNKNOWN;
  }  
}

/* -------------------------------------------------------------------------- */

long CMPEGPlus::GetBitRate()
{
  unsigned long CompressedSize;
  /* Calculate bit rate if not given */
  CompressedSize = CTools::FileSize - CTools::ID3v2Size - CTools::ID3v1Size - CTools::APESize;
  if (FBitRate == 0 && FFrameCount > 0)
    return (long) (CompressedSize / 144l * (FSampleRate/1000l) / FFrameCount); // Neu ab V2.3.2
    /* OLD: return (long) (CompressedSize * 8l * (FSampleRate/1000l) / FFrameCount / 1152l); OLD computation */
  return FBitRate;
}

/* -------------------------------------------------------------------------- */

CAtlString CMPEGPlus::GetFileVersion()
{
  return MPP_PROFILE[FProfileID];
}

/* -------------------------------------------------------------------------- */

float CMPEGPlus::GetDuration()
{
  /* Calculate duration time */
  if (FSampleRate > 0)
    return (float) (FFrameCount * 1152.0f / FSampleRate);
  else
    return 0.0f;
}

/* -------------------------------------------------------------------------- */

bool CMPEGPlus::ReadFromFile(FILE *Stream)
{
  /* Process data if loaded and file valid */
  if (ReadHeader(Stream))
  {
    FValid = (GetStreamVersion() > 0);
    /* Fill properties with header data */
    FSampleRate = GetSampleRate();
    FChannelModeID = GetChannelModeID();
    FFrameCount = GetFrameCount();
    FBitRate = GetBitRateIntern();
    FStreamVersion = GetStreamVersion();
    FProfileID = GetProfileID();
    return true;
  }
  ResetData();
  return false;
}
