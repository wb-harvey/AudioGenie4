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

// CMP4.h: Schnittstelle für die Klasse CMP4.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Blob.h"
#include "Audio.h"
#include "mp4atom.h"
#include "mp4_container.h"
#include "mp4_maincontainer.h"
#include "atlcoll.h"
#include "mp4_atomfactory.h"

/* Anzahl der verschiedenen Boxen */

#define TYPE_AUDIO 1
#define TYPE_VIDEO 2
#define TYPE_NULLMEDIA 3

const static u32 MP4_ALBUM   = '©alb';
const static u32 MP4_ARTIST  = '©ART';
const static u32 MP4_COMMENT = '©cmt';
const static u32 MP4_TITLE   = '©nam';
const static u32 MP4_COMPOSER = '©wrt';

struct pairStruct
{
	int id;
	LPCWSTR text;
};

static const pairStruct MP4_FORMATCODES[] = {
	{ '3g2a', _T("3GPP2") }, 
	{ '3ge6', _T("3GPP Release 6 extended-presentation Profile") }, 
	{ '3gg6', _T("3GPP Release 6 General Profile") }, 
	{ '3gp4', _T("3GPP Release 4") }, 
	{ '3gp5', _T("3GPP Release 5") }, 
	{ '3gp6', _T("3GPP Release 6 basic Profile") }, 
	{ '3gr6', _T("3GPP Release 6 progressive-download Profile") }, 
	{ '3gs6', _T("3GPP Release 6 streaming-server Profile") }, 
	{ 'CAEP', _T("Canon Digital Camera") }, 
	{ 'CDes', _T("Convergent Designs") }, 
	{ 'M4A ', _T("iTunes MPEG-4 audio protected or not,") }, 
	{ 'M4B ', _T("iTunes AudioBook protected or not") }, 
	{ 'M4P ', _T("MPEG-4 protected audio") }, 
	{ 'M4V ', _T("MPEG-4 protected audio+video") }, 
	{ 'MPPI', _T("Photo Player Multimedia Application Format") }, 
	{ 'avc1', _T("Advanced Video Coding extensions") }, 
	{ 'caqv', _T("Casio Digital Camera") }, 
	{ 'da0a', _T("DMB AF audio with MPEG Layer II audio, MOT slide show, DLS, JPG/PNG/MNG images") }, 
	{ 'da0b', _T("DMB AF, extending da0a , with 3GPP timed text, DID, TVA, REL, IPMP") }, 
	{ 'da1a', _T("DMB AF audio with ER-BSAC audio, JPG/PNG/MNG images") }, 
	{ 'da1b', _T("DMB AF, extending da1a, with 3GPP timed text, DID, TVA, REL, IPMP") }, 
	{ 'da2a', _T("DMB AF audio with HE-AAC v2 audio, MOT slide show, DLS, JPG/PNG/MNG images") }, 
	{ 'da2b', _T("DMB AF extending da2a, with 3GPP timed text, DID, TVA, REL, IPMP") }, 
	{ 'da3a', _T("DMB AF audio with HE-AAC, JPG/PNG/MNG images") }, 
	{ 'da3b', _T("DMB AF extending da3a with BIFS, 3GPP timed text, DID, TVA, REL, IPMP") }, 
	{ 'dmb1', _T("DMB AF supporting all the components defined in the specification") }, 
	{ 'dv1a', _T("DMB AF video with AVC video, ER-BSAC audio, BIFS, JPG/PNG/MNG images, TS") }, 
	{ 'dv1b', _T("DMB AF, extending dv1a, with 3GPP timed text, DID, TVA, REL, IPMP") }, 
	{ 'dv2a', _T("DMB AF video with AVC video, HE-AACv2 audio, BIFS, JPG/PNG/MNG images, TS") }, 
	{ 'dv2b', _T("DMB AF extending dv2a, with 3GPP timed text, DID, TVA, REL, IPMP") }, 
	{ 'dv3a', _T("DMB AF video with AVC video, HE-AAC audio, BIFS, JPG/PNG/MNG images, TS") }, 
	{ 'dv3b', _T("DMB AF extending dv3a with 3GPP timed text, DID, TVA, REL, IPMP") }, 
	{ 'dvr1', _T("DVB RTP") }, 
	{ 'dvt1', _T("DVB Transport Stream") }, 
	{ 'isc2', _T("Files encrypted according to ISMACryp 2.0") }, 
	{ 'iso2', _T("All files based on the 2004 edition of the ISO file format") }, 
	{ 'isom', _T("All files based on the ISO Base Media File Format") }, 
	{ 'mj2s', _T("Motion JPEG 2000 simple profile") }, 
	{ 'mjp2', _T("Motion JPEG 2000, general profile") }, 
	{ 'mp21', _T("MPEG-21") }, 
	{ 'mp41', _T("MP4 version 1") }, 
	{ 'mp42', _T("MP4 version 2") }, 
	{ 'odcf', _T("OMA DCF (DRM Content Format)") }, 
	{ 'opf2', _T("OMA PDCF (DRM Content Format)") }, 
	{ 'opx2', _T("OMA Adapted PDCF") }, 
	{ 'pana', _T("Panasonic Digital Camera") }, 
	{ 'qt  ', _T("QuickTime") }, 
	{ 'ROSS', _T("Ross Video") }, 
	{ 'sdv ', _T("SD Video") } 
};

