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

// CMP4.cpp: Implementierung der Klasse CMP4.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdlib.h"
#include "stdio.h"
#include "mp4.h"
#include <io.h>
#include <memory.h>
#include <fcntl.h>
#include "resource.h"
#include <share.h>
#include "id3v1taginfo.h"
#include "mp4_atomfactory.h"
#include "mp4_soun.h"
#include "mp4_mdhd.h"
#include "mp4_mdat.h"
#include "mp4_stco.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CMP4::CMP4()
{
	mainContainer = new CMP4_MainContainer();
}

CMP4::~CMP4()
{
	mainContainer->remove();
	delete mainContainer;
}

void CMP4::ResetData()
{
	/* Reset variables */
	mainContainer->remove();
	CMP4_AtomFactory::lastOffset = 0;
	CMP4_AtomFactory::mediaLength = 0;
	CMP4_AtomFactory::firstAudioPos = 0;
	CMP4_AtomFactory::lastAudioPos = 0;
}

bool CMP4::ReadFromFile(FILE *Stream)
{
	//ResetData();
	CMP4_AtomFactory::lastAudioPos = CTools::FileSize;
	/* Read file data */
	//fseek(Stream, CTools::ID3v2Size, SEEK_SET);
	mainContainer->load(Stream, CTools::ID3v2Size, CTools::FileSize - CTools::ID3v1Size);
	return (mainContainer->find(FTYP_PFAD) != NULL);	
}

CAtlString CMP4::GetFileVersion()
{
	// Hole ftyp atom
	CMP4Atom* atom = mainContainer->find(FTYP_PFAD);
	if (atom == NULL)
		return UNKNOWN;
	int majortype = atom->_blob.Get4B(0); // major type
	for (int i = 0; i < 49; i++)
	{
		if (MP4_FORMATCODES[i].id == majortype)
			return MP4_FORMATCODES[i].text;
	}
	return UNKNOWN;
}

int CMP4::GetPictureCount()
{ 
	CMP4Atom* atom = mainContainer->find(COVR_PFAD);
	if (atom == NULL)
		return 0;
	CMP4_Container *cont = static_cast<CMP4_Container*>(atom);
	return (int)cont->_children.GetCount();
}

CAtlString CMP4::GetPictureMime(int Index)
{
	if (Index < 1)
		return L"";
	CMP4Atom* atom = mainContainer->find(COVR_PFAD);
	if (atom == NULL)
		return L"";
	CMP4_Container *cont = static_cast<CMP4_Container*>(atom);
	if (Index <= (int)cont->_children.GetCount())
		atom = cont->_children[Index - 1];
	else
		return L"";
	return (atom->_blob.Get4B(0) == 14) ? _T("png") : _T("jpg");
}

long CMP4::GetPictureSize(int Index)
{
	if (Index < 1)
		return 0;
	CMP4Atom* atom = mainContainer->find(COVR_PFAD);
	if (atom == NULL)
		return 0;
	CMP4_Container *cont = static_cast<CMP4_Container*>(atom);
	if (Index <= (int)cont->_children.GetCount())
	{
		atom = cont->_children[Index - 1];
		return (long)atom->_blob.GetLength() - 8;
	}
	return 0;	
}

bool CMP4::GetPicture(LPCWSTR file, int Index)
{
	if (Index < 1)
		return false;
	CMP4Atom* atom = mainContainer->find(COVR_PFAD);
	if (atom == NULL)
		return false;
	CMP4_Container *cont = static_cast<CMP4_Container*>(atom);
	if (Index <= (int)cont->_children.GetCount())
		atom = cont->_children[Index - 1];
	else
		return false;
	// aus Speicher in File schreiben
	// write file from memory to disc
	FILE *Stream;
	if ( (Stream = _wfsopen(file, WRITE_ONLY, _SH_DENYWR)) != NULL)
	{
		long ln = (long)atom->_blob.GetLength() - 8;
		long res = (long)fwrite(atom->_blob.m_pData + 8, 1, ln, Stream);
		fflush(Stream);
		fclose(Stream);
		return (res == ln);
	}
	CTools::instance().setLastError(errno);
	return false;
}

