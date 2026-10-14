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
#include <errno.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CFLAC::CFLAC()
{
	covers.SetCount(0, 2);
	oldLen = 0;
	firstAudioPosition = 0;
	mustRebuild = false;
	blocksDamaged = false;
	blockTooBig = false;
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
	blocksDamaged = false;
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
		return (float)(CTools::FileSize / ((double)Samples * Channels * BitsPerSample / 8.0) * 100.0); // (in double: the product overflowed a long, 4.0.4.6 and earlier)
	
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
	memset(tmpHdr, 0, 4);
	if (_fseeki64(Stream, CTools::ID3v2Size, SEEK_SET) == 0 && fread(tmpHdr, 1, 4, Stream) == 4 &&
		memcmp(tmpHdr, FLAC_ID, 4) == 0)
	/* Process data if loaded and header valid */
	{
		// The walk ends at the block marked last. The end of the file, a block running past it, or more than 1024
		// blocks end it too, and the file is then not saved: what follows isn't known to be audio (a block of size 0
		// was taken as the last one, and a save put the new metadata in front of the remaining blocks, 4.0.4.6 and earlier)
		__int64 fileLength = _filelengthi64(_fileno(Stream));
		blocksDamaged = false;
		for (int count = 0; ; count++)
		{
			if (count >= 1024 || !ReadBlock(Stream, fileLength))
			{
				blocksDamaged = true;
				break;
			}
			if (BlockHeader.lastBlock)
				break;
		}
		firstAudioPosition = (long)_ftelli64(Stream);
		oldLen = firstAudioPosition - CTools::ID3v2Size;
		return true;
	}
	ResetData();
	return false;
}

//-------------------------------------------------------------------------

