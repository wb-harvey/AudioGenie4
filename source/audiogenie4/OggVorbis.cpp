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
#include "OGGVORBIS.h"
#include "Blob.h"
#include "io.h"
#include <fcntl.h>
#include "OggHeader.h"
#include "vorbisheader.h"
#include <share.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COggVorbis::COggVorbis()
{
	HeadersRead = false;
	HeaderEndPos = 0;
	LastHeaderPageNumber = 0;
	FIsOpus = false;
	FPreSkip = 0;
	FInputSampleRate = 0;
}

COggVorbis::~COggVorbis()
{
}

/* -------------------------------------------------------------------------- */



/* ********************* Auxiliary functions & procedures ******************** */


/* --------------------------------------------------------------------------- */

void COggVorbis::ReadTag(FILE *Source)
{
	AnalyzeVorbisComments(Source);
	TagEndPos = ftell(Source);
	firstAudioPos = TagEndPos + 28; // 28 = geschätzter Wert
}

/* --------------------------------------------------------------------------- */

int COggVorbis::GetSamples(FILE *Source)
{
	int Index, DataIndex, Iterator;
	char Data[252];
	COGGHeader Header;
	/* Get total number of samples */
	for (Index = 1; Index < 50; Index++)
	{
		DataIndex = CTools::FileSize - (251 - 10) * Index - 10;
		fseek(Source, DataIndex, SEEK_SET);
		fread(Data, 1, 251, Source);
		/* Get number of PCM samples from last Ogg packet header */
		for (Iterator = (251 - 10); Iterator > 0; Iterator--)
		{
			if (memcmp(&Data[Iterator], OGG_PAGE_ID, 4) == 0)
			{
				fseek(Source, DataIndex + Iterator, SEEK_SET);
				Header.ReadFromFile(Source);
				return (long) Header.AbsolutePosition;
			}
		}
	}
	return 0;
}

/* --------------------------------------------------------------------------- */

bool COggVorbis::GetInfo(FILE *Source, bool withComments)
{
	/* Get info from file */
	HeadersRead = false;
	fseek(Source, CTools::ID3v2Size, SEEK_SET);
	if (FPage.ReadFromFile(Source))
	{
		fseek(Source, CTools::ID3v2Size + FPage.Segments + 27, SEEK_SET);
		// 4.0.6: an Opus identification header (RFC 7845: "OpusHead", version, channels, pre-skip, input sample
		// rate, output gain, mapping family), or a Vorbis one. Ogg Opus read as no file at all (4.0.5 and earlier).
		CBlob opusHead;
		opusHead.FileRead(19, Source);
		FIsOpus = opusHead.GetLength() == 19 && memcmp(opusHead.m_pData, OPUS_HEAD_ID, 8) == 0 &&
			(opusHead.m_pData[8] & 0xF0) == 0 && opusHead.m_pData[9] > 0;
		FPreSkip = 0;
		FInputSampleRate = 0;
		if (FIsOpus)
		{
			Parameters.Reset();
			Parameters.ChannelMode = opusHead.m_pData[9];
			Parameters.SampleRate = 48000;                // Opus decodes at 48 kHz; granule positions count it
			FPreSkip = opusHead.m_pData[10] | (opusHead.m_pData[11] << 8);
			FInputSampleRate = opusHead.GetR4B(12);
		}
		else
		{
			/* Read Vorbis parameter header */
			fseek(Source, CTools::ID3v2Size + FPage.Segments + 27, SEEK_SET);
			Parameters.ReadFromFile(Source);
		}
		if (FIsOpus || memcmp(Parameters.ID, VORBIS_PARAMETERS_ID, 7) == 0)
		{
			// The second page starts where the identification packet's page ends
			long firstPageData = 0;
			for (int i = 0; i < FPage.Segments; i++)
				firstPageData += FPage.LacingValues[i];
			SPagePos = CTools::ID3v2Size + 27 + FPage.Segments + firstPageData;
			fseek(Source, SPagePos, SEEK_SET);
			SPage.ReadFromFile(Source);
			// The comment packet is read whole: it spans several pages when it holds a picture
			HeadersRead = ReadHeaderPackets(Source);
			if (HeadersRead)
			{
				if (withComments && FIsOpus && CommentPacket.GetLength() >= 8 && memcmp(CommentPacket.m_pData, OPUS_TAGS_ID, 8) == 0)
					AnalyzeVorbisComments(CommentPacket, 8);
				else if (withComments && !FIsOpus && CommentPacket.GetLength() >= 7 && memcmp(CommentPacket.m_pData, VORBIS_TAG_ID, 7) == 0)
					AnalyzeVorbisComments(CommentPacket, 7);
				TagEndPos = HeaderEndPos;
				firstAudioPos = HeaderEndPos;
			}
			else
			{
				// Another logical stream in between, or a damaged header: the comments of a one-page packet can still
				// be read (the file is not saved, see SaveTag)
				fseek(Source, SPagePos + SPage.Segments + 27, SEEK_SET);
				fread(Tag.ID, 1, 7, Source);
				if (!FIsOpus && memcmp(&Tag.ID, VORBIS_TAG_ID, 7) == 0 && withComments)
					ReadTag(Source);
			}
			// Get total number of samples
			Samples = GetSamples(Source);
			return true;
		}
	}
	return false;
}

