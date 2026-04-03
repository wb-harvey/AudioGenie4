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

static const unsigned short CART_CODES[][2] = { 
	{    0,   4 }, // Version
	{    4,  64 }, // Title
	{   68,  64 }, // Artist
	{  132,  64 }, // CutID
	{  196,  64 }, // ClientID
	{  260,  64 }, // Category
	{  324,  64 }, // Classification
	{  388,  64 }, // OutCue
	{  452,  10 }, // StartDate
	{  462,   8 }, // StartTime
	{  470,  10 }, // EndDate
	{  480,   8 }, // EndTime
	{  488,  64 }, // ProducerAppID
	{  552,  64 }, // ProducerAppVersion
	{  616,  64 }, // UserDef
	{  680,   4 }, // LevelReference
	{  684,   8 }, // PostTimer 0
	{  692,   8 }, // PostTimer 1
	{  700,   8 }, // PostTimer 2
	{  708,   8 }, // PostTimer 3
	{  716,   8 }, // PostTimer 4
	{  724,   8 }, // PostTimer 5
	{  732,   8 }, // PostTimer 6
	{  740,   8 }, // PostTimer 7
	{  748, 276 }, // Reserved
	{ 1024,1024 }, // URL
	{ 2048,   0 }  // TagText
};

#define WAV_CART_VERSION 0
#define WAV_CART_TITLE 1
#define WAV_CART_ARTIST 2
#define WAV_CART_CUTID 3
#define WAV_CART_CLIENTID 4
#define WAV_CART_CATEGORY 5
#define WAV_CART_CLASSIFICATION 6
#define WAV_CART_OUTCUE 7
#define WAV_CART_STARTDATE 8
#define WAV_CART_STARTTIME 9
#define WAV_CART_ENDDATE 10
#define WAV_CART_ENDTIME 11
#define WAV_CART_PRODUCERAPPID 12
#define WAV_CART_PRODUCERAPPVERSION 13
#define WAV_CART_USERDEF 14
#define WAV_CART_LEVELREFERENCE 15
#define WAV_CART_POSTTIMER0 16
#define WAV_CART_POSTTIMER1 17
#define WAV_CART_POSTTIMER2 18
#define WAV_CART_POSTTIMER3 19
#define WAV_CART_POSTTIMER4 20
#define WAV_CART_POSTTIMER5 21
#define WAV_CART_POSTTIMER6 22
#define WAV_CART_POSTTIMER7 23
#define WAV_CART_RESERVED 24
#define WAV_CART_URL 25
#define WAV_CART_TAGTEXT 26

class CWAVCARTChunk :
	public CWAVChunk
{
public:
	CWAVCARTChunk(void);
	~CWAVCARTChunk(void);
	void setASCIIText(CAtlString newText, BYTE nr);
	CAtlString getASCIIText(BYTE nr);
};