bool CFLAC::ReadBlockHeader(FILE *Stream)
{
	memset(tmpHdr, 0, 4);
	if (fread(tmpHdr, 1, 4, Stream) != 4)
		return false;
	BlockHeader.lastBlock = ((tmpHdr[0] & 0x80) == 0x80);
	// letztes Flag loeschen
	tmpHdr[0] &= 0x7F;
	BlockHeader.Type = tmpHdr[0];
	BlockHeader.Size = (tmpHdr[1] << 16) + (tmpHdr[2] << 8) + tmpHdr[3];
	// (a block of size 0 is valid, e.g. empty padding: it isn't the last block unless it says so)
	return true;
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

// Reads one metadata block; false at the end of the file or for a block that runs past it. Each block is read
// whole into memory and parsed from there, and the stream always goes on at the block's end (the comments were
// parsed from the file with no bound by the block's size, 4.0.4.6 and earlier).
bool CFLAC::ReadBlock(FILE *Stream, __int64 fileLength)
{
	if (!ReadBlockHeader(Stream))
		return false;
	int size = BlockHeader.Size;
	__int64 blockStart = _ftelli64(Stream);
	if (blockStart < 0 || blockStart + size > fileLength)
		return false;
	ATLTRACE(L"Block:%i  size:%i\n", BlockHeader.Type, size);
	CBlob block;
	if (BlockHeader.Type != METADATA_BLOCK_PADDING && size > 0)
	{
		block.FileRead(size, Stream);
		if ((int)block.GetLength() != size)
			return false;
	}
	switch (BlockHeader.Type)
	{
	case METADATA_BLOCK_STREAMINFO:
		BlockStreamInfo.Clear();
		BlockStreamInfo.AddBlob(block);
		if (BlockStreamInfo.GetLength() >= 18)
			AnalyzeStreamInfo();
		break;
	case METADATA_BLOCK_PADDING:
		break;
	case METADATA_BLOCK_COMMENT:
		BlockComment.Clear();
		AnalyzeVorbisComments(block, 0);
		break;
	case METADATA_BLOCK_PICTURE:
		{
			// a picture whose lengths don't fit its block isn't read as a cover; it is kept as it is, like an unknown block
			CFlacCover *cover = (block.GetLength() > 32) ? new CFlacCover(&block) : NULL;
			if (cover != NULL && cover->isValid())
			{
				covers.Add(cover);
				break;
			}
			delete cover;
			BlockOther.AddMemory(tmpHdr, 4);
			BlockOther.AddBlob(block);
		}
		break;
	default:
		BlockOther.AddMemory(tmpHdr, 4);
		BlockOther.AddBlob(block);
	};
	return _fseeki64(Stream, blockStart + size, SEEK_SET) == 0;
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
	// a block's size has 24 bits: a bigger one can't be written (it was cut to 24 bits, a broken file)
	blockTooBig = BlockComment.GetLength() > 4 + 0xFFFFFF;
	
	for (size_t i = 0; i < covers.GetCount(); i++)
	{
		BlockCover.Clear();
		covers[i]->Save(&BlockCover); 
		if (BlockCover.GetLength() > 0xFFFFFF)
			blockTooBig = true;
		BuildBlockHeader((int)BlockCover.GetLength(), METADATA_BLOCK_PICTURE);
		Daten.AddMemory(tmpHdr, 4);
		Daten.AddBlob(BlockCover);
	}
		
	Daten.AddBlob(BlockOther);
	Len = (int)Daten.GetLength();
	padding = oldLen - Len - 4;
	// (padding over 24 bits, after a big picture was removed: the file is written again with new padding)
	if (padding > 0 && padding <= 0xFFFFFF)
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
	if (blocksDamaged || oldLen <= 0)
	{
		// the metadata blocks didn't end with a last block before the end of the file (see ReadFromFile)
		CTools::instance().setLastError(ERR_INVALID_FORMAT);
		return false;
	}
	BuildFrame(true);
	if (blockTooBig)
	{
		CTools::instance().setLastError(ERR_FRAME_TOO_BIG);
		return false;
	}
	bool result = mustRebuild ? RebuildFile(FileName) : ReplaceTag(FileName);
	if (result)
	{
		// The new metadata's length, for a second save without a new analyze (it kept the old one: a rebuild's
		// longer metadata was then cut into, 4.0.4.6 and earlier). A rewrite in place keeps the length.
		oldLen = (long)Daten.GetLength();
		firstAudioPosition = CTools::ID3v2Size + oldLen;
	}
	return result;
}

//-------------------------------------------------------------------------

// Copies count bytes in blocks of 1 MB; false on a short read or write
static bool CopyFileBytes(FILE *Source, FILE *Destination, __int64 count)
{
	static const size_t blockSize = 1024 * 1024;
	CBlob buffer;
	while (count > 0)
	{
		size_t want = count > (__int64)blockSize ? blockSize : (size_t)count;
		buffer.FileRead(want, Source);
		if (buffer.GetLength() != want || fwrite(buffer.m_pData, 1, want, Destination) != want)
			return false;
		count -= want;
	}
	return true;
}

//-------------------------------------------------------------------------

// Whether the file still has "fLaC" at the metadata's start and a FLAC frame (or its end) right after the
// metadata as last read: a file changed since its analyze isn't written over with stale positions
static bool CheckLayout(FILE *Stream, __int64 start, __int64 audioStart)
{
	__int64 fileLength = _filelengthi64(_fileno(Stream));
	BYTE buf[4];
	if (start < 0 || audioStart <= start || audioStart > fileLength)
		return false;
	if (_fseeki64(Stream, start, SEEK_SET) != 0 || fread(buf, 1, 4, Stream) != 4 || memcmp(buf, FLAC_ID, 4) != 0)
		return false;
	if (audioStart == fileLength)
		return true;
	if (_fseeki64(Stream, audioStart, SEEK_SET) != 0 || fread(buf, 1, 2, Stream) != 2)
		return false;
	return buf[0] == 0xFF && (buf[1] & 0xFE) == 0xF8; // frame sync
}

//-------------------------------------------------------------------------

bool CFLAC::RebuildFile(LPCWSTR FileName)
{
	// The copy is written in blocks with every read and write checked, and replaces the original in one step only
	// when it is whole; the original is untouched otherwise (the whole file was read into memory unchecked, every
	// stream of the process closed with _fcloseall, and the original deleted before the rename, 4.0.4.6 and earlier)
	FILE *Source;
	FILE *Destination;
	CAtlString NewFileName(FileName);
	if ( (Source = _wfsopen(FileName, READ_ONLY, _SH_DENYNO)) == NULL)
	{
		CTools::instance().setLastError(errno);
		return false;
	}
	__int64 sourceSize = _filelengthi64(_fileno(Source));
	__int64 audioStart = (__int64)CTools::ID3v2Size + oldLen;
	if (!CheckLayout(Source, CTools::ID3v2Size, audioStart))
	{
		fclose(Source);
		CTools::instance().setLastError(ERR_INVALID_FORMAT);
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
	// wenn ID3v2-Tag vorhanden, den auch noch mitschleppen; dann der neue Block, dann die Audiodaten
	bool ok = _fseeki64(Source, 0, SEEK_SET) == 0 && CopyFileBytes(Source, Destination, CTools::ID3v2Size) &&
		Daten.FileWrite(Daten.GetLength(), Destination) == Daten.GetLength() &&
		_fseeki64(Source, audioStart, SEEK_SET) == 0 && CopyFileBytes(Source, Destination, sourceSize - audioStart);
	if (ok && _ftelli64(Destination) != CTools::ID3v2Size + (__int64)Daten.GetLength() + (sourceSize - audioStart))
		ok = false;
	if (ferror(Source))
		ok = false;
	if (fflush(Destination) != 0 || ferror(Destination))
		ok = false;
	if (fclose(Destination) != 0)
		ok = false;
	fclose(Source);

	if (!ok)
	{
		_wremove(NewFileName); // the original is untouched
		CTools::instance().setLastError(errno != 0 ? errno : EIO);
		return false;
	}

	/* Replace the original with the copy in one step: it is either the old file or the new one, never missing */
	if (!MoveFileExW(NewFileName, FileName, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
	{
		DWORD err = GetLastError();
		_wremove(NewFileName);
		CTools::instance().setLastError(err == ERROR_ACCESS_DENIED || err == ERROR_SHARING_VIOLATION ? EACCES : EIO);
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
		// (the new metadata is exactly as long as the old: checked against the file before it is written over)
		bool ok = (__int64)Daten.GetLength() == oldLen &&
			CheckLayout(Stream, CTools::ID3v2Size, (__int64)CTools::ID3v2Size + oldLen) &&
			_fseeki64(Stream, CTools::ID3v2Size, SEEK_SET) == 0 &&
			Daten.FileWrite(Daten.GetLength(), Stream) == Daten.GetLength() &&
			fflush(Stream) == 0 && !ferror(Stream);
		if (fclose(Stream) != 0)
			ok = false;
		if (!ok)
			CTools::instance().setLastError(ERR_INVALID_FORMAT);
		return ok;
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