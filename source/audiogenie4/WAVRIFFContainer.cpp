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

#include "StdAfx.h"
#include "WAVRIFFContainer.h"
#include "WAVDataChunk.h"
#include "Tools.h"
#include "WAVBEXTChunk.h"


CWAVRIFFContainer::CWAVRIFFContainer(void)
{
	formatChunk = NULL;
	_chunkID = 0;
}

CWAVRIFFContainer::~CWAVRIFFContainer(void)
{
	Remove();
	delete formatChunk;
	formatChunk = NULL;
}

bool CWAVRIFFContainer::save(FILE* Source, FILE *Destination)
{
	if (getSize() > 0)
	{
		CBlob *blob = new CBlob(getSize() + 12);
		blob->Add4B('RIFF');
		blob->AddR4B((u32)getSize() - 8);
		blob->Add4B('WAVE');
		blob->FileWrite(blob->GetLength(), Destination);
		delete blob;
		size_t counts = _children.GetCount();
		for (size_t i = 0; i < counts; i++)
			_children[i]->save(Source, Destination);		
	}
	return true;
}

void CWAVRIFFContainer::save(CBlob *blob)
{
	// LIST ( 4 Bytes )
	// datasize ( 4 Bytes )
	// chunkID ( 4 Bytes )
	if (getSize() > 0)
	{
		blob->Add4B('RIFF');
		blob->AddR4B((u32)getSize() - 8);
		blob->Add4B('WAVE');
		size_t counts = _children.GetCount();
		for (size_t i = 0; i < counts; i++)
		{
			_children[i]->save(blob);		
		}
	}
}

bool CWAVRIFFContainer::load(FILE *Stream, u32 offset, u32 size)
{
	// Riff Container , Aufbau zwingend:
	// 4 Bytes 'RIFF'
	// 4 Bytes länge
	// 4 Bytes 'WAVE' als Text
	// dann die restlichen Chunks/Subchunks
	CBlob header;
	fseek(Stream, offset, SEEK_SET);
	header.FileRead(12, Stream);
	if (header.GetLength() != 12)
		return false;

	if (header.Get4B(0) != 'RIFF' || header.Get4B(8) != 'WAVE')
		return false;
	_chunkID = 'WAVE';
	offset+=12;
	u32 endPos = size;
	u32 chunkID, dataSize;
	long nextReadPos;
	while (offset + 8 < endPos)
	{
		fseek(Stream, offset, SEEK_SET);
		header.FileRead(8, Stream);
		chunkID = header.Get4B(0);
		dataSize = header.GetR4B(4);
		nextReadPos = offset + 8;
		if (nextReadPos + dataSize > endPos)
		{
			CTools::instance().writeWarning(L"corrupt or invalid chunk '%c%c%c%c' at position %i ignored", BYTE(chunkID >> 24), BYTE(chunkID >> 16), BYTE(chunkID >> 8), BYTE(chunkID), offset);
			offset = endPos;
		}
		else
		{
			CWAVChunk *chunk;
			if (chunkID == 'LIST')
				chunk = new CWAVContainer(chunkID);
			else if (chunkID == 'data')
				chunk = new CWAVDataChunk();
			else if (chunkID =='cart')
				chunk = new CWAVCARTChunk();
			else if (chunkID =='bext' )
				chunk = new CWAVBEXTChunk();
			else if (chunkID =='DISP')
				chunk = new CWAVDISPChunk();
			else if (chunkID == 'fmt ')
			{
				formatChunk = new CWAVFormatChunk();
				chunk = formatChunk;
			}
			else
				chunk = new CWAVChunk(chunkID);

			_children.Add(chunk);
			CTools::instance().doEvents();
			chunk->load(Stream, nextReadPos, dataSize);
			offset = nextReadPos + dataSize;
			if (offset % 2 == 1)
				offset++;

		}
	}
	return true;
}

void CWAVRIFFContainer::Remove()
{
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
		delete _children[i];
	_children.RemoveAll();
	_children.SetCount(0,10);
	formatChunk = NULL;
}