/* -------------------------------------------------------------------------- */

bool COggVorbis::ReadHeaderPackets(FILE *Source)
{
	/* Reads the comment and setup packets from the pages after the identification page. False when they can't be
	   replaced on their own: a page of another logical stream in between, audio on the setup packet's last page, or
	   a damaged or cut-off page. Opus has no setup packet: its comment packet (OpusTags) ends the header. */
	CommentPacket.Clear();
	SetupPacket.Clear();
	fseek(Source, SPagePos, SEEK_SET);
	int packet = 2;                                   // 2 = comment, 3 = setup
	const int lastPacket = FIsOpus ? 2 : 3;
	for (int pages = 0; packet <= lastPacket; pages++)
	{
		COGGHeader Page;
		if (pages >= 4096 || !Page.ReadFromFile(Source) || Page.Serial != FPage.Serial)
			return false;
		for (int i = 0; i < Page.Segments; i++)
		{
			if (packet > lastPacket)
				return false;
			BYTE len = Page.LacingValues[i];
			CBlob &packetData = (packet == 2) ? CommentPacket : SetupPacket;
			size_t before = packetData.GetLength();
			if (before + len > 64 * 1024 * 1024)
				return false;
			packetData.AddFile(len, Source);
			if (packetData.GetLength() != before + len)
				return false;
			if (len < 255)
				packet++;                             // a segment shorter than 255 ends the packet
		}
		LastHeaderPageNumber = Page.PageNumber;
		HeaderEndPos = ftell(Source);
	}
	return true;
}

/* -------------------------------------------------------------------------- */

void COggVorbis::BuildTag()
{
	Data.Clear();
	if (FIsOpus)
	{
		Data.AddMemory(OPUS_TAGS_ID, 8);              // 4.0.6: OpusTags has no framing bit
		BuildVorbisComments(Data);
		return;
	}
	Data.AddMemory(VORBIS_TAG_ID, 7);
	BuildVorbisComments(Data);
	Data.AddValue(1);                                 // framing bit: the packet is written whole
}

/* -------------------------------------------------------------------------- */

unsigned long COggVorbis::CalculateCRC(unsigned long CRC, BYTE Data[], long Size)
{
	long Index;
	// Calculate CRC through data
	for (Index = 0; Index < Size; Index++)
		CRC = (CRC << 8) XOR CRC_TABLE[((CRC >> 24) &0xff ) XOR Data[Index]];

	return CRC;
}

/* -------------------------------------------------------------------------- */

bool COggVorbis::WritePage(FILE *Destination, BYTE *page, size_t length)
{
	/* Sets the page checksum (computed with the checksum field zero) and writes the page */
	memset(page + 22, 0, 4);
	unsigned long crc = CalculateCRC(0, page, (long)length);
	page[22] = (BYTE)crc;
	page[23] = (BYTE)(crc >> 8);
	page[24] = (BYTE)(crc >> 16);
	page[25] = (BYTE)(crc >> 24);
	return fwrite(page, 1, length, Destination) == length;
}

/* -------------------------------------------------------------------------- */

static bool CopyBytes(FILE *Source, FILE *Destination, __int64 count)
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

