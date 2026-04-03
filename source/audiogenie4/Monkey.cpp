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
#include "io.h"
#include "monkey.h"
#include "Blob.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMonkey::CMonkey()
{
}
CMonkey::~CMonkey()
{
}

void CMonkey::ResetData()
{
  /* Reset data */
  Header.Reset();
}

/* -------------------------------------------------------------------------- */

bool CMonkey::IsValid()
{
  /* Check for right Monkey"s Audio file data */
  return (Header.exists() &&
          Header.SampleRate > 0 &&
          Header.Channels > 0);
}

/* -------------------------------------------------------------------------- */

CAtlString CMonkey::GetFileVersion()
{
  _TCHAR Buf[16];

  /* Get encoder version */
  if (Header.VersionID == 0)
    return EMPTY;
  else
    _stprintf_s(Buf, 16, _T("%4.2f"), Header.VersionID / 1000.0);

  return CAtlString(Buf);
}

/* -------------------------------------------------------------------------- */

CAtlString CMonkey::GetCompression()
{
  /* Get compression level */
  return MONKEY_COMPRESSION[Header.CompressionID / 1000];
}

/* -------------------------------------------------------------------------- */

BYTE CMonkey::GetBits()
{
  /* Get number of bits per sample */
  if (!IsValid())
    return 0;
  if ((Header.Flags & MONKEY_FLAG_8_BIT) > 0)
    return 8;
  if ((Header.Flags & MONKEY_FLAG_24_BIT) > 0)
    return 24;
  return 16;
}

/* -------------------------------------------------------------------------- */

CAtlString CMonkey::GetChannelMode()
{
  /* Get channel mode */
  switch (Header.Channels)
  {
    case 1:
      return MONO;
    case 2:
      return STEREO;
    default:
      return UNKNOWN;
  }  
}

/* -------------------------------------------------------------------------- */

float CMonkey::GetPeak()
{
  /* Get peak level ratio */
  if (IsValid() && ((Header.Flags & MONKEY_FLAG_PEAK_LEVEL) > 0))
  {
    if (GetBits() == 16)
      return Header.PeakLevel / 32768.0f * 100.0f;
    if (GetBits() == 24)
      return Header.PeakLevel / 8388608.0f * 100.0f;
    return Header.PeakLevel / 128.0f * 100.0f;
  }
  return 0.0f;
}

/* -------------------------------------------------------------------------- */

long CMonkey::GetSamplesPerFrame()
{
  /* Get number of samples in a frame */
  if (!IsValid() )
    return 0;
  
  if (Header.VersionID >= 3950)
    return 73728l * 4l;
  if (Header.VersionID >=
      3900 ||
      (Header.VersionID >= 3800 &&
       Header.CompressionID == MONKEY_COMPRESSION_EXTRA_HIGH))
    return 73728l;
  else
    return 9216l;
}

/* -------------------------------------------------------------------------- */

long CMonkey::GetSamples()
{
  /* Get number of samples */
  if (!IsValid())
    return 0l;
  if (Header.VersionID >=3980)
    return (Header.Frames - 1) * Header.BlocksPerFrame + Header.FinalFrameBlocks;
  else
    return (Header.Frames - 1) * GetSamplesPerFrame() + Header.FinalSamples;
}

/* -------------------------------------------------------------------------- */

float CMonkey::GetDuration()
{
  /* Get song duration */
  if (!IsValid())
    return 0.0f;
  return (float)GetSamples() / Header.SampleRate;
}

/* -------------------------------------------------------------------------- */

float CMonkey::GetCompressionRatio()
{
  /* Get compression ratio */
  if (!IsValid())
    return 0.0f;
  long compressedSize = CTools::FileSize - CTools::ID3v2Size - CTools::ID3v1Size - CTools::APESize;
  long uncompressedSize = GetSamples() * GetChannels() * GetBits() / 8l;
  return (float) compressedSize / ( uncompressedSize + Header.HeaderDataBytes);
  //return FileSize /
  //  (GetSamples() * Header.Channels * GetBits() / 8.0f + 44.0f) * 100.0f;
}

/* -------------------------------------------------------------------------- */

long CMonkey::GetBitRate()
{
  if (GetDuration() > 0)
    return (long)(GetSamples() * Header.Channels * GetBits() / GetDuration() * GetCompressionRatio()) / 1000l;
  else
    return 0;
}
/* -------------------------------------------------------------------------- */

bool CMonkey::ReadFromFile(FILE *Stream)
{
  /* Read Monkey"s Audio header data */
  fseek(Stream, CTools::ID3v2Size, SEEK_SET);
  Header.ReadFromFile(Stream);
  if (Header.exists())
  {
    if ((Header.Flags & MONKEY_FLAG_PEAK_LEVEL) == 0)
      Header.PeakLevel = 0;
    if ((Header.Flags & MONKEY_FLAG_SEEK_ELEMENTS) == 0)
      Header.SeekElements = 0;
    return true;
  }
  ResetData();
  return false;
}