void CWAVRIFFContainer::addInfoChunk(u32 FrameID, CAtlString newText)
{
	// Suche den LIST/INFO Chunk
	CWAVContainer* container = getInfoChunk();
	// Neuanlage ? Füge vor data Chunk ein
	if (container == NULL)
	{
		size_t counts = _children.GetCount();
		for (size_t i = 0; i < counts; i++)
		{
			if (_children[i]->getID() == 'data')
			{
				container = new CWAVContainer('INFO');
				_children.InsertAt(i,container);
				i = 999;
			}
		}
	}
	if (container == NULL)
	{
		CTools::instance().setLastError(ERR_NO_DATACHUNK); 
		return; 
	}
	CWAVChunk* chunk = new CWAVChunk(FrameID);
	chunk->getData()->Clear();
	chunk->getData()->AddEncodedString(TEXT_ENCODED_ANSI, newText, false, true);
	container->_children.Add(chunk);
}

CWAVContainer* CWAVRIFFContainer::getInfoChunk()
{
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		if (_children[i]->getID() == 'INFO')
			return static_cast<CWAVContainer*>(_children[i]);
	}
	return NULL;
}

CWAVDISPChunk* CWAVRIFFContainer::getDispChunk()
{
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		// Suche nur DISP chunk mit Textinhalt
		if (_children[i]->getID() == 'DISP')
		{
			CWAVDISPChunk* result = static_cast<CWAVDISPChunk*>(_children[i]);
			if (result->getType() == CF_TEXT)
				return result;
		}
	}
	return NULL;
}

CWAVCARTChunk* CWAVRIFFContainer::getCartChunk()
{
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		if (_children[i]->getID() == 'cart')
			return static_cast<CWAVCARTChunk*>(_children[i]);
	}
	return NULL;
}

CWAVCARTChunk* CWAVRIFFContainer::addCartChunk()
{
	// Suche den CART Chunk
	CWAVCARTChunk* chunk = getCartChunk();
	// Neuanlage ? Füge vor data Chunk ein
	if (chunk == NULL)
	{
		size_t counts = _children.GetCount();
		for (size_t i = 0; i < counts; i++)
		{
			if (_children[i]->getID() == 'data')
			{
				chunk = new CWAVCARTChunk();
				_children.InsertAt(i, chunk);
				i = 999;
			}
		}
	}
	if (chunk == NULL)
		CTools::instance().setLastError(ERR_NO_DATACHUNK); 
	return chunk;
}

CWAVBEXTChunk* CWAVRIFFContainer::getBextChunk()
{
	size_t counts = _children.GetCount();
	for (size_t i = 0; i < counts; i++)
	{
		if (_children[i]->getID() == 'bext')
			return static_cast<CWAVBEXTChunk*>(_children[i]);
	}
	return NULL;
}

CWAVBEXTChunk* CWAVRIFFContainer::addBextChunk()
{
	// Suche den CART Chunk
	CWAVBEXTChunk* chunk = getBextChunk();
	// Neuanlage ? Füge vor data Chunk ein
	if (chunk == NULL)
	{
		size_t counts = _children.GetCount();
		for (size_t i = 0; i < counts; i++)
		{
			if (_children[i]->getID() == 'data')
			{
				chunk = new CWAVBEXTChunk();
				_children.InsertAt(i, chunk);
				i = 999;
			}
		}
	}
	if (chunk == NULL)
		CTools::instance().setLastError(ERR_NO_DATACHUNK); 
	return chunk;
}

CWAVDISPChunk* CWAVRIFFContainer::addDispChunk()
{
	// Suche den DISP Chunk mit Text
	CWAVDISPChunk* chunk = getDispChunk();
	// Neuanlage ? Füge am Ende an
	if (chunk == NULL)
	{
		chunk = new CWAVDISPChunk();
		_children.Add(chunk);			
	}
	return chunk;
}