bool CMP4::AddPictureFile(LPCWSTR FileName)
{
	FILE* Stream;
	if ( (Stream = _wfsopen(FileName, READ_ONLY, _SH_DENYNO)) == NULL)
	{
		CTools::instance().setLastError(errno);
		return false;
	}
	long length = _filelength(_fileno(Stream));
	CBlob arr(length);
	arr.FileRead(length, Stream);
	fclose(Stream);
	return AddPictureArray(arr.m_pData, length);
}

bool CMP4::AddPictureArray(BYTE *arr, u32 length)
{
	mainContainer->checkMetaBox();
	CMP4Atom* atom = mainContainer->find(COVR_PFAD);
	CMP4_Container *cont;
	// Noch kein Bild da, lege neuen Container an
	// no image exists yet, create new container
	if (atom == NULL)
	{
		cont = new CMP4_Container(MP4_COVR);
		cont->setParent(ILST_PFAD);
		mainContainer->addAtom(cont);
	}
	else
		cont = static_cast<CMP4_Container*>(atom);
	atom = new CMP4Atom('data');
	atom->setParent(COVR_PFAD);
	// Bildtype rausfinden, JPEG oder png
	// determine picturetype, JPEG or PNG
	BYTE picType = 13; // JPEG = Default
	if (arr[0] ==  0x89 && arr[1] == 0x50 && arr[2] == 0x4E && arr[3] == 0x47)
		picType = 14;
	atom->_blob.Clear();
	atom->_blob.Add4B(picType);
	atom->_blob.Add4B(0);
	atom->_blob.AddMemory(arr, length);
	cont->_children.Add(atom);
	return true;
}

bool CMP4::DeletePictureFrame(short Index)
{
	CMP4Atom* atom = mainContainer->find(COVR_PFAD);
	if (atom == NULL)
		return false;
	CMP4_Container *cont = static_cast<CMP4_Container*>(atom);
	if (Index > 0 && Index <= (int)cont->_children.GetCount())
	{
		delete cont->_children[Index - 1];
		cont->_children.RemoveAt(Index - 1);
		return true;
	}
	return false;
}

void CMP4::DeletePictures()
{
	mainContainer->removeAtom(COVR_PFAD);
}

long CMP4::GetPictureArray(BYTE *destination, long maxLen, short Index)
{
	if (Index < 1)
		return -1;
	CMP4Atom* atom = mainContainer->find(COVR_PFAD);
	if (atom == NULL)
		return -1;
	CMP4_Container *cont = static_cast<CMP4_Container*>(atom);
	if (Index > 0 && Index <= (int)cont->_children.GetCount())
		atom = cont->_children[Index - 1];
	else
		return -1;
	// 4 bytes version/flags = byte hex version + 24-bit hex flags
	// 0D (13) = jpeg ,  0E (14) = png
	// 4 bytes reserved = 32-bit value set to zero
	long ln =(long)atom->_blob.GetLength() - 8;
	if (ln > maxLen)
	{
		CTools::instance().setLastError(ERR_NOT_ENOUGH_MEMORY);
		return -1;
	}
	if (ln > 0)
		memcpy(destination, atom->_blob.m_pData + 8, ln);
	return ln;
}

long CMP4::GetBitRate()
{
	float duration = GetDuration();
	if (duration > 0)
	{
		if (CMP4_AtomFactory::mediaLength > 0)
			return (long)(CMP4_AtomFactory::mediaLength * 8.0 / duration / 1000.0 + 0.5);
		return (long)((CTools::FileSize - CMP4_AtomFactory::lastOffset) * 8.0 / duration / 1000.0 + 0.5);
	}
	return 0;
}

CAtlString CMP4::GetILSTText(u32 lastFrameID)
{
	CAtlString suchPfad;
	suchPfad.Format(_T("%s.%c%c%c%c"), ILST_PFAD, BYTE(lastFrameID >> 24), BYTE(lastFrameID >> 16), BYTE(lastFrameID >> 8), BYTE(lastFrameID)); 
	CMP4Atom* atom = mainContainer->find(suchPfad);
	if (atom == NULL)
		return EMPTY;
	return CMP4_AtomFactory::instance()->getText(atom);
}

