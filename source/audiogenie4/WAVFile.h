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
/* WAVFile.h          for use with WAVFile.cpp                                */
/* by Stefan Toengi (c) 2003                                                  */
/* -------------------------------------------------------------------------- */
#pragma once

#include "Blob.h"
#include "Audio.h"
#include "wavriffcontainer.h"
#include "wavchunk.h"
#include "wavcontainer.h"

static enum WAV_FRAMETYPES {
	WAV_IARL='IARL', // ArchivalLocation
	WAV_IART='IART', // Artist
	WAV_ICMS='ICMS', // Comissioned
	WAV_ICMT='ICMT', // Comment
	WAV_ICOP='ICOP', // Copyright
	WAV_ICRD='ICRD', // CreationDate
	WAV_ICRP='ICRP', // Cropped
	WAV_IDIM='IDIM', // Dimension
	WAV_IDPI='IDPI', // DotsPerInch
	WAV_IENG='IENG', // Engineer
	WAV_IGNR='IGNR', // Genre
	WAV_IKEY='IKEY', // Keywords
	WAV_ILGT='ILGT', // Lightness
	WAV_IMUS='IMUS', // Composer
	WAV_IMED='IMED', // Medium
	WAV_INAM='INAM', // Name
	WAV_IPLT='IPLT', // PaletteSetting
	WAV_IPRD='IPRD', // Product
	WAV_ISBJ='ISBJ', // Subject
	WAV_ISFT='ISFT', // Software
	WAV_ISHP='ISHP', // Sharpness
	WAV_ISRC='ISRC', // Source
	WAV_ISRF='ISRF', // SourceForm
	WAV_ITCH='ITCH', // Technician
	WAV_ITRK='ITRK'  // Track
};

/* Used with ChannelModeID property */
#define WAV_CM_MONO 1                                  /* Index for mono mode */
#define WAV_CM_STEREO 2                              /* Index for stereo mode */


class CWAVFile: public CAudio
{
private:

	CWAVRIFFContainer *mainContainer;
	bool ReadWAV(FILE *Stream);
	bool HeaderIsValid();
public:
	CWAVFile();
	virtual ~CWAVFile();
	void ResetData();
	CAtlString GetFormat();
	short GetFormatID();
	CAtlString GetChannelMode();
	float GetDuration();
	bool ReadFromFile(FILE *Stream);
	bool SaveToFile(LPCWSTR FileName);
	bool IsValid();
	long GetChannels();
	long GetSampleRate();
	long GetBytesPerSecond();
	WORD GetBlockAlign();
	WORD GetBitsPerSample();
	long GetHeaderSize();
	CAtlString getINFOChunkIDs();
	long GetBitRate();
	CAtlString getTextFrame(u32 FrameID);
	CAtlString getCartText(BYTE nr);
	void setTextFrame(u32 FrameID, CAtlString newText);
	void setCartText(BYTE nr, CAtlString newText);
	CAtlString getDisplayText();
	void setDisplayText(LPCWSTR newText);
	CAtlString getBextText(BYTE nr);
	void setBextText(BYTE nr, CAtlString newText);
};
