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
#include "wavchunk.h"

static const unsigned short BEXT_CODES[][2] = { 
	{    0, 256 }, // Description
	{  256,  32 }, // Originator
	{  288,  32 }, // OriginatorReference
	{  320,  10 }, // OriginationDate
	{  330,   8 }, // OriginationTime
	{  338,   8 }, // 4 Bytes TimeReferenceLow & 4 Bytes TimeReferenceHigh	 
	{  346,   2 }, // Version
	{  348,  64 }, // UMID_0 - UMID_63
	{  412, 190 }, // Reserved
	{  602,   0 } // CodingHistory[]	
};

#define WAV_BEXT_DESCRIPTION 0
#define WAV_BEXT_ORIGINATOR 1
#define WAV_BEXT_ORIGINATORREFERENCE 2
#define WAV_BEXT_ORIGINATIONDATE 3
#define WAV_BEXT_ORIGINATIONTIME 4
#define WAV_BEXT_TIMEREFERENCE 5
#define WAV_BEXT_VERSION 6
#define WAV_BEXT_UMID 7
#define WAV_BEXT_RESERVED 8
#define WAV_BEXT_CODINGHISTORY 9


class CWAVBEXTChunk :
	public CWAVChunk
{
public:
	CWAVBEXTChunk(void);
	~CWAVBEXTChunk(void);	
    void setASCIIText(CAtlString newText, BYTE nr);
	CAtlString getASCIIText(BYTE nr);	
};