void CMP4::SetILSTText(u32 FrameID, CAtlString newText)
{
	if (newText.IsEmpty())
	{
		CAtlString tmp;
		tmp.Format(_T("%s.%c%c%c%c"), ILST_PFAD, BYTE(FrameID >> 24), BYTE(FrameID >> 16), BYTE(FrameID >> 8), BYTE(FrameID)); 
		mainContainer->removeAtom(tmp);
		return;
	}
	CMP4Atom* atom = new CMP4Atom(FrameID);
	atom->setParent(ILST_PFAD);
	CMP4_AtomFactory::instance()->setText(atom, newText);
	mainContainer->checkMetaBox();
	mainContainer->replaceAtom(atom);
}

CAtlString CMP4::GetItuneText(CAtlString frame)
{
	CAtlString suchPfad;
	suchPfad.Format(_T("%s.%s"), ITUN_PFAD, frame);
	CMP4Atom* atom = mainContainer->find(suchPfad);
	if (atom == NULL)
		return EMPTY;
	return CMP4_AtomFactory::instance()->getiTuneText(atom);	
}
void CMP4::SetItuneText(CAtlString frame, CAtlString newText)
{
	CAtlString suchPfad;
	suchPfad.Format(_T("%s.%s"), ITUN_PFAD, frame); 
	if (newText.IsEmpty())
	{
		mainContainer->removeAtom(suchPfad);
		return;
	}
	mainContainer->checkMetaBox();
	CMP4Atom* atom = mainContainer->find(suchPfad);
	if (atom == NULL) // Neuanlage
										// create new
	{
		atom = new CMP4Atom('----');
		atom->setParent(ILST_PFAD);
		CMP4_AtomFactory::instance()->setItuneText(atom, frame, newText);
		mainContainer->addAtom(atom);
	}
	else // Ersetzen
		   // replace
	{
		CMP4_AtomFactory::instance()->setItuneText(atom, frame, newText);
	}
}
CAtlString CMP4::getILSTFrameIDs()
{
	CMP4Atom* atom = mainContainer->find(ILST_PFAD);
	if (atom == NULL)
		return EMPTY;
	CAtlString result;
	CSimpleArray<CAtlString> ids;
	CMP4_Container *cont = static_cast<CMP4_Container*>(atom);
	size_t count = cont->_children.GetCount();
	u32 frame;
	CAtlString tmp;
	for (size_t i = 0; i < count; i++)
	{
		atom = cont->_children[i];
		frame = atom->getFrameID();
		if (frame == '----')
		{
			tmp = _T("---- ");		
			tmp.Append(CMP4_AtomFactory::instance()->getiTuneFrame(atom));
		}
		else
			tmp = atom->getId();

		if (ids.Find(tmp) == -1)
		{
			ids.Add(tmp);
			if (result.GetLength() > 0)
				result.Append(_T(","));
			result.Append(tmp);
		}		
	}
	return result;
}

CAtlString CMP4::GetTrack()
{
	CMP4Atom* atom = mainContainer->find(TRKN_PFAD);
	if (atom == NULL || atom->_blob.GetLength() < 20)
		return L"";
	CAtlString tmp;
	if (atom->getDataLen() >= 22 && atom->_blob.GetAt(21) > 0)
		tmp.Format(_T("%i/%i"), atom->_blob.GetAt(19), atom->_blob.GetAt(21));
	else
		tmp.Format(_T("%i"), atom->_blob.GetAt(19));
	return tmp;
}

void CMP4::SetTrack(LPCWSTR newTrack)
{
	if (wcslen(newTrack) == 0)
	{
		CAtlString id;
		id.Format(L"%s.trkn", ILST_PFAD);
		mainContainer->removeAtom(id);
		return;
	}
	// Format entweder als eine Zahl oder als zwei Zahlen mit / getrennt
	// format either as number or two numbers separated by /
	CAtlString info(newTrack);
	BYTE von = 0, bis = 0;
	int pos = info.Find('/');
	if (pos == -1) // not found
		von = (BYTE)_wtoi(newTrack) ;	
	else
	{
		von = (BYTE)_wtoi(info.Left(pos));
		bis = (BYTE)_wtoi(info.Mid(pos + 1));
	}
	CMP4Atom* atom = new CMP4Atom(ILST_TRKN);
	atom->setParent(ILST_PFAD);
	CMP4_AtomFactory::instance()->setTrack(atom, von, bis);
	mainContainer->replaceAtom(atom);
}

