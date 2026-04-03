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
#include "stdio.h"
#include "io.h"
#include "flac.h"
#include "FlacCover.h"
#include "Blob.h"
#include <share.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CFLAC::CFLAC()
{
	covers.SetCount(0, 2);
}

CFLAC::~CFLAC()
{
	DeletePictures();
}

void CFLAC::ResetData()
{
	/* Reset data */
	Channels = 0;
	SampleRate = 0;
	BitsPerSample = 0;
	Samples = 0;
	minBlockSize = 0;
	maxBlockSize = 0;
	minFrameSize = 0;
	maxFrameSize = 0;
	oldLen = 0;
	BlockComment.Clear();
	BlockOther.Clear();
	BlockStreamInfo.Clear();
	Daten.Clear();
	BlockHeader.lastBlock = false;
	BlockHeader.Type = 0;
	BlockHeader.Size = 0;
	firstAudioPosition = 0;
	CVorbisComment::ResetData();
	DeletePictures();
}

/* -------------------------------------------------------------------------- */

bool CFLAC::IsValid()
{
	/* Check for right FLAC's Audio file data */
	return (Channels > 0 && SampleRate > 0 && BitsPerSample > 0 && Samples > 0);
}

//-------------------------------------------------------------------------

float CFLAC::GetDuration()
{
	/* Get song duration */
	if (IsValid())
		return Samples / (float) SampleRate;
	
	return 0.0f;	
}

/* -------------------------------------------------------------------------- */

float CFLAC::GetRatio()
{
	/* Get compression ratio */
	if (IsValid())
		return CTools::FileSize / (float) (Samples * Channels * BitsPerSample / 8.0f) * 100.0f;
	
	return 0.0f;	
}

/* -------------------------------------------------------------------------- */

long CFLAC::GetBitRate()
{
	long dur = (long) GetDuration();
	if (dur > 0)
		return (long) ((CTools::FileSize - CTools::ID3v2Size - CTools::ID3v1Size - oldLen) / 125 / dur + .5);
	return 0;
}

/* -------------------------------------------------------------------------- */

bool CFLAC::ReadFromFile(FILE *Stream)
{
	/* Read FLAC's Audio header data */
	fseek(Stream, CTools::ID3v2Size, SEEK_SET);
	fread(tmpHdr, 1, 4, Stream);
	/* Process data if loaded and header valid */
	if (memcmp(tmpHdr, FLAC_ID, 4) == 0)
	{
		do
		{
			ReadBlock(Stream);
		}
		while (!BlockHeader.lastBlock);
		firstAudioPosition = ftell(Stream);
		oldLen = firstAudioPosition - CTools::ID3v2Size;	
		return true;
	}
	ResetData();
	return false;
}

//-------------------------------------------------------------------------

void CFLAC::ReadBlockHeader(FILE *Stream)
{
	memset(tmpHdr, 0, 4);	
	fread(tmpHdr, 1, 4, Stream);	
	BlockHeader.lastBlock = ((tmpHdr[0] & 0x80) == 0x80);
	// letztes Flag löschen
	tmpHdr[0] &= 0x7F;
	BlockHeader.Type = tmpHdr[0];
	BlockHeader.Size = (tmpHdr[1] << 16) + (tmpHdr[2] << 8) + tmpHdr[3];
	if (BlockHeader.Size <= 0)
		BlockHeader.lastBlock = true;
}

//-------------------------------------------------------------------------

void CFLAC::BuildBlockHeader(int Len, BYTE typ)
{
	tmpHdr[0] = typ;
	tmpHdr[1] = (BYTE) (Len >> 16);
	tmpHdr[2] = (BYTE) (Len >> 8);
	tmpHdr[3] = (BYTE) (Len & 0xFF);
}

//-------------------------------------------------------------------------

void CFLAC::ReadBlock(FILE *Stream)
{
	ReadBlockHeader(Stream);
	int size = BlockHeader.Size;
	if (size <= 0)
		return;
	ATLTRACE(L"Block:%i  size:%i\n", BlockHeader.Type, size);
	switch (BlockHeader.Type)
	{
	case METADATA_BLOCK_STREAMINFO:
		BlockStreamInfo.FileRead(size, Stream);
		AnalyzeStreamInfo();
		break;
	case METADATA_BLOCK_PADDING:
		fseek(Stream, size, SEEK_CUR);
		break;
	case METADATA_BLOCK_COMMENT:
		BlockComment.Clear();
		AnalyzeVorbisComments(Stream);
		break;
	case METADATA_BLOCK_PICTURE:
		BlockCover.FileRead(size, Stream);
		if (BlockCover.GetLength() > 32)
			covers.Add(new CFlacCover(&BlockCover));		
		break;
	default:
		Daten.FileRead(size, Stream);
		BlockOther.AddMemory(tmpHdr, 4);
		BlockOther.AddBlob(Daten);
	};
}

//-------------------------------------------------------------------------

void CFLAC::AnalyzeStreamInfo()
{
	minBlockSize = BlockStreamInfo.Get2B(0);
	maxBlockSize = BlockStreamInfo.Get2B(2);
	minFrameSize = (long)BlockStreamInfo.Get3B(4);
	maxFrameSize = (long)BlockStreamInfo.Get3B(7);
	SampleRate = (long)(BlockStreamInfo.GetAt(10) << 12) + (BlockStreamInfo.GetAt(11) << 4) + (BlockStreamInfo.GetAt(12) >> 4);
	Channels = ((BlockStreamInfo.GetAt(12) >> 1) & 0x7) + 1;
	BitsPerSample = ((BlockStreamInfo.GetAt(12) & 1) << 4) + (BlockStreamInfo.GetAt(13) >> 4) + 1;
	Samples = (long)BlockStreamInfo.Get4B(14);
}