bool COggVorbis::RebuildFile(LPCWSTR FileName)
{
	/* Writes the file again with the new comment packet. The comment and setup packets go on new pages (as many
	   as they need: a picture makes the comments longer than one page holds), the audio pages after them are
	   renumbered when the header now takes a different number of pages, and the copy replaces the original in
	   one step only when every read and write succeeded. Up to 4.0.2 the comments had to fit the second page:
	   longer ones overran its lacing table, crashing the process or writing a broken file */
	FILE *Source;
	FILE *Destination;
	CAtlString NewFileName(FileName);
	if ( (Source = _wfsopen(FileName, READ_ONLY, _SH_DENYNO)) == NULL)
	{
		CTools::instance().setLastError(errno);
		return false;
	}
	_fseeki64(Source, 0, SEEK_END);
	__int64 sourceSize = _ftelli64(Source);

	NewFileName+=TILDE;
	if ( (Destination = _wfsopen(NewFileName, READ_AND_WRITENEW, _SH_DENYWR)) == NULL)
	{
		CTools::instance().setLastError(errno);
		fclose(Source);
		return false;
	}

	// 1. Everything up to the comment packet's first page: anything in front of the stream, the identification page
	_fseeki64(Source, 0, SEEK_SET);
	bool ok = sourceSize >= HeaderEndPos && CopyBytes(Source, Destination, SPagePos);

	// 2. The comment and setup packets on new pages (Opus: the comment packet alone)
	CBlob *packets[2] = { &Data, &SetupPacket };
	const int packetCount = FIsOpus ? 1 : 2;
	int pageNumber = SPage.PageNumber;
	__int64 headerBytes = 0;
	BYTE lacing[255];
	int segments = 0;
	CBlob body;
	bool startsContinued = false;                     // the page goes on with a packet begun on the page before
	bool packetEnds = false;                          // a packet ends on the page
	for (int p = 0; p < packetCount && ok; p++)
	{
		size_t length = packets[p]->GetLength();
		size_t offset = 0;
		for (;;)
		{
			size_t segment = length - offset > 255 ? 255 : length - offset;
			lacing[segments++] = (BYTE)segment;
			if (segment > 0)
				body.AddMemory(packets[p]->m_pData + offset, segment);
			offset += segment;
			bool done = segment < 255;
			if (done)
				packetEnds = true;
			bool last = done && p == packetCount - 1;
			if (segments == 255 || last)
			{
				CBlob page;
				page.AddMemory(OGG_PAGE_ID, 4);
				page.AddValue(0);                     // stream structure version
				page.AddValue(startsContinued ? 1 : 0);
				// Granule position: 0 on header pages, -1 on a page where no packet ends
				page.AddR4B(packetEnds ? 0 : 0xFFFFFFFF);
				page.AddR4B(packetEnds ? 0 : 0xFFFFFFFF);
				page.AddR4B(FPage.Serial);
				page.AddR4B(pageNumber++);
				page.AddR4B(0);                       // checksum, set by WritePage
				page.AddValue((BYTE)segments);
				page.AddMemory(lacing, segments);
				page.AddBlob(body);
				ok = WritePage(Destination, page.m_pData, page.GetLength());
				headerBytes += page.GetLength();
				startsContinued = !done;
				packetEnds = false;
				segments = 0;
				body.Clear();
				if (!ok)
					break;
			}
			if (done)
				break;
		}
	}

	// 3. The pages after the header, renumbered if the header's page count changed
	int shift = (pageNumber - 1) - LastHeaderPageNumber;
	__int64 pos = HeaderEndPos;
	if (ok)
		ok = _fseeki64(Source, pos, SEEK_SET) == 0;
	if (ok && shift == 0)
		ok = CopyBytes(Source, Destination, sourceSize - pos);
	else
	{
		CBlob page;
		while (ok && pos < sourceSize)
		{
			page.Clear();
			page.AddFile(27, Source);
			bool isPage = page.GetLength() == 27 && memcmp(page.m_pData, OGG_PAGE_ID, 4) == 0;
			size_t pageLength = 0;
			if (isPage)
			{
				int pageSegments = page.m_pData[26];
				page.AddFile(pageSegments, Source);
				isPage = page.GetLength() == (size_t)(27 + pageSegments);
				if (isPage)
				{
					size_t bodyLength = 0;
					for (int i = 0; i < pageSegments; i++)
						bodyLength += page.m_pData[27 + i];
					page.AddFile(bodyLength, Source);
					pageLength = 27 + pageSegments + bodyLength;
					isPage = page.GetLength() == pageLength;
				}
			}
			if (!isPage)
			{
				// Not a whole page (a tag after the stream, or a cut-off end): copied as it is
				ok = _fseeki64(Source, pos, SEEK_SET) == 0 && CopyBytes(Source, Destination, sourceSize - pos);
				pos = sourceSize;
				break;
			}
			if (page.GetR4B(14) == FPage.Serial)
			{
				long number = page.GetR4B(18) + shift;
				memcpy(page.m_pData + 18, &number, 4);
				ok = WritePage(Destination, page.m_pData, pageLength);
			}
			else
				ok = fwrite(page.m_pData, 1, pageLength, Destination) == pageLength; // a stream chained after this one
			pos += pageLength;
		}
	}

	// The copy must hold everything: the old start, the new header pages and the rest of the old file
	if (ok && _ftelli64(Destination) != SPagePos + headerBytes + (sourceSize - HeaderEndPos))
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

/* -------------------------------------------------------------------------- */

void COggVorbis::ResetData()
{
	/* Reset variables */
	FPage.Reset();
	SPage.Reset();
	LPage.Reset();
	Parameters.Reset();
	Samples = 0;                                   /* Total number of samples */
	SPagePos = 0;                                  /* Position of second Ogg page */
	TagEndPos = 0;                                 /* Tag end position */
	FChannelModeID = 0;
	FSampleRate = 0;
	FBitRateNominal = 0;
	FSamples = 0;
	Data.Clear();
	firstAudioPos = 0;
	CommentPacket.Clear();
	SetupPacket.Clear();
	HeadersRead = false;
	HeaderEndPos = 0;
	LastHeaderPageNumber = 0;
	FIsOpus = false;
	FPreSkip = 0;
	FInputSampleRate = 0;
	CVorbisComment::ResetData();
}

/* --------------------------------------------------------------------------- */

float COggVorbis::FGetDuration()
{
	/* Calculate duration time */
	if (FSamples > 0)
		if (FSampleRate > 0)
		{
			// 4.0.6: in seconds with their fraction (an integer division cut it to whole seconds), and for Opus
			// without the pre-skip, which the granule positions count
			long samples = FIsOpus ? FSamples - FPreSkip : FSamples;
			return samples > 0 ? (float)((double)samples / FSampleRate) : 0.0f;
		}
		else
			return 0.0f;
	else if (FBitRateNominal > 0 && FChannelModeID > 0)
		return (float) (CTools::FileSize - CTools::ID3v2Size) /
		FBitRateNominal /
		FChannelModeID /
		125.0f * 2.0f;
	else
		return 0.0f;
}

/* --------------------------------------------------------------------------- */

int COggVorbis::FGetBitRate()
{
	/* Calculate average bit rate */
	float Duration = FGetDuration();
	if (Duration > 0)
		return (int) ((CTools::FileSize - CTools::ID3v2Size) / Duration / 125);
	else
		return 0;
}

/* --------------------------------------------------------------------------- */

bool COggVorbis::FIsValid()
{
	/* Check for file correctness */
	return ((FChannelModeID == VORBIS_CM_MONO ||
		FChannelModeID == VORBIS_CM_STEREO) &&
		FSampleRate >
		0 &&
		FGetDuration() >
		0.1f &&
		FGetBitRate() >
		0);
}

/* --------------------------------------------------------------------------- */

bool COggVorbis::ReadFromFile(FILE *Stream)
{
	/* Read data from file */
	if (GetInfo(Stream, true))
	{
		/* Fill variables */
		FChannelModeID = Parameters.ChannelMode;
		FSampleRate = Parameters.SampleRate;
		FBitRateNominal = int(Parameters.BitRateNominal / 1000);
		FSamples = Samples;
		return true;
	}
	return false;
}

/* --------------------------------------------------------------------------- */

bool COggVorbis::SaveTag(LPCWSTR FileName)
{
	// Save Vorbis tag
	bool Result = false;
	FILE *Source;
	if ( (Source = _wfsopen(FileName, READ_ONLY, _SH_DENYWR)) != NULL)
	{
		Result = GetInfo(Source, false);
		fclose(Source);
		if (Result && !HeadersRead)
		{
			// Its header pages can't be replaced on their own (see ReadHeaderPackets): the file is left alone
			CTools::instance().setLastError(EINVAL);
			Result = false;
		}
		if (Result)
		{
			FChannelModeID = Parameters.ChannelMode;
			FSampleRate = Parameters.SampleRate;
			FBitRateNominal = int(Parameters.BitRateNominal / 1000);
			FSamples = Samples;
			/* Prepare tag data and save to file */
			BuildTag();
			Result = RebuildFile(FileName);
		}
	}
	else
		CTools::instance().setLastError(errno);

	return Result;
}

/* -------------------------------------------------------------------------- */

bool COggVorbis::RemoveTag(LPCWSTR FileName)
{
	// Clear Vorbis tag excepts Vendor
	Data.Clear();
	CVorbisComment::ResetData();
	CVorbisComment::BuildVorbisComments(Data);
	return SaveTag(FileName);
}

/* -------------------------------------------------------------------------- */

CAtlString COggVorbis::GetChannelMode()
{ 
	switch (FChannelModeID)
	{
	case VORBIS_CM_MONO:
		return MONO;
	case VORBIS_CM_STEREO:
		return STEREO;
	case VORBIS_CM_MULTICHANNEL:
		return MULTICHANNEL;
	default:
		return UNKNOWN;
	}
};