static const LPCWSTR FTYP_PFAD = _T("ftyp");

static const LPCWSTR MDAT_PFAD = _T("mdat");

static const LPCWSTR ILST_PFAD = _T("moov.udta.meta.ilst");

static const LPCWSTR ITUN_PFAD = _T("moov.udta.meta.ilst.----");

static const LPCWSTR COVR_PFAD = _T("moov.udta.meta.ilst.covr");

static const LPCWSTR MDIA_PFAD = _T("moov.trak.mdia");

static const u32 ILST_DAY = '©day';

static const u32 ILST_TRKN = 'trkn';

static const u32 ILST_TEXT_GENRE = '©gen';

static const u32 ILST_ZAHL_GENRE = 'gnre';

static const LPCWSTR TRKN_PFAD = _T("moov.udta.meta.ilst.trkn");

static const LPCWSTR TEXT_GENRE_PFAD = _T("moov.udta.meta.ilst.©gen");

static const LPCWSTR ZAHL_GENRE_PFAD = _T("moov.udta.meta.ilst.gnre");

static const LPCWSTR STSD_PFAD = _T("moov.trak.mdia.minf.stbl.stsd");

static const LPCWSTR STCO_PFAD = _T("moov.trak.mdia.minf.stbl.stco");

static const LPCWSTR MDHD_PFAD = _T("moov.trak.mdia.mdhd");

static const LPCWSTR MINF_PFAD = _T("moov.trak.mdia.minf.smhd");

class CMP4: public CAudio
{
private:
	CMP4_MainContainer *mainContainer;
public:
	CAtlString GetILSTText(u32 frameID);
	void SetILSTText(u32 FrameID, CAtlString newText);
	CAtlString GetTrack();
	CAtlString GetGenre();
	void SetGenre(LPCWSTR newGenre);
	void SetTrack(LPCWSTR newTrack);
	void ResetData();
	bool SaveToFile(LPCWSTR FileName);
	bool AddPictureArray(BYTE *arr, u32 length);
	bool AddPictureFile(LPCWSTR FileName);
	CAtlString GetPictureMime(int Index);
	bool DeletePictureFrame(short Index);
	void DeletePictures();
	void RemoveTag();
	CMP4();
	virtual ~CMP4();
	CAtlString GetFileVersion();
	bool ReadFromFile(FILE *Stream);
	bool IsValid()                        { return (GetSampleRate() > 100); };
	long GetSampleRate();
	float GetDuration();
	int GetPictureCount();
	bool GetPicture(LPCWSTR file, int i);
	long GetPictureArray(BYTE *destination, long maxLen, short Index);
	long GetPictureSize(int Index);
	long GetChannels();
	long GetBitRate();  
	long GetFirstAudioPosition() { return CMP4_AtomFactory::firstAudioPos; };
	long GetLastAudioPosition()  { return CMP4_AtomFactory::lastAudioPos; };
	CAtlString GetItuneText(CAtlString frame);
	void SetItuneText(CAtlString frame, CAtlString newText);
	CAtlString getILSTFrameIDs();
};