CAtlString CMP4::GetGenre()
{
	// Genre entweder als ©gen (Text) oder als gnre (Zahl)
	// genre either as ©gen (Text) or gnre (number)
	
	// Suche zuerst nach Textframe
	// firstly search for textframes
	CMP4Atom* atom = mainContainer->find(TEXT_GENRE_PFAD);
	if (atom != NULL)
		return CMP4_AtomFactory::instance()->getText(atom);
	// Nicht als Text vorhanden, vielleicht als Zahl ??
	// not found as text, maybe as number?
	atom = mainContainer->find(ZAHL_GENRE_PFAD);
	if (atom == NULL || atom->getDataLen() < 18)
		return L"";
	// Gefunden, hole Zahl und bilde text daraus
	// found; fetch number and cerate text
	BYTE genreID = atom->_blob.GetAt(17);
	if (genreID > 0 && genreID <= MAX_MUSIC_GENRES)
		return MUSIC_GENRE[genreID - 1];
	// Zahl nicht innerhalb des Bereiches, dann einfach nur Zahl ausgeben
	// number not in valid range; return only the number
	CAtlString ausgabe;
	ausgabe.Format(_T("%i"), genreID);
	return ausgabe;
}

void CMP4::SetGenre(LPCWSTR newgenre)
{
	// Erst mal alle Genre löschen
	// firstly delete all genres
	mainContainer->removeAtom(TEXT_GENRE_PFAD);
	mainContainer->removeAtom(ZAHL_GENRE_PFAD);
	if (newgenre == NULL || wcslen(newgenre) == 0)
		return;

	CAtlString info(newgenre);
	// Zuerst rausfinden, ob Genre in den Standards definiert ist
	// determine if the gemnre is defined within the standards
	for (BYTE i = 0; i < MAX_MUSIC_GENRES; i++)
	{
		if (info.CompareNoCase(MUSIC_GENRE[i]) == 0)
		{
			CMP4Atom* atom = new CMP4Atom(ILST_ZAHL_GENRE);
			atom->setParent(ILST_PFAD);
			CMP4_AtomFactory::instance()->buildData(atom, 2, 0);
			atom->_blob.AddNullByte();
			atom->_blob.AddValue(i + 1);
			mainContainer->replaceAtom(atom); // neues Genre setzen
																			  // set new genre
			return;
		}
	}
	// Kein Standard Genre gefunden, setze Custom Genre
	// no standard genre found; set custom genre
	CMP4Atom* atom = new CMP4Atom(ILST_TEXT_GENRE);
	atom->setParent(ILST_PFAD);
	CMP4_AtomFactory::instance()->setText(atom, info);
	mainContainer->replaceAtom(atom); // neues Genre setzen
																	  // set new genre
}

long CMP4::GetChannels()
{
	// TODO
	// Suche nach stsd, MP4A-Atom, falls gefunden, nehme NumberOfChannels ansonsten 2
	// search for standaeds, MP4A-Atom, if found, aplpy NumberOfChannels, otherwise 2 
	CMP4Atom* atom = mainContainer->find(STSD_PFAD);
	if (atom != NULL)
	{
		// Analyse stsd atom
	}
	return 2;
}

long CMP4::GetSampleRate()
{
	int start = 1;
	CMP4_MDHD* atom;
	long tmpSamplerate = 0;
	int count = 0;
	while ( (atom = cMDHD(mainContainer->find(MDHD_PFAD, start))) != NULL)
	{
		start++;
		if (atom->isSoundAtom)
		{
			tmpSamplerate+=atom->timeScale;
			count++;
		}
	}
	return (count > 0 ) ? tmpSamplerate / count : 0;
}

float CMP4::GetDuration()
{
	int start = 1;
	float duration = 0.0;
	CMP4_MDHD* atom;
	while ( (atom = cMDHD(mainContainer->find(MDHD_PFAD, start))) != NULL)
	{
		if (atom->isSoundAtom && atom->duration > 0 && atom->timeScale > 0)
			duration+= (float)atom->duration / (float)atom->timeScale;
		start++;		
	}
	return duration;
}

void CMP4::RemoveTag()
{
	mainContainer->removeAtom(_T("moov.udta"));	
}