//-------------------------------------------------------------------------

void CFLAC::BuildComment()
{
	int Len = 0;
	anzComments = 0;
	BlockComment.Clear();
	neu.Clear();
	BuildVorbisComments(neu);
	Len = (long)neu.GetLength();
	BuildBlockHeader(Len, METADATA_BLOCK_COMMENT);
	BlockComment.AddMemory(tmpHdr, 4);
	BlockComment.AddBlob(neu); 
}

//-------------------------------------------------------------------------

void CFLAC::BuildFrame(bool withComment)
{
	int Len = 0, padding = 0;
	if (withComment)
		BuildComment();
	else
		BlockComment.Clear();

	// Ganzen Block neu zusammenstellen
	// zuerst fLaC dann StreamInfo dann Comment dann Pictures dann Sonstiges
	Daten.Clear();
	Daten.AddMemory(FLAC_ID, 4);
	BuildBlockHeader((int)BlockStreamInfo.GetLength(), METADATA_BLOCK_STREAMINFO);
	Daten.AddMemory(tmpHdr, 4);
	Daten.AddBlob(BlockStreamInfo);
	Daten.AddBlob(BlockComment);
	
	for (size_t i = 0; i < covers.GetCount(); i++)
	{
		BlockCover.Clear();
		covers[i]->Save(&BlockCover); 
		BuildBlockHeader((int)BlockCover.GetLength(), METADATA_BLOCK_PICTURE);
		Daten.AddMemory(tmpHdr, 4);
		Daten.AddBlob(BlockCover);
	}
		
	Daten.AddBlob(BlockOther);
	Len = (int)Daten.GetLength();
	padding = oldLen - Len - 4;
	if (padding > 0)
	{
		mustRebuild = false;
	}
	else
	{
		mustRebuild = true;
		padding = 1000;
	}
	// füge padding als letzten Block hinzu
	BuildBlockHeader(padding, METADATA_BLOCK_PADDING + 0x80);
	Daten.AddMemory(tmpHdr, 4);
	Daten.AddValue(0, padding);  
}

//-------------------------------------------------------------------------

bool CFLAC::SaveToFile(LPCWSTR FileName)
{
	BuildFrame(true);
	if (mustRebuild)
		return RebuildFile(FileName);
	return ReplaceTag(FileName);
}

//-------------------------------------------------------------------------

bool CFLAC::RebuildFile(LPCWSTR FileName)
{
	FILE *Source;
	FILE *Destination;
	CAtlString NewFileName(FileName);
	CBlob tmp;
	if ( (Source = _wfsopen(FileName, READ_ONLY, _SH_DENYNO)) == NULL)
	{
		CTools::instance().setLastError(errno);
		return false;
	}

	/* Create file streams */
	NewFileName+=TILDE;
	if ( (Destination = _wfsopen(NewFileName, WRITE_ONLY, _SH_DENYWR)) == NULL)
	{
		fclose(Source);
		CTools::instance().setLastError(errno);
		return false;
	};
	// wenn ID3v2-Tag vorhanden, den auch noch mitschleppen
	if (CTools::ID3v2Size > 0)
	{
		tmp.FileRead(CTools::ID3v2Size, Source);
		tmp.FileWrite(CTools::ID3v2Size, Destination);
	}
	// alten Block überspringen
	fseek(Source, oldLen, SEEK_CUR);
	Daten.FileWrite(Daten.GetLength(), Destination);
	Daten.FileRead(_filelength(_fileno(Source)), Source);
	Daten.FileWrite(Daten.GetLength(), Destination);
	_flushall();
	_fcloseall();
	CTools::instance().doEventsNow();
	/* Replace old file and delete temporary file */
	if (_wremove(FileName) != 0)
	{
		CTools::instance().setLastError(errno);
		return false;
	}
	CTools::instance().doEventsNow();
	if (_wrename(NewFileName, FileName) != 0)
	{
		CTools::instance().setLastError(errno);
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------

bool CFLAC::ReplaceTag(LPCWSTR FileName)
{
	FILE *Stream;
	if ( (Stream = _wfsopen(FileName, READ_AND_WRITE, _SH_DENYWR)) != NULL)
	{
		fseek(Stream, CTools::ID3v2Size, SEEK_SET);
		Daten.FileWrite(Daten.GetLength(), Stream);
		fflush(Stream);
		fclose(Stream);
		return true;
	}
	CTools::instance().setLastError(errno);
	return false;
}

CFlacCover* CFLAC::GetCover(short Index)
{
	if (covers.GetCount() == 0 || (size_t)Index > covers.GetCount() || Index < 1)
		return NULL;
	return covers.GetAt(Index - 1);
}

bool CFLAC::DeletePicture(short Index)
{
	if (covers.GetCount() == 0 || (size_t)Index > covers.GetCount() || Index < 1)
		return false;
	delete covers.GetAt(Index -1);
	covers.RemoveAt(Index - 1);
	return true;
}

void CFLAC::DeletePictures()
{
	for (size_t i = 0; i < covers.GetCount(); i++)
		delete covers[i];	
	covers.RemoveAll();
}

bool CFLAC::replaceCover(CFlacCover *cover)
{
	for (size_t i = 0; i < covers.GetCount(); i++)
	{
		if (cover->description.Compare(covers[i]->description) == 0)
		{
			delete covers[i];
			covers[i] = cover;
			return true;
		}
	}
	// Neu hinzugekommen
	covers.Add(cover);
	return false;
}