bool CMP4::SaveToFile(LPCWSTR FileName)
{
	// Ermittle den Anfangsbereich der Daten
	// Determine startingpoint of data
	FILE *Source;
	FILE *Destination;
	CAtlString NewFileName(FileName);
	//long FrameOldSize = 0;
	if ( (Source = _wfsopen(FileName, READ_AND_WRITE, _SH_DENYNO)) == NULL)
	{
		CTools::instance().setLastError(errno);
		return false;
	}
	// Alte Taggings sichern
	// save old tags
	CMP4Atom *oldTaggings = NULL, *atom = NULL;
	atom = mainContainer->find(ILST_PFAD);
	if (atom != NULL)
	{
		oldTaggings = atom->copy();
	}
	fseek(Source, CTools::ID3v2Size, SEEK_SET);
	CTools::FileSize = _filelength(_fileno(Source));
	CMP4_MainContainer *newData = new CMP4_MainContainer();
	newData->load(Source, CTools::ID3v2Size, CTools::FileSize - CTools::ID3v1Size);
	atom = newData->find(MDAT_PFAD);
	if (atom == NULL)
	{
		CTools::instance().setLastError(ERR_INVALID_FORMAT);
		delete newData;
		fclose(Source);
		return false;
	}
	CMP4_MDAT *mdat = cMDAT(atom);
	long oldMDATPosition = mdat->getPosition();
	mdat->setSourceFile(FileName);
	u32 sizeBefore = newData->getSize();
	// falsche Paddings loeschen
	// delete wrong paddings
	atom = newData->find(_T("moov.udta.meta.free"));
	if (atom != NULL)
		newData->removeAtom(atom);
	newData->adjustPadding(0);
	if (oldTaggings != NULL)
	{
		newData->checkMetaBox();
		newData->replaceAtom(oldTaggings);
	}
	else
	{	
		// Keine Tagging daten gewünscht	
		// no tagging data desired
		newData->removeAtom(_T("moov.udta"));
	}
	u32 sizeAfter = newData->getSize();
	long paddingBlockSize = CTools::configValues[CONFIG_MP4PADDINGSIZE];
	u32 optimalSize = sizeAfter;
	if (paddingBlockSize > 0)
		optimalSize = ((u32)(sizeAfter / paddingBlockSize) + 1) * paddingBlockSize;

	if (sizeAfter > sizeBefore || optimalSize < sizeBefore || paddingBlockSize == 0)
	{ 
		CTools::instance().writeDebug(_T("Rebuild mp4 tag")); 
		mdat->setSameFile(false);
		// rebuild File
		NewFileName+=TILDE;
		/* Create file streams */
		if ( (Destination = _wfsopen(NewFileName, WRITE_ONLY, _SH_DENYWR)) == NULL)
		{
			delete newData;		
			CTools::instance().setLastError(errno);
			fclose(Source);
			return false;
		};
		/* adjust padding  */
		newData->adjustPadding(paddingBlockSize - 8);		
		/* Copy atom blocks */
		newData->save(Destination);
		_flushall();
		/* if mdat position is different and stco is present, then adjust the stco atom */
		CMP4_STCO* stco = cSTCO(newData->find(STCO_PFAD));
		CMP4_MDAT* mdat = cMDAT(newData->find(MDAT_PFAD));
		// Indizes richten
		// align indices
		if (stco != NULL && mdat != NULL && mdat->getPosition() != oldMDATPosition)
		{
			stco->move(mdat->getPosition() - oldMDATPosition, Destination);
			_flushall();
		}		
		_fcloseall();
		delete newData;
		/* Replace old file and delete temporary file */
		CTools::instance().doEventsNow();
		if (_wremove(FileName) != 0)
		{
			// Lösche auch temp file
			// delete the temp file
			_wremove(NewFileName);
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
	CTools::instance().writeDebug(_T("Rewrite mp4 tag")); 
	mdat->setSameFile(false);
	// Passe Padding an
	errno = 0;
	newData->adjustPadding(sizeBefore - sizeAfter);
	fseek(Source, CTools::ID3v2Size, SEEK_SET);
	newData->save(Source);
	_flushall();
	_fcloseall();
	newData->remove();
	delete newData;
	if (errno != 0)
	{
		CTools::instance().setLastError(errno);
		return false;
	}
	return true;
}
