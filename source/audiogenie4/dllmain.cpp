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
#include "resource.h"
// #include "AudioGenie4_i.h"
#include "dllmain.h"
#include "dlldatax.h"
#include "io.h"


CAudioGenie4Module _AtlModule;

// DLL-Einstiegspunkt
extern "C" BOOL WINAPI DllMainW(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}

// ******************************************************************************

BYTE GetFileFormat(FILE *Stream)
{
	BYTE result = header.ReadFromFile(Stream);
	if (result == AUDIO_FORMAT_ID3V2)
	{
		CID3V2 v2;
		if (v2.ReadHeader(Stream))
		{
			CTools::ID3v2Size = v2.Size;
			fseek(Stream, v2.Size, SEEK_SET);
			result = header.ReadFromFile(Stream);
			if (result == AUDIO_FORMAT_INVALID)
				result = AUDIO_FORMAT_UNKNOWN;   // Bugfix für mpeg headers, die nicht direkt hinter dem id3v2 tag folgen
		 																		 // Bugfix for mgep headers, that do not follow directly after id3v2 tag
		v2.ResetData();
		}
	}
	return result;
}

BYTE GetFormat(LPCWSTR FileName)
{
	FILE *Source;
	BYTE result = AUDIO_FORMAT_UNKNOWN;
	FileName = getValidPointer(FileName);
	if ( (Source = _wfsopen(FileName, READ_ONLY, _SH_DENYNO)) != NULL)
	{
		result = GetFileFormat(Source);
		if (result == AUDIO_FORMAT_UNKNOWN || result == AUDIO_FORMAT_WAV) // Entweder AAC oder MPEG oder MPEG mit WavHeader vorweg 
																																		  // Either AAC or MPEG or MPEG with WavHeader ahead
		{
			endung = FileName;
			endung = endung.Trim();			
			if (endung.GetLength() > 3)
				endung = endung.Right(4);
			// Nun die Wackelkandidaten, Endung prüfen
			// check the "doubtful" candidates, check extension
			if (endung.CompareNoCase(_T(".AAC")) == 0)
			{
				CAAC tmpAac;
				if (tmpAac.ReadFromFile(Source))
					result = AUDIO_FORMAT_AAC;
			}
			// Nun noch MP3
			// Only MP3
			else if ( (endung.CompareNoCase(_T(".MP3")) == 0  || 
				endung.CompareNoCase(_T(".MP2")) == 0  ||
				endung.CompareNoCase(_T(".MP1")) == 0  ||
				endung.CompareNoCase(_T("MP3~")) == 0))
			{
				CMPEGAudio tmpMpeg;
				if (tmpMpeg.ReadFromFile(Source))
					result = AUDIO_FORMAT_MPEG;
				tmpMpeg.ResetData();
			}
		}
		fclose(Source);
	}
	return result;
}

void ClearAllTags()
{
	endung.Empty();
	Title.Empty();
	Artist.Empty();
	Album.Empty();
	Comment.Empty();
	Genre.Empty();
	Track.Empty();
	Year.Empty();
	Composer.Empty();
	aac.ResetData();
	ape.ResetData();
	mpp.ResetData();
	flac.ResetData();
	wav.ResetData();
	ogg.ResetData();
	wma.ResetData();
	mp4.ResetData();
	mpeg.ResetData();
	id3v1.ResetData();
	id3v2.ResetData();
	monkey.ResetData();  
	lyrics.ResetData();
	tta.ResetData();
	wavpack.ResetData();
	CTools::instance().reset();
	Format = 0;
	audio = &defaultaudio;
	Format = AUDIO_FORMAT_UNKNOWN;
	CTools::instance().setLastError(0);
}
/**
* Methode: AUDIOAnalyzeFileW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liest eine Musik-Datei ein. Dabei werden auch alle bekannten Tags eingelesen:<br />Die Tags werden folgendermassen ausgewertet:<br />Bei Dateien mit den Formaten WMA, MP4, FLAC und OGG werden die eigenen Tags benutzt. Ansonsten gilt folgende Tag-Priorität;<br />zuerst ID3V2, falls keiner da dann APE, falls keiner da dann ID3v1 und als letztes Lyrics.
* Beschreibung english: read an audio-file and all known tags<br />the tags will be reading in this way:<br />files with format WMA, MP4, FLAC and OGG uses their own fields. All other formats have this rules:<br />at first id3v2 tag, if not exist then ape tag, if not exist then id3v1 tag and at least lyrics tag.
* @paramD LPCWSTR FileName Name der Datei
* @paramE LPCWSTR filename name of the file
* @returnD Int16 Rückgabewert der Funktion mit folgender Bedeutung:<br /><br /><table width="220" class="tablelayout"><tr><th width="22%">Wert</th><th width="78%">Beschreibung</th></tr><tr><td width="22%">0</td><td width="78%">unbekanntes Format</td></tr><tr><td width="22%">1</td><td width="78%">MP3</td></tr><tr><td width="22%">2</td><td width="78%">WMA</td></tr><tr><td width="22%">3</td><td width="78%">MONKEY</td></tr><tr><td width="22%">4</td><td width="78%">FLAC</td></tr><tr><td width="22%">5</td><td width="78%">WAV</td></tr><tr><td width="22%">6</td><td width="78%">OGG VORBIS</td></tr><tr><td width="22%">7</td><td width="78%">MPP</td></tr><tr><td width="22%">8</td><td width="78%">AAC</td></tr><tr><td width="22%">9</td><td width="78%">MP4/M4A</td></tr><tr><td width="22%">10</td><td width="78%">TTA</td></tr><tr><td width="22%">11</td><td width="78%">wavpack</td></tr></table>
* @returnE Int16 return value from 0 to 10 with the following meaning:<br /><br /><table width="220" class="tablelayout"><tr><th width="22%">Value</th><th width="78%">Description</th></tr><tr><td width="22%">0</td><td width="78%">unknown Format</td></tr><tr><td width="22%">1</td><td width="78%">MP3</td></tr><tr><td width="22%">2</td><td width="78%">WMA</td></tr><tr><td width="22%">3</td><td width="78%">MONKEY</td></tr><tr><td width="22%">4</td><td width="78%">FLAC</td></tr><tr><td width="22%">5</td><td width="78%">WAV</td></tr><tr><td width="22%">6</td><td width="78%">OGG VORBIS</td></tr><tr><td width="22%">7</td><td width="78%">MPP</td></tr><tr><td width="22%">8</td><td width="78%">AAC</td></tr><tr><td width="22%">9</td><td width="78%">MP4/M4A</td></tr><tr><td width="22%">10</td><td width="78%">TTA</td></tr><tr><td width="22%">11</td><td width="78%">wavpack</td></tr></table>
*/
extern "C" long __stdcall AUDIOAnalyzeFileW(LPCWSTR FileName)
{
	ClearAllTags();
	FILE *Source;
	FileName = getValidPointer(FileName);
	errno = 0;
	ATLTRACE(_T("Analysiere %s\n"), FileName);
	if ( (Source = _wfsopen(FileName, READ_ONLY, _SH_DENYNO)) != NULL)
	{
		CTools::FileSize = _filelength(_fileno(Source));
		CTools::instance().writeInfo(L"parse file '%s' [%u bytes]...", FileName, CTools::FileSize);
		CTools::instance().doEvents();
		lastFile = FileName;
		if (CTools::FileSize < 32)
		{
			CTools::instance().writeInfo(_T("no valid audio format detected"));
			goto ende;
		}
		BYTE possibleFormat = GetFileFormat(Source);
		// Fallunterscheidungen je nach Format
		if (possibleFormat == AUDIO_FORMAT_INVALID) // Eindeutig ungültiges Format
																								// surely an invalid format
		{
			fclose(Source);
			CTools::instance().writeInfo(_T("no valid audio format detected"));
			return AUDIO_FORMAT_UNKNOWN;
		}
		// Näher betrachten
		// invesitate in detail
		
		// Dateiendung holen, letzten 4 Bytes
		// Check file extension, last 4 bytes
		endung = FileName;
		if (endung.GetLength() > 3)
			endung = endung.Right(4);
		// ID3V2 tag parsen, falls vorhanden
		// passe Id3v2 if existing
		if (CTools::ID3v2Size > 0)
			id3v2.ReadFromFile(Source);
		// Lade Tags am Ende der Datei
		// load tags at the file ending
		if (CTools::FileSize > 132)
		{
			id3v1.ReadFromFile(Source);
			lyrics.ReadFromFile(Source);
		}
		ape.ReadFromFile(Source);
		// Zuerst Formate ohne Tags
		// firstly formats without tags
		if (possibleFormat == AUDIO_FORMAT_WMA && wma.ReadFromFile(Source))
		{
			Format = AUDIO_FORMAT_WMA;
			CTools::instance().writeInfo(_T("identified as wma file"));
			audio = &wma;
			Album = wma.GetUserItem(WM_ALBUMTITLE);
			Artist = wma.GetUserItem(WM_AUTHOR);
			Comment = wma.GetUserItem(WM_DESCRIPTION);
			Genre = wma.GetUserItem(WM_GENRE);
			Title = wma.GetUserItem(WM_TITLE);
			Track = wma.GetUserItem(WM_TRACKNUMBER);
			Year = wma.GetUserItem(WM_YEAR);
			Composer = wma.GetUserItem(WM_COMPOSER);
			goto ende;
		}
		if (possibleFormat == AUDIO_FORMAT_MP4 && mp4.ReadFromFile(Source))
		{
			Format = AUDIO_FORMAT_MP4;
			CTools::instance().writeInfo(_T("identified as mp4/m4a file"));
			audio = &mp4;
			Album = mp4.GetILSTText(MP4_ALBUM);
			Artist = mp4.GetILSTText(MP4_ARTIST);
			Comment = mp4.GetILSTText(MP4_COMMENT);
			Genre = mp4.GetGenre();
			Title = mp4.GetILSTText(MP4_TITLE);
			Track = mp4.GetTrack();
			Year = mp4.GetILSTText(ILST_DAY);
			Composer = mp4.GetILSTText(MP4_COMPOSER);
			goto ende;
		}
		// Bei mp* Dateien nicht auf WAV testen, da manche MP3s auch WAV-Header haben
		// don't check MP* files for WAV, since some MP3 files do contain a WAV header
		if (  possibleFormat == AUDIO_FORMAT_WAV && endung.CompareNoCase(_T(".MP3")) != 0  && 
			endung.CompareNoCase(_T(".MP2")) != 0  &&
			endung.CompareNoCase(_T(".MP1")) != 0  &&
			endung.CompareNoCase(_T("MP3~")) != 0  && 
			wav.ReadFromFile(Source) )
		{
			Format = AUDIO_FORMAT_WAV;
			CTools::instance().writeInfo(_T("identified as wav file"));
			audio = &wav;
			Album = wav.getTextFrame(WAV_IPRD);
			Artist = wav.getTextFrame(WAV_IART);
			Title = wav.getTextFrame(WAV_INAM);
			Comment = wav.getTextFrame(WAV_ICMT);
			Year = wav.getTextFrame(WAV_ICRD);
			Genre = wav.getTextFrame(WAV_IGNR);
			Track = wav.getTextFrame(WAV_ITRK);	
			Composer = wav.getTextFrame(WAV_IMUS);
			goto ende;
		}
		// Zuerst die eindeutig identifizierbaren Formate
		// firstly the explicit identified formats
		if (possibleFormat == AUDIO_FORMAT_FLAC && flac.ReadFromFile(Source))
		{
			Format = AUDIO_FORMAT_FLAC;
			CTools::instance().writeInfo(_T("identified as flac file"));
			audio = &flac;
			Album = flac.GetUserItem(VORBIS_ALBUM);
			Artist = flac.GetUserItem(VORBIS_ARTIST);
			Comment = flac.GetUserItem(VORBIS_COMMENT);
			Genre = flac.GetUserItem(VORBIS_GENRE);
			Title = flac.GetUserItem(VORBIS_TITLE);
			Track = flac.GetUserItem(VORBIS_TRACKNUMBER);
			Year = flac.GetUserItem(VORBIS_DATE);
			Composer = flac.GetUserItem(VORBIS_COMPOSER);
			goto ende;
		}
		if (possibleFormat == AUDIO_FORMAT_OGGVORBIS && ogg.ReadFromFile(Source))
		{
			Format = AUDIO_FORMAT_OGGVORBIS;
			CTools::instance().writeInfo(_T("identified as ogg vorbis file"));
			audio = &ogg;
			Album = ogg.GetUserItem(VORBIS_ALBUM);
			Artist = ogg.GetUserItem(VORBIS_ARTIST);
			Comment = ogg.GetUserItem(VORBIS_COMMENT);
			Genre = ogg.GetUserItem(VORBIS_GENRE);
			Title = ogg.GetUserItem(VORBIS_TITLE);
			Track = ogg.GetUserItem(VORBIS_TRACKNUMBER);
			Year = ogg.GetUserItem(VORBIS_DATE); 
			Composer = ogg.GetUserItem(VORBIS_COMPOSER);
			goto ende;
		}

		

		// Tags laden, ID3v2 ist vorhanden
		// laod tags, ID3v2 is present
		if (CTools::ID3v2Size > 0)
		{			
			Album = id3v2.GetText(F_TALB);
			Artist = id3v2.GetText(F_TPE1);
			CID3_Frame *frame = id3v2.findFrame(F_COMM);
			if (frame != NULL)
				Comment = cCOMM(frame)->getText();
			else
				Comment.Empty();

			Genre = id3v2.GetGenre();
			Title = id3v2.GetText(F_TIT2);
			Track = id3v2.GetText(F_TRCK);
			Year = id3v2.GetText(F_TYER);
			Composer = id3v2.GetText(F_TCOM);
		}
		else if (CTools::APESize > 0)
		{
			Album = ape.GetTagItem(APE_ALBUM);
			Artist = ape.GetTagItem(APE_ARTIST);
			Comment = ape.GetTagItem(APE_COMMENT);
			Genre = ape.GetTagItem(APE_GENRE);
			Title = ape.GetTagItem(APE_TITLE);
			Track = ape.GetTagItem(APE_TRACK);
			Year = ape.GetTagItem(APE_YEAR);
			Composer = ape.GetTagItem(APE_COMPOSER);
		}
		else if (CTools::ID3v1Size > 0)
		{
			Album = id3v1.GetAlbum();
			Artist = id3v1.GetArtist();
			Comment = id3v1.GetComment();
			Genre = id3v1.GetGenre();
			Title = id3v1.GetTitle();
			Track = id3v1.GetTrack(); 
			Year = id3v1.GetYear();         
		}
		else if (CTools::LyricsSize > 0)
		{
			Album = lyrics.GetAlbum();
			Artist = lyrics.GetArtist();
			Genre = lyrics.GetGenre();
			Title = lyrics.GetTitle();      
		}

		if (possibleFormat == AUDIO_FORMAT_MONKEY && monkey.ReadFromFile(Source))
		{
			Format = AUDIO_FORMAT_MONKEY;
			CTools::instance().writeInfo(_T("identified as monkey file"));
			audio = &monkey;
			goto ende;
		}

		if (possibleFormat == AUDIO_FORMAT_TTA && tta.ReadFromFile(Source)) 
		{
			Format = AUDIO_FORMAT_TTA;
			CTools::instance().writeInfo(_T("identified as tta file"));
			audio = &tta;
			goto ende;
		}
		if (possibleFormat == AUDIO_FORMAT_WAVPACK && wavpack.ReadFromFile(Source))
		{
			Format = AUDIO_FORMAT_WAVPACK;
			CTools::instance().writeInfo(_T("identified as wavpack file"));
			audio = &wavpack; 
			goto ende;
		}
		if (possibleFormat == AUDIO_FORMAT_MPP && mpp.ReadFromFile(Source))
		{
			Format = AUDIO_FORMAT_MPP;
			CTools::instance().writeInfo(_T("identified as mpeg plus file"));
			audio = &mpp;
			goto ende;
		}
		// Nun noch die Wackelkandidaten, Endung prüfen
		// doubtful candidates: check file ending
		if (endung.CompareNoCase(_T(".AAC")) == 0 && aac.ReadFromFile(Source))
		{
			Format = AUDIO_FORMAT_AAC;
			CTools::instance().writeInfo(_T("identified as aac file"));
			audio = &aac;
			goto ende;
		}

		// Nun noch MP3
		// only mp3 left
		if ( (endung.CompareNoCase(_T(".MP3")) == 0  || 
			endung.CompareNoCase(_T(".MP2")) == 0  ||
			endung.CompareNoCase(_T(".MP1")) == 0  ||
			endung.CompareNoCase(_T(".MSF")) == 0  ||
			endung.CompareNoCase(_T("MP3~")) == 0) && 
			mpeg.ReadFromFile(Source))
		{
			Format = AUDIO_FORMAT_MPEG;
			CTools::instance().writeInfo(_T("identified as mpeg file"));
			audio = &mpeg;
			// Letzte AudioPosition anhand der Tags ermitteln
			// identify audioposition by last tag
			long last = CTools::FileSize - 1;
			if (ape.Exists() )
				last = min(last, CTools::FileSize - CTools::APESize - 32 - CTools::ID3v1Size - 1);
			if (lyrics.Exists() )
				last = min(last, lyrics.GetStartPosition() - 1);
			if (id3v1.Exists() )
				last = min(last, CTools::FileSize - 128 - 1);
			mpeg.setLastAudioPosition(last);
		}

ende:
		fclose(Source);
		if (Format == 0)
			CTools::instance().writeInfo(_T("unknown or invalid audio format detected"));					
	}
	else
		CTools::instance().setLastError(errno); 
	return Format;
}


/**
* Methode: AUDIOSaveChangesToFileW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: speichert die Änderungen in eine Datei, abhängig vom Format der Zieldatei:<br /><br /><table width="90%" class="tablelayout"><tr><th width="15%">Audioformat</th><th width="80%">Speicherung erfolgt als</th></tr><tr><td>MP3</td><td>ID3V2-Tag</td></tr><tr><td>WMA</td><td>WMA Felder</td></tr><tr><td>MONKEY</td><td>APE-Tag</td></tr><tr><td>FLAC</td><td>Vorbis Comment</td></tr><tr><td>WAV</td><td>wav chunk</td></tr><tr><td>OGG</td><td>Vorbis Comment</td></tr><tr><td>MPP</td><td>ID3V2-Tag</td></tr><tr><td>AAC</td><td>ID3V2-Tag</td></tr><tr><td>MP4</td><td>MP4 atom</td><tr><td>WavPack</td><td>APE-Tag</td></tr></tr><tr><td>TTA</td><td>ID3V2-Tag</td></tr></table>
* Beschreibung english: store the changes in a file, depending on format of the destination file:<br /><br /><table width="90%" class="tablelayout"><tr><th width="15%">audio format</th><th width="80%">changes saved as</th></tr><tr><td>MP3</td><td>ID3V2-Tag</td></tr><tr><td>WMA</td><td>WMA Fields</td></tr><tr><td>MONKEY</td><td>APE-Tag</td></tr><tr><td>FLAC</td><td>Vorbis Comment</td></tr><tr><td>WAV</td><td>wav chunk</td></tr><tr><td>OGG</td><td>Vorbis Comment</td></tr><tr><td>MPP</td><td>ID3V2-Tag</td></tr><tr><td>AAC</td><td>ID3V2-Tag</td></tr><tr><td>MP4</td><td>mp4 atoms</td></tr><tr><td>WavPack</td><td>APE-Tag</td></tr><tr><td>TTA</td><td>ID3V2-Tag</td></tr></table>
* @paramD LPCWSTR FileName Name der Datei
* @paramE LPCWSTR filename name of the file
* @returnD Int16 liefert 0 bei Fehler, ansonsten -1
* @returnE Int16 0 on error, otherwise -1
*/
extern "C" short __stdcall AUDIOSaveChangesToFileW(LPCWSTR FileName) 
{
	CTools::instance().setLastError(0);
	FileName = getValidPointer(FileName);
	Format = (short)GetFormat(FileName);
	bool result;
	switch (Format)
	{
	case AUDIO_FORMAT_WMA:
		wma.SetUserItem(WM_ALBUMTITLE, Album);
		wma.SetUserItem(WM_AUTHOR, Artist);
		wma.SetUserItem(WM_DESCRIPTION, Comment);
		wma.SetUserItem(WM_GENRE, Genre);
		wma.SetUserItem(WM_TITLE, Title);
		wma.SetUserItem(WM_TRACKNUMBER, Track);
		wma.SetUserItem(WM_YEAR, Year);      
		wma.SetUserItem(WM_COMPOSER, Composer);
		result = wma.SaveToFile(FileName);
		if (result && !lastFile.IsEmpty() && lastFile.CompareNoCase(FileName) == 0)
			AUDIOAnalyzeFileW(FileName);
		return b2s(result);		
	case AUDIO_FORMAT_FLAC:
		flac.SetUserItem(VORBIS_ALBUM, Album);
		flac.SetUserItem(VORBIS_ARTIST, Artist);
		flac.SetUserItem(VORBIS_COMMENT, Comment);
		flac.SetUserItem(VORBIS_GENRE, Genre);
		flac.SetUserItem(VORBIS_TITLE, Title);
		flac.SetUserItem(VORBIS_TRACKNUMBER, Track);
		flac.SetUserItem(VORBIS_DATE, Year); 
		flac.SetUserItem(VORBIS_COMPOSER, Composer); 
		return b2s(flac.SaveToFile(FileName));
	case AUDIO_FORMAT_OGGVORBIS:
		ogg.SetUserItem(VORBIS_ALBUM, Album);
		ogg.SetUserItem(VORBIS_ARTIST, Artist);
		ogg.SetUserItem(VORBIS_COMMENT, Comment);
		ogg.SetUserItem(VORBIS_GENRE, Genre);
		ogg.SetUserItem(VORBIS_TITLE, Title);
		ogg.SetUserItem(VORBIS_TRACKNUMBER, Track);
		ogg.SetUserItem(VORBIS_DATE, Year);  
		ogg.SetUserItem(VORBIS_COMPOSER, Composer);  
		return b2s(ogg.SaveTag(FileName));
	case AUDIO_FORMAT_MP4:
		mp4.SetILSTText(MP4_ALBUM, Album);
		mp4.SetILSTText(MP4_ARTIST, Artist);
		mp4.SetILSTText(MP4_COMMENT, Comment);
		mp4.SetGenre(Genre); 
		mp4.SetILSTText(MP4_TITLE, Title);
		mp4.SetTrack(Track);
		mp4.SetILSTText(ILST_DAY, Year);
		mp4.SetILSTText(MP4_COMPOSER, Composer);
		result = mp4.SaveToFile(FileName);
		if (result && !lastFile.IsEmpty() && lastFile.CompareNoCase(FileName) == 0)
			AUDIOAnalyzeFileW(FileName);
		return b2s(result);	
	case AUDIO_FORMAT_MONKEY:
		ape.SetTagItem(APE_ALBUM, Album);
		ape.SetTagItem(APE_ARTIST, Artist);
		ape.SetTagItem(APE_COMMENT, Comment);
		ape.SetTagItem(APE_GENRE, Genre);
		ape.SetTagItem(APE_TITLE, Title);
		ape.SetTagItem(APE_TRACK, Track);
		ape.SetTagItem(APE_YEAR, Year);
		ape.SetTagItem(APE_COMPOSER, Composer);
		return b2s(ape.SaveToFile(FileName));
	case AUDIO_FORMAT_WAVPACK:
		ape.SetTagItem(APE_ALBUM, Album);
		ape.SetTagItem(APE_ARTIST, Artist);
		ape.SetTagItem(APE_COMMENT, Comment);
		ape.SetTagItem(APE_GENRE, Genre);
		ape.SetTagItem(APE_TITLE, Title);
		ape.SetTagItem(APE_TRACK, Track);
		ape.SetTagItem(APE_YEAR, Year);
		ape.SetTagItem(APE_COMPOSER, Composer);
		return b2s(ape.SaveToFile(FileName));
	case AUDIO_FORMAT_AAC:
		ape.SetTagItem(APE_ALBUM, Album);
		ape.SetTagItem(APE_ARTIST, Artist);
		ape.SetTagItem(APE_COMMENT, Comment);
		ape.SetTagItem(APE_GENRE, Genre);
		ape.SetTagItem(APE_TITLE, Title);
		ape.SetTagItem(APE_TRACK, Track);
		ape.SetTagItem(APE_YEAR, Year);
		ape.SetTagItem(APE_COMPOSER, Composer);
		return b2s(ape.SaveToFile(FileName));
	case AUDIO_FORMAT_UNKNOWN:
		CTools::instance().setLastError(ERR_TAG_NOT_ALLOWED);
		return b2s(false);
	case AUDIO_FORMAT_WAV:
		wav.setTextFrame(WAV_IPRD, Album);
		wav.setTextFrame(WAV_IART, Artist);
		wav.setTextFrame(WAV_INAM, Title);
		wav.setTextFrame(WAV_ICMT, Comment);
		wav.setTextFrame(WAV_ICRD, Year);
		wav.setTextFrame(WAV_IGNR, Genre);
		wav.setTextFrame(WAV_ITRK, Track);
		wav.setTextFrame(WAV_IMUS, Composer);
		result = wav.SaveToFile(FileName);
		if (result && !lastFile.IsEmpty() && lastFile.CompareNoCase(FileName) == 0)
			AUDIOAnalyzeFileW(FileName);
		return b2s(result);		
	default:
		id3v2.SetText(F_TALB, Album);
		id3v2.SetText(F_TPE1, Artist);
		id3v2.replaceFrame(new CID3F_COMM(_T("eng"), EMPTY, Comment));
		id3v2.SetText(F_TCON, Genre);
		id3v2.SetText(F_TIT2, Title);
		id3v2.SetText(F_TRCK, Track);
		id3v2.SetText(F_TYER, Year);
		id3v2.SetText(F_TCOM, Composer);
		return b2s(id3v2.SaveToFile(FileName));		
	}  
}

/**
* Methode: AUDIOSaveChangesW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: speichert die Änderungen in die zuletz analysierte Datei, abhängig vom Format der Zieldatei:<br /><br /><table width="90%" class="tablelayout"><tr><th width="15%">Audioformat</th><th width="80%">Speicherung erfolgt als</th></tr><tr><td>MP3</td><td>ID3V2-Tag</td></tr><tr><td>WMA</td><td>WMA Felder</td></tr><tr><td>MONKEY</td><td>APE-Tag</td></tr><tr><td>FLAC</td><td>Vorbis Comment</td></tr><tr><td>WAV</td><td>wav chunk</td></tr><tr><td>OGG</td><td>Vorbis Comment</td></tr><tr><td>MPP</td><td>ID3V2-Tag</td></tr><tr><td>AAC</td><td>ID3V2-Tag</td></tr><tr><td>MP4</td><td>MP4 atom</td><tr><td>WavPack</td><td>APE-Tag</td></tr></tr><tr><td>TTA</td><td>ID3V2-Tag</td></tr></table>
* Beschreibung english: store the changes in the last analyzed file, depending on format of the destination file:<br /><br /><table width="90%" class="tablelayout"><tr><th width="15%">audio format</th><th width="80%">changes saved as</th></tr><tr><td>MP3</td><td>ID3V2-Tag</td></tr><tr><td>WMA</td><td>WMA Fields</td></tr><tr><td>MONKEY</td><td>APE-Tag</td></tr><tr><td>FLAC</td><td>Vorbis Comment</td></tr><tr><td>WAV</td><td>wav chunk</td></tr><tr><td>OGG</td><td>Vorbis Comment</td></tr><tr><td>MPP</td><td>ID3V2-Tag</td></tr><tr><td>AAC</td><td>ID3V2-Tag</td></tr><tr><td>MP4</td><td>mp4 atoms</td></tr><tr><td>WavPack</td><td>APE-Tag</td></tr><tr><td>TTA</td><td>ID3V2-Tag</td></tr></table>
* @returnD Int16 liefert 0 bei Fehler, ansonsten -1
* @returnE Int16 0 on error, otherwise -1
*/
extern "C" short __stdcall AUDIOSaveChangesW()
{
	return AUDIOSaveChangesToFileW(lastFile);
}

/**
* Methode: AUDIOGetChannelsW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Kanäle
* Beschreibung english: get the number of channels
* @returnD Int32 Anzahl Kanäle
* @returnE Int32 number of channels
*/
extern "C" long __stdcall AUDIOGetChannelsW()
{
	return audio->GetChannels();
}


/**
* Methode: AUDIOGetDurationW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Gesamtspieldauer in Sekunden
* Beschreibung english: get the duration in seconds
* @returnD Float Spieldauer
* @returnE Float Duration
*/
extern "C" float __stdcall AUDIOGetDurationW()
{
	return audio->GetDuration();  
}

/**
* Methode: AUDIOGetDurationMillisW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Gesamtspieldauer in Millisekunden
* Beschreibung english: get the duration in milliseconds
* @returnD Int32 Spieldauer
* @returnE Int32 Duration
*/
extern "C" long __stdcall AUDIOGetDurationMillisW()
{
	return (long)(audio->GetDuration() * 1000.0);
}

/**
* Methode: AUDIOGetSampleRateW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Samplerate in Hz
* Beschreibung english: get the Samplerate in Hz
* @returnD Int32 Samplerate
* @returnE Int32 Samplerate
*/
extern "C" long __stdcall AUDIOGetSampleRateW()
{
	return audio->GetSampleRate();
}


/**
* Methode: AUDIOGetBitrateW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Bitrate in kbps
* Beschreibung english: get the Bitrate in kbps
* @returnD Int32 Bitrate
* @returnE Int32 Bitrate
*/
extern "C" long __stdcall AUDIOGetBitrateW()
{
	return audio->GetBitRate();
}


/**
* Methode: AUDIOGetChannelModeW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Kanal-Modus (Mono / Stereo )
* Beschreibung english: get the channel mode ( mono / stereo )
* @returnD BSTR Kanal-Modus
* @returnE BSTR channel mode
*/
extern "C" BSTR __stdcall AUDIOGetChannelModeW()
{
	return audio->GetChannelMode().AllocSysString();
}


/**
* Methode: AUDIOGetLastErrorNumberW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Fehlernummer des zuletzt aufgetretenen Fehlers
* Beschreibung english: get the error number of the last error
* @returnD Int32 Fehler Nummer
* @returnE Int32 error number
*/
extern "C" long __stdcall AUDIOGetLastErrorNumberW()
{
	return CTools::instance().getLastError();
}


/**
* Methode: AUDIOGetLastErrorTextW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Fehlertext des zuletzt aufgetretenen Fehlers
* Beschreibung english: get the description of the last error
* @returnD BSTR Fehlertext
* @returnE BSTR error string
*/
extern "C" BSTR __stdcall AUDIOGetLastErrorTextW()
{
	return CTools::instance().GetLastErrorText().AllocSysString();
}

/**
* Methode: AUDIOGetLastFileW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Dateinamen der zuletzt analysierten Datei
* Beschreibung english: get the last analyzed file name
* @returnD BSTR Dateiname der zuletzt analysierten Datei
* @returnE BSTR file name of the last analyzed file
*/
extern "C" BSTR __stdcall AUDIOGetLastFileW()
{
	return lastFile.AllocSysString();
}

/**
* Methode: AUDIOGetVersionW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Version
* Beschreibung english: get the version
* @returnD BSTR Version
* @returnE BSTR version
*/
extern "C" BSTR __stdcall AUDIOGetVersionW()
{
	return audio->GetFileVersion().AllocSysString();
}

/**
* Methode: AUDIOIsValidFormatW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: zeigt an, ob die Audio-Datei gültig ist
* Beschreibung english: shows -1, if the File is valid
* @returnD Int16 -1 wenn Datei gültig ist, ansonsten 0
* @returnE Int16 -1 if file is valid, otherwise 0
*/
extern "C" short __stdcall AUDIOIsValidFormatW()
{
	return b2s(audio->IsValid());
}

/**
* Methode: AUDIOGetFileSizeW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Dateigrösse in Bytes
* Beschreibung english: get the filesize in bytes
* @returnD Int32 Dateigrösse
* @returnE Int32 filesize
*/
extern "C" long __stdcall AUDIOGetFileSizeW()
{
	return CTools::FileSize;
}

/**
* Methode: AUDIOGetTitleW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Titel
* Beschreibung english: get the title
* @returnD BSTR Titel
* @returnE BSTR title
*/
extern "C" BSTR __stdcall AUDIOGetTitleW() 
{
	return Title.AllocSysString();
}

/**
* Methode: AUDIOSetTitleW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Titel
* Beschreibung english: set the title
* @paramD LPCWSTR myTitle Titel
* @paramE LPCWSTR myTitle title
*/
extern "C" void __stdcall AUDIOSetTitleW(LPCWSTR textString) 
{
	textString = getValidPointer(textString);
	Title = textString;  
}

/**
* Methode: AUDIOGetArtistW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Künstler
* Beschreibung english: get the artist
* @returnD BSTR Interpret
* @returnE BSTR artist
*/
extern "C" BSTR __stdcall AUDIOGetArtistW() 
{
	return Artist.AllocSysString();	
}

/**
* Methode: AUDIOSetArtistW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Künstler
* Beschreibung english: set the artist
* @paramD LPCWSTR myArtist Künstler
* @paramE LPCWSTR myArtist artist
*/
extern "C" void __stdcall AUDIOSetArtistW(LPCWSTR textString) 
{ 
	Artist = getValidPointer(textString);
}

/**
* Methode: AUDIOGetAlbumW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Album
* Beschreibung english: get the album
* @returnD BSTR Album
* @returnE BSTR album
*/
extern "C" BSTR __stdcall AUDIOGetAlbumW() 
{
	return Album.AllocSysString();
}

/**
* Methode: AUDIOSetAlbumW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Album
* Beschreibung english: set the album
* @paramD LPCWSTR myAlbum Album
* @paramE LPCWSTR myAlbum album
*/
extern "C" void __stdcall AUDIOSetAlbumW(LPCWSTR textString) 
{
	Album = getValidPointer(textString);  
}

/**
* Methode: AUDIOGetCommentW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert einen Kommentar
* Beschreibung english: get the comment
* @returnD BSTR Kommentar
* @returnE BSTR Comment
*/
extern "C" BSTR __stdcall AUDIOGetCommentW() 
{
	return Comment.AllocSysString();
}

/**
* Methode: AUDIOSetCommentW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: setzt einen Kommentar
* Beschreibung english: set the comment
* @paramD LPCWSTR myComment Kommentar
* @paramE LPCWSTR myComment comment
*/
extern "C" void __stdcall AUDIOSetCommentW(LPCWSTR textString) 
{
	Comment = getValidPointer(textString);
}

/**
* Methode: AUDIOGetComposerW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Komponisten
* Beschreibung english: get the composer
* @returnD BSTR Komponist
* @returnE BSTR Composer
*/
extern "C" BSTR __stdcall AUDIOGetComposerW() 
{
	return Composer.AllocSysString();
}

/**
* Methode: AUDIOSetComposerW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: setzt einen Komponisten
* Beschreibung english: set the composer
* @paramD LPCWSTR myComposer Komponist
* @paramE LPCWSTR myComposer composer
*/
extern "C" void __stdcall AUDIOSetComposerW(LPCWSTR textString) 
{
	Composer = getValidPointer(textString);
}

/**
* Methode: AUDIOGetGenreW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Genre
* Beschreibung english: get the genre
* @returnD BSTR Genre
* @returnE BSTR genre
*/
extern "C" BSTR __stdcall AUDIOGetGenreW() 
{
	return Genre.AllocSysString();
}

/**
* Methode: AUDIOSetGenreW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Genre
* Beschreibung english: set the genre
* @paramD LPCWSTR myGenre Genre
* @paramE LPCWSTR myGenre genre
*/
extern "C" void __stdcall AUDIOSetGenreW(LPCWSTR textString) 
{
	Genre = getValidPointer(textString);  
}

/**
* Methode: AUDIOGetTrackW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Track
* Beschreibung english: get the track
* @returnD BSTR Track
* @returnE BSTR track
*/
extern "C" BSTR __stdcall AUDIOGetTrackW() 
{
	return Track.AllocSysString();
}

/**
* Methode: AUDIOSetTrackW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Track
* Beschreibung english: set the track
* @paramD LPCWSTR myTrack Track
* @paramE LPCWSTR myTrack track
*/
extern "C" void __stdcall AUDIOSetTrackW(LPCWSTR textString) 
{
	Track = getValidPointer(textString);  
}

/**
* Methode: AUDIOGetYearW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Jahr
* Beschreibung english: get the year
* @returnD BSTR Jahr
* @returnE BSTR year
*/
extern "C" BSTR __stdcall AUDIOGetYearW() 
{
	return Year.AllocSysString();
}

/**
* Methode: AUDIOSetYearW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Jahr
* Beschreibung english: set the year
* @paramD LPCWSTR myYear Jahr
* @paramE LPCWSTR myYear year
*/
extern "C" void __stdcall AUDIOSetYearW(LPCWSTR textString) 
{
	Year = getValidPointer(textString);
}


/* ------------------------------------------------------------------------------------- */

/**
* Methode: AACGetHeaderTypeW
* @link AAC
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Header Typ
* Beschreibung english: get the Header Type
* @returnD BSTR Header Typ
* @returnE BSTR Header Type
*/
extern "C" BSTR __stdcall AACGetHeaderTypeW()
{
	return aac.GetHeaderType().AllocSysString();
}


/**
* Methode: AACGetProfileW
* @link AAC
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Profil
* Beschreibung english: get the Profile
* @returnD BSTR Profil
* @returnE BSTR Profile
*/
extern "C" BSTR __stdcall AACGetProfileW()
{
	return aac.GetProfile().AllocSysString();
}


/**
* Methode: AACGetBitRateTypeW
* @link AAC
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Bitraten-Typ ( CBR oder VBR )
* Beschreibung english: get the Bitratetype (CBR or VBR )
* @returnD BSTR Bitraten-Typ
* @returnE BSTR Bitratetype
*/
extern "C" BSTR __stdcall AACGetBitRateTypeW()
{
	return aac.GetBitRateType().AllocSysString();
}

/**
* Methode: FLACGetBitsPerSampleW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Bits pro Sample
* Beschreibung english: get the bits per sample
* @returnD Int32 Bits pro Sample
* @returnE Int32 bits per Sample
*/
extern "C" long __stdcall FLACGetBitsPerSampleW()
{
	return flac.GetBitsPerSample();
}


/**
* Methode: FLACGetCompressionRatioW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Kompressionsrate
* Beschreibung english: get the compression rate
* @returnD Float Kompressionsrate
* @returnE Float compression rate
*/
extern "C" float __stdcall FLACGetCompressionRatioW()
{
	return flac.GetRatio();
}


/**
* Methode: FLACGetSamplesW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Samples
* Beschreibung english: get the number of samples
* @returnD Int32 Anzahl Samples
* @returnE Int32 number of samples
*/
extern "C" long __stdcall FLACGetSamplesW()
{
	return flac.GetSamples();
}

/**
* Methode: FLACGetMinBlockSizeW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Minimum Blockgrösse in Bytes
* Beschreibung english: get the minimum blocksize
* @returnD Int32 minimale Blockgrösse
* @returnE Int32 minimum blocksize 
*/
extern "C" long __stdcall FLACGetMinBlockSizeW()
{
	return flac.GetMinBlockSize();
}


/**
* Methode: FLACGetMaxBlockSizeW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Maximum Blockgrösse in Bytes
* Beschreibung english: get the maximum blocksize in bytes
* @returnD Int32 maximale Blockgrösse
* @returnE Int32 maximum blocksize
*/
extern "C" long __stdcall FLACGetMaxBlockSizeW()
{
	return flac.GetMaxBlockSize();
}


/**
* Methode: FLACGetMinFrameSizeW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: liefert die minimale Framegrösse in Bytes
* Beschreibung english: get the minimum framesize in bytes
* @returnD Int32 minimale Framegrösse
* @returnE Int32 minimum framesize
*/
extern "C" long __stdcall FLACGetMinFrameSizeW()
{
	return flac.GetMinFrameSize();
}


/**
* Methode: FLACGetMaxFrameSizeW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: liefert die maximale Framegrösse in Bytes
* Beschreibung english: get the maximum framesize in bytes
* @returnD Int32
* @returnE Int32
*/
extern "C" long __stdcall FLACGetMaxFrameSizeW()
{
	return flac.GetMaxFrameSize();
}

/**
* Methode: FLACGetVendorW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Anbieter
* Beschreibung english: get the vendor
* @returnD BSTR Anbieter
* @returnE BSTR vendor
*/
extern "C" BSTR __stdcall FLACGetVendorW() 
{
	return flac.GetVendor().AllocSysString();
}


/**
* Methode: FLACSetVendorW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Anbieter
* Beschreibung english: set the vendor
* @paramD LPCWSTR myVendor Anbieter
* @paramE LPCWSTR myVendor Vendor
*/
extern "C" void __stdcall FLACSetVendorW(LPCWSTR textString) 
{
	flac.SetVendor(getValidPointer(textString));
}

/**
* Methode: FLACGetUserItemW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: liefert ein benutzerdefiniertes Feld<br />Eine Aufzählung aller vorhandenen Feldnamen liefert die Funktion <a href="FLACGetItemKeysW.html">FLACGetItemKeysW</a>.
* Beschreibung english: get a user defined field<br />use the method <a href="FLACGetItemKeysW.html">FLACGetItemKeysW</a> for a list of all existing keys.
* @paramD LPCWSTR myKey benutzerdefinierter Schlüssel
* @paramE LPCWSTR myKey userdefined key
* @returnD BSTR field Feldinhalt oder leer, falls Schlüssel nicht gefunden
* @returnE BSTR field userdefined item or empty if key not present
*/
extern "C" BSTR __stdcall FLACGetUserItemW(LPCWSTR ItemKey) 
{
	return flac.GetUserItem(getValidPointer(ItemKey)).AllocSysString();
}

/**
* Methode: FLACSetUserItemW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: setzt ein benutzerdefiniertes Feld<br />Eine Aufzählung aller vorhandenen Feldnamen liefert die Funktion <a href="FLACGetItemKeysW.html">FLACGetItemKeysW</a>.
* Beschreibung english: set a user defined field<br />use the method <a href="FLACGetItemKeysW.html">FLACGetItemKeysW</a> for a list of all existing keys.
* @paramD LPCWSTR myKey der Feldname der gesetzt werden soll
* @paramD LPCWSTR myValue der Wert des Feldes, ein Leerstring entfernt das Feld
* @paramE LPCWSTR myKey the key you want to set
* @paramE LPCWSTR myValue new value for the key, empty string remove this item
*/
extern "C" void __stdcall FLACSetUserItemW(LPCWSTR ItemKey, LPCWSTR textString) 
{
	flac.SetUserItem(getValidPointer(ItemKey), getValidPointer(textString));
}
/**
* Methode: FLACGetItemKeysW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: liefert alle Feldnamen durch Kommas getrennt 
* Beschreibung english: get all field names ( comma separated )
* @returnD BSTR Feldnamen
* @returnE BSTR fieldnames
*/
extern "C" BSTR __stdcall FLACGetItemKeysW() 
{
	return flac.GetAllKeys().AllocSysString();
}

/**
* Methode: FLACAddPictureFileW
* @link FLAC
* @since 2.0.2.0
* Beschreibung deutsch: überträgt ein externes Bild in einen FLAC-Tag<br /><br />Es kann mehr als einen Eintrag geben, aber nur einen mit der gleichen Sprache und Inhaltsbeschreibung.
* Beschreibung english: store a picture from a file in the tag<br />There may be more than one picture frame in each tag, but only one with the same content descriptor.
* @paramD LPCWSTR Filename Name der Bild-Datei
* @paramD LPCWSTR Beschreibung Kommentar zu dem Bild
* @paramD Int16 PictureTyp Typ des Bildes von 0 bis 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @paramD Int16 asLink Zeigt an, wie das Bild übernommen wird<br /><br />-1 es wird nur der Dateiname übernommen<br />0 = das Bild selbst wird übernommen
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR Filename name of the picture file
* @paramE LPCWSTR description a description of the picture
* @paramE Int16 picture type from 0 to 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @paramE Int16 asLink shows how the picture will be stored<br /><br />-1 = only a link to the image will be stored<br />0 = the image data will be stored
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall FLACAddPictureFileW(LPCWSTR FileName, LPCWSTR Description, short PictureType, short asLink)
{
	CFlacCover *pic = new CFlacCover( (BYTE)PictureType, Description);
	bool result = (asLink) ? pic->setFileLink(getValidPointer(FileName)) : pic->setPictureFile(getValidPointer(FileName));
	if (result)
		flac.replaceCover(pic);
	else
		delete pic;
	return b2s(result);
}


/**
* Methode: FLACAddPictureArrayW
* @link FLAC
* @since 2.0.2.0
* Beschreibung deutsch: überträgt ein Bild aus einem Byte-Array in einen FLAC-Tag<br /><br />Es kann mehr als einen Eintrag geben, aber nur einen mit der gleichen Sprache und Inhaltsbeschreibung.
* Beschreibung english: store a picture from a byte array in the flac tag<br />There may be more than one picturee in each tag, but only one with the same content descriptor.
* @paramD Pointer Zeiger auf ein Byte Array mit den Daten des Bildes
* @paramD UInt32 Length die Grösse des Arrays in Bytes
* @paramD LPCWSTR Beschreibung Kommentar zu dem Bild
* @paramD Int16 PictureTyp Typ des Bildes von 0 bis 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer pointer to a byte array with the picture datas
* @paramE UInt32 Length the size of the array
* @paramE LPCWSTR description a description of the picture
* @paramE Int16 index Index from 1 to n; if n is not present a new entry will be created
* @paramE Int16 picture type from 0 to 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall FLACAddPictureArrayW(BYTE *arr, u32 Length, LPCWSTR Description, short PictureType)
{
	CFlacCover *pic = new CFlacCover( (BYTE)PictureType, getValidPointer(Description));
	pic->data.AddMemory(arr, Length);
	return b2s(flac.replaceCover(pic));
}


/**
* Methode: FLACDeletePicturesW
* @link FLAC
* @since 2.0.2.0
* Beschreibung deutsch: entfernt alle Bilder aus dem flac tag
* Beschreibung english: remove all pictures from the flag tag
*/
extern "C" void __stdcall FLACDeletePicturesW() 
{
	flac.DeletePictures();
}

/**
* Methode: FLACDeletePictureW
* @link FLAC
* @since 2.0.2.0
* Beschreibung deutsch: entfernt ein Bild aus dem flag tag
* Beschreibung english: remove a picture from the flag tag
* @paramD Int16 Index von 1 bis <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnD Int16 normalerweise -1, bei Fehler oder nicht vorhandenem Bild 0
* @paramE Int16 Index index from 1 to <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnE Int16 normally -1, 0 on error or picture not present
*/
extern "C" short __stdcall FLACDeletePictureW(short Index) 
{
	return b2s(flac.DeletePicture(Index));
}


/**
* Methode: FLACGetPictureDescriptionW
* @link FLAC
* @since 2.0.2.0
* Beschreibung deutsch: liefert die Beschreibung eines Bildes
* Beschreibung english: get the description from a picture
* @paramD Int16 index Index von 1 bis <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnD BSTR Beschreibung eines Bildes
* @paramE Int16 index index from 1 <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnE BSTR the description of the picture
*/
extern "C" BSTR __stdcall FLACGetPictureDescriptionW(short Index) 
{
	CFlacCover* cover = flac.GetCover(Index);
	return (cover != NULL) ? cover->description.AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: FLACGetPictureSizeW
* @link FLAC
* @since 2.0.2.0
* Beschreibung deutsch: liefert die Grösse eines Bildes in Bytes
* Beschreibung english: get the size from a picture in bytes
* @paramD Int16 index Index von 1 bis <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnD Int32 die Grösse des Bildes in Bytes
* @paramE Int16 index index from 1 to <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnE Int32 the size of the picture in bytes
*/
extern "C" long __stdcall FLACGetPictureSizeW(short Index) 
{
	CFlacCover* cover = flac.GetCover(Index);
	return (cover != NULL) ? cover->getPictureSize() : 0;
}

/**
* Methode: FLACGetPictureTypeW
* @link FLAC
* @since 2.0.2.0
* Beschreibung deutsch: liefert den Bildtyp eines Bildes
* Beschreibung english: get the picture type from a picture
* @paramD Int16 index Index von 1 bis <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnD Int16 Typ des Bildes von 0 bis 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @paramE Int16 index index from 1 to <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnE Int16 picture type from 0 to 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
*/
extern "C" short __stdcall FLACGetPictureTypeW(short Index) 
{
	CFlacCover* cover = flac.GetCover(Index);
	return (cover != NULL) ? (short)cover->type : 0;
}



/**
* Methode: FLACGetPictureTypeTextW
* @link FLAC
* @since 2.0.2.0
* Beschreibung deutsch: liefert den Bildtyp eines Bildes
* Beschreibung english: get the picture type from a picture
* @paramD Int16 index Index von 1 bis <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnD BSTR Typ des Bildes als Text
* @paramE Int16 index index from 1 to<a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnE BSTR picture type as text
*/
extern "C" BSTR __stdcall FLACGetPictureTypeTextW(short Index) 
{
	CFlacCover* cover = flac.GetCover(Index);
	return (cover != NULL) ? cover->getPictureTypeAsText().AllocSysString() : CTools::instance().GetEmptyBSTR();
}


/**
* Methode: FLACGetPictureMimeW
* @link FLAC
* @since 2.0.2.0
* Beschreibung deutsch: liefert den Mimetyp eines Bildes, z.B. "image/jpeg" oder "image/bmp" oder "XXX" bei unbekanntem Format<br\>Ist das Bild als Verweis gespeichert worden, liefert die Funktion den Dateinamen des Bildes angeführt von "-->", also z.B. "-->C:\test.jpg".
* Beschreibung english: get the mime type from a picture, e.g. "image/jpeg" or "image/bmp" or "XXX" for unknown format<br\>If the file was stored as a Link then this method returns then filename starting with "-->", e.g. "-->C:\test.jpg"
* @paramD Int16 index Index von 1 bis <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnD BSTR Mimetyp eines Bildes
* @paramE Int16 index index from 1 to <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnE BSTR the mime type of the picture
*/
extern "C" BSTR __stdcall FLACGetPictureMimeW(short Index) 
{
	CFlacCover* cover = flac.GetCover(Index);
	return (cover != NULL) ? cover->getMime().AllocSysString() : CTools::instance().GetEmptyBSTR();
}



/**
* Methode: FLACGetPictureCountW
* @link FLAC
* @since 2.0.2.0
* Beschreibung deutsch: liefert die Anzahl der Bilder
* Beschreibung english: get the number of pictures
* @returnD Int16 Anzahl Bilder
* @returnE Int16 number of pictures
*/
extern "C" short __stdcall FLACGetPictureCountW()
{
	return flac.GetPictureCount();
}

/**
* Methode: FLACGetPictureFileW
* @link FLAC
* @since 2.0.2.0
* Beschreibung deutsch: überträgt ein Bild aus dem Flac Tag in die angegebene Datei
* Beschreibung english: get a picture from the tag and store it in the specified file
* @paramD LPCWSTR Filename Name der Datei in die das Bild übertragen wird
* @paramD Int16 index Index von 1 bis <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnD Int16 normalerweise -1, bei Fehler oder nicht gefundenem Index 0
* @paramE LPCWSTR Filename Name of the file where the picture will be stored
* @paramE Int16 index index from 1 to <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnE Int16 normally -1, 0 on error or index not present
*/
extern "C" short __stdcall FLACGetPictureFileW(LPCWSTR FileName, short Index) 
{
	CFlacCover* cover = flac.GetCover(Index);
	return (cover != NULL) ? b2s(cover->getPictureFile(getValidPointer(FileName))) : b2s(0);	
}

/**
* Methode: FLACGetPictureArrayW
* @link FLAC
* @since 2.0.2.0
* Beschreibung deutsch: überträgt ein Bild aus dem FLAC-Tag in ein Byte-Array
* Beschreibung english: get a picture from the tag and copy it in a byte array
* @paramD Pointer Zeiger auf das Array mit den Byte-Daten
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD Int16 index Index von 1 bis <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer pointer to a Byte array
* @paramE UInt32 maxLen maximum size of the byte array
* @paramE Int16 index index from 1 to <a href="FLACGetPictureCountW.html">FLACGetPictureCountW</a>
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall FLACGetPictureArrayW(BYTE *arr, u32 maxLen, short Index)
{
	CFlacCover* cover = flac.GetCover(Index);
	return (cover != NULL) ? cover->getPictureArray(arr, maxLen) : -1;
}

/**
* Methode: FLACSaveChangesToFileW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: speichert die Kommentare im Vorbis-Format in eine FLAC-Datei
* Beschreibung english: store the comments in vorbis format in a file
* @paramD LPCWSTR FileName Name der Datei
* @paramE LPCWSTR filename name of the file
* @returnD Int16 liefert 0 bei Fehler, ansonsten -1
* @returnE Int16 0 on error, otherwise -1
*/
extern "C" short __stdcall FLACSaveChangesToFileW(LPCWSTR FileName)
{
	FileName = getValidPointer(FileName);
	if (GetFormat(FileName) == AUDIO_FORMAT_FLAC)
		return b2s(flac.SaveToFile(FileName));
	CTools::instance().setLastError(ERR_TAG_NOT_ALLOWED);
	return b2s(false);  
}

/**
* Methode: FLACSaveChangesW
* @link FLAC
* @since 2.0.1.0
* Beschreibung deutsch: speichert die Kommentare im Vorbis-Format in die zuletzt analysierte Datei
* Beschreibung english: store the comments in vorbis format in the last analyzed file
* @returnD Int16 liefert 0 bei Fehler, ansonsten -1
* @returnE Int16 0 on error, otherwise -1
*/
extern "C" short __stdcall FLACSaveChangesW()
{
	return FLACSaveChangesToFileW(lastFile);
}

/* ------------------------------------------------------------------------------------- */

/**
* Methode: MONKEYGetBitsW
* @link MONKEY
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Bits pro Sample
* Beschreibung english: get the bits per sample
* @returnD Int16 Bits pro Sample
* @returnE Int16 bits per sample
*/
extern "C" short __stdcall MONKEYGetBitsW()
{
	return monkey.GetBits();
}


/**
* Methode: MONKEYGetCompressionW
* @link MONKEY
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Kompression
* Beschreibung english: get the compression
* @returnD BSTR Kompression
* @returnE BSTR compression
*/
extern "C" BSTR __stdcall MONKEYGetCompressionW()
{
	return monkey.GetCompression().AllocSysString();
}


/**
* Methode: MONKEYGetFramesW
* @link MONKEY
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Frames
* Beschreibung english: get the number of frames
* @returnD Int32 Anzahl der Frames
* @returnE Int32 number of frames
*/
extern "C" long __stdcall MONKEYGetFramesW()
{
	return monkey.GetFrames();
}


/**
* Methode: MONKEYGetPeakW
* @link MONKEY
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Spitzenwert
* Beschreibung english: get the peak level
* @returnD Float Spitzenwert
* @returnE Float peak level
*/
extern "C" float __stdcall MONKEYGetPeakW()
{
	return monkey.GetPeak();
}


/**
* Methode: MONKEYGetCompressionRatioW
* @link MONKEY
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Kompressionsrate
* Beschreibung english: get the compression ratio
* @returnD Float Kompressionsrate
* @returnE Float compression ratio
*/
extern "C" float __stdcall MONKEYGetCompressionRatioW()
{
	return monkey.GetCompressionRatio();
}


/**
* Methode: MONKEYGetSamplesW
* @link MONKEY
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Samples
* Beschreibung english: get the number of samples
* @returnD Int32 Samples
* @returnE Int32 samples
*/
extern "C" long __stdcall MONKEYGetSamplesW()
{
	return monkey.GetSamples();
}


/**
* Methode: MONKEYGetSamplesPerFrameW
* @link MONKEY
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Samples pro Frame
* Beschreibung english: get the number of samples per frame
* @returnD Int32 Samples pro Frame
* @returnE Int32 samples per frame
*/
extern "C" long __stdcall MONKEYGetSamplesPerFrameW()
{
	return monkey.GetSamplesPerFrame();
}

/* ------------------------------------------------------------------------------------- */

/**
* Methode: MP4GetAllFrameIDsW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: liefert eine kommagetrennte Liste aller vorkommenden eindeutigen Frame IDs 
* Beschreibung english: get a comma based List of all unique frame ids
* @returnD BSTR der Textstring Kommagetrennter String mit Frame IDs
* @returnE BSTR the text string comma based text string with Frame IDs
*/
extern "C" BSTR __stdcall MP4GetAllFrameIDsW()
{
	return mp4.getILSTFrameIDs().AllocSysString();
}

/**
* Methode: MP4GetTextFrameW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Text eines ILST mp4 Textframes
* Beschreibung english: get the text entry of a mp4 ILST textframe
* @paramD UInt32 FrameID die ID des Frames
* @paramE UInt32 FrameID the frame id
* @returnD BSTR liefert den Text
* @returnE BSTR frame text
*/
extern "C" BSTR __stdcall MP4GetTextFrameW(u32 FrameID)
{
	return mp4.GetILSTText(FrameID).AllocSysString();
}

/**
* Methode: MP4SetTextFrameW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: setzt den text eines mp4 ILST Textframes
* Beschreibung english: set the text entry of a mp4 ILST textframe
* @paramD UInt32 FrameID die ID des Frames 
* @paramD LPCWSTR textString der neue Text
* @paramE UInt32 FrameID the ID of the frame
* @paramE LPCWSTR textString the new text
*/
extern "C" void __stdcall MP4SetTextFrameW(u32 FrameID, LPCWSTR textString)
{
	mp4.SetILSTText(FrameID, getValidPointer(textString));
}

/**
* Methode: MP4GetiTuneFrameW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Text eines mp4 iTune Textframes
* Beschreibung english: get the text entry of a mp4 iTune textframe
* @paramD LPCWSTR FrameID die ID des Frames
* @paramE LPCWSTR FrameID the frame id
* @returnD BSTR liefert den Text
* @returnE BSTR frame text
*/
extern "C" BSTR __stdcall MP4GetiTuneFrameW(LPCWSTR FrameID)
{
	return mp4.GetItuneText(getValidPointer(FrameID)).AllocSysString();
}

/**
* Methode: MP4SetiTuneFrameW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: setzt den text eines mp4 iTune Textframes
* Beschreibung english: set the text entry of a mp4 iTune textframe
* @paramD LPCWSTR FrameID die ID des Frames 
* @paramD LPCWSTR textString der neue Text
* @paramE LPCWSTR FrameID the ID of the frame
* @paramE LPCWSTR textString the new text
*/
extern "C" void __stdcall MP4SetiTuneFrameW(LPCWSTR FrameID, LPCWSTR textString)
{
	mp4.SetItuneText(getValidPointer(FrameID), getValidPointer(textString));
}

/**
* Methode: MP4GetVersionW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Format / die Version
* Beschreibung english: get the format / version of the file
* @returnD BSTR Format
* @returnE BSTR format
*/
extern "C" BSTR __stdcall MP4GetVersionW()
{
	return mp4.GetFileVersion().AllocSysString();
}

/**
* Methode: MP4GetGenreW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Genre
* Beschreibung english: get the genre
* @returnD BSTR Genre
* @returnE BSTR genre
*/
extern "C" BSTR __stdcall MP4GetGenreW()
{
	return mp4.GetGenre().AllocSysString();	
}

/**
* Methode: MP4SetGenreW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Genre
* Beschreibung english: set the genre
* @paramD LPCWSTR myGenre Genre
* @paramE LPCWSTR myGenre genre
*/
extern "C" void __stdcall MP4SetGenreW(LPCWSTR textString)
{
	mp4.SetGenre(getValidPointer(textString)); 
}


/**
* Methode: MP4GetPictureCountW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Bilder
* Beschreibung english: get the number of pictures
* @returnD Int32 Anzahl Bilder
* @returnE Int32 number of pictures
*/
extern "C" long __stdcall MP4GetPictureCountW()
{
	return mp4.GetPictureCount();
}

/**
* Methode: MP4GetTrackW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Track
* Beschreibung english: get the track
* @returnD BSTR Track
* @returnE BSTR track
*/
extern "C" BSTR __stdcall MP4GetTrackW()
{
	return mp4.GetTrack().AllocSysString();	
}


/**
* Methode: MP4SetTrackW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Track
* Beschreibung english: set the track
* @paramD LPCWSTR myTrack Track
* @paramE LPCWSTR myTrack track
*/
extern "C" void __stdcall MP4SetTrackW(LPCWSTR textString)
{
	mp4.SetTrack(getValidPointer(textString));
}


/**
* Methode: MP4GetPictureFileW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: überträgt ein Bild mit einem bestimmten Index in eine Datei
* Beschreibung english: get a Picture with a specific Index from the Frame and store it in a file
* @paramD LPCWSTR FileName Name der Datei
* @paramE LPCWSTR filename name of the file
* @paramD Int16 Index Index der Bilddatei von 1 bis <a href="MP4GetPictureCountW.html">MP4GetPictureCountW</a>
* @paramE Int16 index index from 1 to <a href="MP4GetPictureCountW.html">MP4GetPictureCountW</a>
* @returnD Int16 liefert 0 bei Fehler, ansonsten -1
* @returnE Int16 0 on error, otherwise -1
*/
extern "C" short __stdcall MP4GetPictureFileW(LPCWSTR FileName, short Index)
{
	return b2s(mp4.GetPicture(getValidPointer(FileName), Index));
}

/**
* Methode: MP4GetPictureArrayW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: überträgt ein Bild aus dem frame in ein Byte-Array
* Beschreibung english: get a picture from the tag and copy it in a byte array
* @paramD Pointer Zeiger auf das Array mit den Byte-Daten
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD Int16 index Index von 1 bis <a href="MP4GetPictureCountW.html">MP4GetPictureCountW</a>
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer pointer to a Byte array
* @paramE UInt32 maxLen maximum size of the byte array
* @paramE Int16 index index from 1 to <a href="MP4GetPictureCountW.html">MP4GetPictureCountW</a>
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall MP4GetPictureArrayW(BYTE *arr, u32 maxLen, short Index)
{
	return mp4.GetPictureArray(arr, maxLen, Index);		
}


/**
* Methode: MP4GetPictureMimeW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Mimetyp eines Bildes, z.B. "jpg" oder "png"
* Beschreibung english: get the mime type from a picture, e.g. "jpg" or "png"
* @paramD Int16 index Index von 1 bis <a href="MP4GetPictureCountW.html">MP4GetPictureCountW</a>
* @returnD BSTR Mimetyp eines Bildes
* @paramE Int16 index index from 1 to <a href="MP4GetPictureCountW.html">MP4GetPictureCountW</a>
* @returnE BSTR the mime type of the picture
*/
extern "C" BSTR __stdcall MP4GetPictureMimeW(short Index) 
{
	return mp4.GetPictureMime(Index).AllocSysString();
}

/**
* Methode: MP4GetPictureSizeW
* @link MP4
* @since 2.0.2.0
* Beschreibung deutsch: liefert die Grösse eines Bildes in Bytes
* Beschreibung english: get the size from a picture in bytes
* @paramD Int16 index Index von 1 bis Anzahl Picture frames
* @returnD Int32 die Grösse des Bildes in Bytes
* @paramE Int16 index index from 1 to picture frame count
* @returnE Int32 the size of the picture in bytes
*/
extern "C" long __stdcall MP4GetPictureSizeW(short Index) 
{
	return mp4.GetPictureSize(Index);
}

/**
* Methode: MP4AddPictureArrayW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: überträgt ein Bild aus einem Byte-Array in den Tag
* Beschreibung english: store a picture from a byte array in the tag
* @paramD Pointer Zeiger auf ein Byte Array mit den Daten des Bildes
* @paramD UInt32 Length die Grösse des Arrays in Bytes
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE Pointer pointer to a byte array with the picture datas
* @paramE UInt32 Length the size of the array
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall MP4AddPictureArrayW(BYTE *arr, u32 Length)
{
	return  b2s(mp4.AddPictureArray(arr, Length));
}

/**
* Methode: MP4AddPictureFileW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: überträgt ein Bild aus einer Datei in einen Tag
* Beschreibung english: store a picture from a file in the tag
* @paramD LPCWSTR Filename Name der Bild-Datei
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR Filename name of the picture file
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall MP4AddPictureFileW(LPCWSTR FileName)
{
	return b2s(mp4.AddPictureFile(getValidPointer(FileName)));
}


/**
* Methode: MP4DeletePicturesW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: entfernt alle Bilder
* Beschreibung english: remove all pictures
*/
extern "C" void __stdcall MP4DeletePicturesW() 
{
	mp4.DeletePictures();
}

/**
* Methode: MP4DeletePictureW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: entfernt ein Bild
* Beschreibung english: remove a picture
* @paramD Int16 Index von 1 bis <a href=MP4GetPictureCountW.html">MP4GetPictureCountW</a>
* @returnD Int16 normalerweise -1, bei Fehler oder nicht vorhandenem Bild 0
* @paramE Int16 Index index from 1 to <a href="MP4GetPictureCountW.html">MP4GetPictureCountW</a>
* @returnE Int16 normally -1, 0 on error or picture not present
*/
extern "C" short __stdcall MP4DeletePictureW(short Index) 
{
	return b2s(mp4.DeletePictureFrame(Index));
}


/**
* Methode: MP4DeleteEntriesW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: löscht alle benutzerdefinierten Einträge wie Titel, Bilder etc.
* Beschreibung english: deletes all userdefined entries like title, pictures etc.
*/
extern "C" void __stdcall MP4DeleteEntriesW()
{
	mp4.RemoveTag();
}

/**
* Methode: MP4SaveChangesToFileW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: speichert Änderungen der mp4 Datei
* Beschreibung english: store the mp4 changes in a file
* @paramD LPCWSTR FileName Name der Datei
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR FileName name of the file
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall MP4SaveChangesToFileW(LPCWSTR FileName)
{
	FileName = getValidPointer(FileName);
	if (GetFormat(FileName) != AUDIO_FORMAT_INVALID)
	{
		bool result = mp4.SaveToFile(FileName);
		if (result && !lastFile.IsEmpty() && lastFile.CompareNoCase(FileName) == 0)
			AUDIOAnalyzeFileW(FileName);
		return b2s(result);
	}
	CTools::instance().setLastError(ERR_TAG_NOT_ALLOWED);
	return b2s(false);   
}

/**
* Methode: MP4SaveChangesW
* @link MP4
* @since 2.0.1.0
* Beschreibung deutsch: speichert die MP4-Änderungen in die zuletzt analysierte Datei
* Beschreibung english: store the mp4 in the last analyzed file
* @returnD Int16 normalerweise -1, bei Fehler 0
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall MP4SaveChangesW()
{
	return MP4SaveChangesToFileW(lastFile);
}

/* ------------------------------------------------------------------------------------- */


/**
* Methode: MPEGIsCopyrightedW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: zeigt an, ob die Datei Copyright geschützt ist
* Beschreibung english: shows the copyright state of a file
* @returnD Int16 -1 wenn Copyright geschützt, ansonsten 0
* @returnE Int16 -1 if copyright protected, otherwise 0
*/
extern "C" short __stdcall MPEGIsCopyrightedW()
{
	return b2s(mpeg.GetCopyrightBit());
}


/**
* Methode: MPEGIsOriginalW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: zeigt an, ob die Datei ein Original ist
* Beschreibung english: shows the original state of the file
* @returnD Int16 -1, wenn es die Originaldatei ist, ansonsten 0
* @returnE Int16 -1 if the file is the original, otherwise 0
*/
extern "C" short __stdcall MPEGIsOriginalW()
{
	return b2s(mpeg.GetOriginalBit());
}


/**
* Methode: MPEGIsPaddingW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: zeigt an, ob Auffüllen verwendet wird
* Beschreibung english: shows the state of padding
* @returnD Int16 -1, wenn Auffüllen verwendet wird, ansonsten 0
* @returnE Int16 -1 if padding used, otherwise 0
*/
extern "C" short __stdcall MPEGIsPaddingW()
{
	return b2s(mpeg.GetPaddingBit());
}


/**
* Methode: MPEGIsPrivateW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: zeigt an, ob das Privat bit gesetzt ist
* Beschreibung english: shows the state of the private bit
* @returnD Int16 -1 wenn gesetzt, ansonsten 0
* @returnE Int16 -1 if set, otherwise 0
*/
extern "C" short __stdcall MPEGIsPrivateW()
{
	return b2s(mpeg.GetPrivateBit());
}

/**
* Methode: MPEGIsProtectedW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: zeigt an, ob das Protected bit gesetzt ist
* Beschreibung english: shows the state of the protected bit
* @returnD Int16 -1 wenn gesetzt, ansonsten 0
* @returnE Int16 -1 if set, otherwise 0
*/
extern "C" short __stdcall MPEGIsProtectedW() 
{
	return b2s(!mpeg.GetProtectionBit());
}

/**
* Methode: MPEGGetEmphasisW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Rauschunterdrückung
* Beschreibung english: get the emphasis
* @returnD BSTR Rauschunterdrückung
* @returnE BSTR emphasis
*/
extern "C" BSTR __stdcall MPEGGetEmphasisW()
{
	return mpeg.GetEmphasis().AllocSysString();
}


/**
* Methode: MPEGGetEncoderW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Kodierer
* Beschreibung english: get the encoder
* @returnD BSTR Kodierer
* @returnE BSTR encoder
*/
extern "C" BSTR __stdcall MPEGGetEncoderW()
{
	return mpeg.GetEncoder().AllocSysString();
}

/**
* Methode: MPEGGetVersionW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Version z.B. "MPEG 2.0"
* Beschreibung english: get the mpeg version e.g. "MPEG 2.0"
* @returnD BSTR Version
* @returnE BSTR version
*/
extern "C" BSTR __stdcall MPEGGetVersionW() 
{
	return mpeg.GetFileVersion().AllocSysString();
}

/**
* Methode: MPEGGetFramePositionW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Position des ersten gültigen Frames
* Beschreibung english: get the position of the first valid frame
* @returnD Int32 Position des Frames
* @returnE Int32 position of the frame
*/
extern "C" long __stdcall MPEGGetFramePositionW()
{
	return mpeg.GetFramePosition();
}


/**
* Methode: MPEGGetFrameSizeW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Frame-Grösse in Bytes
* Beschreibung english: get the frame size in bytes
* @returnD Int32 Frame-Grösse
* @returnE Int32 frame size
*/
extern "C" long __stdcall MPEGGetFrameSizeW()
{
	return mpeg.GetFrameSize();
}


/**
* Methode: MPEGGetFramesW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Frames
* Beschreibung english: get the number of Frames
* @returnD Int32 Anzahl der Frames
* @returnE Int32 number of frames
*/
extern "C" long __stdcall MPEGGetFramesW()
{
	return mpeg.GetFrames();
}


/**
* Methode: MPEGGetLayerW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Layer
* Beschreibung english: get the layer
* @returnD BSTR Layer
* @returnE BSTR layer
*/
extern "C" BSTR __stdcall MPEGGetLayerW()
{
	return mpeg.GetLayer().AllocSysString();
}


/**
* Methode: MPEGIsVBRW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: zeigt an, ob variable Bitrate verwendet wird
* Beschreibung english: shows -1, if it is variable bitrate
* @returnD Int16 -1 bei variabler Bitrate, ansonsten 0
* @returnE Int16 -1 if variable bitrate, otherwise 0
*/
extern "C" short __stdcall MPEGIsVBRW()
{
	return b2s(mpeg.IsVBR());
}


/**
* Methode: MPEGSetCopyrightedW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: verändert das Copyright Bit einer MPEG-Datei
* Beschreibung english: modify the State of the Copyright Bit
* @paramD LPCWSTR FileName Name der Datei
* @paramD Int16 newValue 1 setzt und 0 löscht das Copyright Bit
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR filename name of the file
* @paramE Int16 newValue 1 set and 0 reset the Copyright Bit
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall MPEGSetCopyrightedW(LPCWSTR FileName, short newValue)
{
	return b2s(mpeg.SetCopyrightBit(getValidPointer(FileName), (newValue != 0) ));
}


/**
* Methode: MPEGSetOriginalW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: verändert das Original Bit einer MPEG-Datei
* Beschreibung english: modify the State of the Original Bit
* @paramD LPCWSTR FileName Name der Datei
* @paramD Int16 newValue 1 setzt und 0 löscht das Original Bit
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR filename name of the file
* @paramE Int16 newValue 1 set and 0 reset the Original Bit
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall MPEGSetOriginalW(LPCWSTR FileName, short newValue)
{
	return b2s(mpeg.SetOriginalBit(getValidPointer(FileName), (newValue != 0) ));
}


/**
* Methode: MPEGSetPrivateW
* @link MPEG
* @since 2.0.1.0
* Beschreibung deutsch: verändert das Private Bit einer MPEG-Datei
* Beschreibung english: modify the State of the Private Bit
* @paramD LPCWSTR FileName Name der Datei
* @paramD Int16 newValue 1 setzt und 0 löscht das Private Bit
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR filename name of the file
* @paramE Int16 newValue 1 set and 0 reset the Private Bit
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall MPEGSetPrivateW(LPCWSTR FileName, short newValue)
{
	return b2s(mpeg.SetPrivateBit(getValidPointer(FileName), (newValue != 0) ));
}


/**
* Methode: MPPGetFramesW
* @link MPP
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Frames
* Beschreibung english: get the number of frames
* @returnD Int32 Anzahl Frames
* @returnE Int32 number of Frames
*/
extern "C" long __stdcall MPPGetFramesW()
{
	return mpp.GetFrameCount();
}


/**
* Methode: MPPGetStreamVersionW
* @link MPP
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Stream Version
* Beschreibung english: get the stream version
* @returnD Int16 Version
* @returnE Int16 version
*/
extern "C" short __stdcall MPPGetStreamVersionW()
{
	return mpp.GetStreamVersion();
}

/* ------------------------------------------------------------------------------------- */

/**
* Methode: OGGGetBitRateNominalW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert die nominale Bitrate
* Beschreibung english: get the nominal bitrate
* @returnD Int32 nominale Bitrate
* @returnE Int32 nominal bitrate
*/
extern "C" long __stdcall OGGGetBitRateNominalW()
{
	return ogg.GetBitRateNominal();
}


/**
* Methode: OGGGetAlbumW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Album
* Beschreibung english: get the album
* @returnD BSTR Album
* @returnE BSTR album
*/
extern "C" BSTR __stdcall OGGGetAlbumW()
{
	return ogg.GetUserItem(VORBIS_ALBUM).AllocSysString();
}


/**
* Methode: OGGSetAlbumW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Album
* Beschreibung english: set the album
* @paramD LPCWSTR myAlbum Album
* @paramE LPCWSTR myAlbum album
*/
extern "C" void __stdcall OGGSetAlbumW(LPCWSTR textString)
{
	ogg.SetUserItem(VORBIS_ALBUM, getValidPointer(textString));
}


/**
* Methode: OGGGetArtistW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Künstler
* Beschreibung english: get the artist
* @returnD BSTR Künstler
* @returnE BSTR artist
*/
extern "C" BSTR __stdcall OGGGetArtistW()
{
	return ogg.GetUserItem(VORBIS_ARTIST).AllocSysString();
}


/**
* Methode: OGGSetArtistW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Künstler
* Beschreibung english: set the artist
* @paramD LPCWSTR myArtist Künstler
* @paramE LPCWSTR myArtist artist
*/
extern "C" void __stdcall OGGSetArtistW(LPCWSTR textString)
{
	ogg.SetUserItem(VORBIS_ARTIST, getValidPointer(textString));
}


/**
* Methode: OGGGetCommentW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Kommentar
* Beschreibung english: get the comment
* @returnD BSTR Kommentar
* @returnE BSTR comment
*/
extern "C" BSTR __stdcall OGGGetCommentW()
{
	return ogg.GetUserItem(VORBIS_COMMENT).AllocSysString();
}


/**
* Methode: OGGSetCommentW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Kommentar
* Beschreibung english: set the comment
* @paramD LPCWSTR myComment Kommentar
* @paramE LPCWSTR myComment comment
*/
extern "C" void __stdcall OGGSetCommentW(LPCWSTR textString)
{
	ogg.SetUserItem(VORBIS_COMMENT, getValidPointer(textString));
}


/**
* Methode: OGGGetDateW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Datum
* Beschreibung english: get the date
* @returnD BSTR Datum
* @returnE BSTR date
*/
extern "C" BSTR __stdcall OGGGetDateW()
{
	return ogg.GetUserItem(VORBIS_DATE).AllocSysString();
}


/**
* Methode: OGGSetDateW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Datum
* Beschreibung english: set the date
* @paramD LPCWSTR myDate Datum
* @paramE LPCWSTR myDate date
*/
extern "C" void __stdcall OGGSetDateW(LPCWSTR textString)
{
	ogg.SetUserItem(VORBIS_DATE, getValidPointer(textString));
}


/**
* Methode: OGGGetDescriptionW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Beschreibung
* Beschreibung english: get the description
* @returnD BSTR Beschreibung
* @returnE BSTR description
*/
extern "C" BSTR __stdcall OGGGetDescriptionW()
{
	return ogg.GetUserItem(VORBIS_DESCRIPTION).AllocSysString();
}


/**
* Methode: OGGSetDescriptionW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt die Beschreibung
* Beschreibung english: set the description
* @paramD LPCWSTR myDescription Beschreibung
* @paramE LPCWSTR myDescription description
*/
extern "C" void __stdcall OGGSetDescriptionW(LPCWSTR textString)
{
	ogg.SetUserItem(VORBIS_DESCRIPTION, getValidPointer(textString));
}


/**
* Methode: OGGGetGenreW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Genre
* Beschreibung english: get the genre
* @returnD BSTR Genre
* @returnE BSTR genre
*/
extern "C" BSTR __stdcall OGGGetGenreW()
{
	return ogg.GetUserItem(VORBIS_GENRE).AllocSysString();
}


/**
* Methode: OGGSetGenreW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Genre
* Beschreibung english: set the genre
* @paramD LPCWSTR myGenre Genre
* @paramE LPCWSTR myGenre genre
*/
extern "C" void __stdcall OGGSetGenreW(LPCWSTR textString)
{
	ogg.SetUserItem(VORBIS_GENRE, getValidPointer(textString));
}


/**
* Methode: OGGGetPerformerW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Künstler
* Beschreibung english: get the Performer
* @returnD BSTR Künstler
* @returnE BSTR Performer
*/
extern "C" BSTR __stdcall OGGGetPerformerW()
{
	return ogg.GetUserItem(VORBIS_PERFORMER).AllocSysString();
}


/**
* Methode: OGGSetPerformerW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Künstler
* Beschreibung english: set the Performer
* @paramD LPCWSTR myPerformer Künstler
* @paramE LPCWSTR myPerformer performer
*/
extern "C" void __stdcall OGGSetPerformerW(LPCWSTR textString)
{
	ogg.SetUserItem(VORBIS_PERFORMER, getValidPointer(textString));
}


/**
* Methode: OGGGetTitleW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Titel
* Beschreibung english: get the title
* @returnD BSTR Titel
* @returnE BSTR title
*/
extern "C" BSTR __stdcall OGGGetTitleW()
{
	return ogg.GetUserItem(VORBIS_TITLE).AllocSysString();
}


/**
* Methode: OGGSetTitleW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Titel
* Beschreibung english: set the title
* @paramD LPCWSTR myTitle Titel
* @paramE LPCWSTR myTitle title
*/
extern "C" void __stdcall OGGSetTitleW(LPCWSTR textString)
{
	ogg.SetUserItem(VORBIS_TITLE, getValidPointer(textString));
}


/**
* Methode: OGGGetTrackW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Track
* Beschreibung english: get the track
* @returnD BSTR Track
* @returnE BSTR track
*/
extern "C" BSTR __stdcall OGGGetTrackW()
{
	return ogg.GetUserItem(VORBIS_TRACKNUMBER).AllocSysString();
}


/**
* Methode: OGGSetTrackW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Track
* Beschreibung english: set the track
* @paramD LPCWSTR myTrack Track
* @paramE LPCWSTR myTrack track
*/
extern "C" void __stdcall OGGSetTrackW(LPCWSTR textString)
{
	ogg.SetUserItem(VORBIS_TRACKNUMBER, getValidPointer(textString));
}


/**
* Methode: OGGGetVendorW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Anbieter
* Beschreibung english: get the vendor
* @returnD BSTR Anbieter
* @returnE BSTR vendor
*/
extern "C" BSTR __stdcall OGGGetVendorW()
{
	return ogg.GetVendor().AllocSysString();
}


/**
* Methode: OGGSetVendorW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Anbieter
* Beschreibung english: set the vendor
* @paramD LPCWSTR myVendor Anbieter
* @paramE LPCWSTR myVendor vendor
*/
extern "C" void __stdcall OGGSetVendorW(LPCWSTR textString)
{
	ogg.SetVendor(getValidPointer(textString));
}

/**
* Methode: OGGGetVersionW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Version
* Beschreibung english: get the version
* @returnD BSTR Version
* @returnE BSTR version
*/
extern "C" BSTR __stdcall OGGGetVersionW() 
{
	return ogg.GetUserItem(VORBIS_VERSION).AllocSysString();
}

/**
* Methode: OGGSetVersionW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt die Version
* Beschreibung english: set the version
* @paramD LPCWSTR myVersion Version
* @paramE LPCWSTR myVersion version
*/
extern "C" void __stdcall OGGSetVersionW(LPCWSTR textString) 
{
	ogg.SetUserItem(VORBIS_VERSION, getValidPointer(textString));
}

/**
* Methode: OGGGetCopyrightW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Copyright
* Beschreibung english: get the copyright
* @returnD BSTR Copyright
* @returnE BSTR copyright
*/
extern "C" BSTR __stdcall OGGGetCopyrightW() 
{
	return ogg.GetUserItem(VORBIS_COPYRIGHT).AllocSysString();
}

/**
* Methode: OGGSetCopyrightW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Copyright
* Beschreibung english: set the copyright
* @paramD LPCWSTR myCopyright Copyright
* @paramE LPCWSTR myCopyright copyright
*/
extern "C" void __stdcall OGGSetCopyrightW(LPCWSTR textString) 
{
	ogg.SetUserItem(VORBIS_COPYRIGHT, getValidPointer(textString));
}

/**
* Methode: OGGGetLicenseW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Lizenz
* Beschreibung english: get the license
* @returnD BSTR Lizenz
* @returnE BSTR license
*/
extern "C" BSTR __stdcall OGGGetLicenseW() 
{
	return ogg.GetUserItem(VORBIS_LICENSE).AllocSysString();
}

/**
* Methode: OGGSetLicenseW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt die Lizenz
* Beschreibung english: set the license
* @paramD LPCWSTR myLicense Lizenz
* @paramE LPCWSTR myLicense license
*/
extern "C" void __stdcall OGGSetLicenseW(LPCWSTR textString) 
{
	ogg.SetUserItem(VORBIS_LICENSE, getValidPointer(textString));
}

/**
* Methode: OGGGetOrganizationW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Organisation
* Beschreibung english: get the organization
* @returnD BSTR Organisation
* @returnE BSTR organization
*/
extern "C" BSTR __stdcall OGGGetOrganizationW() 
{
	return ogg.GetUserItem(VORBIS_ORGANIZATION).AllocSysString();
}

/**
* Methode: OGGSetOrganizationW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt die Organisation
* Beschreibung english: set the organization
* @paramD LPCWSTR myOrganization Organisation
* @paramE LPCWSTR myOrganization organization
*/
extern "C" void __stdcall OGGSetOrganizationW(LPCWSTR textString) 
{
	ogg.SetUserItem(VORBIS_ORGANIZATION, getValidPointer(textString));
}

/**
* Methode: OGGGetLocationW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert eine Ortsangabe
* Beschreibung english: get the location
* @returnD BSTR Ortsangabe
* @returnE BSTR location
*/
extern "C" BSTR __stdcall OGGGetLocationW() 
{
	return ogg.GetUserItem(VORBIS_LOCATION).AllocSysString();
}

/**
* Methode: OGGSetLocationW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt eine Ortsangabe
* Beschreibung english: set the location
* @paramD LPCWSTR myLocation Ortsangabe
* @paramE LPCWSTR myLocation location
*/
extern "C" void __stdcall OGGSetLocationW(LPCWSTR textString) 
{
	ogg.SetUserItem(VORBIS_LOCATION, getValidPointer(textString));
}

/**
* Methode: OGGGetContactW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert einen Kontakt
* Beschreibung english: get the contact
* @returnD BSTR Kontakt
* @returnE BSTR contact
*/
extern "C" BSTR __stdcall OGGGetContactW() 
{
	return ogg.GetUserItem(VORBIS_CONTACT).AllocSysString();
}

/**
* Methode: OGGSetContactW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt einen Kontakt
* Beschreibung english: set the contact
* @paramD LPCWSTR myContact Kontakt
* @paramE LPCWSTR myContact contact
*/
extern "C" void __stdcall OGGSetContactW(LPCWSTR textString) 
{
	ogg.SetUserItem(VORBIS_CONTACT, getValidPointer(textString));
}

/**
* Methode: OGGGetISRCW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert den <a href="http://de.wikipedia.org/wiki/International_Standard_Recording_Code" target="_blank">International Standard Recording Code</a>
* Beschreibung english: get the <a href="http://en.wikipedia.org/wiki/International_Standard_Recording_Code" target="_blank">International Standard Recording Code</a>
* @returnD BSTR International Standard Recording Code
* @returnE BSTR International Standard Recording Code
*/
extern "C" BSTR __stdcall OGGGetISRCW() 
{
	return ogg.GetUserItem(VORBIS_ISRC).AllocSysString();
}

/**
* Methode: OGGSetISRCW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt den <a href="http://de.wikipedia.org/wiki/International_Standard_Recording_Code" target="_blank">International Standard Recording Code</a>
* Beschreibung english: set the <a href="http://en.wikipedia.org/wiki/International_Standard_Recording_Code" target="_blank">International Standard Recording Code</a>
* @paramD LPCWSTR myISRC International Standard Recording Code
* @paramE LPCWSTR myISRC International Standard Recording Code
*/
extern "C" void __stdcall OGGSetISRCW(LPCWSTR textString) 
{
	ogg.SetUserItem(VORBIS_ISRC, getValidPointer(textString));
}

/**
* Methode: OGGGetItemKeysW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert alle Feldnamen durch Kommas getrennt 
* Beschreibung english: get all field names ( comma separated )
* @returnD BSTR Feldnamen 
* @returnE BSTR fieldnames
*/
extern "C" BSTR __stdcall OGGGetItemKeysW() 
{
	return ogg.GetAllKeys().AllocSysString();
}

/**
* Methode: OGGGetUserItemW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: liefert ein benutzerdefiniertes Feld<br />Eine Aufzählung aller vorhandenen Feldnamen liefert die Funktion OGGGetItemKeys.
* Beschreibung english: get a user defined field<br />use the method OGGGetItemKeys for a list of all existing keys.
* @paramD LPCWSTR myKey benutzerdefinierter Schlüssel
* @paramE LPCWSTR myKey userdefined key
* @returnD BSTR field Feldinhalt oder leer, falls Schlüssel nicht gefunden
* @returnE BSTR field userdefined item or empty if key not present
*/
extern "C" BSTR __stdcall OGGGetUserItemW(LPCWSTR ItemKey) 
{
	return ogg.GetUserItem(getValidPointer(ItemKey)).AllocSysString();
}

/**
* Methode: OGGSetUserItemW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: setzt ein benutzerdefiniertes Feld<br />Eine Aufzählung aller vorhandenen Feldnamen liefert die Funktion OGGGetItemKeys.
* Beschreibung english: set a user defined field<br />use the method OGGGetItemKeys for a list of all existing keys.
* @paramD LPCWSTR myKey der Feldname der gesetzt werden soll
* @paramD LPCWSTR myValue der Wert des Feldes, ein Leerstring entfernt das Feld
* @paramE LPCWSTR myKey the key you want to set
* @paramE LPCWSTR myValue new value for the key, empty string remove this item
*/
extern "C" void __stdcall OGGSetUserItemW(LPCWSTR ItemKey, LPCWSTR textString) 
{
	ogg.SetUserItem(getValidPointer(ItemKey), getValidPointer(textString));
}


/**
* Methode: OGGSaveChangesToFileW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: speichert die Ogg-Kommentare in eine Datei
* Beschreibung english: stores the ogg comments in a file
* @paramD LPCWSTR FileName Name der Datei
* @paramE LPCWSTR filename name of the file
* @returnD Int16 liefert 0 bei Fehler, ansonsten -1
* @returnE Int16 0 on error, otherwise -1
*/
extern "C" short __stdcall OGGSaveChangesToFileW(LPCWSTR FileName)
{
	FileName = getValidPointer(FileName);
	if (GetFormat(FileName) == AUDIO_FORMAT_OGGVORBIS)
		return b2s(ogg.SaveTag(FileName));
	CTools::instance().setLastError(ERR_TAG_NOT_ALLOWED);
	return b2s(false);
}

/**
* Methode: OGGSaveChangesW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: speichert die Ogg-Kommentare in die zuletzt analysierte Datei
* Beschreibung english: stores the ogg comments in the last analyzed file
* @returnD Int16 liefert 0 bei Fehler, ansonsten -1
* @returnE Int16 0 on error, otherwise -1
*/
extern "C" short __stdcall OGGSaveChangesW()
{
	return OGGSaveChangesToFileW(lastFile);
}


/**
* Methode: OGGRemoveTagFromFileW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: entfernt den OGG-Kommentar einer Datei. Achtung: Die Löschung wird sofort durchgeführt!
* Beschreibung english: remove the ogg comment from a file. Attention: This function removes the tag immediately!
* @paramD LPCWSTR FileName Name der Datei
* @paramE LPCWSTR filename name of the file
* @returnD Int16 liefert -1 wenn erfolgreich entfernt, ansonsten 0
* @returnE Int16 -1 if removed, otherwise 0
*/
extern "C" short __stdcall OGGRemoveTagFromFileW(LPCWSTR FileName)
{
	FileName = getValidPointer(FileName);
	if (GetFormat(FileName) == AUDIO_FORMAT_OGGVORBIS)
		return b2s(ogg.RemoveTag(FileName));
	return b2s(false);
}

/**
* Methode: OGGRemoveTagW
* @link OGG
* @since 2.0.1.0
* Beschreibung deutsch: entfernt den OGG-Kommentar der zuletzt analysierten Datei. Achtung: Die Löschung wird sofort durchgeführt!
* Beschreibung english: remove the ogg comment from the last analyzed file. Attention: This function removes the tag immediately!
* @returnD Int16 liefert -1 wenn erfolgreich entfernt, ansonsten 0
* @returnE Int16 -1 if removed, otherwise 0
*/
extern "C" short __stdcall OGGRemoveTagW()
{
	if (GetFormat(lastFile) == AUDIO_FORMAT_OGGVORBIS)
		return b2s(ogg.RemoveTag(lastFile));
	return b2s(false);
}

/* ------------------------------------------------------------------------------------- */

/**
* Methode: WAVGetBitsPerSampleW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Bits pro Sample
* Beschreibung english: get the bits per sample
* @returnD Int32 Bits pro Sample
* @returnE Int32 bits per sample
*/
extern "C" long __stdcall WAVGetBitsPerSampleW()
{
	return wav.GetBitsPerSample();
}


/**
* Methode: WAVGetBlockAlignW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Blockausrichtung
* Beschreibung english: get the block align
* @returnD Int32 Blockausrichtung
* @returnE Int32 block align
*/
extern "C" long __stdcall WAVGetBlockAlignW()
{
	return wav.GetBlockAlign();
}


/**
* Methode: WAVGetBytesPerSecondW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Bytes pro Sekunde
* Beschreibung english: get the number of bytes per second
* @returnD Int32 Byteanzahl
* @returnE Int32 number of bytes
*/
extern "C" long __stdcall WAVGetBytesPerSecondW()
{
	return wav.GetBytesPerSecond();
}


/**
* Methode: WAVGetChannelsW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Kanäle
* Beschreibung english: get the number of channels
* @returnD Int32 Anzahl Kanäle
* @returnE Int32 number of channels
*/
extern "C" long __stdcall WAVGetChannelsW()
{
	return wav.GetChannels();
}


/**
* Methode: WAVGetHeaderSizeW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Grösse des Headers in Byte
* Beschreibung english: get the size of the header
* @returnD Int32 Headergrösse
* @returnE Int32 header size
*/
extern "C" long __stdcall WAVGetHeaderSizeW()
{
	return wav.GetHeaderSize();
}

/**
* Methode: WAVGetFormatW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Format im Klartext
* Beschreibung english: get the format as text string
* @returnD BSTR Format
* @returnE BSTR format 
*/
extern "C" BSTR __stdcall WAVGetFormatW() 
{
	return wav.GetFormat().AllocSysString();
}

/**
* Methode: WAVGetFormatIDW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Format-ID der Datei
* Beschreibung english: get the format-id of the file
* @returnD Int16 Format Nummer mit folgender Bedeutung<br /><br /><table width="280" class="tablelayout"><tr><th width="22%">id</th><th width="78%">format</th></tr><tr><td width="22%">0</td><td width="78%">unknown</td></tr><tr><td width="22%">1</td><td width="78%">PCM/uncompressed</td></tr><tr><td width="22%">2</td><td width="78%">Microsoft ADPCM</td></tr><tr><td width="22%">6</td><td width="78%">ITU G.711 a-law</td></tr><tr><td width="22%">7</td><td width="78%">ITU G.711 µ-law</td></tr><tr><td width="22%">17</td><td width="78%">IMA ADPCM</td></tr><tr><td width="22%">20</td><td width="78%">ITU G.723 ADPCM ( Yamaha)</td></tr><tr><td width="22%">49</td><td width="78%">GSM 6.10</td></tr><tr><td width="22%">64</td><td width="78%">ITU G.721 ADPCM</td></tr><tr><td width="22%">80</td><td width="78%">MPEG</td></tr></table>
* @returnE Int16 format number with the following meaning<br /><br /><table width="280" class="tablelayout"><tr><th width="22%">id</th><th width="78%">format</th></tr><tr><td width="22%">0</td><td width="78%">unknown</td></tr><tr><td width="22%">1</td><td width="78%">PCM/uncompressed</td></tr><tr><td width="22%">2</td><td width="78%">Microsoft ADPCM</td></tr><tr><td width="22%">6</td><td width="78%">ITU G.711 a-law</td></tr><tr><td width="22%">7</td><td width="78%">ITU G.711 µ-law</td></tr><tr><td width="22%">17</td><td width="78%">IMA ADPCM</td></tr><tr><td width="22%">20</td><td width="78%">ITU G.723 ADPCM ( Yamaha)</td></tr><tr><td width="22%">49</td><td width="78%">GSM 6.10</td></tr><tr><td width="22%">64</td><td width="78%">ITU G.721 ADPCM</td></tr><tr><td width="22%">80</td><td width="78%">MPEG</td></tr></table>
*/
extern "C" short __stdcall WAVGetFormatIDW() 
{
	return wav.GetFormatID();
}

/**
* Methode: WAVGetInfoChunkIDsW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: liefert eine kommagetrennte Liste aller vorkommenden eindeutigen Info Chunk IDs
* Beschreibung english: get a comma based List of all unique info chunk ids
* @returnD BSTR der Textstring Kommagetrennter String mit info chunk IDs
* @returnE BSTR the text string comma based text string with info chunk IDs
*/
extern "C" BSTR __stdcall WAVGetInfoChunkIDsW()
{
	return wav.getINFOChunkIDs().AllocSysString();
}

/* ------------------------------------------------------------------------------------- */

extern "C" void __stdcall SetPopupKeyW(LPCWSTR keyValue)
{
	keyValue;
	// This isn't necessary anymore
}

/**
* Methode: GetAudioGenieVersionW
* @link UNIVERSAL
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Versions-Nummer von AudioGenie
* Beschreibung english: get the AudioGenie version
* @returnD Version Versions-Nummer von AudioGenie
* @returnE version version number from AudioGenie
*/
extern "C" BSTR __stdcall GetAudioGenieVersionW() 
{
	return CAtlString(L"2.0.4.0").AllocSysString();	
}

/* ------------------------------------------------------------------------------------- */

/**
* Methode: WMAGetPictureFileW
* @link WMA
* @since 2.0.1.0
* @frame WM/Picture
* Beschreibung deutsch: "WM/Picture" überträgt ein Bild aus dem Tag in eine Datei
* Beschreibung english: get a picture from the tag and store it in a file
* @paramD LPCWSTR FileName Name der Datei in die das Bild übertragen wird
* @paramD Int16 index Index von 1 bis <a href="WMAGetPictureCountW.html">WMAGetPictureCountW</a>
* @returnD Int16 normalerweise -1, bei Fehler oder nicht gefundenem Index 0
* @paramE LPCWSTR FileName Name of the file where the picture will be stored
* @paramE Int16 index index from 1 to <a href="WMAGetPictureCountW.html">WMAGetPictureCountW</a>
* @returnE Int16 normally -1, 0 on error or index not present
*/
extern "C" short __stdcall WMAGetPictureFileW(LPCWSTR FileName, short Index) 
{
	return b2s(wma.GetPicture(Index, getValidPointer(FileName)));
}

/**
* Methode: WMAGetPictureArrayW
* @link WMA
* @since 2.0.1.0
* @frame WM/PICTURE
* Beschreibung deutsch: "WM/Picture" überträgt ein Bild aus dem Tag in ein Byte-Array
* Beschreibung english: get a picture from the tag and store it in a byte array
* @paramD Pointer Zeiger auf das Array mit den Byte-Daten
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD Int16 index Index von 1 bis <a href="WMAGetPictureCountW.html">WMAGetPictureCountW</a>
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer pointer to a Byte array
* @paramE UInt32 maxLen maximum size of the byte array
* @paramE Int16 index index from 1 to <a href="WMAGetPictureCountW.html">WMAGetPictureCountW</a>
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall WMAGetPictureArrayW(BYTE *arr, u32 maxLen, short Index)
{
	return (long)wma.GetPictureArray(arr, maxLen, Index);	
}

/**
* Methode: WMAGetPictureDescriptionW
* @link WMA
* @since 2.0.1.0
* @frame WM/Picture
* Beschreibung deutsch: liefert die Beschreibung eines Bildes
* Beschreibung english: get the description from a picture
* @paramD Int16 index Index von 1 bis <a href="WMAGetPictureCountW.html">WMAGetPictureCountW</a>
* @returnD BSTR Beschreibung eines Bildes
* @paramE Int16 index index from 1 to <a href="WMAGetPictureCountW.html">WMAGetPictureCountW</a>
* @returnE BSTR the description of the picture
*/
extern "C" BSTR __stdcall WMAGetPictureDescriptionW(short Index) 
{
	return wma.GetPictureDescription(Index).AllocSysString();  
}

/**
* Methode: WMAGetPictureMimeW
* @link WMA
* @since 2.0.1.0
* @frame WM/Picture
* Beschreibung deutsch: liefert den Mimetyp eines Bildes
* Beschreibung english: get the mime type from a picture
* @paramD Int16 index Index von 1 bis <a href="WMAGetPictureCountW.html">WMAGetPictureCountW</a>
* @returnD BSTR Mimetyp eines Bildes
* @paramE Int16 index index from 1 to <a href="WMAGetPictureCountW.html">WMAGetPictureCountW</a>
* @returnE BSTR the mime type of the picture
*/
extern "C" BSTR __stdcall WMAGetPictureMimeW(short Index) 
{
	return wma.GetPictureMime(Index).AllocSysString();
}

/**
* Methode: WMAGetPictureSizeW
* @link WMA
* @since 2.0.2.0
* Beschreibung deutsch: liefert die Grösse eines Bildes in Bytes
* Beschreibung english: get the size from a picture in bytes
* @paramD Int16 index Index von 1 bis Anzahl Picture frames
* @returnD Int32 die Grösse des Bildes in Bytes
* @paramE Int16 index index from 1 to picture frame count
* @returnE Int32 the size of the picture in bytes
*/
extern "C" long __stdcall WMAGetPictureSizeW(short Index) 
{
	return wma.GetPictureSize(Index);
}

/**
* Methode: WMAGetPictureTypeW
* @link WMA
* @since 2.0.1.0
* @frame WM/Picture
* Beschreibung deutsch: liefert den Bildtyp eines Bildes
* Beschreibung english: get the picture type from a picture
* @paramD Int16 index Index von 1 bis <a href="WMAGetPictureCountW.html">WMAGetPictureCountW</a>
* @returnD Int16 Typ des Bildes von 0 bis 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @paramE Int16 index index from 1 to <a href="WMAGetPictureCountW.html">WMAGetPictureCountW</a>
* @returnE Int16 picture type from 0 to 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
*/
extern "C" short __stdcall WMAGetPictureTypeW(short Index) 
{
	return (short)wma.GetPictureType(Index);
}

/**
* Methode: WMAGetPictureCountW
* @link WMA
* @since 2.0.1.0
* @frame WM/Picture
* Beschreibung deutsch: liefert die Anzahl der Bilder
* Beschreibung english: get the number of pictures
* @returnD Int16 Anzahl Bilder
* @returnE Int16 number of pictures
*/
extern "C" short __stdcall WMAGetPictureCountW() 
{
	return (short)wma.GetPictures();
}

/**
* Methode: WMAAddPictureFileW
* @link WMA
* @since 2.0.1.0
* @frame WM/Picture
* Beschreibung deutsch: überträgt ein externes Bild in einen Tag
* Beschreibung english: store a picture from a file in the tag
* @paramD LPCWSTR Filename Name der Bild-Datei
* @paramD LPCWSTR Beschreibung Kommentar zu dem Bild
* @paramD Int16 PictureTyp Typ des Bildes von 0 bis 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @paramD Int16 index von 1 bis n; falls n nicht exisitert wird ein neuer Eintrag generiert
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR Filename name of the picture file
* @paramE LPCWSTR description a description of the picture
* @paramE Int16 picture type from 0 to 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @paramE Int16 index Index from 1 to n; if n is not present a new entry will be created
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall WMAAddPictureFileW(LPCWSTR FileName, LPCWSTR Description, short PictureType, short Index) 
{
	return b2s(wma.SetPicture(getValidPointer(FileName), getValidPointer(Description), Index, PictureType));	
}

/**
* Methode: WMAAddPictureArrayW
* @link WMA
* @since 2.0.1.0
* @frame WM/Picture
* Beschreibung deutsch: überträgt ein Bild aus einem Byte-Array in den Tag
* Beschreibung english: store a picture from a byte array in the tag
* @paramD Pointer Zeiger auf ein Byte Array mit den Daten des Bildes
* @paramD UInt32 Length die Grösse des Arrays in Bytes
* @paramD LPCWSTR Beschreibung Kommentar zu dem Bild
* @paramD Int16 PictureTyp Typ des Bildes von 0 bis 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @paramD Int16 index von 1 bis n; falls n nicht exisitert wird ein neuer Eintrag generiert
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer pointer to a byte array with the picture datas
* @paramE UInt32 Length the size of the array
* @paramE LPCWSTR description a description of the picture
* @paramE Int16 picture type from 0 to 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @paramE Int16 index Index from 1 to n; if n is not present a new entry will be created
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall WMAAddPictureArrayW(BYTE *arr, u32 Length, LPCWSTR Description, short PictureType, short index )
{
  return b2s(wma.SetPictureArray(arr, Length, getValidPointer(Description), index, PictureType));  
} 


/**
* Methode: WMAGetUserItemW
* @link WMA
* @since 2.0.1.0
* Beschreibung deutsch: liefert ein benutzerdefiniertes Feld<br />Eine Aufzählung aller vorhandenen Feldnamen liefert die Funktion <a href="WMAGetItemKeysW.html">WMAGetItemKeysW</a>.
* Beschreibung english: get a user defined field<br />use the method <a href="WMAGetItemKeysW.html">WMAGetItemKeysW</a> for a list of all existing keys.
* @paramD LPCWSTR myKey benutzerdefinierter Schlüssel
* @paramE LPCWSTR myKey userdefined key
* @returnD BSTR field Feldinhalt oder leer, falls Schlüssel nicht gefunden
* @returnE BSTR field userdefined item or empty if key not present
*/
extern "C" BSTR __stdcall WMAGetUserItemW(LPCWSTR ItemKey) 
{
	return wma.GetUserItem(getValidPointer(ItemKey)).AllocSysString();
}

/**
* Methode: WMASetUserItemW
* @link WMA
* @since 2.0.1.0
* Beschreibung deutsch: setzt ein benutzerdefiniertes Feld<br />Eine Aufzählung aller vorhandenen Feldnamen liefert die Funktion <a href="WMAGetItemKeysW.html">WMAGetItemKeysW</a>.
* Beschreibung english: set a user defined field<br />use the method <a href="WMAGetItemKeysW.html">WMAGetItemKeysW</a> for a list of all existing keys.
* @paramD LPCWSTR myKey der Feldname der gesetzt werden soll
* @paramD LPCWSTR myValue der Wert des Feldes, ein Leerstring entfernt das Feld
* @paramE LPCWSTR myKey the key you want to set
* @paramE LPCWSTR myValue new value for the key, empty string remove this item
*/
extern "C" void __stdcall WMASetUserItemW(LPCWSTR ItemKey, LPCWSTR textString) 
{
	wma.SetUserItem(getValidPointer(ItemKey), getValidPointer(textString));
}

/**
* Methode: WMAGetItemKeysW
* @link WMA
* @since 2.0.1.0
* Beschreibung deutsch: liefert alle Feldnamen durch Kommas getrennt
* Beschreibung english: get all field names ( comma separated ) 
* @returnD BSTR Feldnamen
* @returnE BSTR fieldnames
*/
extern "C" BSTR __stdcall WMAGetItemKeysW() 
{
	return wma.GetItemKeys().AllocSysString();
}

/**
* Methode: WMADeletePictureW
* @link WMA
* @since 2.0.1.0
* @frame WM/Picture
* Beschreibung deutsch: entfernt ein Bild mit WM/Picture Tag
* Beschreibung english: remove a picture with WM/Picture Tag
* @paramD Int16 Index von 1 bis <a href="WMAGetPictureCountW.html">WMAGetPictureCountW</a>
* @returnD Int16 normalerweise -1, bei Fehler oder nicht vorhandenem Bild 0
* @paramE Int16 Index index from 1 to <a href="WMAGetPictureCountW.html">WMAGetPictureCountW</a>
* @returnE Int16 normally -1, 0 on error or picture not present
*/
extern "C" short __stdcall WMADeletePictureW(short Index) 
{
	return b2s(wma.DeletePictureFrame(Index));
}

/**
* Methode: WMADeletePicturesW
* @link WMA
* @since 2.0.1.0
* @frame WM/Picture
* Beschreibung deutsch: entfernt alle Bilder mit WM/Picture Tag
* Beschreibung english: remove all pictures with WM/Picture tag
*/
extern "C" void __stdcall WMADeletePicturesW() 
{
	wma.DeletePictures();
}

/**
* Methode: WMAIsVBRW
* @link WMA
* @since 2.0.1.0
* Beschreibung deutsch: zeigt an, ob variable Bitrate verwendet wird
* Beschreibung english: shows -1, if it is variable bitrate
* @returnD Int16 -1 bei variabler Bitrate, ansonsten 0
* @returnE Int16 -1 if variable bitrate, otherwise 0
*/
extern "C" short __stdcall WMAIsVBRW()
{
	return b2s(wma.IsVBR());
}

/**
* Methode: WMASaveChangesToFileW
* @link WMA
* @since 2.0.1.0
* Beschreibung deutsch: speichert die Änderungen im WMA-Tag in eine Datei
* Beschreibung english: store the wma tag in a file
* @paramD LPCWSTR FileName Name der Datei
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR FileName name of the file
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall WMASaveChangesToFileW(LPCWSTR FileName) 
{
	FileName = getValidPointer(FileName);
	if (GetFormat(FileName) == AUDIO_FORMAT_WMA)
	{
		bool result = wma.SaveToFile(FileName);
		if (result && !lastFile.IsEmpty() && lastFile.CompareNoCase(FileName) == 0)
		{
			CTools::instance().writeInfo(L"Lese %s neu ein:", FileName);
			AUDIOAnalyzeFileW(FileName);
		}
		return b2s(result);
	}
	CTools::instance().setLastError(ERR_TAG_NOT_ALLOWED);
	return b2s(false); 
}

/**
* Methode: WMASaveChangesW
* @link WMA
* @since 2.0.1.0
* Beschreibung deutsch: speichert die Änderungen im WMA-Tag in die zuletzt analysierte Datei
* Beschreibung english: store the wma tag in the last analyzed file
* @returnD Int16 normalerweise -1, bei Fehler 0
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall WMASaveChangesW()
{
	return WMASaveChangesToFileW(lastFile);
}
/* ------------------------------------------------------------------------------------- */

/**
* Methode: APEGetAlbumW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Album
* Beschreibung english: get the album
* @returnD BSTR Album
* @returnE BSTR album
*/
extern "C" BSTR __stdcall APEGetAlbumW()
{
	return ape.GetTagItem(APE_ALBUM).AllocSysString();
}


/**
* Methode: APESetAlbumW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Album
* Beschreibung english: set the album
* @paramD LPCWSTR myAlbum Album
* @paramE LPCWSTR myAlbum album
*/
extern "C" void __stdcall APESetAlbumW(LPCWSTR textString)
{
	ape.SetTagItem(APE_ALBUM, getValidPointer(textString));
}


/**
* Methode: APEGetArtistW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Interpreten
* Beschreibung english: get the artist
* @returnD BSTR Interpret
* @returnE BSTR artist
*/
extern "C" BSTR __stdcall APEGetArtistW()
{
	return ape.GetTagItem(APE_ARTIST).AllocSysString();
}


/**
* Methode: APESetArtistW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Interpreten
* Beschreibung english: set the artist
* @paramD LPCWSTR myArtist Interpret
* @paramE LPCWSTR myArtist artist
*/
extern "C" void __stdcall APESetArtistW(LPCWSTR textString)
{
	ape.SetTagItem(APE_ARTIST, getValidPointer(textString));
}


/**
* Methode: APEGetCommentW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Kommentar
* Beschreibung english: get the comment
* @returnD BSTR Kommentar
* @returnE BSTR comment
*/
extern "C" BSTR __stdcall APEGetCommentW()
{
	return ape.GetTagItem(APE_COMMENT).AllocSysString();
}


/**
* Methode: APESetCommentW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Kommentar
* Beschreibung english: set the comment
* @paramD LPCWSTR myComment Kommentar
* @paramE LPCWSTR myComment comment
*/
extern "C" void __stdcall APESetCommentW(LPCWSTR textString)
{
	ape.SetTagItem(APE_COMMENT, getValidPointer(textString));
}


/**
* Methode: APEGetCopyrightW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Copyright
* Beschreibung english: get the copyright
* @returnD BSTR Copyright 
* @returnE BSTR copyright
*/
extern "C" BSTR __stdcall APEGetCopyrightW()
{
	return ape.GetTagItem(APE_COPYRIGHT).AllocSysString();
}


/**
* Methode: APESetCopyrightW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Copyright
* Beschreibung english: set the copyright
* @paramD LPCWSTR myCopyright Copyright
* @paramE LPCWSTR myCopyright Copyright
*/
extern "C" void __stdcall APESetCopyrightW(LPCWSTR textString)
{
	ape.SetTagItem(APE_COPYRIGHT, getValidPointer(textString));
}


/**
* Methode: APEGetGenreW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Genre
* Beschreibung english: get the genre
* @returnD BSTR Genre
* @returnE BSTR genre
*/
extern "C" BSTR __stdcall APEGetGenreW()
{
	return ape.GetTagItem(APE_GENRE).AllocSysString();
}


/**
* Methode: APESetGenreW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Genre
* Beschreibung english: set the genre
* @paramD LPCWSTR myGenre Genre
* @paramE LPCWSTR myGenre genre
*/
extern "C" void __stdcall APESetGenreW(LPCWSTR textString)
{
	ape.SetTagItem(APE_GENRE, getValidPointer(textString));
}


/**
* Methode: APEGetTitleW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Titel
* Beschreibung english: get the title
* @returnD BSTR Titel
* @returnE BSTR title
*/
extern "C" BSTR __stdcall APEGetTitleW()
{
	return ape.GetTagItem(APE_TITLE).AllocSysString();
}


/**
* Methode: APESetTitleW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Titel
* Beschreibung english: set the title
* @paramD LPCWSTR myTitle Titel
* @paramE LPCWSTR myTitle title
*/
extern "C" void __stdcall APESetTitleW(LPCWSTR textString)
{
	ape.SetTagItem(APE_TITLE, getValidPointer(textString));
}


/**
* Methode: APEGetTrackW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Track
* Beschreibung english: get the track
* @returnD BSTR Track
* @returnE BSTR track
*/
extern "C" BSTR __stdcall APEGetTrackW()
{
	return ape.GetTagItem(APE_TRACK).AllocSysString();
}


/**
* Methode: APESetTrackW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Track
* Beschreibung english: set the track
* @paramD LPCWSTR myTrack Track
* @paramE LPCWSTR myTrack track
*/
extern "C" void __stdcall APESetTrackW(LPCWSTR textString)
{
	ape.SetTagItem(APE_TRACK, getValidPointer(textString));
}


/**
* Methode: APEGetYearW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Jahr
* Beschreibung english: get the year
* @returnD BSTR Jahr
* @returnE BSTR year
*/
extern "C" BSTR __stdcall APEGetYearW()
{
	return ape.GetTagItem(APE_YEAR).AllocSysString();
}


/**
* Methode: APESetYearW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Jahr
* Beschreibung english: set the year
* @paramD LPCWSTR myYear Jahr
* @paramE LPCWSTR myYear year
*/
extern "C" void __stdcall APESetYearW(LPCWSTR textString)
{
	ape.SetTagItem(APE_YEAR, getValidPointer(textString));
}


/**
* Methode: APEGetSizeW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Grösse des APE-Tags in Bytes
* Beschreibung english: get the size of the tag in bytes
* @returnD Int32 Tag-Grösse
* @returnE Int32 tag size
*/
extern "C" long __stdcall APEGetSizeW()
{
	return ape.GetSize();
}


/**
* Methode: APEExistsW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: zeigt an, ob ein APE-Tag existiert
* Beschreibung english: shows -1, if the APE-Tag exists
* @returnD Int16 -1 wenn APE-Tag existiert, ansonsten 0
* @returnE Int16 -1 if APE tag exists, otherwise 0
*/
extern "C" short __stdcall APEExistsW()
{
	return b2s(ape.Exists());
}


/**
* Methode: APEGetVersionW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Version des APE-Tags
* Beschreibung english: get the ape version
* @returnD BSTR Version
* @returnE BSTR version
*/
extern "C" BSTR __stdcall APEGetVersionW()
{
	return ape.GetTagVersion().AllocSysString();
}


/**
* Methode: APEGetUserItemW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: liefert ein benutzerdefiniertes Feld<br />Eine Aufzählung aller vorhandenen Feldnamen liefert die Funktion APEGetItemKeys.
* Beschreibung english: get a user defined field<br />use the method APEGetItemKeys for a list of all existing keys.
* @paramD LPCWSTR myKey benutzerdefinierter Schlüssel
* @paramE LPCWSTR myKey userdefined key
* @returnD BSTR field Feldinhalt oder leer, falls Schlüssel nicht gefunden
* @returnE BSTR field userdefined item or empty if key not present
*/
extern "C" BSTR __stdcall APEGetUserItemW(LPCWSTR Key) 
{
	return ape.GetUserDefined(getValidPointer(Key)).AllocSysString();
}


/**
* Methode: APESetUserItemW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: setzt ein benutzerdefiniertes Feld<br />Eine Aufzählung aller vorhandenen Feldnamen liefert die Funktion APEGetItemKeys.
* Beschreibung english: set a user defined field<br />use the method APEGetItemKeys for a list of all existing keys.
* @paramD LPCWSTR myKey benutzerdefinierter Schlüssel
* @paramE LPCWSTR myKey userdefined key
* @paramD LPCWSTR myValue neu zu setzender Wert, ein Leerstring entfernt das Feld
* @paramE LPCWSTR myValue new value, empty string remove this item
*/
extern "C" void __stdcall APESetUserItemW(LPCWSTR Key, LPCWSTR textString) 
{
	ape.SetUserDefined(getValidPointer(Key), getValidPointer(textString));
}


/**
* Methode: APEGetItemKeysW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: liefert alle Feldnamen durch Kommas getrennt, die Textwerte beinhalten 
* Beschreibung english: get all field names ( comma separated ) with text values
* @returnD BSTR Feldnamen
* @returnE BSTR fieldnames
*/
extern "C" BSTR __stdcall APEGetItemKeysW() 
{
	return ape.GetAllKeys().AllocSysString();
}


/**
* Methode: APESaveChangesToFileW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: speichert den APE-Tag in eine Datei
* Beschreibung english: store the APE tag in a file
* @paramD LPCWSTR FileName Name der Datei
* @paramE LPCWSTR filename name of the file
* @returnD Int16 liefert 0 bei Fehler, ansonsten -1
* @returnE Int16 0 on error, otherwise -1
*/
extern "C" short __stdcall APESaveChangesToFileW(LPCWSTR FileName)
{
	FileName = getValidPointer(FileName);
	if (GetFormat(FileName) != AUDIO_FORMAT_INVALID)
		return b2s(ape.SaveToFile(FileName));
	CTools::instance().setLastError(ERR_TAG_NOT_ALLOWED);
	return b2s(false);  
}

/**
* Methode: APESaveChangesW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: speichert den APE-Tag in die zuletzt analysierte Datei
* Beschreibung english: store the APE tag in the last analyzed file
* @returnD Int16 liefert 0 bei Fehler, ansonsten -1
* @returnE Int16 0 on error, otherwise -1
*/
extern "C" short __stdcall APESaveChangesW()
{
	return APESaveChangesToFileW(lastFile);
}

/**
* Methode: APERemoveTagFromFileW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: entfernt den APE-Tag von einer Datei. Achtung: Die Löschung wird sofort durchgeführt!
* Beschreibung english: remove the APE tag from a file. Attention: This function removes the tag immediately!
* @paramD LPCWSTR FileName Name der Datei
* @paramE LPCWSTR filename name of the file
* @returnD Int16 -1 wenn erfolgreich gelöscht, ansonsten 0
* @returnE Int16 -1 if removed, otherwise 0
*/
extern "C" short __stdcall APERemoveTagFromFileW(LPCWSTR FileName)
{
	FileName = getValidPointer(FileName);
	if (ape.RemoveFromFile(FileName, true))
	{
		AUDIOAnalyzeFileW(FileName);
		return b2s(true);
	}
	return b2s(false);
}

/**
* Methode: APERemoveTagW
* @link APE
* @since 2.0.1.0
* Beschreibung deutsch: entfernt den APE-Tag von der zuletzt analysierten Datei. Achtung: Die Löschung wird sofort durchgeführt!
* Beschreibung english: remove the APE tag from the last analyzed file. Attention: This function removes the tag immediately!
* @returnD Int16 -1 wenn erfolgreich gelöscht, ansonsten 0
* @returnE Int16 -1 if removed, otherwise 0
*/
extern "C" short __stdcall APERemoveTagW()
{
	if (ape.RemoveFromFile(lastFile, true))
	{
		AUDIOAnalyzeFileW(lastFile);
		return b2s(true);
	}
	return b2s(false);
}
/* ------------------------------------------------------------------------------------- */

/**
* Methode: ID3V1GetGenreW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Genre
* Beschreibung english: get the genre
* @returnD BSTR Genre
* @returnE BSTR genre
*/
extern "C" BSTR __stdcall ID3V1GetGenreW()
{
	return id3v1.GetGenre().AllocSysString();
}


/**
* Methode: ID3V1SetGenreW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Genre
* Beschreibung english: set the genre
* @paramD LPCWSTR myGenre Genre
* @paramE LPCWSTR myGenre genre
*/
extern "C" void __stdcall ID3V1SetGenreW(LPCWSTR textString)
{
	id3v1.SetGenre(getValidPointer(textString));
}


/**
* Methode: ID3V1GetAlbumW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Album
* Beschreibung english: get the album
* @returnD BSTR Album
* @returnE BSTR album
*/
extern "C" BSTR __stdcall ID3V1GetAlbumW()
{
	return id3v1.GetAlbum().AllocSysString();
}


/**
* Methode: ID3V1SetAlbumW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Album
* Beschreibung english: set the album
* @paramD LPCWSTR myAlbum Album
* @paramE LPCWSTR myAlbum album
*/
extern "C" void __stdcall ID3V1SetAlbumW(LPCWSTR textString)
{
	id3v1.SetAlbum(getValidPointer(textString));
}


/**
* Methode: ID3V1GetArtistW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Künstler
* Beschreibung english: get the artist
* @returnD BSTR Künstler
* @returnE BSTR artist
*/
extern "C" BSTR __stdcall ID3V1GetArtistW()
{
	return id3v1.GetArtist().AllocSysString();
}


/**
* Methode: ID3V1SetArtistW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Künstler
* Beschreibung english: set the artist
* @paramD LPCWSTR myArtist Künstler
* @paramE LPCWSTR myArtist artist
*/
extern "C" void __stdcall ID3V1SetArtistW(LPCWSTR textString)
{
	id3v1.SetArtist(getValidPointer(textString));
}


/**
* Methode: ID3V1GetCommentW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Kommentar
* Beschreibung english: get the comment
* @returnD BSTR Kommentar
* @returnE BSTR comment
*/
extern "C" BSTR __stdcall ID3V1GetCommentW()
{
	return id3v1.GetComment().AllocSysString();
}


/**
* Methode: ID3V1SetCommentW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Kommentar
* Beschreibung english: set the comment
* @paramD LPCWSTR myComment Kommentar
* @paramE LPCWSTR myComment comment
*/
extern "C" void __stdcall ID3V1SetCommentW(LPCWSTR textString)
{
	id3v1.SetComment(getValidPointer(textString));
}


/**
* Methode: ID3V1GetTitleW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Titel
* Beschreibung english: get the title
* @returnD BSTR Titel
* @returnE BSTR title
*/
extern "C" BSTR __stdcall ID3V1GetTitleW()
{
	return id3v1.GetTitle().AllocSysString();
}


/**
* Methode: ID3V1SetTitleW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Titel
* Beschreibung english: set the title
* @paramD LPCWSTR myTitle Titel
* @paramE LPCWSTR myTitle title
*/
extern "C" void __stdcall ID3V1SetTitleW(LPCWSTR textString)
{
	id3v1.SetTitle(getValidPointer(textString));
}


/**
* Methode: ID3V1GetTrackW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Track
* Beschreibung english: get the track
* @returnD BSTR Track
* @returnE BSTR track
*/
extern "C" BSTR __stdcall ID3V1GetTrackW()
{
	return id3v1.GetTrack().AllocSysString();
}


/**
* Methode: ID3V1SetTrackW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Track
* Beschreibung english: set the track
* @paramD LPCWSTR myTrack Track
* @paramE LPCWSTR myTrack track
*/
extern "C" void __stdcall ID3V1SetTrackW(LPCWSTR textString)
{
	id3v1.SetTrack(getValidPointer(textString));
}


/**
* Methode: ID3V1GetYearW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Jahr
* Beschreibung english: get the year
* @returnD BSTR Jahr
* @returnE BSTR year
*/
extern "C" BSTR __stdcall ID3V1GetYearW()
{
	return id3v1.GetYear().AllocSysString();
}


/**
* Methode: ID3V1SetYearW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Jahr
* Beschreibung english: set the year
* @paramD LPCWSTR myYear Jahr
* @paramE LPCWSTR myYear year
*/
extern "C" void __stdcall ID3V1SetYearW(LPCWSTR textString)
{
	id3v1.SetYear(getValidPointer(textString));
}

/**
* Methode: ID3V1GetGenreIDW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Nummer des Genres
* Beschreibung english: get the genre number
* @returnD Int16 Nummer des Genres
* @returnE Int16 genre number
*/
extern "C" short __stdcall ID3V1GetGenreIDW() 
{
	return (short)id3v1.GetGenreID();
}

/**
* Methode: ID3V1SetGenreIDW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: setzt die Nummer des Genres
* Beschreibung english: set the genre number
* @paramD Int16 myGenreID Nummer des Genres
* @paramE Int16 myGenreID genre number
*/
extern "C" void __stdcall ID3V1SetGenreIDW(short nNewValue) 
{
	id3v1.SetGenreID(nNewValue);
}

/**
* Methode: ID3V1GetGenreItemW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: liefert ein Genre zu einer bestimmten Nummer ( 0 bis <a href="ID3V1GetGenresW.html">ID3V1GetGenresW</a> - 1 )
* Beschreibung english: get a genre for a specific number ( 0 to <a href="ID3V1GetGenresW.html">ID3V1GetGenresW</a> - 1 )
* @paramD Int16 myNumber Nummer
* @paramE Int16 myNumber number
* @returnD BSTR Genre Text
* @returnE BSTR genre string
*/
extern "C" BSTR __stdcall ID3V1GetGenreItemW(short number) 
{
	return id3v1.GetGenreItem(number).AllocSysString();
}

/**
* Methode: ID3V1GetGenresW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der unterstützten Genres
* Beschreibung english: get the number of supported genres
* @returnD Int16 Anzahl der Genres
* @returnE Int16 number of genres
*/
extern "C" short __stdcall ID3V1GetGenresW() 
{
	return (short)id3v1.NumberOfGenres();
}

/**
* Methode: ID3V1ExistsW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: zeigt an, ob ein ID3v1-Tag existiert
* Beschreibung english: shows -1, if ID3v1 tag exists
* @returnD Int16 -1 wenn Tag existiert, ansonsten 0
* @returnE Int16 -1 if Tag exists, otherwise 0
*/
extern "C" short __stdcall ID3V1ExistsW()
{
	return b2s(id3v1.Exists());
}


/**
* Methode: ID3V1GetVersionW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Version des ID3v1 Tags
* Beschreibung english: get the version of the id3v1 tag
* @returnD BSTR Version
* @returnE BSTR version
*/
extern "C" BSTR __stdcall ID3V1GetVersionW()
{
	return id3v1.GetTagVersion().AllocSysString();
}


/**
* Methode: ID3V1RemoveTagFromFileW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: entfernt den ID3v1-Tag einer Datei. Achtung: Die Löschung wird sofort durchgeführt!
* Beschreibung english: remove the ID3v1-Tag from a File. Attention: This function removes the tag immediately!
* @paramD LPCWSTR FileName Name der Datei
* @paramE LPCWSTR FileName the name of the file
* @returnD Int16 -1 wenn erfolgreich entfernt, ansonsten 0
* @returnE Int16 -1 if removed, 0 on error
*/
extern "C" short __stdcall ID3V1RemoveTagFromFileW(LPCWSTR FileName)
{
	FileName = getValidPointer(FileName);
	if (lyrics.Exists())
		lyrics.RemoveFromFile(FileName);
	if (id3v1.RemoveFromFile(FileName))
	{
		AUDIOAnalyzeFileW(FileName);
		return b2s(true);	
	}
	return b2s(false);
}

/**
* Methode: ID3V1RemoveTagW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: entfernt den ID3v1-Tag der zuletzt analysierten Datei. Achtung: Die Löschung wird sofort durchgeführt!
* Beschreibung english: remove the ID3v1-Tag from the last analyzed File. Attention: This function removes the tag immediately!
* @returnD Int16 -1 wenn erfolgreich entfernt, ansonsten 0
* @returnE Int16 -1 if removed, 0 on error
*/
extern "C" short __stdcall ID3V1RemoveTagW()
{
	if (lyrics.Exists())
		lyrics.RemoveFromFile(lastFile);
	if (id3v1.RemoveFromFile(lastFile))
	{ 
		AUDIOAnalyzeFileW(lastFile);
		return b2s(true);
	}
	return b2s(false);	
}

/**
* Methode: ID3V1SaveChangesToFileW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: speichert den ID3v1-Tag in eine Datei
* Beschreibung english: stores the ID3v1 tag into a file
* @paramD LPCWSTR FileName Name der Datei
* @paramE LPCWSTR FileName the name of the file
* @returnD Int16 normalerweise -1, bei Fehler 0
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall ID3V1SaveChangesToFileW(LPCWSTR FileName)
{
	FileName = getValidPointer(FileName);
	if (GetFormat(FileName) != AUDIO_FORMAT_INVALID)
		return b2s(id3v1.SaveToFile(FileName));
	CTools::instance().setLastError(ERR_TAG_NOT_ALLOWED);
	return b2s(false);   
}

/**
* Methode: ID3V1SaveChangesW
* @link ID3V1
* @since 2.0.1.0
* Beschreibung deutsch: speichert den ID3v1-Tag in die zuletzt analysierte Datei
* Beschreibung english: stores the ID3v1 tag into the last analyzed file
* @returnD Int16 normalerweise -1, bei Fehler 0
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall ID3V1SaveChangesW()
{
	return ID3V1SaveChangesToFileW(lastFile);
}

/* ------------------------------------------------------------------------------------- */

/**
* Methode: LYRICSExistsW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: zeigt an, ob ein Lyrics-Tag existiert
* Beschreibung english: shows -1, if the Lyrics tag exists
* @returnD Int16 -1 wenn Lyrics-Tag vorhanden ist, ansonsten 0
* @returnE Int16 -1 if a Lyrics tag exists, otherwise 0
*/
extern "C" short __stdcall LYRICSExistsW()
{
	return b2s(lyrics.Exists());
}


/**
* Methode: LYRICSGetSizeW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Grösse des Lyrics-Tags in Bytes
* Beschreibung english: get the size of the tag in bytes
* @returnD Int32 Grösse in Bytes
* @returnE Int32 size in bytes
*/
extern "C" long __stdcall LYRICSGetSizeW()
{
	return lyrics.GetSize();
}


/**
* Methode: LYRICSGetStartPositionW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Startposition des Lyrics-Tags
* Beschreibung english: get the start position of the tag 
* @returnD Int32 Startposition
* @returnE Int32 start position
*/
extern "C" long __stdcall LYRICSGetStartPositionW()
{
	return lyrics.GetStartPosition();
}


/**
* Methode: LYRICSGetVersionW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Version
* Beschreibung english: get the version
* @returnD BSTR Version
* @returnE BSTR version
*/
extern "C" BSTR __stdcall LYRICSGetVersionW()
{
	return lyrics.GetTagVersion().AllocSysString();
}


/**
* Methode: LYRICSGetAlbumW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Album
* Beschreibung english: get the album
* @returnD BSTR Album
* @returnE BSTR album
*/
extern "C" BSTR __stdcall LYRICSGetAlbumW()
{
	return lyrics.GetAlbum().AllocSysString();
}


/**
* Methode: LYRICSSetAlbumW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Album
* Beschreibung english: set the album
* @paramD LPCWSTR myAlbum Album
* @paramE LPCWSTR myAlbum album
*/
extern "C" void __stdcall LYRICSSetAlbumW(LPCWSTR textString)
{
	lyrics.SetAlbum(getValidPointer(textString));
}


/**
* Methode: LYRICSGetArtistW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Künstler
* Beschreibung english: get the artist
* @returnD BSTR Künstler
* @returnE BSTR artist
*/
extern "C" BSTR __stdcall LYRICSGetArtistW()
{
	return lyrics.GetArtist().AllocSysString();
}


/**
* Methode: LYRICSSetArtistW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Künstler
* Beschreibung english: set the artist
* @paramD LPCWSTR myArtist Künstler
* @paramE LPCWSTR myArtist artist
*/
extern "C" void __stdcall LYRICSSetArtistW(LPCWSTR textString)
{
	lyrics.SetArtist(getValidPointer(textString));
}


/**
* Methode: LYRICSGetAuthorW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Verfasser
* Beschreibung english: get the author
* @returnD BSTR Verfasser
* @returnE BSTR author
*/
extern "C" BSTR __stdcall LYRICSGetAuthorW()
{
	return lyrics.GetAuthor().AllocSysString();
}


/**
* Methode: LYRICSSetAuthorW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Verfasser
* Beschreibung english: set the author
* @paramD LPCWSTR myAuthor Verfasser
* @paramE LPCWSTR myAuthor author
*/
extern "C" void __stdcall LYRICSSetAuthorW(LPCWSTR textString)
{
	lyrics.SetAuthor(getValidPointer(textString));
}


/**
* Methode: LYRICSGetGenreW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Genre
* Beschreibung english: get the genre
* @returnD BSTR Genre
* @returnE BSTR genre
*/
extern "C" BSTR __stdcall LYRICSGetGenreW()
{
	return lyrics.GetGenre().AllocSysString();
}


/**
* Methode: LYRICSSetGenreW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Genre
* Beschreibung english: set the genre
* @paramD LPCWSTR myGenre Genre
* @paramE LPCWSTR myGenre genre
*/
extern "C" void __stdcall LYRICSSetGenreW(LPCWSTR textString)
{
	lyrics.SetGenre(getValidPointer(textString));
}


/**
* Methode: LYRICSGetImageLinkW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: liefert einen Link zu einem Bild
* Beschreibung english: get a link to an image
* @returnD BSTR Bilderlink
* @returnE BSTR image link
*/
extern "C" BSTR __stdcall LYRICSGetImageLinkW()
{
	return lyrics.GetImageLink().AllocSysString();
}


/**
* Methode: LYRICSSetImageLinkW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: setzt einen Link zu einem Bild
* Beschreibung english: set a link to an image
* @paramD LPCWSTR myImageLink Bilderlink
* @paramE LPCWSTR myImageLink image link
*/
extern "C" void __stdcall LYRICSSetImageLinkW(LPCWSTR textString)
{
	lyrics.SetImageLink(getValidPointer(textString));
}


/**
* Methode: LYRICSGetIndicationW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Kodierung
* Beschreibung english: get the internal Indication
* @returnD BSTR Kodierung
* @returnE BSTR indication
*/
extern "C" BSTR __stdcall LYRICSGetIndicationW()
{
	return lyrics.GetIndication().AllocSysString();
}


/**
* Methode: LYRICSSetIndicationW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: setzt die Kodierung
* Beschreibung english: set the internal Indication
* @paramD LPCWSTR myIndication Kodierung
* @paramE LPCWSTR myIndication Indication
*/
extern "C" void __stdcall LYRICSSetIndicationW(LPCWSTR textString)
{
	lyrics.SetIndication(getValidPointer(textString));
}


/**
* Methode: LYRICSGetInformationW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: liefert Zusatz-Informationen
* Beschreibung english: get the information
* @returnD BSTR Zusatz-Informationen
* @returnE BSTR informations
*/
extern "C" BSTR __stdcall LYRICSGetInformationW()
{
	return lyrics.GetInformation().AllocSysString();
}


/**
* Methode: LYRICSSetInformationW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: setzt Zusatz-Informationen
* Beschreibung english: set the information
* @paramD LPCWSTR myInformation Zusatz-Informationen
* @paramE LPCWSTR myInformation informations
*/
extern "C" void __stdcall LYRICSSetInformationW(LPCWSTR textString)
{
	lyrics.SetInformation(getValidPointer(textString));
}


/**
* Methode: LYRICSGetLyricsW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Songtext
* Beschreibung english: get the lyrics
* @returnD BSTR Songtext
* @returnE BSTR lyrics
*/
extern "C" BSTR __stdcall LYRICSGetLyricsW()
{
	return lyrics.GetLyrics().AllocSysString();
}


/**
* Methode: LYRICSSetLyricsW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Songtext
* Beschreibung english: set the lyrics
* @paramD LPCWSTR myLyrics Songtext
* @paramE LPCWSTR myLyrics Lyrics
*/
extern "C" void __stdcall LYRICSSetLyricsW(LPCWSTR textString)
{
	lyrics.SetLyrics(getValidPointer(textString));
}


/**
* Methode: LYRICSGetTitleW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Titel
* Beschreibung english: get the title
* @returnD BSTR Titel
* @returnE BSTR title
*/
extern "C" BSTR __stdcall LYRICSGetTitleW()
{
	return lyrics.GetTitle().AllocSysString();
}


/**
* Methode: LYRICSSetTitleW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Titel
* Beschreibung english: set the title
* @paramD LPCWSTR myTitle Titel
* @paramE LPCWSTR myTitle title
*/
extern "C" void __stdcall LYRICSSetTitleW(LPCWSTR textString)
{
	lyrics.SetTitle(getValidPointer(textString));
}


/**
* Methode: LYRICSRemoveTagFromFileW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: löscht den Lyrics-Tag von einer Datei. Achtung: Die Löschung wird sofort durchgeführt!
* Beschreibung english: remove the Lyrics tag from a file. Attention: This function removes the tag immediately!
* @paramD LPCWSTR FileName Name der Datei
* @paramE LPCWSTR filename name of the file
* @returnD Int16 -1 wenn erfolgreich entfernt, ansonsten 0
* @returnE Int16 -1 if removed, otherwise 0
*/
extern "C" short __stdcall LYRICSRemoveTagFromFileW(LPCWSTR FileName)
{
	FileName = getValidPointer(FileName);
	if (lyrics.RemoveFromFile(FileName))
	{    
		AUDIOAnalyzeFileW(FileName);
		return b2s(true);	
	}
	return b2s(false); 
}

/**
* Methode: LYRICSRemoveTagW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: löscht den Lyrics-Tag von der zuletzt analysierten Datei. Achtung: Die Löschung wird sofort durchgeführt!
* Beschreibung english: remove the Lyrics tag from the last analyzed file. Attention: This function removes the tag immediately!
* @returnD Int16 -1 wenn erfolgreich entfernt, ansonsten 0
* @returnE Int16 -1 if removed, otherwise 0
*/
extern "C" short __stdcall LYRICSRemoveTagW()
{
	if (lyrics.RemoveFromFile(lastFile))
	{    
		AUDIOAnalyzeFileW(lastFile);
		return b2s(true);	
	}
	return b2s(false); 
}

/**
* Methode: LYRICSSaveChangesToFileW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: speichert den Lyrics-Tag in eine Datei
* Beschreibung english: stores the Lyrics tag in a file
* @paramD LPCWSTR FileName Name der Datei
* @paramE LPCWSTR filename name of the file
* @returnD Int16 liefert 0 bei Fehler, ansonsten -1
* @returnE Int16 0 on error, otherwise -1
*/
extern "C" short __stdcall LYRICSSaveChangesToFileW(LPCWSTR FileName)
{
	FileName = getValidPointer(FileName);
	if (GetFormat(FileName) != AUDIO_FORMAT_INVALID)
		return b2s(lyrics.SaveToFile(FileName));
	CTools::instance().setLastError(ERR_TAG_NOT_ALLOWED);
	return b2s(false); 
}

/**
* Methode: LYRICSSaveChangesW
* @link LYRICS
* @since 2.0.1.0
* Beschreibung deutsch: speichert den Lyrics-Tag in die zuletzt analysierte Datei
* Beschreibung english: stores the Lyrics tag in the last analyzed file
* @returnD Int16 liefert 0 bei Fehler, ansonsten -1
* @returnE Int16 0 on error, otherwise -1
*/
extern "C" short __stdcall LYRICSSaveChangesW()
{
	return LYRICSSaveChangesToFileW(lastFile);
}
/* ------------------------------------------------------------------------------------- */

/**
* Methode: ID3V2GetTextFrameW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Text eines id3v2 Frames
* Beschreibung english: get the text entry of a id3v2 frame
* @paramD UInt32 FrameID die ID des Frames
* @paramE UInt32 FrameID the frame id
* @returnD BSTR liefert den Text
* @returnE BSTR frame text
*/
extern "C" BSTR __stdcall ID3V2GetTextFrameW(u32 FrameID)
{
	return id3v2.GetText(FrameID).AllocSysString();
}

/**
* Methode: ID3V2SetTextFrameW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: setzt den text eines Frames
* Beschreibung english: set the text entry of a frame
* @paramD UInt32 FrameID die ID des Frames die ID des Frames
* @paramD LPCWSTR textString der neue Text
* @paramE UInt32 FrameID the ID of the frame
* @paramE LPCWSTR textString the new text
*/
extern "C" void __stdcall ID3V2SetTextFrameW(u32 FrameID, LPCWSTR textString)
{
	if (textString == NULL || wcslen(textString) == 0)
		id3v2.deleteFrame(FrameID);
	else
		id3v2.SetText(FrameID, getValidPointer(textString));
}

/**
* Methode: ID3V2GetURLFrameW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Text eines URL Frames
* Beschreibung english: get the text of an url frame
* @paramD UInt32 FrameID die ID des Frames
* @returnD BSTR der Textstring
* @paramE UInt32 FrameID the ID of the frame
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetURLFrameW(u32 FrameID)
{
	return id3v2.GetURL(FrameID).AllocSysString();
}

/**
* Methode: ID3V2SetURLFrameW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Text eines URL Frames
* Beschreibung english: set the text of an url frame
* @paramD UInt32 FrameID die ID des Frames
* @paramD LPCWSTR textString der neue Text
* @paramE UInt32 FrameID the ID of the frame
* @paramE LPCWSTR textString the new text
*/
extern "C" void __stdcall ID3V2SetURLFrameW(u32 FrameID, LPCWSTR textString)
{
	if (textString == NULL || wcslen(textString) == 0)
		id3v2.deleteFrame(FrameID);
	else
		id3v2.SetURL(FrameID, getValidPointer(textString));
}
/**
* Methode: ID3V2GetFrameCountW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Frames einer ID
* Beschreibung english: get the frame count of a frame ID
* @paramD UInt32 FrameID die ID des Frames 
* @returnD Int16 Anzahl Frames
* @paramE UInt32 FrameID the ID of the frame
* @returnE Int16 frame count
*/
extern "C" short __stdcall ID3V2GetFrameCountW(u32 FrameID)
{
	return (short)id3v2.countFrame(FrameID);
}

/**
* Methode: ID3V2DeleteAllFramesW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: löscht alle Frames mit der FrameID
* Beschreibung english: deletes all frames with the frame id
* @paramD UInt32 FrameID die ID des Frames
* @returnD Int16 normalerweise -1, 0 falls ID nicht gefunden 
* @paramE UInt32 FrameID the ID of the frame
* @returnE Int16 normally -1, 0 if frame not found
*/
extern "C" short __stdcall ID3V2DeleteAllFramesW(u32 FrameID)
{
	return (id3v2.deleteAllFrames(FrameID) == 0) ? 0 : -1 ;
}

/**
* Methode: ID3V2DeleteSelectedFrameW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: löscht einen bestimmten Frame 
* Beschreibung english: deletes a specific frame
* @paramD UInt32 FrameID die ID des Frames
* @paramD Int16 Index der Index ab 1 bis Anzahl Vorkommen des Frames
* @returnD Int16 normalerweise -1, 0 falls Frame nicht gefunden
* @paramE UInt32 FrameID the ID of the frame
* @paramE Int16 Index from 1 to Framecount
* @returnE Int16 normally -1, 0 if frame not found
*/
extern "C" short __stdcall ID3V2DeleteSelectedFrameW(u32 FrameID, short Index)
{
	return b2s(id3v2.deleteFrame(FrameID, Index));
}

/**
* Methode: ID3V2SetFormatAndEncodingW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: setzt das Format und das Textencoding des id3v2 Tags
* Beschreibung english: set the format and text encoding of the id3v2 tag
* @paramD Int16 format 0=bestehendes Format 1=id3v2.2  2=id3v2.3 3=id3v2.4
* @paramD Int16 encoding 0= ISO-8859-1  1=UTF-16 mit BOM  2=UTF-16 ohne BOM  3=UTF-8
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE Int16 format 0=existing format 1=id3v2.2  2=id3v2.3 3=id3v2.4
* @paramE Int16 encoding 0= ISO-8859-1  1=UTF-16 with BOM  2=UTF-16 without BOM  3=UTF-8
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall ID3V2SetFormatAndEncodingW(short format, short encoding)
{
	return b2s(id3v2.setTargetFormatAndEncoding((BYTE)format, (BYTE)encoding));
}

/**
* Methode: ID3V2GetEncodingW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert die momentane Zeichenkodierung des gewünschten Frames
* Beschreibung english: get the encoding of the selected frame
* @paramD UInt32 FrameID die ID des Frames
* @paramE UInt32 FrameID the ID of the frame
* @returnD Int16 Zeichenkodierung -1= Frame nicht gefunden  0= ISO-8859-1  1=UTF-16 mit BOM  2=UTF-16 ohne BOM  3=UTF-8
* @returnE Int16 Encoding -1= FrameID not found  0= ISO-8859-1  1=UTF-16 with BOM  2=UTF-16 without BOM  3=UTF-8
*/
extern "C" short __stdcall ID3V2GetEncodingW(u32 FrameID)
{
	return id3v2.getEncoding(FrameID);
}

/**
* Methode: SetConfigValueW
* @link UNIVERSAL
* @since 2.0.1.0
* Beschreibung deutsch: setzt einen Konfigurationswert.<br />Momentan werden folgende Werte unterstützt:<br /><br /><table class="tablelayout"><tr><th>Schlüssel</th><th>Bezeichnung</th><th>Default-Wert</th><th>Erläuterung</th></tr><tr><td>0</td><td>MPEGEXACTREAD</td><td>0</td><td>Ein Wert ungleich 0 schaltet das Auslesen aller MPEG Frames an;<br />ersetzt SetMPEGAnalyzeAllFrames</td></tr><tr><td>1</td><td>ID3V2PADDINGSIZE</td><td>4096</td><td>die Padding-Grösse in Bytes im id3v2 tag</td></tr><tr><td>2</td><td>WRITEBLOCKSIZE</td><td>524288</td><td>die Blockgrösse in Bytes, die beim internen Kopieren von Dateien verwendet wird</td></tr><tr><td>3</td><td>DOEVENTSMILLIS </td><td>250</td><td>die Anzahl der Millisekunden, nachdem ein DoEvent abgefeuert wird</td></tr><tr><td>4</td><td>MAXTEXTBUFFER</td><td>262144</td><td>die maximale Grösse des internen Textpuffers in Byte</td></tr><tr><td>5</td><td>WMAPADDINGSIZE</td><td>4096</td><td>die Padding-Grösse in Bytes im wma tag</td></tr><tr><td>6</td><td>MP4PADDINGSIZE</td><td>4096</td><td>die Padding-Grösse in Bytes im mp4 tag</td></tr></table>
* Beschreibung english: set a configuration value.<br />The following keys are currently supported: <br /><br /><table class="tablelayout"><tr><th>Key</td><th>ID</th><th>Defaultvalue</th><th>Description</th></tr><tr><td>0</td><td>MPEGEXACTREAD</td><td>0</td><td>a non 0 value activates the reading from all MPEG frames<br />replaces SetMPEGAnalyzeAllFrames</td></tr><tr><td>1</td><td>ID3V2PADDINGSIZE</td><td>4096</td><td>the padding size in bytes for an id3v2 tag</td></tr><tr><td>2</td><td>WRITEBLOCKSIZE</td><td>524288</td><td>the blocksize in bytes for internal file copy</td></tr><tr><td>3</td><td>DOEVENTSMILLIS </td><td>250</td><td>millseconds, after that AudioGenie fires a DoEvent</td></tr><tr><td>4</td><td>MAXTEXTBUFFER</td><td>262144</td><td>the maximum text size in bytes</td></tr><tr><td>5</td><td>WMAPADDINGSIZE</td><td>4096</td><td>the padding size in bytes for a wma tag</td></tr><tr><td>6</td><td>MP4PADDINGSIZE</td><td>4096</td><td>the padding size in bytes for a mp4 tag</td></tr></table>
* @paramD Int32 key der zu setzende Schlüssel ( siehe oben ). 
* @paramD Int32 value der neue Wert
* @paramE Int32 key the specific key. 
* @paramE Int32 value the new value
*/
extern "C" void __stdcall SetConfigValueW(long key, long value)
{
	CTools::instance().setConfigValue(key, value);
}
/**
* Methode: GetConfigValueW
* @link UNIVERSAL
* @since 2.0.1.0
* Beschreibung deutsch: liefert einen Konfigurationswert<br />Momentan werden folgende Werte unterstützt:<br /><br /><table class="tablelayout"><tr><th>Schlüssel</th><th>Bezeichnung</th><th>Erläuterung</th></tr><tr><td>0</td><td>MPEGEXACTREAD</td><td>Wert ungleich 0 zeigt an, dass alle MPEG Frames analysiert werden</td></tr><tr><td>1</td><td>ID3V2PADDINGSIZE</td><td>die Padding-Grösse in Bytes im id3v2 tag</td></tr><tr><td>2</td><td>WRITEBLOCKSIZE</td><td>die Blockgrösse in Bytes, die beim internen Kopieren von Dateien verwendet wird</td></tr><tr><td>3</td><td>DOEVENTSMILLIS </td><td>die Anzahl der Millisekunden, nachdem ein DoEvent abgefeuert wird</td></tr><tr><td>4</td><td>MAXTEXTBUFFER</td><td>die maximale Grösse des internen Textpuffers in Byte</td></tr><tr><td>5</td><td>WMAPADDINGSIZE</td><td>die Padding-Grösse in Bytes im wma tag</td></tr><tr><td>6</td><td>MP4PADDINGSIZE</td><td>die Padding-Grösse in Bytes im mp4 tag</td></tr></table>
* Beschreibung english: get a config value,<br />The following keys are currently supported: <br /><br /><table class="tablelayout"><tr><th>Key</td><th>ID</th><th>Description</th></tr><tr><td>0</td><td>MPEGEXACTREAD</td><td>a non 0 value indicates the reading from all MPEG frames</td></tr><tr><td>1</td><td>ID3V2PADDINGSIZE</td><td>the padding size in bytes for an id3v2 tag</td></tr><tr><td>2</td><td>WRITEBLOCKSIZE</td><td>the blocksize in bytes for internal file copy</td></tr><tr><td>3</td><td>DOEVENTSMILLIS </td><td>millseconds after AudioGenie fires a DoEvent</td></tr><tr><td>4</td><td>MAXTEXTBUFFER</td><td>the maximum text size in bytes</td></tr><tr><td>5</td><td>WMAPADDINGSIZE</td><td>the padding size in bytes for a wma tag</td></tr><tr><td>6</td><td>MP4PADDINGSIZE</td><td>the padding size in bytes for a mp4 tag</td></tr></table></body>
* @paramD Int32 key der Konfigurations-Schlüssel
* @returnD Int32 der Wert
* @paramE Int32 key the configuration key
* @returnE Int32 the value 
*/
extern "C" long __stdcall GetConfigValueW(long key)
{
	return CTools::instance().getConfigValue(key);
}

/**
* Methode: ID3V2GetGenreW
* @link ID3V2
* @frame TCON
* @since 2.0.1.0
* Beschreibung deutsch: "TCON" liefert das Genre
* Beschreibung english: "TCON" get the content type / genre
* @returnD BSTR Genre
* @returnE BSTR genre
*/
extern "C" BSTR __stdcall ID3V2GetGenreW()
{
	return id3v2.GetGenre().AllocSysString();
}


/**
* Methode: ID3V2SetGenreW
* @link ID3V2
* @frame TCON
* @since 2.0.1.0
* Beschreibung deutsch: "TCON" setzt das Genre
* Beschreibung english: "TCON" set the content type / genre
* @paramD LPCWSTR myGenre Genre
* @paramE LPCWSTR myGenre genre
*/
extern "C" void __stdcall ID3V2SetGenreW(LPCWSTR textString)
{
	id3v2.SetText(F_TCON, getValidPointer(textString));
}



/**
* Methode: ID3V2GetPlayCounterW
* @link ID3V2
* @frame PCNT
* @since 2.0.1.0
* Beschreibung deutsch: "PCNT" liefert einen Zähler, der die Anzahl der Abspielungen anzeigt
* Beschreibung english: "PCNT" get a counter of the number of times a file has been played
* @returnD Int32 counter
* @returnE Int32 counter
*/
extern "C" long __stdcall ID3V2GetPlayCounterW() 
{
	id3frame = id3v2.findFrame(F_PCNT);
	return (id3frame != NULL) ? cPCNT(id3frame)->getCounter() : 0;
}

/**
* Methode: ID3V2SetPlayCounterW
* @link ID3V2
* @frame PCNT
* @since 2.0.1.0
* Beschreibung deutsch: "PCNT" setzt einen Zähler, der die Anzahl der Abspielungen anzeigt
* Beschreibung english: "PCNT" set a counter of the number of times a file has been played
* @paramD Int32 myCounter Zähler
* @paramE Int32 mycounter counter
*/
extern "C" void __stdcall ID3V2SetPlayCounterW(long counter) 
{
	id3v2.replaceFrame(new CID3F_PCNT(counter));	
}


/**
* Methode: ID3V2ExistsW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: zeigt an, ob ein ID3v2-Tag existiert
* Beschreibung english: shows -1, if the ID3v2 tag exists
* @returnD Int16 -1 wenn ein ID3v2-Tag existiert, ansonsten 0
* @returnE Int16 -1 if ID3v2 tag exists, otherwise 0
*/
extern "C" short __stdcall ID3V2ExistsW()
{
	return b2s(id3v2.Size > 0);
}


/**
* Methode: ID3V2GetSizeW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: gibt die ID3v2-Tag-Länge in Bytes zurück
* Beschreibung english: get the size of the tag in bytes 
* @returnD Int32 die Länge in Bytes
* @returnE Int32 the size in bytes
*/
extern "C" long __stdcall ID3V2GetSizeW()
{
	return id3v2.Size;
}


/**
* Methode: ID3V2GetVersionW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: gibt die ID3v2-Version zurück
* Beschreibung english: get the ID3v2 version number
* @returnD BSTR die Versions-Nummer
* @returnE BSTR the version number
*/
extern "C" BSTR __stdcall ID3V2GetVersionW()
{
	CAtlString result;
	switch (id3v2.Version)
	{
	case 2:	result = _T("2.2"); break;
	case 3:	result = _T("2.3"); break;
	case 4: result = _T("2.4"); break;
	default: result.Empty();
	}
	return result.AllocSysString();
}


/**
* Methode: ID3V2AddCommentW
* @link ID3V2
* @frame COMM
* @since 2.0.1.0
* Beschreibung deutsch: "COMM" setzt oder ersetzt einen Kommentar<br />Es kann mehr als einen 'COMM' Eintrag geben, aber nur einen mit der gleichen Sprache und Inhaltsbeschreibung.
* Beschreibung english: "COMM" set a comment<br/>There may be more than one 'COMM' frame in each tag, but only one with the same language and content descriptor.
* @paramD LPCWSTR language das Sprachkürzel des Eintrages ( genau 3 Bytes lang ) z.B. ENG
* @paramD LPCWSTR description die Beschreibung des Eintrages
* @paramD LPCWSTR text der eigentliche Kommentar
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE LPCWSTR Language id of the entry, e.g. ENG for english ( must be 3 bytes long )
* @paramE LPCWSTR description the description of the entry
* @paramE LPCWSTR text the comment
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddCommentW(LPCWSTR Language, LPCWSTR Description, LPCWSTR Text)
{
	return b2s(id3v2.replaceFrame(new CID3F_COMM(getValidPointer(Language), getValidPointer(Description), getValidPointer(Text))));	
}


/**
* Methode: ID3V2GetCommentDescriptionW
* @link ID3V2
* @frame COMM
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Beschreibung zu einem Kommentar
* Beschreibung english: get the description from a comment
* @paramD Int16 index Index von 1 bis  Anzahl Comment frames
* @returnD BSTR die Beschreibung des Kommentares
* @paramE Int16 index index from 1 to comment frame count
* @returnE BSTR the description of the comment
*/
extern "C" BSTR __stdcall ID3V2GetCommentDescriptionW(short Index)
{
	id3frame = id3v2.findFrame(F_COMM, Index);
	return (id3frame != NULL) ? cCOMM(id3frame)->getDescription().AllocSysString() : CTools::instance().GetEmptyBSTR();
}


/**
* Methode: ID3V2GetCommentLanguageW
* @link ID3V2
* @frame COMM
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Sprachkürzel des Kommentares
* Beschreibung english: get the language from a comment
* @paramD Int16 index Index von 1 bis Anzahl Comment frames
* @returnD BSTR das Sprachkürzel des Kommentares, z.B. DEU für Deutsch
* @paramE Int16 index Index from 1 to comment frame count
* @returnE BSTR the language id of the comment, e.g. ENG for english
*/
extern "C" BSTR __stdcall ID3V2GetCommentLanguageW(short Index)
{
	id3frame = id3v2.findFrame(F_COMM, Index);
	return (id3frame != NULL) ? cCOMM(id3frame)->getLanguage().AllocSysString() : CTools::instance().GetEmptyBSTR();
}


/**
* Methode: ID3V2GetCommentW
* @link ID3V2
* @frame COMM
* @since 2.0.1.0
* Beschreibung deutsch: liefert einen Kommentar
* Beschreibung english: get a comment 
* @paramD Int16 index Index von 1 bis Anzahl Kommentar frames
* @returnD BSTR der eigentliche Kommentar
* @paramE Int16 index index from 1 to comment frame count
* @returnE BSTR the real comment
*/
extern "C" BSTR __stdcall ID3V2GetCommentW(short Index)
{
	id3frame = id3v2.findFrame(F_COMM, Index);
	return (id3frame != NULL) ? cCOMM(id3frame)->getText().AllocSysString() : CTools::instance().GetEmptyBSTR();
}


/**
* Methode: ID3V2ImportCueFile
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: importiert eine .CUE Datei und erstellt daraus CTOC / CHAP Frames
* Beschreibung english: imports a .cue file and creates CTOC / CHAP frames
* @paramD LPCWSTR FileName der Name der zu importierenden Datei
* @returnD Int16 normalerweise -1, 0 wenn Fehler aufgetreten sind
* @paramE LPCWSTR FileName the name of the import file 
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall ID3V2ImportCueFileW(LPCWSTR FileName)
{
	return b2s(id3v2.parseCueFile(getValidPointer(FileName)));
}

/**
* Methode: ID3V2AddLyricW
* @link ID3V2
* @frame USLT
* @since 2.0.1.0
* Beschreibung deutsch: "USLT" setzt oder ersetzt einen unsynchronisierten Liedtext-Eintrag<br />Es kann mehr als einen 'USLT' Eintrag geben, aber nur einen mit der gleichen Sprache und Inhaltsbeschreibung.
* Beschreibung english: "USLT" set a unsynchronised lyric<br />There may be more than one 'USLT' frame in each tag, but only one with the same language and content descriptor.
* @paramD LPCWSTR language das Sprachkürzel des Eintrages ( genau 3 Bytes ) z.B. ENG
* @paramD LPCWSTR description die Beschreibung des Eintrages
* @paramD LPCWSTR text der eigenliche Liedtext
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE LPCWSTR LanguageID ID of the entry, e.g. ENG for english ( must be 3 bytes long )
* @paramE LPCWSTR description the description of the entry
* @paramE LPCWSTR text the lyric
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddLyricW(LPCWSTR Language, LPCWSTR Description, LPCWSTR Text)
{
	return b2s(id3v2.replaceFrame(new CID3F_USLT(getValidPointer(Language), getValidPointer(Description), getValidPointer(Text))));	
}


/**
* Methode: ID3V2GetLyricDescriptionW
* @link ID3V2
* @frame USLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Beschreibung eines Liedtext-Eintrages
* Beschreibung english: get the description from a lyric
* @paramD Int16 index Index von 1 bis Anzahl lyric frames
* @returnD BSTR Beschreibung eines Liedtext-Eintrages
* @paramE Int16 index index from 1 to lyric frame count
* @returnE BSTR the description of the lyric
*/
extern "C" BSTR __stdcall ID3V2GetLyricDescriptionW(short Index)
{
	id3frame = id3v2.findFrame(F_USLT, Index);
	return (id3frame != NULL) ? cUSLT(id3frame)->getDescription().AllocSysString() : CTools::instance().GetEmptyBSTR();	
}


/**
* Methode: ID3V2GetLyricLanguageW
* @link ID3V2
* @frame USLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Sprachkürzel eines Liedtext-Eintrages
* Beschreibung english: get the language from a lyrics
* @paramD Int16 index Index von 1 bis Anzahl Lyric Frames
* @returnD BSTR Sprachkürzel des Eintrages, z.B. DEU für Deutsch
* @paramE Int16 index index from 1 to lyric frame count
* @returnE BSTR the language id of the lyric, e.g. ENG for english
*/
extern "C" BSTR __stdcall ID3V2GetLyricLanguageW(short Index)
{
	id3frame = id3v2.findFrame(F_USLT, Index);
	return (id3frame != NULL) ? cUSLT(id3frame)->getLanguage().AllocSysString() : CTools::instance().GetEmptyBSTR();	
}


/**
* Methode: ID3V2GetLyricW
* @link ID3V2
* @frame USLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Liedtext eines Eintrages
* Beschreibung english: get a Lyric
* @paramD Int16 index Index von 1 bis Anzahl Lyric Frames
* @returnD BSTR Liedtext eines Eintrages
* @paramE Int16 index Index from 1 to Lyric frame count
* @returnE BSTR the text of the lyric
*/
extern "C" BSTR __stdcall ID3V2GetLyricW(short Index)
{
	id3frame = id3v2.findFrame(F_USLT, Index);
	return (id3frame != NULL) ? cUSLT(id3frame)->getText().AllocSysString() : CTools::instance().GetEmptyBSTR();	
}

/**
* Methode: ID3V2GetPictureDescriptionW
* @link ID3V2
* @frame APIC
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Beschreibung eines Bildes
* Beschreibung english: get the description from a picture
* @paramD Int16 index Index von 1 bis Anzahl Picture frames
* @returnD BSTR Beschreibung eines Bildes
* @paramE Int16 index index from 1 to picture frame count
* @returnE BSTR the description of the picture
*/
extern "C" BSTR __stdcall ID3V2GetPictureDescriptionW(short Index) 
{
	id3frame = id3v2.findFrame(F_APIC, Index);
	return (id3frame != NULL) ? cAPIC(id3frame)->getDescription().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetPictureSizeW
* @link ID3V2
* @frame APIC
* @since 2.0.2.0
* Beschreibung deutsch: liefert die Grösse eines Bildes in Bytes
* Beschreibung english: get the size from a picture in bytes
* @paramD Int16 index Index von 1 bis Anzahl Picture frames
* @returnD Int32 die Grösse des Bildes in Bytes
* @paramE Int16 index index from 1 to picture frame count
* @returnE Int32 the size of the picture in bytes
*/
extern "C" long __stdcall ID3V2GetPictureSizeW(short Index) 
{
	id3frame = id3v2.findFrame(F_APIC, Index);
	return (id3frame != NULL) ? cAPIC(id3frame)->getPictureSize() : 0;
}

/**
* Methode: ID3V2GetPictureTypeW
* @link ID3V2
* @frame APIC
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Bildtyp eines Bildes
* Beschreibung english: get the picture type from a picture
* @paramD Int16 index Index von 1 bis Anzahl Picture frames
* @returnD Int16 Typ des Bildes von 0 bis 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @paramE Int16 index index from 1 to picture frame count
* @returnE Int16 picture type from 0 to 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
*/
extern "C" short __stdcall ID3V2GetPictureTypeW(short Index) 
{
	id3frame = id3v2.findFrame(F_APIC, Index);
	return (id3frame != NULL) ? cAPIC(id3frame)->getPictureType() : 0;
}

/**
* Methode: ID3V2GetPictureTypeTextW
* @link ID3V2
* @frame APIC
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Bildtyp eines Bildes
* Beschreibung english: get the picture type from a picture
* @paramD Int16 index Index von 1 bis Anzahl Picture frames
* @returnD BSTR Typ des Bildes als Text
* @paramE Int16 index index from 1 to picture frame count
* @returnE BSTR picture type as text
*/
extern "C" BSTR __stdcall ID3V2GetPictureTypeTextW(short Index) 
{
	id3frame = id3v2.findFrame(F_APIC, Index);
	return (id3frame != NULL) ? cAPIC(id3frame)->getPictureTypeAsText().AllocSysString() : CTools::instance().GetEmptyBSTR();
}


/**
* Methode: ID3V2GetPictureMimeW
* @link ID3V2
* @frame APIC
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Mimetyp eines Bildes, z.B. "image/jpeg" oder "image/bmp" oder "XXX" bei unbekanntem Format<br\>Ist das Bild als Verweis gespeichert worden, liefert die Funktion den Dateinamen des Bildes angeführt von "-->", also z.B. "-->C:\test.jpg".
* Beschreibung english: get the mime type from a picture, e.g. "image/jpeg" or "image/bmp" or "XXX" for unknown format<br\>If the file was stored as a Link then this method returns then filename starting with "-->", e.g. "-->C:\test.jpg"
* @paramD Int16 index Index von 1 bis Anzahl Picture Frames
* @returnD BSTR Mimetyp eines Bildes
* @paramE Int16 index index from 1 to picture frame count
* @returnE BSTR the mime type of the picture
*/
extern "C" BSTR __stdcall ID3V2GetPictureMimeW(short Index) 
{
	id3frame = id3v2.findFrame(F_APIC, Index);
	return (id3frame != NULL) ? cAPIC(id3frame)->getMime().AllocSysString() : CTools::instance().GetEmptyBSTR();
}


/**
* Methode: ID3V2GetPictureFileW
* @link ID3V2
* @frame APIC
* @since 2.0.1.0
* Beschreibung deutsch: überträgt ein Bild aus dem ID3v2-Tag in die angegebene Datei
* Beschreibung english: get a picture from the tag and store it in the specified file
* @paramD LPCWSTR Filename Name der Datei in die das Bild übertragen wird
* @paramD Int16 index Index von 1 bis Anzahl Picture frames
* @returnD Int16 normalerweise -1, bei Fehler oder nicht gefundenem Index 0
* @paramE LPCWSTR Filename Name of the file where the picture will be stored
* @paramE Int16 index index from 1 to picture frame count
* @returnE Int16 normally -1, 0 on error or index not present
*/
extern "C" short __stdcall ID3V2GetPictureFileW(LPCWSTR FileName, short Index) 
{
	id3frame = id3v2.findFrame(F_APIC, Index);
	return (id3frame != NULL) ? b2s(cAPIC(id3frame)->getPictureFile(getValidPointer(FileName))) : b2s(false);
}

/**
* Methode: ID3V2GetPictureArrayW
* @link ID3V2
* @frame APIC
* @since 2.0.1.0
* Beschreibung deutsch: überträgt ein Bild aus dem ID3v2-Tag in ein Byte-Array
* Beschreibung english: get a picture from the tag and copy it in a byte array
* @paramD Pointer Zeiger auf das Array mit den Byte-Daten
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD Int16 index Index von 1 bis Anzahl Picture frames
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer pointer to a Byte array
* @paramE UInt32 maxLen maximum size of the byte array
* @paramE Int16 index index from 1 to picture frame count
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetPictureArrayW(BYTE *arr, u32 maxLen, short Index)
{
	id3frame = id3v2.findFrame(F_APIC, Index);
	return (id3frame != NULL) ? cAPIC(id3frame)->getData(arr, maxLen) : -1;	
}

/**
* Methode: ID3V2AddPictureFileW
* @link ID3V2
* @frame APIC
* @since 2.0.1.0
* Beschreibung deutsch: "APIC" überträgt ein externes Bild in einen ID3V2-Tag<br /><br />Es kann mehr als einen 'APIC' Eintrag geben, aber nur einen mit der gleichen Sprache und Inhaltsbeschreibung.
* Beschreibung english: "APIC" store a picture from a file in the tag<br />There may be more than one 'APIC' frame in each tag, but only one with the same content descriptor.
* @paramD LPCWSTR Filename Name der Bild-Datei
* @paramD LPCWSTR Beschreibung Kommentar zu dem Bild
* @paramD Int16 PictureTyp Typ des Bildes von 0 bis 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @paramD Int16 asLink Zeigt an, wie das Bild übernommen wird<br /><br />-1 es wird nur der Dateiname übernommen<br />0 = das Bild selbst wird übernommen
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR Filename name of the picture file
* @paramE LPCWSTR description a description of the picture
* @paramE Int16 picture type from 0 to 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @paramE Int16 asLink shows how the picture will be stored<br /><br />-1 = only a link to the image will be stored<br />0 = the image data will be stored
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall ID3V2AddPictureFileW(LPCWSTR FileName, LPCWSTR Description, short PictureType, short asLink)
{
	CID3F_APIC *pic = new CID3F_APIC((BYTE)PictureType, getValidPointer(Description));
	bool result = (asLink) ? pic->setFileLink(getValidPointer(FileName)) : pic->setPictureFile(getValidPointer(FileName));
	if (result)
		id3v2.replaceFrame(pic);
	else
		delete pic;
	return b2s(result);
}


/**
* Methode: ID3V2AddPictureArrayW
* @link ID3V2
* @frame APIC
* @since 2.0.1.0
* Beschreibung deutsch: "APIC" überträgt ein Bild aus einem Byte-Array in einen ID3V2-Tag<br /><br />Es kann mehr als einen 'APIC' Eintrag geben, aber nur einen mit der gleichen Sprache und Inhaltsbeschreibung.
* Beschreibung english: "APIC" store a picture from a byte array in the tag<br />There may be more than one 'APIC' frame in each tag, but only one with the same content descriptor.
* @paramD Pointer Zeiger auf ein Byte Array mit den Daten des Bildes
* @paramD UInt32 Length die Grösse des Arrays in Bytes
* @paramD LPCWSTR Beschreibung Kommentar zu dem Bild
* @paramD Int16 PictureTyp Typ des Bildes von 0 bis 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer pointer to a byte array with the picture datas
* @paramE UInt32 Length the size of the array
* @paramE LPCWSTR description a description of the picture
* @paramE Int16 picture type from 0 to 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddPictureArrayW(BYTE *arr, u32 Length, LPCWSTR Description, short PictureType)
{
	CID3F_APIC *pic = new CID3F_APIC((BYTE)PictureType, getValidPointer(Description));
	pic->setData(arr, Length);	
	return b2s(id3v2.replaceFrame(pic));
}



// AENC
/**
* Methode: ID3V2AddAudioEncryptionW
* @link ID3V2
* @frame AENC
* @since 2.0.1.0
* Beschreibung deutsch: fügt ein Verschlüsselungsframe hinzu<br />Es kann mehr als einen 'AENC' Eintrag geben, aber nur einen mit dem gleichen Identifikator.
* Beschreibung english: add an audio encryption frame<br />There may be more than one 'AENC' frames in a tag, but only one with the same 'Owner identifier'.
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 die Grösse des Arrays in Bytes
* @paramD LPCWSTR Identifikator des Eigentümers
* @paramD Int16 PreviewStart
* @paramD Int16 PreviewLength
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 size of the array in bytes
* @paramE LPCWSTR owner identifier
* @paramE Int16 PreviewStart
* @paramE Int16 PreviewLength
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddAudioEncryptionW(BYTE *arr, u32 maxLen, LPCWSTR URL, short PreviewStart, short PreviewLength)
{
	CID3F_AENC *f = new CID3F_AENC(getValidPointer(URL), PreviewStart, PreviewLength);
	f->setData(arr, maxLen);
	return b2s(id3v2.replaceFrame(f));
}

/**
* Methode: ID3V2GetAudioEncryptionDataW
* @link ID3V2
* @frame AENC
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Verschlüsselung
* Beschreibung english: get the encryption info
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD Int16 Index von 1 bis Anzahl AudioEncryptionFrames
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @paramE Int16 Index from 1 to AudioEncryptionFrame count
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetAudioEncryptionDataW(BYTE *arr, u32 maxLen, short Index)
{
	id3frame = id3v2.findFrame(F_AENC, Index);
	return (id3frame != NULL) ? cAENC(id3frame)->getData(arr, maxLen) : -1;	
}

/**
* Methode: ID3V2GetAudioEncryptionURLW
* @link ID3V2
* @frame AENC
* @since 2.0.1.0
* Beschreibung deutsch: liefert die URL mit einer email Adresse
* Beschreibung english: get the url containing an email address
* @paramD Int16 Index von 1 bis Anzahl AudioEncryptionFrames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to AudioEncryptionFrame count
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetAudioEncryptionURLW(short Index) 
{
	id3frame = id3v2.findFrame(F_AENC, Index);
	return (id3frame != NULL) ? cAENC(id3frame)->getIdentifier().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetAudioEncryptionPreviewStartW
* @link ID3V2
* @frame AENC
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Vorschaustartframe
* Beschreibung english: get the preview start frame
* @paramD Int16 Index von 1 bis Anzahl AudioEncryptionFrames
* @returnD Int16 Wert oder -1 falls Fehler
* @paramE Int16 Index from 1 to AudioEncryptionFrame count
* @returnE Int16 value or -1 if error
*/
extern "C" short __stdcall ID3V2GetAudioEncryptionPreviewStartW(short Index) 
{
	id3frame = id3v2.findFrame(F_AENC, Index);
	return (short)((id3frame != NULL) ? cAENC(id3frame)->getStart() : -1);
}

/**
* Methode: ID3V2GetAudioEncryptionPreviewLengthW
* @link ID3V2
* @frame AENC
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Vorschauframes
* Beschreibung english: get the preview length count
* @paramD Int16 Index von 1 bis Anzahl AudioEncryptionFrames
* @returnD Int16 Wert oder -1 falls Fehler
* @paramE Int16 Index from 1 to AudioEncryptionFrame count
* @returnE Int16 value or -1 if error
*/
extern "C" short __stdcall ID3V2GetAudioEncryptionPreviewLengthW(short Index) 
{
	id3frame = id3v2.findFrame(F_AENC, Index);
	return (short)((id3frame != NULL) ? cAENC(id3frame)->getLength() : -1);
}

// ASPI
/**
* Methode: ID3V2AddAudioSeekPointW
* @link ID3V2
* @frame ASPI
* @since 2.0.1.0
* Beschreibung deutsch: fügt einen Audio Suchindex hinzu<br />Es kann nur einen 'ASPI' Eintrag geben.
* Beschreibung english: add an audio seek point<br />There may only be one 'audio seek point index' frame in a tag.
* @paramD Pointer Zeiger auf ein Byte Array mit den Verschlüsselungsdaten
* @paramD UInt32 die Grösse des Arrays in Bytes
* @paramD Int32 start
* @paramD Int32 length
* @paramD Int16 numbers
* @paramD Byte BitsPerPoint
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array with the encryption data
* @paramE UInt32 size of the array in bytes
* @paramE Int32 start
* @paramE Int32 length
* @paramE Int16 numbers
* @paramE Byte BitsPerPoint
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddAudioSeekPointW(BYTE *arr, u32 maxLen, long start, long length, short numbers, BYTE BitsPerPoint)
{
	CID3F_ASPI *f = new CID3F_ASPI(start, length, numbers, BitsPerPoint);
	f->setData(arr, maxLen);
	return b2s(id3v2.replaceFrame(f));
}
/**
* Methode: ID3V2GetAudioSeekPointStartW
* @link ID3V2
* @frame ASPI
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Start des Suchindex
* Beschreibung english: Indexed data start
* @returnD Int32 Wert oder -1 falls Fehler
* @returnE Int32 value or -1 if error
*/
extern "C" long __stdcall ID3V2GetAudioSeekPointStartW() 
{
	id3frame = id3v2.findFrame(F_ASPI);
	return (id3frame != NULL) ? cASPI(id3frame)->getStart() : -1;
}
/**
* Methode: ID3V2GetAudioSeekPointLengthW
* @link ID3V2
* @frame ASPI
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Länge des Suchindexes
* Beschreibung english: Indexed data length
* @returnD Int32 Wert oder -1 falls Fehler
* @returnE Int32 value or -1 if error
*/
extern "C" long __stdcall ID3V2GetAudioSeekPointLengthW() 
{
	id3frame = id3v2.findFrame(F_ASPI);
	return (id3frame != NULL) ? cASPI(id3frame)->getLength() : -1;
}
/**
* Methode: ID3V2GetAudioSeekPointNumberW
* @link ID3V2
* @frame ASPI
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Suchindexe
* Beschreibung english: get the number of index points

* @returnD Int32 Wert oder -1 falls Fehler
* @returnE Int32 value or -1 if error
*/
extern "C" long __stdcall ID3V2GetAudioSeekPointNumberW() 
{
	id3frame = id3v2.findFrame(F_ASPI);
	return (id3frame != NULL) ? cASPI(id3frame)->getNumber() : -1;
}
/**
* Methode: ID3V2GetAudioSeekPointBitsPerIndexpointW
* @link ID3V2
* @frame ASPI
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Bits pro Suchindex
* Beschreibung english: get the bits per index point
* @returnD Int32 Wert oder -1 falls Fehler
* @returnE Int32 value or -1 if error
*/
extern "C" long __stdcall ID3V2GetAudioSeekPointBitsPerIndexpointW() 
{
	id3frame = id3v2.findFrame(F_ASPI);
	return (id3frame != NULL) ? cASPI(id3frame)->getBpi() : 0;
}
/**
* Methode: ID3V2GetAudioSeekPointDataW
* @link ID3V2
* @frame ASPI
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Datenarray mit den Suchindizes
* Beschreibung english: get the data array with the index points
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetAudioSeekPointDataW(BYTE *arr, u32 maxLen)
{
	id3frame = id3v2.findFrame(F_ASPI);
	return (id3frame != NULL) ? cASPI(id3frame)->getData(arr, maxLen) : -1;	
}

// COMR
/**
* Methode: ID3V2AddCommercialFrameW
* @link ID3V2
* @frame COMR
* @since 2.0.1.0
* Beschreibung deutsch: fügt einen Commercial Frame hinzu<br />Es kann mehr als einen 'COMR' Eintrag vorkommen, aber keine identischen. 
* Beschreibung english: add a commercial frame<br />There may be more than one 'COMR' in a tag, but no two may be identical.
* @paramD Pointer Zeiger auf ein Byte Array mit dem Logo ds Verkäufers
* @paramD UInt32 size die Grösse des Arrays in Bytes
* @paramD LPCWSTR Price Preisfeld 
* @paramD LPCWSTR validUntil (genau 8 Stellen ) gültig bis Format JJJJMMTT
* @paramD LPCWSTR contactUrl Kontakt-URL
* @paramD Int16 receivedAs Erhalten
* @paramD LPCWSTR seller Verkäufer
* @paramD LPCWSTR description Beschreibung
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array with the seller logo
* @paramE UInt32 size the size of the array in bytes
* @paramE LPCWSTR Price
* @paramE LPCWSTR validUntil in format YYYYMMDD ( 8 chars )
* @paramE LPCWSTR contactUrl
* @paramE Int16 receivedAs
* @paramE LPCWSTR seller
* @paramE LPCWSTR description
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddCommercialFrameW(BYTE *arr, u32 maxLen, LPCWSTR Price, LPCWSTR validUntil, LPCWSTR contactUrl , short receivedAs , LPCWSTR seller, LPCWSTR description)
{
	CID3F_COMR *f = new CID3F_COMR(getValidPointer(Price), getValidPointer(validUntil), getValidPointer(contactUrl), (BYTE)receivedAs, getValidPointer(seller), getValidPointer(description));
	f->setPicture(arr, maxLen);
	return b2s(id3v2.replaceFrame(f));
}

/**
* Methode: ID3V2GetCommercialFramePictureW
* @link ID3V2
* @frame COMR
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Verkäufer Logo
* Beschreibung english: get the seller logo
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD Int16 Index von 1 bis Anzahl CommercialFrames
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @paramE Int16 Index from 1 to Commercial Frame count
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetCommercialFramePictureW(BYTE *arr, u32 maxLen, short Index)
{
	id3frame = id3v2.findFrame(F_COMR, Index);
	return (id3frame != NULL) ? cCOMR(id3frame)->getData(arr, maxLen) : -1;	
}

/**
* Methode: ID3V2GetCommercialFramePriceW
* @link ID3V2
* @frame COMR
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Preis
* Beschreibung english: get the price
* @paramD Int16 Index von 1 bis Anzahl CommercialFrames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to Commercial Frame count
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetCommercialFramePriceW(short Index) 
{
	id3frame = id3v2.findFrame(F_COMR, Index);
	return (id3frame != NULL) ? cCOMR(id3frame)->getPrice().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetCommercialFrameValidUntilW
* @link ID3V2
* @frame COMR
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Gültig bis Datum
* Beschreibung english: get the valid until field
* @paramD Int16 Index von 1 bis Anzahl CommercialFrames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to Commercial Frame count
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetCommercialFrameValidUntilW(short Index) 
{
	id3frame = id3v2.findFrame(F_COMR, Index);
	return (id3frame != NULL) ? cCOMR(id3frame)->getValidUntil().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetCommercialFrameContactURLW
* @link ID3V2
* @frame COMR
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Kontakt URL 
* Beschreibung english: get the contact url
* @paramD Int16 Index von 1 bis Anzahl CommercialFrames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to Commercial Frame count
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetCommercialFrameContactURLW(short Index) 
{
	id3frame = id3v2.findFrame(F_COMR, Index);
	return (id3frame != NULL) ? cCOMR(id3frame)->getContactUrl().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetCommercialFrameSellerNameW
* @link ID3V2
* @frame COMR
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Verkäufernamen 
* Beschreibung english: get the seller name
* @paramD Int16 Index von 1 bis Anzahl CommercialFrames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to Commercial Frame count
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetCommercialFrameSellerNameW(short Index) 
{
	id3frame = id3v2.findFrame(F_COMR, Index);
	return (id3frame != NULL) ? cCOMR(id3frame)->getSellerName().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetCommercialFrameDescriptionW
* @link ID3V2
* @frame COMR
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Beschreibung  
* Beschreibung english: get the description
* @paramD Int16 Index von 1 bis Anzahl CommercialFrames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to Commercial Frame count
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetCommercialFrameDescriptionW(short Index) 
{
	id3frame = id3v2.findFrame(F_COMR, Index);
	return (id3frame != NULL) ? cCOMR(id3frame)->getDescription().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetCommercialFramePictureMimeW
* @link ID3V2
* @frame COMR
* @since 2.0.1.0
* Beschreibung deutsch: liefert den MIME Typ des Logos
* Beschreibung english: get the mime type of the logo
* @paramD Int16 Index von 1 bis Anzahl CommercialFrames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to Commercial Frame count
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetCommercialFramePictureMimeW(short Index) 
{
	id3frame = id3v2.findFrame(F_COMR, Index);
	return (id3frame != NULL) ? cCOMR(id3frame)->getPictureMime().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetCommercialFrameReceivedAsW
* @link ID3V2
* @frame COMR
* @since 2.0.1.0
* Beschreibung deutsch: liefert den "geliefert als" Wert , 0=Other, 1=Standard CD Album with other songs 2=Compressed audio on CD  3=File over the Internet  4=Stream over the Internet  5=As note sheets  6=As note sheets in a book with other sheets  7=Music on other media  8=Non-musical merchandise
* Beschreibung english: get the "received as" field<br />0=Other, 1=Standard CD Album with other songs 2=Compressed audio on CD  3=File over the Internet  4=Stream over the Internet  5=As note sheets  6=As note sheets in a book with other sheets  7=Music on other media  8=Non-musical merchandise
* @paramD Int16 Index von 1 bis Anzahl CommercialFrames
* @returnD Int16 Wert oder -1 falls Fehler
* @paramE Int16 Index from 1 to Commercial Frame count
* @returnE Int16 value or -1 if error
*/
extern "C" short __stdcall ID3V2GetCommercialFrameReceivedAsW(short Index) 
{
	id3frame = id3v2.findFrame(F_COMR, Index);
	return (id3frame != NULL) ? cCOMR(id3frame)->getReceivedAs() : -1;
}

// CTOC/CHAP
/**
* Methode: ID3V2AddTableOfContentW
* @link ID3V2
* @frame CTOC
* @since 2.0.1.0
* Beschreibung deutsch: CTOC Fügt ein Inhaltsverzeichnis hinzu<br />Es kann mehr als einen 'CTOC' Eintrag geben, aber jeder muss einen eindeutigen Identifikator haben.
* Beschreibung english: CTOC add a table of content<br />There may be more than one frame of this type in a tag but each must have an Element ID that is unique with respect to any other "CTOC" or "CHAP" frame in the tag
* @paramD LPCWSTR ID die eindeutige ID des Elementes
* @paramD LPCWSTR Title der Titel
* @paramD LPCWSTR Description die Beschreibung
* @paramD Int16 isOrdered -1 wenn die Untereinträge sortiert sind oder 0 falls unsortiert
* @returnD Int16 -1 falls Element ersetzt wurde, ansonsten 0
* @paramE LPCWSTR ID the unique id of the element
* @paramE LPCWSTR Title the title of the element
* @paramE LPCWSTR Description the description of the element
* @paramE Int16 isOrdered 0 if child elements are ordered or -1 if child elements are not ordered
* @returnE Int16 -1 if chapter was replaced, otherwise 0
*/
extern "C" short __stdcall ID3V2AddTableOfContentW(LPCWSTR ID, LPCWSTR Title, LPCWSTR Description, short isOrdered)
{
	// Wenn noch kein CTOC existiert, dann setze ROOT-Flag auf true
	CID3F_CTOC *f = new CID3F_CTOC(getValidPointer(ID), getValidPointer(Title), getValidPointer(Description), (isOrdered != 0));
	if (id3v2.findFrame(F_CTOC) == NULL)
		f->setRoot(true);
	return b2s(id3v2.replaceFrame(f));
}

/**
* Methode: ID3V2AddChapterW
* @link ID3V2
* @frame CHAP
* @since 2.0.1.0
* Beschreibung deutsch: CHAP fügt ein Kapitel hinzu<br />Es kann mehr als einen 'CHAP' Eintrag geben, aber jeder muss einen eindeutigen Identifikator haben.
* Beschreibung english: CHAP add a chapter<br />There may be more than one frame of this type in a tag but each must have an Element ID that is unique with respect to any other "CTOC" or "CHAP" frame in the tag
* @paramD LPCWSTR ID die eindeutige ID des Kapitels
* @paramD LPCWSTR Title der Titel
* @paramD LPCWSTR Description die Beschreibung
* @paramD UInt32 startTime Startzeit des Kapitels in Millisekunden
* @paramD UInt32 endTime Endezeit des Kapitels in Millisekunden
* @returnD Int16 -1 falls Element ersetzt wurde, ansonsten 0
* @paramE LPCWSTR ID the unique id
* @paramE LPCWSTR Title the title
* @paramE LPCWSTR Description the description 
* @paramE UInt32 startTime the start time in milliseconds
* @paramE UInt32 endTime the end time in milliseconds
* @returnE Int16 -1 if chapter was replaced, otherwise 0
*/
extern "C" short __stdcall ID3V2AddChapterW(LPCWSTR ID, LPCWSTR Title, LPCWSTR Description, u32 startTime, u32 endTime)
{
	CID3F_CHAP *f = new CID3F_CHAP(getValidPointer(ID), getValidPointer(Title), getValidPointer(Description));
	f->setTimes(startTime, endTime);
	return b2s(id3v2.replaceFrame(f));
}

/**
* Methode: ID3V2AddChildElementW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: fügt ein Unterelement zu einem übergeordneten Element hinzu 
* Beschreibung english: add a child element to a parent element
* @paramD LPCWSTR ParentTocID die ID des übergeordneten Elementes
* @paramD LPCWSTR ChildID die ID des hinzuzufügenden Elementes
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR ParentTocID
* @paramE LPCWSTR ChildID
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall ID3V2AddChildElementW(LPCWSTR ParentTocID, LPCWSTR ChildID)
{
	id3frame = id3v2.findFrame(getValidPointer(ParentTocID));
	if (id3frame == NULL)
		return b2s(false);
	cCTOC(id3frame)->addChildElement(getValidPointer(ChildID));
	return b2s(true);
}

/**
* Methode: ID3V2DeleteChildElementW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: löscht ein Unterelement und alle Referenzen
* Beschreibung english: delete the child element and all references
* @paramD LPCWSTR ParentTocID die ID des übergeordneten Elementes
* @paramD LPCWSTR ChildID die ID des zu löschenden Elementes
* @returnD Int16 normalerweise -1, 0 falls Element nicht gefunden
* @paramE LPCWSTR ParentTocID the parent ID
* @paramE LPCWSTR ChildID the ID to remove
* @returnE Int16 normally -1, 0 if id not found
*/
extern "C" short __stdcall ID3V2DeleteChildElementW(LPCWSTR ParentTocID, LPCWSTR ChildID)
{
	id3frame = id3v2.findFrame(getValidPointer(ParentTocID));
	if (id3frame == NULL)
		return b2s(false);
	return cCTOC(id3frame)->deleteChildElement(getValidPointer(ChildID));	
}

/**
* Methode: ID3V2DeleteAddendumW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: löscht ein CTOC oder CHAP Element und alle Unterverweise
* Beschreibung english: deletes a CTOC or CHAP Element and all of the child elements
* @paramD LPCWSTR ID die ID des Elementes
* @returnD Int16 normalerweise -1, 0 falls ID nicht gefunden
* @paramE LPCWSTR ID the ID of the element
* @returnE Int16 normally -1, 0 if ID not found
*/
extern "C" short __stdcall ID3V2DeleteAddendumW(LPCWSTR ID)
{
	ATLTRACE(_T("lösche %s\n"), ID);
	CID3_Frame *delFrame = id3v2.findFrame(getValidPointer(ID));
	if (delFrame == NULL)
		return-b2s(false);
	CID3F_Chapter *chap = cCHAPTER(delFrame);
	// Zuerst alle Subframes löschen
	// firstly delete all subframes
	chap->deleteAllFrames();
	// falls CTOC Element: rekursiv untergeordnete Zweige entfernen
	// in case of CTOC elemts: Remove subsequent nodes recursively
	if (chap->isCTOC())
	{
		CID3F_CTOC *toc = cCTOC(chap);
		for (u16 i = toc->getNumberOfEntries(); i > 0; i--)
		{
			ATLTRACE(_T(" rekursiv lösche %s\n"), toc->getChildElementID(i));
			ID3V2DeleteAddendumW(toc->getChildElementID(i));
		}
	}
	// Verweise aus verbleibenden TOCs löschen
	// delete references from remaining TOCs
	size_t pos = 0;
	CID3_Frame *found;
	do
	{
		found = id3v2.findNextFrame(F_CTOC, pos);
		if (found != NULL)
			cCTOC(found)->deleteChildElement(ID);

	} while (found != NULL);
	// zu guter letzt Element selbst löschen
	// finally delete last element itself
	id3v2.deleteAllFrames(getValidPointer(ID));
	return b2s(true);
}

/**
* Methode: ID3V2GetAddendumTitleW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Titel eines CTOC oder CHAP Elementes
* Beschreibung english: get the title of a CTOC or CHAP Element
* @paramD LPCWSTR ID die ID des gewünschten Elementes
* @returnD BSTR der Textstring 
* @paramE LPCWSTR ID the ID of the element
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetAddendumTitleW(LPCWSTR ID)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return CTools::instance().GetEmptyBSTR();
	return cCHAPTER(id3frame)->getTitle().AllocSysString();
}

/**
* Methode: ID3V2SetAddendumTitleW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Titel eines CHAP or CTOC Elementes
* Beschreibung english: set the title of a CHAP or CTOC element
* @paramD LPCWSTR ID die ID des gewünschten Elementes
* @paramD LPCWSTR newTitle der neue Titel
* @returnD Int16 normalerweise -1, 0 falls ID nicht gefunden
* @paramE LPCWSTR ID the ID of the element
* @paramE LPCWSTR newTitle the new title
* @returnE Int16 normally -1, 0 if ID not found
*/
extern "C" short __stdcall ID3V2SetAddendumTitleW(LPCWSTR ID, LPCWSTR newTitle)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return b2s(false);
	cCHAPTER(id3frame)->setTitle(getValidPointer(newTitle));
	return b2s(true);
}

/**
* Methode: ID3V2SetChapterTimesW
* @link ID3V2
* @frame CHAP
* @since 2.0.1.0
* Beschreibung deutsch: setzt die Zeiten eines CHAP Elementes
* Beschreibung english: set the times of a CHAP element
* @paramD LPCWSTR ID die ID des gewünschten Kapitels
* @paramD UInt32 startTime die Startzeit in Millisekunden
* @paramD UInt32 endTime die Endezeit in Millisekunden
* @returnD Int16 normalerweise -1, 0 falls ID nicht gefunden oder Element kein CHAP Element ist
* @paramE LPCWSTR ID the ID of the element
* @paramE UInt32 startTime the start time in millisecondes
* @paramE UInt32 endTime the end time in milliseconds
* @returnE Int16 normally -1, 0 if ID not found or element is not a CHAP element
*/
extern "C" short __stdcall ID3V2SetChapterTimesW(LPCWSTR ID, u32 startTime, u32 endTime)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return b2s(false);
	if (cCHAPTER(id3frame)->isCHAP())
	{
		cCHAP(id3frame)->setTimes(startTime, endTime);
		return b2s(true);
	}
	return b2s(false);
}

/**
* Methode: ID3V2GetAddendumDescriptionW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Beschreibung eines CHAP oder CTOC Elementes
* Beschreibung english: get the description of a CHAP or CTOC element
* @paramD LPCWSTR ID die ID des gewünschten Elementes
* @returnD BSTR der Textstring
* @paramE LPCWSTR ID the ID of the element
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetAddendumDescriptionW(LPCWSTR ID)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return CTools::instance().GetEmptyBSTR();
	return cCHAPTER(id3frame)->getDescription().AllocSysString();
}
/**
* Methode: ID3V2SetAddendumDescriptionW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: setzt die Beschreibung eines CHAP oder CTOC Elementes 
* Beschreibung english: set the description of a CTOC or CHAP element
* @paramD LPCWSTR ID die ID des Elementes
* @paramD LPCWSTR newDescription die neue Beschreibung
* @returnD Int16 normalerweise -1, 0 falls ID nicht gefunden
* @paramE LPCWSTR ID the id of the element
* @paramE LPCWSTR newDescription the new description
* @returnE Int16 normally -1, 0 if ID not found
*/
extern "C" short __stdcall ID3V2SetAddendumDescriptionW(LPCWSTR ID, LPCWSTR newDescription)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return b2s(false);
	cCHAPTER(id3frame)->setDescription(getValidPointer(newDescription));
	return b2s(true);
}

/**
* Methode: ID3V2GetAddendumTypeW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Typ des Elementes
* Beschreibung english: get the type of the element
* @paramD LPCWSTR ID die ID des gewünschten Elementes
* @returnD Int16 0= Element ist ein CHAP Element, 1=Element ist ein CTOC Element, -1=ID nicht gefunden
* @paramE LPCWSTR ID the ID of the element
* @returnE Int16 0=element is a CHAP element, 1=element is a CTOC element, -1=ID not found
*/
extern "C" short __stdcall ID3V2GetAddendumTypeW(LPCWSTR ID)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return -1;
	return (cCHAPTER(id3frame)->isCTOC()) ? 1 : 0;
}

/**
* Methode: ID3V2GetAllFrameIDsW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert eine kommagetrennte Liste aller vorkommenden eindeutigen Frame IDs 
* Beschreibung english: get a comma based List of all unique frame ids
* @returnD BSTR der Textstring Kommagetrennter String mit Frame IDs
* @returnE BSTR the text string comma based text string with Frame IDs
*/
extern "C" BSTR __stdcall ID3V2GetAllFrameIDsW()
{
	return id3v2.getAllFrameIDs().AllocSysString();
}

/**
* Methode: ID3V2GetChapterStartTimeW
* @link ID3V2
* @frame CHAP
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Startzeit eines CHAP Elementes in Millisekunden
* Beschreibung english: get the start time of a CHAP element in milliseconds
* @paramD LPCWSTR ID die ID des Kapitels
* @returnD UInt32 Zeit in Millisekunden
* @paramE LPCWSTR ID the ID of the Chapter
* @returnE UInt32 time in milliseconds
*/
extern "C" u32 __stdcall ID3V2GetChapterStartTimeW(LPCWSTR ID)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return 0;
	return (cCHAPTER(id3frame)->isCHAP()) ? cCHAP(id3frame)->getStartTime() : 0;
}

/**
* Methode: ID3V2GetChapterEndTimeW
* @link ID3V2
* @frame CHAP
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Endzeit eines CHAP Elementes in Millisekunden
* Beschreibung english: get the end time of a CHAP element in milliseconds
* @paramD LPCWSTR ID die ID des Kapitels
* @returnD UInt32 Zeit in Millisekunden
* @paramE LPCWSTR ID the ID of the Chapter
* @returnE UInt32 time in milliseconds
*/
extern "C" u32 __stdcall ID3V2GetChapterEndTimeW(LPCWSTR ID)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return 0;
	return (cCHAPTER(id3frame)->isCHAP()) ? cCHAP(id3frame)->getEndTime() : 0;
}

/**
* Methode: ID3V2GetTOCIsOrderedW
* @link ID3V2
* @frame CTOC
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Sortierstatus eines CTOC Elementes
* Beschreibung english: get the sort order of a CTOC element
* @paramD LPCWSTR ID die ID des CTOC Elementes
* @returnD Int16 0=nicht sortiert, -1=sortiert
* @paramE LPCWSTR ID the ID of the CTOC element
* @returnE Int16 0=nicht sortiert, -1=sortiert
*/
extern "C" short __stdcall ID3V2GetTOCIsOrderedW(LPCWSTR ID)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return 0;
	return (cCHAPTER(id3frame)->isCTOC()) ? b2s(cCTOC(id3frame)->isOrdered()) : 0;
} 
/**
* Methode: ID3V2SetTOCIsOrderedW
* @link ID3V2
* @frame CTOC
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Sortierstatus eines CTOC Elementes 
* Beschreibung english: set the sort order of a CTOC element
* @paramD LPCWSTR ID die ID des CTOC Elementes
* @paramD Int16 status der neue Status 0=nicht sortiert -1=sortiert
* @returnD Int16 normalerweise -1, 0 falls ID nicht gefunden
* @paramE LPCWSTR ID the ID of the CTOC element
* @paramE Int16 status the new status, 0=ordered -1=not ordered
* @returnE Int16 normally -1, 0 if ID not found
*/
extern "C" short __stdcall ID3V2SetTOCIsOrderedW(LPCWSTR ID, short status)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return b2s(false);
	if (cCHAPTER(id3frame)->isCTOC())
		cCTOC(id3frame)->setOrdered(status != 0);
	return b2s(true);
} 


/**
* Methode: ID3V2GetSubFramesW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl von Unterframes eines CTOC oder CHAP Elementes
* Beschreibung english: get the count of subframes from an ID
* @paramD LPCWSTR ID die ID des Parent-Frames
* @returnD Int16 die Anzahl der Subframes oder -1 bei Fehler
* @paramE LPCWSTR ID the ID of the Parent frame
* @returnE Int16 subframe count or -1 if error
*/
extern "C" short __stdcall ID3V2GetSubFramesW(LPCWSTR ID)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return -1;
	return cCHAPTER(id3frame)->getFrameCount();
}
/**
* Methode: ID3V2GetSubFrameIDW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert die ID des Unterframes eines Parentframes
* Beschreibung english: get the ID of a subframe from a parent frame
* @paramD LPCWSTR ID die ID des übergeordneten Frames
* @paramD Int16 Index von 1 bis ID3V2GetSubFramesW
* @returnD UInt32 die Frame ID oder 0 falls übergeordneter Frame nicht gefunden
* @paramE LPCWSTR ID the ID of the parent frame
* @paramE Int16 Index the index from 1 to ID3V2GetSubFramesW
* @returnE UInt32 Frame ID or 0 if parent frame not found
*/
extern "C" u32 __stdcall ID3V2GetSubFrameIDW(LPCWSTR ID, short Index)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return 0;
	return cCHAPTER(id3frame)->getFrameID(Index);
}

/**
* Methode: ID3V2GetSubFrameTypeW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Typ des Unterframes eines Parentframes
* Beschreibung english: get the type of a subframe from a parent frame
* @paramD LPCWSTR ID die ID des übergeordneten Frames
* @paramD Int16 Index von 1 bis ID3V2GetSubFramesW
* @returnD Int16 1=normaler Textframe T000, 2=Benutzerdefinierter Textframe TXXX, 3=normaler URL Frame W000, 4=Benutzerdefinierter URL Frame WXXX, 5=Bild APIC, -1=ID nicht gefunden
* @paramE LPCWSTR ID the ID of the parent frame
* @paramE Int16 Index the index from 1 to ID3V2GetSubFramesW
* @returnE Int16 1=normal textframe T000, 2=userdefined textframe TXXX, 3=normal URL frame W000, 4=userdefined URL frame WXXX, 5=Picture APIC, -1=ID not found
*/
extern "C" short __stdcall ID3V2GetSubFrameTypeW(LPCWSTR ID, short Index)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return -1;
	return cCHAPTER(id3frame)->getFrameTyp(Index);
}

/**
* Methode: ID3V2GetSubFrameTextW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Text eines Unterframes
* Beschreibung english: get the text of a subframe
* @paramD LPCWSTR ID die ID des übergeordneten Frames
* @paramD Int16 Index von 1 bis ID3V2GetSubFramesW
* @returnD BSTR der Textstring
* @paramE LPCWSTR ID the ID of the parent frame
* @paramE Int16 Index the index from 1 to ID3V2GetSubFramesW
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetSubFrameTextW(LPCWSTR ID, short Index)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return CTools::instance().GetEmptyBSTR();
	return cCHAPTER(id3frame)->getFrameText(Index).AllocSysString();
}

/**
* Methode: ID3V2GetSubFrameDescriptionW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Beschreibung eines Unterframes
* Beschreibung english: get the description of a subframe
* @paramD LPCWSTR ID  die ID des übergeordneten Frames
* @paramD Int16 Index von 1 bis ID3V2GetSubFramesW
* @returnD BSTR der Textstring
* @paramE LPCWSTR ID the ID of the parent frame
* @paramE Int16 Index the index from 1 to ID3V2GetSubFramesW
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetSubFrameDescriptionW(LPCWSTR ID, short Index)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return CTools::instance().GetEmptyBSTR();
	return cCHAPTER(id3frame)->getFrameDescription(Index).AllocSysString();
}

/**
* Methode: ID3V2GetSubFrameImageW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert eine Byte Array eines Bild-Unterframes 
* Beschreibung english: get a byte array from a picture subframe
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD LPCWSTR ID die ID des übergeordneten Frames
* @paramD Int16 Index von 1 bis ID3V2GetSubFramesW
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to a byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @paramE LPCWSTR ID the ID of the parent frame
* @paramE Int16 Index the index from 1 to ID3V2GetSubFramesW
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetSubFrameImageW(BYTE *arr, u32 maxLen, LPCWSTR ID, short Index)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return -1;
	CID3F_Chapter *chap = cCHAPTER(id3frame);
	if (chap->getFrameTyp(Index) != 5) // Element ist kein APIC Frame
		return -1;
	return cAPIC(chap->getFrame(Index))->getData(arr, maxLen);	
}

/**
* Methode: ID3V2GetSubFrameImageTypeW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Bildtyp eines Bild-Unterframes
* Beschreibung english: get the picture type of picture subframe
* @paramD LPCWSTR ID die ID des übergeordneten Frames
* @paramD Int16 Index von 1 bis ID3V2GetSubFramesW
* @returnD Int16 Typ des Bildes von 0 bis 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @paramE LPCWSTR ID the ID of the parent frame
* @paramE Int16 Index the index from 1 to ID3V2GetSubFramesW
* @returnE Int16 picture type from 0 to 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
*/
extern "C" short __stdcall ID3V2GetSubFrameImageTypeW(LPCWSTR ID, short Index)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return b2s(false);
	CID3F_Chapter *chap = cCHAPTER(id3frame);
	if (chap->getFrameTyp(Index) != 5) // Element ist kein APIC Frame
		return -2;
	return cAPIC(chap->getFrame(Index))->getPictureType();	
}

/**
* Methode: ID3V2GetPossibleTOCIDW
* @link ID3V2
* @frame CTOC
* @since 2.0.1.0
* Beschreibung deutsch: liefert eine freie ID für einen CTOC Frame
* Beschreibung english: get a free ID for a CTOC frame
* @returnD BSTR die freie ID
* @returnE BSTR the free ID
*/
extern "C" BSTR __stdcall ID3V2GetPossibleTOCIDW()
{
	size_t index = 0;
	int lfdNr = 0;
	CAtlString tmp, defaultString;
	defaultString = _T("toc");
	tmp.Format(_T("%s%i"), defaultString, lfdNr);
	do 
	{
		id3frame = id3v2.findNextFrame(F_CTOC, index);
		if (id3frame != NULL && cCHAPTER(id3frame)->getID().CompareNoCase(tmp) == 0)
		{
			index = 0;
			tmp.Format(_T("%s%i"), defaultString, ++lfdNr);
		}
	} while (id3frame != NULL);
	return tmp.AllocSysString();
}

/**
* Methode: ID3V2GetPossibleCHAPIDW
* @link ID3V2
* @frame CHAP
* @since 2.0.1.0
* Beschreibung deutsch: liefert eine freie ID für einen CHAP Frame
* Beschreibung english: get a free ID for a CHAP frame
* @returnD BSTR die freie ID
* @returnE BSTR the free ID
*/
extern "C" BSTR __stdcall ID3V2GetPossibleCHAPIDW()
{
	size_t index = 0;
	int lfdNr = 0;
	CAtlString tmp, defaultString;
	defaultString = _T("ch");
	tmp.Format(_T("%s%i"), defaultString, lfdNr);
	do 
	{
		id3frame = id3v2.findNextFrame(F_CHAP, index);
		if (id3frame != NULL && cCHAPTER(id3frame)->getID().CompareNoCase(tmp) == 0)
		{
			index = 0;
			tmp.Format(_T("%s%i"), defaultString, ++lfdNr);
		}
	} while (id3frame != NULL);
	return tmp.AllocSysString();
}

/**
* Methode: ID3V2GetTOCRootIDW
* @link ID3V2
* @frame CTOC
* @since 2.0.1.0
* Beschreibung deutsch: liefert die ID des obersten CTOC Elementes
* Beschreibung english: get the ID of the root CTOC element
* @returnD BSTR die ID des Root Elementes
* @returnE BSTR the id of the rrot CTOC element
*/
extern "C" BSTR __stdcall ID3V2GetTOCRootIDW()
{
	size_t pos = 0;
	do
	{
		id3frame = id3v2.findNextFrame(F_CTOC, pos);		

	} while (id3frame != NULL && !cCTOC(id3frame)->isRoot());
	return (id3frame != NULL) ? cCTOC(id3frame)->getID().AllocSysString() : CTools::instance().GetEmptyBSTR(); 
}
/**
* Methode: ID3V2GetChildElementsW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der zugeordneten Elemente eines CTOC oder CHAP Frames
* Beschreibung english: get the count of the child elements from a CTOC or CHAP element
* @paramD LPCWSTR ID die übergeordnete Frame ID
* @returnD Int16 Anzahl der Elemente oder -1
* @paramE LPCWSTR ID the parent frame ID
* @returnE Int16 count of the elements or -1
*/
extern "C" short __stdcall ID3V2GetChildElementsW(LPCWSTR ID)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return -1;
	CID3F_Chapter *chap = cCHAPTER(id3frame);
	return (chap->isCTOC()) ? (cCTOC(chap))->getNumberOfEntries() : -1;
}
/**
* Methode: ID3V2GetChildElementIDW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: liefert die ID eines Unterelementes 
* Beschreibung english: get the ID of a child element
* @paramD LPCWSTR ID die ID des übergeordneten Frames
* @paramD Int16 Index von 1 bis ID3V2GetChildElementsW
* @returnD BSTR die ID des Elementes
* @paramE LPCWSTR ID the ID of the parent frame
* @paramE Int16 Index from 1 to ID3V2GetChildElementsW
* @returnE BSTR the ID of the element
*/
extern "C" BSTR __stdcall ID3V2GetChildElementIDW(LPCWSTR ID, short Index)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return CTools::instance().GetEmptyBSTR();
	CID3F_Chapter *chap = cCHAPTER(id3frame);
	return (chap->isCTOC()) ? (cCTOC(chap))->getChildElementID(Index).AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2SetTextSubFrameW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: setzt einen Textframe, falls vorhanden wird er ersetzt
* Beschreibung english: set a textframe, will be replaced if exists
* @paramD LPCWSTR ID die ID des übergeordneten Frames
* @paramD UInt32 FrameID die ID des neuen Frames
* @paramD LPCWSTR textString der neue Text
* @paramD LPCWSTR description die neue Beschreibung
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR ID the ID of the parent frame
* @paramE UInt32 FrameID the ID of the new frame
* @paramE LPCWSTR textString the new text
* @paramE LPCWSTR description the new description
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall ID3V2SetTextSubFrameW(LPCWSTR ID, u32 FrameID, LPCWSTR textString, LPCWSTR description)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return b2s(false);
	CID3F_Chapter *chap = cCHAPTER(id3frame);
	if (textString == NULL || wcslen(textString) == 0)
		chap->deleteFrame(FrameID);
	else
	{
		if (FrameID == F_TXXX)
			chap->addFrame(new CID3F_TXXX(getValidPointer(description), getValidPointer(textString)));
		else
			chap->addFrame(new CID3F_T000(FrameID, getValidPointer(textString)));
	}
	return b2s(true);
}

/**
* Methode: ID3V2SetURLSubFrameW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: setzt einen URL Frame, falls vorhanden wird er ersetzt
* Beschreibung english: set a url frame, will be replaced if exists
* @paramD LPCWSTR ID die ID des übergeordneten Frames
* @paramD UInt32 FrameID die ID des neuen Frames
* @paramD LPCWSTR urlString die neue URL
* @paramD LPCWSTR description die neue Beschreibung
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR ID the ID of the parent frame
* @paramE UInt32 FrameID the ID of the new frame
* @paramE LPCWSTR urlString the new url
* @paramE LPCWSTR description the new description
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall ID3V2SetURLSubFrameW(LPCWSTR ID, u32 FrameID, LPCWSTR urlString, LPCWSTR description)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return b2s(false);
	CID3F_Chapter *chap = cCHAPTER(id3frame);
	if (urlString == NULL || wcslen(urlString) == 0)
		chap->deleteFrame(FrameID);
	else
	{
		if (FrameID == F_WXXX)
			chap->addFrame(new CID3F_WXXX(getValidPointer(description), getValidPointer(urlString)));
		else
			chap->addFrame(new CID3F_W000(FrameID, getValidPointer(urlString)));
	}
	return b2s(true);
}

/**
* Methode: ID3V2SetImageSubFrameW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: setzt einen neuen Imageframe, falls vorhanden wird er ersetzt
* Beschreibung english: set a picture frame, will be replaced if exists
* @paramD Pointer Zeiger auf ein Byte Array mit den Bilddaten
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD LPCWSTR ID die ID des übergeordneten Frames
* @paramD LPCWSTR Description die Beschreibung des Bildes
* @paramD Int16 PictureType Typ des Bildes von 0 bis 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE Pointer pointer to a byte array with picture data 
* @paramE UInt32 maxLen maximum size of the array in bytes
* @paramE LPCWSTR ID the ID of the parent frame
* @paramE LPCWSTR Description the description of the image
* @paramE Int16 PictureType type from 0 to 20 <br />00 = Other<br />01 = 32*32 pixels file icon<br />02 = Other file icon<br />03 = Cover front<br />04 = Cover back<br />05 = Leaflet page<br />06 = Media ( label side of CD )<br />07 = Lead artist/solist<br />08 = Artist/Performer<br />09 = Conductor<br />10 = Band/Orchestra<br />11 = Composer<br />12 = Lyricist/text writer<br />13 = Recording Location<br />14 = During recording<br />15 = During performance<br />16 = Movie/video screen capture<br />17 = A bright coloured fish<br />18 = Illustration<br />19 = Band/artist logotype<br />20 = Publisher/Studio logotype
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall ID3V2SetImageSubFrameW(BYTE *arr, u32 maxLen, LPCWSTR ID, LPCWSTR Description, short PictureType)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return b2s(false);
	CID3F_APIC *pic = new CID3F_APIC((BYTE)PictureType, getValidPointer(Description));
	pic->setData(arr, maxLen);
	cCHAPTER(id3frame)->addFrame(pic);
	return b2s(true);
}

/**
* Methode: ID3V2DeleteSubFrameW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: entfernt einen Subframe
* Beschreibung english: deletes a subframe
* @paramD LPCWSTR ID die ID des übergeordneten Frames
* @paramD UInt32 FrameID die ID des zu löschenden Frames
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR ID the ID of the parent frame
* @paramE UInt32 FrameID the ID of the frame to remove
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall ID3V2DeleteSubFrameW(LPCWSTR ID, u32 FrameID)
{
	id3frame = id3v2.findFrame(getValidPointer(ID));
	if (id3frame == NULL)
		return b2s(false);
	cCHAPTER(id3frame)->deleteFrame(FrameID);
	return b2s(true);
}

// ENCR
/**
* Methode: ID3V2AddEncryptionW
* @link ID3V2
* @frame ENCR
* @since 2.0.1.0
* Beschreibung deutsch: ENCR fügt einen Enryption Frame hinzu<br />Es kann mehr als einen 'ENCR' Eintrag geben, aber nur einen mit dem gleichen Symbol und Identifikator.
* Beschreibung english: ENCR add an encryption frame<br \There may be more than one 'ENCR' frame in each tag, but only one with the same symbol and owner identifier.
* @paramD Pointer Zeiger auf ein Byte Array mit den Verschlüsselungsdaten
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD LPCWSTR Identifikator des Besitzers
* @paramD Int16 Symbol Methode Symbol
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array with the encryption data
* @paramE UInt32 maxLen maximum size of the array in bytes
* @paramE LPCWSTR Owner Identifier
* @paramE Int16 Symbol Method symbol
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddEncryptionW(BYTE *arr, u32 maxLen, LPCWSTR URL, short Symbol)
{
	CID3F_ENCR *f = new CID3F_ENCR(getValidPointer(URL), (BYTE)Symbol);
	f->setData(arr, maxLen);
	return b2s(id3v2.replaceFrame(f));
}

/**
* Methode: ID3V2GetEncryptionSymbolW
* @link ID3V2
* @frame ENCR
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Verschlüsselungs-Symbol 
* Beschreibung english: get the Method Symbol
* @paramD Int16 Index von 1 bis Anzahl Encryption Frames
* @returnD Int16 Symbol oder -1 falls nicht gefunden 
* @paramE Int16 Index from 1 to encryption frame count
* @returnE Int16 Symbol or -1 if not found
*/
extern "C" short __stdcall ID3V2GetEncryptionSymbolW(short Index) 
{
	id3frame = id3v2.findFrame(F_ENCR, Index);
	return (id3frame != NULL) ? cENCR(id3frame)->getSymbol() : -1;
}

/**
* Methode: ID3V2GetEncryptionURLW
* @link ID3V2
* @frame ENCR
* @since 2.0.1.0
* Beschreibung deutsch: liefert die URL des Verschlüsselers
* Beschreibung english: get the encryption url
* @paramD Int16 Index von 1 bis Anzahl Encryption Frames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to encryption frame count
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetEncryptionURLW(short Index) 
{
	id3frame = id3v2.findFrame(F_ENCR, Index);
	return (id3frame != NULL) ? cENCR(id3frame)->getOwner().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetEncryptionDataW
* @link ID3V2
* @frame ENCR
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Verschlüsselungsdaten  
* Beschreibung english: get the encryption data
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD Int16 Index von 1 bis Anzahl Encryption Frames
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @paramE Int16 Index from 1 to encryption frame count
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetEncryptionDataW(BYTE *arr, u32 maxLen, short Index)
{
	id3frame = id3v2.findFrame(F_ENCR, Index);
	return (id3frame != NULL) ? cENCR(id3frame)->getData(arr, maxLen) : -1;	
}
// EQUA
/**
* Methode: ID3V2AddEqualisationW
* @link ID3V2
* @frame EQUA
* @since 2.0.1.0
* Beschreibung deutsch: EQUA fügt einen Equalisator frame hinzu<br />Es kann mehr als einen 'EQUA' Eintrag geben, aber nur einen mit dem gleichen Identifikator.
* Beschreibung english: EQUA add an equalization frame<br />There may be more than one 'EQUA' frame in each tag, but only one with the same identification string.
* @paramD Pointer Zeiger auf ein Byte Array mit den Daten
* @paramD UInt32 Grösse des Arrays in Bytes
* @paramD Byte Interpolation Methode 0=Band (no interpolation) 1=Linear
* @paramD LPCWSTR Identification Identifizierer
* @returnD Int16-1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array with the equalization data
* @paramE UInt32 size of the array in bytes
* @paramE Byte Interpolationmethod 0=Band (no interpolation) 1=Linear
* @paramE LPCWSTR Identification
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddEqualisationW(BYTE *arr, u32 maxLen, BYTE Interpolationmethod, LPCWSTR Identification)
{
	CID3F_EQUA *f = new CID3F_EQUA(Interpolationmethod, getValidPointer(Identification));
	f->setData(arr, maxLen);
	return b2s(id3v2.replaceFrame(f));
}
/**
* Methode: ID3V2GetEqualisationInterpolationW
* @link ID3V2
* @frame EQUA
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Interpolationsart 0=Band (no interpolation) 1=Linear
* Beschreibung english: get the interpolation value 0=Band (no interpolation) 1=Linear
* @paramD Int16 Index von 1 bis Anzahl Equalisation Frames
* @returnD Int16 Wert oder -1 falls Fehler
* @paramE Int16 Index from 1 to Equalisation frame count
* @returnE Int16 value or -1 if error
*/
extern "C" short __stdcall ID3V2GetEqualisationInterpolationW(short Index) 
{
	id3frame = id3v2.findFrame(F_EQUA, Index);
	return (id3frame != NULL) ? cEQUA(id3frame)->getFormat() : -1;
}
/**
* Methode: ID3V2GetEqualisationAdjustmentBitsW
* @link ID3V2
* @frame EQUA
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Anzahl der Bits für die Anpassung
* Beschreibung english: get the number of bits used for representation of the adjustment
* @paramD Int16 Index von 1 bis Anzahl Equalisation Frames
* @returnD Int16 Wert oder -1 falls Fehler
* @paramE Int16 Index from 1 to Equalisation frame count
* @returnE Int16 value or -1 if error
*/
extern "C" short __stdcall ID3V2GetEqualisationAdjustmentBitsW(short Index) 
{
	id3frame = id3v2.findFrame(F_EQUA, Index);
	return (id3frame != NULL) ? cEQUA(id3frame)->getAdjustmentBits() : -1;
}
/**
* Methode: ID3V2GetEqualisationIdentificationW
* @link ID3V2
* @frame EQUA
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Identificator
* Beschreibung english: get the Identificator
* @paramD Int16 Index von 1 bis Anzahl Equalisation Frames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to Equalisation frame count
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetEqualisationIdentificationW(short Index) 
{
	id3frame = id3v2.findFrame(F_EQUA, Index);
	return (id3frame != NULL) ? cEQUA(id3frame)->getIdentification().AllocSysString() : CTools::instance().GetEmptyBSTR();
}
/**
* Methode: ID3V2GetEqualisationDataW
* @link ID3V2
* @frame EQUA
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Daten-Array mit den Equalizerwerten 
* Beschreibung english: get the data array with the equalization points
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD Int16 Index von 1 bis Anzahl Equalisation Frames
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @paramE Int16 Index from 1 to Equalisation frame count
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetEqualisationDataW(BYTE *arr, u32 maxLen, short Index)
{
	id3frame = id3v2.findFrame(F_EQUA, Index);
	return (id3frame != NULL) ? cEQUA(id3frame)->getData(arr, maxLen) : -1;	
}

// ETCO
/**
* Methode: ID3V2AddEventTimingCodesW
* @link ID3V2
* @frame ETCO
* @since 2.0.1.0
* Beschreibung deutsch: ETCO fügt einen  Event Timing code frame hinzu<br />Es darf nur ein 'ETCO' Eintrag vorkommen.
* Beschreibung english: ETCO add an event timing code frame<br /><br \There may only be one 'ETCO' frame in each tag.
* @paramD Pointer Zeiger auf ein Byte Array mit den Event Codes
* @paramD UInt32 Grösse des Arrays in Bytes
* @paramD Byte Zeitformat 1=als Frames  2=als Millisekunden
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array with the event codes
* @paramE UInt32 size of the array in bytes with the event codes
* @paramE Byte TimestampFormat 1=frames as unit  2=milliseconds as unit
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddEventTimingCodesW(BYTE *arr, u32 maxLen, BYTE TimestampFormat)
{
	CID3F_ETCO *f = new CID3F_ETCO(TimestampFormat);
	f->setData(arr, maxLen);
	return b2s(id3v2.replaceFrame(f));
}
/**
* Methode: ID3V2GetEventTimingCodesTimestampFormatW
* @link ID3V2
* @frame ETCO
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Zeitformat  1=als Frames  2=als Millisekunden
* Beschreibung english: get the timestamp format  1=frames as unit  2=milliseconds as unit
* @returnD Int16 Wert oder -1 falls Fehler
* @returnE Int16 value or -1 if error
*/
extern "C" short __stdcall ID3V2GetEventTimingCodesTimestampFormatW() 
{
	id3frame = id3v2.findFrame(F_ETCO);
	return (id3frame != NULL) ? cETCO(id3frame)->getFormat() :-1;
}
/**
* Methode: ID3V2GetEventTimingCodesDataW
* @link ID3V2
* @frame ETCO
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Event Daten
* Beschreibung english: get the event datas
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetEventTimingCodesDataW(BYTE *arr, u32 maxLen)
{
	id3frame = id3v2.findFrame(F_ETCO);
	return (id3frame != NULL) ? cETCO(id3frame)->getData(arr, maxLen) : -1;	
}


// GEOB
/**
* Methode: ID3V2AddGeneralObjectW
* @link ID3V2
* @frame GEOB
* @since 2.0.1.0
* Beschreibung deutsch: GEOB fügt einen gekapselten Frame hinzu<br />Es kann mehr als einen 'GEOB' Eintrag geben, aber nur einen mit der gleichen Beschreibung.
* Beschreibung english: GEOB add a general encapsulated object<br />There may be more than one 'GEOB' frame in each tag, but only one with the same content descriptor.
* @paramD Pointer Zeiger auf ein Byte Array mit den Objektdaten
* @paramD UInt32 Grösse des Arrays in Bytes
* @paramD LPCWSTR Mime der Mime Typ des Objektes
* @paramD LPCWSTR FileName der Dateiname
* @paramD LPCWSTR Description Beschreibung
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array with the object datas
* @paramE UInt32 size of the array in bytes
* @paramE LPCWSTR Mime Type of the object
* @paramE LPCWSTR FileName 
* @paramE LPCWSTR Content descriptor
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddGeneralObjectW(BYTE *arr, u32 maxLen, LPCWSTR Mime, LPCWSTR FileName, LPCWSTR Description)
{
	CID3F_GEOB *f = new CID3F_GEOB(getValidPointer(Mime), getValidPointer(FileName), getValidPointer(Description));
	f->setData(arr, maxLen);
	return b2s(id3v2.replaceFrame(f));
}

/**
* Methode: ID3V2GetGeneralObjectMimeW
* @link ID3V2
* @frame GEOB
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Mime Typ des Objektes 
* @paramD Int16 Index von 1 bis Anzahl GeneralObject Frames
* @returnD BSTR der Textstring
* Beschreibung english: get the mime type of the object
* @paramE Int16 Index from 1 to GeneralObject frames
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetGeneralObjectMimeW(short Index) 
{
	id3frame = id3v2.findFrame(F_GEOB, Index);
	return (id3frame != NULL) ? cGEOB(id3frame)->getMime().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetGeneralObjectFilenameW
* @link ID3V2
* @frame GEOB
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Dateinamen 
* Beschreibung english: get the filename
* @paramD Int16 Index von 1 bis Anzahl GeneralObject Frames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to GeneralObject frames
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetGeneralObjectFilenameW(short Index) 
{
	id3frame = id3v2.findFrame(F_GEOB, Index);
	return (id3frame != NULL) ? cGEOB(id3frame)->getFilename().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetGeneralObjectDescriptionW
* @link ID3V2
* @frame GEOB
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Beschreibung
* Beschreibung english: get the description of the general object
* @paramD Int16 Index von 1 bis Anzahl GeneralObject Frames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to GeneralObject frames
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetGeneralObjectDescriptionW(short Index) 
{
	id3frame = id3v2.findFrame(F_GEOB, Index);
	return (id3frame != NULL) ? cGEOB(id3frame)->getDescription().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetGeneralObjectDataW
* @link ID3V2
* @frame GEOB
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Objektdaten 
* Beschreibung english: get the object datas
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maximale Grösse des Arrays in Bytes
* @paramD Int16 Index von 1 bis Anzahl GeneralObject Frames
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @paramE Int16 Index from 1 to GeneralObject frames
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetGeneralObjectDataW(BYTE *arr, u32 maxLen, short Index)
{
	id3frame = id3v2.findFrame(F_GEOB, Index);
	return (id3frame != NULL) ? cGEOB(id3frame)->getData(arr, maxLen) : -1;	
}
// GRID
/**
* Methode: ID3V2AddGroupIdentificationW
* @link ID3V2
* @frame GRID
* @since 2.0.1.0
* Beschreibung deutsch: GRID fügt ein Identifikator hinzu<br />Es kann mehr als einen 'GRID' Eintrag geben, aber nur einen mit dem gleichen Identifikator und Symbol.
* Beschreibung english: add a GRID frame<br />There may be more than one 'GRID' frame in each tag, but only one with the same owner identifier and group symbol.
* @paramD Pointer Zeiger auf ein Byte Array mit den Daten
* @paramD UInt32 Grösse des Arrays in Bytes
* @paramD LPCWSTR Eigentümer Identifikator
* @paramD Byte symbol zwischen 128 und 240
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 size of the array in bytes
* @paramE LPCWSTR Owner identifier
* @paramE Byte group symbol in the range 128 - 240
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddGroupIdentificationW(BYTE *arr, u32 length, LPCWSTR Url, BYTE symbol)
{
	CID3F_GRID *f = new CID3F_GRID(getValidPointer(Url), symbol);
	f->setData(arr, length);
	return b2s(id3v2.replaceFrame(f));
}

/**
* Methode: ID3V2GetGroupIdentificationURLW
* @link ID3V2
* @frame GRID
* @since 2.0.1.0
* Beschreibung deutsch: liefert die GroupIdentification URL
* Beschreibung english: get the group identification url
* @paramD Int16 Index von 1 bis Anzahl GroupIdentification Frames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to GroupIdentification frames
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetGroupIdentificationURLW(short Index) 
{
	id3frame = id3v2.findFrame(F_GRID, Index);
	return (id3frame != NULL) ? cGRID(id3frame)->getURL().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetGroupIdentificationSymbolW
* @link ID3V2
* @frame GRID
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Group Identification Symbol 
* Beschreibung english: get the group identifiation symbol
* @paramD Int16 Index von 1 bis Anzahl GroupIdentification Frames
* @returnD Int16 Wert oder -1 falls Fehler
* @paramE Int16 Index from 1 to GroupIdentification frames
* @returnE Int16 value or -1 if error
*/
extern "C" short __stdcall ID3V2GetGroupIdentificationSymbolW(short Index) 
{
	id3frame = id3v2.findFrame(F_GRID, Index);
	return (id3frame != NULL) ? cGRID(id3frame)->getSymbol() : -1;
}

/**
* Methode: ID3V2GetGroupIdentificationDataW
* @link ID3V2
* @frame GRID
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Daten des Group Identification Frames  
* Beschreibung english: get the data of the group identification frame
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD Int16 Index von 1 bis Anzahl GroupIdentification Frames
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @paramE Int16 Index from 1 to GroupIdentification frames
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetGroupIdentificationDataW(BYTE *arr, u32 maxLen, short Index)
{
	id3frame = id3v2.findFrame(F_GRID, Index);
	return (id3frame != NULL) ? cGRID(id3frame)->getData(arr, maxLen) : -1;	
}
// LINK
/**
* Methode: ID3V2AddLinkedInformationW
* @link ID3V2
* @frame LINK
* @since 2.0.1.0
* Beschreibung deutsch: LINK fügt einen LINK-Frame hinzu<br />Es kann mehr als einen 'LINK' Eintrag geben, aber keine zwei gleichen.
* Beschreibung english: LINK add a LINK frame<br />There may be more than one 'LINK' frame in each tag, but only one with the same contents.
* @paramD Int32 FrameIdentifier die ID des zu verlinkenden Frames
* @paramD LPCWSTR URL die URL zu der Datei mit dem Frame
* @paramD LPCWSTR additionalData zusätzliche Daten
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Int32 FrameIdentifier the ID of the linked frame
* @paramE LPCWSTR URL the url to the file with the frame
* @paramE LPCWSTR additionalData additional Data
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddLinkedInformationW(long FrameIdentifier, LPCWSTR URL, LPCWSTR additionalData)
{
	return b2s(id3v2.replaceFrame(new CID3F_LINK(FrameIdentifier, getValidPointer(URL), getValidPointer(additionalData))));	
}
/**
* Methode: ID3V2GetLinkedInformationFrameIdentifierW
* @link ID3V2
* @frame LINK
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Frame ID des verlinkten Frames
* Beschreibung english: get the frame id of the linked frame
* @paramD Int16 Index von 1 bis LinkedInformation Frames
* @returnD Int32 Wert oder -1 falls Fehler
* @paramE Int16 Index from 1 to LinkedInformation frame count
* @returnE Int32 value or -1 if error
*/
extern "C" long __stdcall ID3V2GetLinkedInformationFrameIdentifierW(short Index) 
{
	id3frame = id3v2.findFrame(F_LINK, Index);
	return (id3frame != NULL) ? cLINK(id3frame)->getIdentifier() : -1;
}
/**
* Methode: ID3V2GetLinkedInformationURLW
* @link ID3V2
* @frame LINK
* @since 2.0.1.0
* Beschreibung deutsch: liefert die URL zu dem verlinkten Frame
* Beschreibung english: get the url to the linked frame
* @paramD Int16 Index von 1 bis LinkedInformation Frames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to LinkedInformation frame count
* @returnE BSTR the text string with the url
*/
extern "C" BSTR __stdcall ID3V2GetLinkedInformationURLW(short Index) 
{
	id3frame = id3v2.findFrame(F_LINK, Index);
	return (id3frame != NULL) ? cLINK(id3frame)->getURL().AllocSysString() : CTools::instance().GetEmptyBSTR();
}
/**
* Methode: ID3V2GetLinkedInformationAdditionalDataW
* @link ID3V2
* @frame LINK
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Zusatzdaten zu dem verlinkten Frame
* Beschreibung english: get the additional data of the linked frame
* @paramD Int16 Index von 1 bis LinkedInformation Frames 
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to LinkedInformation frame count
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetLinkedInformationAdditionalDataW(short Index) 
{
	id3frame = id3v2.findFrame(F_LINK, Index);
	return (id3frame != NULL) ? cLINK(id3frame)->getAdditional().AllocSysString() : CTools::instance().GetEmptyBSTR();
}
// MCDI
/**
* Methode: ID3V2AddMusicCdIdentifierW
* @link ID3V2
* @frame MCDI
* @since 2.0.1.0
* Beschreibung deutsch: MCDI fügt einen Music CD Identifier frame hinzu<br />Es kann nur einen 'MCDI' Eintrag geben
* Beschreibung english: MCDI add a Music CD Identifier frame<br />There may only be one 'MCDI' frame in each tag
* @paramD Pointer Zeiger auf ein Byte Array mit dem CD TOC
* @paramD UInt32 Grösse des Arrays in Bytes 
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array with the CD TOC
* @paramE UInt32 size of the array
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddMusicCdIdentifierW(BYTE *arr, u32 length)
{
	CID3F_MCDI *f = new CID3F_MCDI();
	f->setData(arr, length);
	return b2s(id3v2.replaceFrame(f));
}
/**
* Methode: ID3V2GetMusicCdIdentifierDataW
* @link ID3V2
* @frame MCDI
* @since 2.0.1.0
* Beschreibung deutsch: liefert die CD TOC Daten 
* Beschreibung english: get the CD TOC data
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetMusicCdIdentifierDataW(BYTE *arr, u32 maxLen)
{
	id3frame = id3v2.findFrame(F_MCDI);
	return (id3frame != NULL) ? cMCDI(id3frame)->getData(arr, maxLen) : -1;	
}
// MLLT
/**
* Methode: ID3V2AddMpegLocationLookupTableW
* @link ID3V2
* @frame MLLT
* @since 2.0.1.0
* Beschreibung deutsch: MLLT fügt einen Mpeg Location Lookuptable Frame hinzu<br />Es kann nur einen 'MLLT' Eintrag geben.
* Beschreibung english: MLLT add a Mpeg Location Lookuptable frame<br />There may only be one 'MLLT' frame in each tag.
* @paramD Pointer Zeiger auf ein Byte Array mit dne Abweichungen in bytes und millisekunden
* @paramD UInt32 Grösse des Arrays
* @paramD Int32 Frames frames zwischen Referenzen
* @paramD Int32 Bytes Bytes zwischen Referenzen
* @paramD Int32 Milliseconds millisekunden zwischen Referenzen
* @paramD Byte BytesDeviation bits der Byte-Abweichung
* @paramD Byte MillisecondsDeviation Millisekunden der Byte-Abweichung
* @returnD Int16-1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array with the deviation in bytes and milliseconds
* @paramE UInt32 size of the array
* @paramE Int32 Frames mpeg frames between reference
* @paramE Int32 Bytes Bytes between reference
* @paramE Int32 Milliseconds milliseconds between reference
* @paramE Byte BytesDeviation bits for bytes deviation
* @paramE Byte MillisecondsDeviation bits for milliseconds deviation
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddMpegLocationLookupTableW(BYTE *arr, u32 length, long Frames, long Bytes, long Milliseconds, BYTE BytesDeviation, BYTE MillisecondsDeviation)
{
	CID3F_MLLT *f = new CID3F_MLLT(Frames, Bytes, Milliseconds, BytesDeviation, MillisecondsDeviation);
	f->setData(arr, length);
	return b2s(id3v2.replaceFrame(f));
}
/**
* Methode: ID3V2GetMpegLocationLookupTableDataW
* @link ID3V2
* @frame MLLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Daten eines MLLT Frames
* Beschreibung english: get the data of an MLLT Frame
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetMpegLocationLookupTableDataW(BYTE *arr, u32 maxLen)
{
	id3frame = id3v2.findFrame(F_MLLT);
	return (id3frame != NULL) ? cMLLT(id3frame)->getData(arr, maxLen) : -1;	
}
/**
* Methode: ID3V2GetMpegLocationLookupTableFramesW
* @link ID3V2
* @frame MLLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert die MPEG Frames
* Beschreibung english: get the mpeg frames 
* @returnD Int32 Wert oder -1 falls Fehler
* @returnE Int32 value or -1 if error
*/
extern "C" long __stdcall ID3V2GetMpegLocationLookupTableFramesW()
{
	id3frame = id3v2.findFrame(F_MLLT);
	return (id3frame != NULL) ? cMLLT(id3frame)->getFrames() : -1;	
}
/**
* Methode: ID3V2GetMpegLocationLookupTableBytesW
* @link ID3V2
* @frame MLLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Bytes zwischen Referenzen 
* Beschreibung english: get the bytes between reference
* @returnD Int32 Wert oder -1 falls Fehler
* @returnE Int32 value or -1 if error
*/
extern "C" long __stdcall ID3V2GetMpegLocationLookupTableBytesW()
{
	id3frame = id3v2.findFrame(F_MLLT);
	return (id3frame != NULL) ? cMLLT(id3frame)->getBytes() : -1;	
}
/**
* Methode: ID3V2GetMpegLocationLookupTableMillisecondsW
* @link ID3V2
* @frame MLLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Millisekunden zwischen Referenzen
* Beschreibung english: get the milliseconds between reference
* @returnD Int32 Wert oder -1 falls Fehler
* @returnE Int32 value or -1 if error
*/
extern "C" long __stdcall ID3V2GetMpegLocationLookupTableMillisecondsW()
{
	id3frame = id3v2.findFrame(F_MLLT);
	return (id3frame != NULL) ? cMLLT(id3frame)->getMillis() : -1;	
}
/**
* Methode: ID3V2GetMpegLocationLookupTableBytesDeviationW
* @link ID3V2
* @frame MLLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert die bits der byte Abweichung
* Beschreibung english: get the bits for byte deviation
* @returnD Int32 Wert oder -1 falls Fehler
* @returnE Int32 value or -1 if error
*/
extern "C" long __stdcall ID3V2GetMpegLocationLookupTableBytesDeviationW()
{
	id3frame = id3v2.findFrame(F_MLLT);
	return (id3frame != NULL) ? cMLLT(id3frame)->getBdev() : -1;	
}
/**
* Methode: ID3V2GetMpegLocationLookupTableMillisecondsDeviationW
* @link ID3V2
* @frame MLLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Bits der Millisekunden Abweichung
* Beschreibung english: get the bits for milliseconds deviation
* @returnD Int32 Wert oder -1 falls Fehler
* @returnE Int32 value or -1 if error
*/
extern "C" long __stdcall ID3V2GetMpegLocationLookupTableMillisecondsDeviationW()
{
	id3frame = id3v2.findFrame(F_MLLT);
	return (id3frame != NULL) ? cMLLT(id3frame)->getMdev() : -1;	
}

// OWNE
/**
* Methode: ID3V2AddOwnershipW
* @link ID3V2
* @frame OWNE
* @since 2.0.1.0
* Beschreibung deutsch: OWNE fügt einen Eigentümer frame hinzu<br />Es kann nur einen 'OWNE' Eintrag geben.
* Beschreibung english: OWNE add an Ownership frame<br />There may be only one 'OWNE' frame in each tag.
* @paramD LPCWSTR Price der bezahlte Preis
* @paramD LPCWSTR dateString Datum des Verkaufs
* @paramD LPCWSTR Seller Verkäufer
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE LPCWSTR Price the price paid
* @paramE LPCWSTR dateString date of purchase
* @paramE LPCWSTR Seller the seller
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddOwnershipW(LPCWSTR Price, LPCWSTR dateString, LPCWSTR Seller)
{
	return b2s(id3v2.replaceFrame(new CID3F_OWNE(getValidPointer(Price), getValidPointer(dateString), getValidPointer(Seller))));	
}

/**
* Methode: ID3V2GetOwnershipPriceW
* @link ID3V2
* @frame OWNE
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Preis
* Beschreibung english: get the price paid
* @returnD BSTR der Textstring
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetOwnershipPriceW() 
{
	id3frame = id3v2.findFrame(F_OWNE);
	return (id3frame != NULL) ? cOWNE(id3frame)->getPrice().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetOwnershipDateW
* @link ID3V2
* @frame OWNE
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Datum des Verkaufs 
* Beschreibung english: get the date of purchase
* @returnD BSTR der Textstring
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetOwnershipDateW() 
{
	id3frame = id3v2.findFrame(F_OWNE);
	return (id3frame != NULL) ? cOWNE(id3frame)->getDate().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetOwnershipSellerW
* @link ID3V2
* @frame OWNE
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Verkäufernamen
* Beschreibung english: get the seller
* @returnD BSTR der Textstring
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetOwnershipSellerW() 
{
	id3frame = id3v2.findFrame(F_OWNE);
	return (id3frame != NULL) ? cOWNE(id3frame)->getSeller().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

// POSS
/**
* Methode: ID3V2AddPositionSynchronisationW
* @link ID3V2
* @frame POSS
* @since 2.0.1.0
* Beschreibung deutsch: fügt einen Position Synchron frame hinzu<br />Es kann nur einen 'POSS' Eintrag geben.
* Beschreibung english: add a position synchron frame<br />There may be only one 'POSS' frame in each tag.
* @paramD Byte TimestampFormat 1=mpeg frames als Einheit  2=millisekunden als Einheit
* @paramD Int32 Position die Position
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Byte TimestampFormat 1=mpeg frames as unit  2=milliseconds as unit
* @paramE Int32 Position the position where the listener starts to receive
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddPositionSynchronisationW(BYTE TimestampFormat, long Position)
{
	return b2s(id3v2.replaceFrame(new CID3F_POSS(TimestampFormat, Position)));	
}
/**
* Methode: ID3V2GetPositionSynchronisationTimestampFormatW
* @link ID3V2
* @frame POSS
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Format der Zeitangaben,  1=mpeg frames als Einheit  2=millisekunden als Einheit
* Beschreibung english: get the timestamp format, 1=mpeg frames as unit  2=milliseconds as unit
* @returnD Int16 Wert oder -1 falls Fehler
* @returnE Int16 value or -1 if error
*/
extern "C" short __stdcall ID3V2GetPositionSynchronisationTimestampFormatW()
{
	id3frame = id3v2.findFrame(F_POSS);
	return (id3frame != NULL) ? cPOSS(id3frame)->getFormat() : -1;	
}
/**
* Methode: ID3V2GetPositionSynchronisationValueW
* @link ID3V2
* @frame POSS
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Positionsangabe
* Beschreibung english: get the position
* @returnD Int32 Wert oder -1 falls Fehler
* @returnE Int32 value or -1 if error
*/
extern "C" long __stdcall ID3V2GetPositionSynchronisationValueW()
{
	id3frame = id3v2.findFrame(F_POSS);
	return (id3frame != NULL) ? cPOSS(id3frame)->getPosition() : -1;	
}

// PRIV
/**
* Methode: ID3V2AddPrivateFrameW
* @link ID3V2
* @frame PRIV
* @since 2.0.1.0
* Beschreibung deutsch: fügt einen Private frame hinzu<br />Es kann mehr als einen 'PRIV' Eintrag geben, aber nur einen mit dem gleichen Identifikator 
* Beschreibung english: add a private frame<br />There may be more than one 'PRIV' frame in each tag, but only one with the same owner identifier.
* @paramD Pointer Zeiger auf ein Byte Array mit den Daten
* @paramD UInt32 die Grösse des Arrays in bytes
* @paramD LPCWSTR der Besitzer Identifikator
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 length size of array in bytes
* @paramE LPCWSTR owner identifier
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddPrivateFrameW(BYTE *arr, u32 length, LPCWSTR URL)
{
	CID3F_PRIV *f = new CID3F_PRIV(getValidPointer(URL));
	f->setData(arr, length);
	return b2s(id3v2.replaceFrame(f));
}

/**
* Methode: ID3V2GetPrivateFrameURLW
* @link ID3V2
* @frame PRIV
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Besitzer-ID
* Beschreibung english: get the owner identifier
* @paramD Int16 Index von 1 bis PrivateFrame Anzahl
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to Private Frame count
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetPrivateFrameURLW(short Index) 
{
	id3frame = id3v2.findFrame(F_PRIV, Index);
	return (id3frame != NULL) ? cPRIV(id3frame)->getURL().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetPrivateFrameDataW
* @link ID3V2
* @frame PRIV
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Daten des Private Frame 
* Beschreibung english: get the data of the private frame
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD Int16 Index von 1 bis PrivateFrame Anzahl
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @paramE Int16 Index from 1 to Private Frame count
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetPrivateFrameDataW(BYTE *arr, u32 maxLen, short Index)
{
	id3frame = id3v2.findFrame(F_PRIV, Index);
	return (id3frame != NULL) ? cPRIV(id3frame)->getData(arr, maxLen) : -1;	
}

// RBUF
/**
* Methode: ID3V2AddRecommendedBufferSizeW
* @link ID3V2
* @frame RBUF
* @since 2.0.1.0
* Beschreibung deutsch: fügt einen RecommendedBufferFrame hinzu<br />Es kann nur einen 'RBUF' Eintrag geben.
* Beschreibung english: add a recommended buffer frame<br />There may be only one 'RBUF' frame in each tag.
* @paramD Int32 BufferSize die Grösse des Puffers
* @paramD Byte EmbeddedInfoFlag 0=false 1=true
* @paramD Int32 Offset offset zum nächsten tag
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Int32 BufferSize the buffersize
* @paramE Byte EmbeddedInfoFlag 0=false 1=true
* @paramE Int32 Offset offset to next tag
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddRecommendedBufferSizeW(long BufferSize, BYTE EmbeddedInfoFlag, long Offset)
{
	return b2s(id3v2.replaceFrame(new CID3F_RBUF(BufferSize, EmbeddedInfoFlag, Offset )));
}
/**
* Methode: ID3V2GetRecommendedBufferSizeValueW
* @link ID3V2
* @frame RBUF
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Grösse des Puffers
* Beschreibung english: get the buffer size
* @returnD Int32 Grösse des Puffers oder -1 bei Fehler
* @returnE Int32 buffer size or -1 on error
*/
extern "C" long __stdcall ID3V2GetRecommendedBufferSizeValueW()
{
	id3frame = id3v2.findFrame(F_RBUF);
	return (id3frame != NULL) ? cRBUF(id3frame)->getBufferSize() : -1;
}
/**
* Methode: ID3V2GetRecommendedBufferSizeFlagW
* @link ID3V2
* @frame RBUF
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Embedded info flag 0=false 1=true
* Beschreibung english: get the embedded info flag 0=false 1=true 
* @returnD Int16 0=false 1=true, bei Fehler -1
* @returnE Int16 0=false 1=true, -1 on error
*/
extern "C" short __stdcall ID3V2GetRecommendedBufferSizeFlagW()
{
	id3frame = id3v2.findFrame(F_RBUF);
	return (id3frame != NULL) ? cRBUF(id3frame)->getFlag() : -1;
}
/**
* Methode: ID3V2GetRecommendedBufferSizeOffsetW
* @link ID3V2
* @frame RBUF
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Offset zum nächsten tag
* Beschreibung english: get the offset to next tag
* @returnD Int32 Wert oder -1 falls Fehler
* @returnE Int32 value or -1 if error
*/
extern "C" long __stdcall ID3V2GetRecommendedBufferSizeOffsetW()
{
	id3frame = id3v2.findFrame(F_RBUF);
	return (id3frame != NULL) ? cRBUF(id3frame)->getOffset() : -1;
}
// RVAD
/**
* Methode: ID3V2AddRelativeVolumeAdjustmentW
* @link ID3V2
* @frame RVAD
* @since 2.0.1.0
* Beschreibung deutsch: fügt einen Relative Volume Adjustment frame hinzu<br />Es kann mehr als einen 'RVAD' Eintrag geben, aber nur einen mit dem gleichen Identifikator.
* Beschreibung english: add a relative volume adjustment frame<br />There may be more than one 'RVAD' frame in each tag, but only one with the same identification string.
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 length die Länge des Arrays in Bytes
* @paramD LPCWSTR Identifier der eindeutige Bezeichner
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 length the size of the array in bytes
* @paramE LPCWSTR Identifier the unique identifier
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddRelativeVolumeAdjustmentW(BYTE *arr, u32 length, LPCWSTR Identifier)
{
	CID3F_RVAD *f = new CID3F_RVAD(getValidPointer(Identifier));
	f->setData(arr, length);
	return b2s(id3v2.replaceFrame(f));
}

/** 
* Methode: ID3V2GetRelativeVolumeAdjustmentDataW
* @link ID3V2
* @frame RVAD
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Daten des Relative Volume Adjustment frames
* Beschreibung english: get the data from the relative volume adjustment frame
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLength die maximale Länge des Arrays in Bytes
* @paramD Int16 Index von 1 bis Anzahl Relative Volume Adjustment frames
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 length the size of the array in bytes
* @paramE Int16 Index from 1 to relative volume adjustment frame count
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetRelativeVolumeAdjustmentDataW(BYTE *arr, u32 maxLen, short Index)
{
	id3frame = id3v2.findFrame(F_RVAD, Index);
	return (id3frame != NULL) ? cRVAD(id3frame)->getData(arr, maxLen) : -1;
}

/**
* Methode: ID3V2GetRelativeVolumeAdjustmentIdentifierW
* @link ID3V2
* @frame RVAD
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Bezeichner des Relative Volume Adjustment frames
* Beschreibung english: get the identifier of the relative volume adjustment frame
* @paramD Int16 Index von 1 bis Anzahl Relative Volume Adjustment frames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to relative volume adjustment frame count
* @returnE BSTR der Textstring
*/
extern "C" BSTR __stdcall ID3V2GetRelativeVolumeAdjustmentIdentifierW(short Index) 
{
	id3frame = id3v2.findFrame(F_RVAD, Index);
	return (id3frame != NULL) ? cRVAD(id3frame)->getIdentification().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

// RVRB
/**
* Methode: ID3V2AddReverbW
* @link ID3V2
* @frame RVRB
* @since 2.0.1.0
* Beschreibung deutsch: fügt einen Reverb frame hinzu<br />Es kann nur einen 'RVRB' Eintrag geben.
* Beschreibung english: add a reverb frame<br />There may be only one 'RVRB' frame in each tag
* @paramD Int16 reverbLeft
* @paramD Int16 reverbRight
* @paramD Byte bouncesLeft
* @paramD Byte bouncesRight
* @paramD Byte feedbackLeftToLeft
* @paramD Byte feedbackLeftToRight
* @paramD Byte feedbackRightToRight
* @paramD Byte feedbackRightToLeft
* @paramD Byte premixLeftToRight
* @paramD Byte premixRightToLeft
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Int16 reverbLeft
* @paramE Int16 reverbRight
* @paramE Byte bouncesLeft
* @paramE Byte bouncesRight
* @paramE Byte feedbackLeftToLeft
* @paramE Byte feedbackLeftToRight
* @paramE Byte feedbackRightToRight
* @paramE Byte feedbackRightToLeft
* @paramE Byte premixLeftToRight
* @paramE Byte premixRightToLeft
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddReverbW(short reverbLeft, short reverbRight, BYTE bouncesLeft, BYTE bouncesRight, BYTE feedbackLeftToLeft, BYTE feedbackLeftToRight, BYTE feedbackRightToRight, BYTE feedbackRightToLeft, BYTE premixLeftToRight, BYTE premixRightToLeft)
{
	return b2s(id3v2.replaceFrame(new CID3F_RVRB(reverbLeft, reverbRight, bouncesLeft, bouncesRight, feedbackLeftToLeft, feedbackLeftToRight, feedbackRightToRight, feedbackRightToLeft,  premixLeftToRight, premixRightToLeft)));
}
/**
* Methode: ID3V2GetReverbLeftW
* @link ID3V2
* @frame RVRB
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Wert für Reverb Links
* Beschreibung english: get the value for reverb left
* @returnD Int16 reverb links in ms oder -1 bei Fehler
* @returnE Int16 reverb left in ms or -1 if error
*/
extern "C" short __stdcall ID3V2GetReverbLeftW()
{
	id3frame = id3v2.findFrame(F_RVRB);
	return (id3frame != NULL) ? cRVRB(id3frame)->getDatas(0) : -1;
}
/**
* Methode: ID3V2GetReverbRightW
* @link ID3V2
* @frame RVRB
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Wert für Reverb rechts
* Beschreibung english: get the value for reverb right
* @returnD Int16 reverb rechts in ms oder -1 bei Fehler
* @returnE Int16 reverb right in ms or -1 if error
*/
extern "C" short __stdcall ID3V2GetReverbRightW()
{
	id3frame = id3v2.findFrame(F_RVRB);
	return (id3frame != NULL) ? cRVRB(id3frame)->getDatas(1) : -1;
}
/**
* Methode: ID3V2GetReverbBouncesLeftW
* @link ID3V2
* @frame RVRB
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Wert für Reverb bounces links
* Beschreibung english: get the value for reverb bounces left
* @returnD Int16 reverb bounces links oder -1 bei Fehler
* @returnE Int16 reverb bounces left or -1 if error
*/
extern "C" short __stdcall ID3V2GetReverbBouncesLeftW()
{
	id3frame = id3v2.findFrame(F_RVRB);
	return (id3frame != NULL) ? cRVRB(id3frame)->getDatas(2) : -1;
}
/**
* Methode: ID3V2GetReverbBouncesRightW
* @link ID3V2
* @frame RVRB
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Wert für Reverb bounces rechts
* Beschreibung english: get the value for reverb bounces right
* @returnD Int16 reverb bounces rechts oder -1 bei Fehler
* @returnE Int16 reverb bounces right or -1 if error
*/
extern "C" short __stdcall ID3V2GetReverbBouncesRightW()
{
	id3frame = id3v2.findFrame(F_RVRB);
	return (id3frame != NULL) ? cRVRB(id3frame)->getDatas(3) : -1;
}
/**
* Methode: ID3V2GetReverbFeedbackLeftToLeftW
* @link ID3V2
* @frame RVRB
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Wert für Reverb feedback links zu links
* Beschreibung english: get the value for reverb feedback left to left
* @returnD Int16 reverb feedback links zu links oder -1 bei Fehler
* @returnE Int16 reverb feedback left to left or -1 if error
*/
extern "C" short __stdcall ID3V2GetReverbFeedbackLeftToLeftW()
{
	id3frame = id3v2.findFrame(F_RVRB);
	return (id3frame != NULL) ? cRVRB(id3frame)->getDatas(4) : -1;
}
/**
* Methode: ID3V2GetReverbFeedbackLeftToRightW
* @link ID3V2
* @frame RVRB
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Wert für Reverb feedback links zu rechts
* Beschreibung english: get the value for reverb feedback left to right
* @returnD Int16 reverb feedback links zu rechts oder -1 bei Fehler
* @returnE Int16 reverb feedback left to right or -1 if error
*/
extern "C" short __stdcall ID3V2GetReverbFeedbackLeftToRightW()
{
	id3frame = id3v2.findFrame(F_RVRB);
	return (id3frame != NULL) ? cRVRB(id3frame)->getDatas(5) : -1;
}
/**
* Methode: ID3V2GetReverbFeedbackRightToRightW
* @link ID3V2
* @frame RVRB
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Wert für Reverb feedback rechts zu rechts
* Beschreibung english: get the value for reverb feedback right to right
* @returnD Int16 reverb feedback rechts zu rechts oder -1 bei Fehler
* @returnE Int16 reverb feedback right to right or -1 if error
*/
extern "C" short __stdcall ID3V2GetReverbFeedbackRightToRightW()
{
	id3frame = id3v2.findFrame(F_RVRB);
	return (id3frame != NULL) ? cRVRB(id3frame)->getDatas(6) : -1;
}
/**
* Methode: ID3V2GetReverbFeedbackRightToLeftW
* @link ID3V2
* @frame RVRB
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Wert für Reverb feedback rechts zu links
* Beschreibung english: get the value for reverb feedback right to left
* @returnD Int16 reverb feedback rechts zu links oder -1 bei Fehler
* @returnE Int16 reverb feedback right to left or -1 if error
*/
extern "C" short __stdcall ID3V2GetReverbFeedbackRightToLeftW()
{
	id3frame = id3v2.findFrame(F_RVRB);
	return (id3frame != NULL) ? cRVRB(id3frame)->getDatas(7) : -1;
}
/**
* Methode: ID3V2GetReverbPremixLeftToRightW
* @link ID3V2
* @frame RVRB
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Wert für premix links zu rechts
* Beschreibung english: get the value for premix left to right
* @returnD Int16 premix links zu rechts oder -1 bei Fehler
* @returnE Int16 premix left to right or -1 if error
*/
extern "C" short __stdcall ID3V2GetReverbPremixLeftToRightW()
{
	id3frame = id3v2.findFrame(F_RVRB);
	return (id3frame != NULL) ? cRVRB(id3frame)->getDatas(8) : -1;
}
/**
* Methode: ID3V2GetReverbPremixRightToLeftW
* @link ID3V2
* @frame RVRB
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Wert für premix rechts zu links
* Beschreibung english: get the value for premix right to left
* @returnD Int16 premix rechts zu links oder -1 bei Fehler
* @returnE Int16 premix right to left or -1 if error
*/
extern "C" short __stdcall ID3V2GetReverbPremixRightToLeftW()
{
	id3frame = id3v2.findFrame(F_RVRB);
	return (id3frame != NULL) ? cRVRB(id3frame)->getDatas(9) : -1;
}

// SEEK
/**
* Methode: ID3V2AddSeekOffsetW
* @link ID3V2
* @frame SEEK
* @since 2.0.1.0
* Beschreibung deutsch: fügt einen Seek Frame hinzu<br />Es kann nur einen 'SEEK' Eintrag geben
* Beschreibung english: add a seek frame<br />There may only be one 'SEEK' frame in a tag.
* @paramD Int32 offset der Offset zum nächsten tag
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Int32 offset the offset to next tag
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddSeekOffsetW(long offset)
{
	return id3v2.replaceFrame(new CID3F_SEEK(offset));
}

/**
* Methode: ID3V2GetSeekOffsetW
* @link ID3V2
* @frame SEEK
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Offset zum nächsten tag 
* Beschreibung english: get the offset to next tag
* @returnD Int32 Wert oder -1 bei Fehler
* @returnE Int32 value or -1 if error
*/
extern "C" long __stdcall ID3V2GetSeekOffsetW()
{
	id3frame = id3v2.findFrame(F_SEEK);
	return (id3frame != NULL) ? cSEEK(id3frame)->getOffset() : -1;
}
// SIGN
/**
* Methode: ID3V2AddSignatureFrameW
* @link ID3V2
* @frame SIGN
* @since 2.0.1.0
* Beschreibung deutsch: fügt einen Signatur frame hinzu<br />Es kann mehr als einen 'SIGN' Eintrag geben, aber keine zwei mit dem gleichen Symbol.
* Beschreibung english: add a signature frame<br />There may be more than one 'SIGN' frame in a tag, but no two with the same group symbol
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 Grösse des Arrays in Bytes
* @paramD Int16 GroupSymbol Symbol
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 length of the array in bytes
* @paramE Int16 GroupSymbol symbol
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddSignatureFrameW(BYTE *arr, u32 length, short GroupSymbol)
{
	CID3F_SIGN *f = new CID3F_SIGN((BYTE)GroupSymbol);
	f->setData(arr, length);
	return b2s(id3v2.replaceFrame(f));
}

/**
* Methode: ID3V2GetSignatureFrameDataW
* @link ID3V2
* @frame SIGN
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Daten eines Signature Frames 
* Beschreibung english: get the data of a signature frame
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD Int16 Index von 1 bis Anzahl Siganture frames
* @returnD UInt32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @paramE Int16 Index from 1 to signature frame count
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetSignatureFrameDataW(BYTE *arr, u32 maxLen, short Index)
{
	id3frame = id3v2.findFrame(F_SIGN, Index);
	return (id3frame != NULL) ? cSIGN(id3frame)->getData(arr, maxLen) : -1;	
}

/**
* Methode: ID3V2GetSignatureFrameGroupSymbolW
* @link ID3V2
* @frame SIGN
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Symbol eines Signature frames
* Beschreibung english: get the symbol of a signature frame
* @paramD Int16 Index von 1 bis Anzahl Siganture frames
* @returnD Int16 Wert oder -1 bei Fehler
* @paramE Int16 Index from 1 to signature frame count
* @returnE Int16 Value or -1 if error
*/
extern "C" short __stdcall ID3V2GetSignatureFrameGroupSymbolW(short Index)
{
	id3frame = id3v2.findFrame(F_SIGN, Index);
	return (id3frame != NULL) ? cSIGN(id3frame)->getSymbol() : -1;	
}

// SYTC
/**
* Methode: ID3V2AddSynchronizedTempoW
* @link ID3V2
* @frame SYTC
* @since 2.0.1.0
* Beschreibung deutsch: setzt oder ersetzt einen Snchronized Tempo Frame<br />Es kann nur einen 'SYTC' Eintrag geben.
* Beschreibung english: set or replace a synchronized Tempo frame<br />There may only be one 'SYTC' frame in each tag.
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 Grösse des Arrays in Bytes
* @paramD Int16 Format Zeitformat 1=als frames  2= als millisekunden
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 size of array in bytes
* @paramE Int16 Format Timestamp format 1=frames as unit  2=milliseconds as unit
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddSynchronizedTempoW(BYTE *arr, u32 length, short Format)
{
	CID3F_SYTC *f = new CID3F_SYTC((BYTE)Format);
	f->setData(arr, length);
	return b2s(id3v2.replaceFrame(f));
}

/**
* Methode: ID3V2GetSynchronizedTempoDataW
* @link ID3V2
* @frame SYTC
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Daten eines Synchronized tempo frame
* Beschreibung english: get the data of a Synchronized tempo frame
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetSynchronizedTempoDataW(BYTE *arr, u32 maxLen)
{
	id3frame = id3v2.findFrame(F_SYTC);
	return (id3frame != NULL) ? cSYTC(id3frame)->getData(arr, maxLen) : -1;	
}

/**
* Methode: ID3V2GetSynchronizedTempoFormatW
* @link ID3V2
* @frame SYTC
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Zeitformat
* Beschreibung english: get the timestamp format
* @returnD Int16 1=in frames 2=in Millisekunden -1=Fehler
* @returnE Int16 1=frames as unit  2=milliseconds as unit  -1=error
*/
extern "C" short __stdcall ID3V2GetSynchronizedTempoFormatW()
{
	id3frame = id3v2.findFrame(F_SYTC);
	return (id3frame != NULL) ? cSYTC(id3frame)->getFormat() : -1;	
}

// UFID
/**
* Methode: ID3V2AddUniqueFileIdentifierW
* @link ID3V2
* @frame UFID
* @since 2.0.1.0
* Beschreibung deutsch: setzt oder ersetzt einen Unique File Identifier Frame<br />Es kann mehr als einen 'UFID' Eintrag geben, aber keine zwei mit dem gleichen Eigentümer.
* Beschreibung english: set or replace a unique file identifier frame<br />There may be more than one 'UFID' frame in a tag, but only one with the same Owner identifier.
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 Grösse des Arrays
* @paramD LPCWSTR Eigentümer
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 length the size of the array
* @paramE LPCWSTR Owner owner identifier
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddUniqueFileIdentifierW(BYTE *arr, u32 length, LPCWSTR Owner)
{
	CID3F_UFID *f = new CID3F_UFID(getValidPointer(Owner));
	f->setData(arr, length);
	return b2s(id3v2.replaceFrame(f));
}
/**
* Methode: ID3V2GetUniqueFileIdentifierOwnerW
* @link ID3V2
* @frame UFID
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Eigentümer des frames
* Beschreibung english: get the owner of the frame identifier
* @paramD Int16 Index von 1 bis Anzahl Unique File Identifier frames
* @returnD BSTR der Textstring
* @paramE Int16 Index from 1 to Unique File Identifier frame count
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall ID3V2GetUniqueFileIdentifierOwnerW(short Index)
{
	id3frame = id3v2.findFrame(F_UFID, Index);
	return (id3frame != NULL) ? cUFID(id3frame)->getOwner().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2GetUniqueFileIdentifierDataW
* @link ID3V2
* @frame UFID
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Daten des Unique File Identifier Frames 
* Beschreibung english: get the data of the unique file identifier frame
* @paramD Pointer Zeiger auf ein Byte Array
* @paramD UInt32 maxLen maximale Grösse des Arrays in Bytes
* @paramD Int16 Index von 1 bis Anzahl Unique File Identifier frames
* @returnD Int32 Grösse des tatsächlichen Arrays in Bytes oder -1 bei Fehler
* @paramE Pointer a pointer to the byte array
* @paramE UInt32 maxLen maximum size of the array in bytes
* @paramE Int16 Index from 1 to Unique File Identifier frame count
* @returnE Int32 the real length of the array in bytes or -1 if error
*/
extern "C" long __stdcall ID3V2GetUniqueFileIdentifierDataW(BYTE *arr, u32 maxLen, short Index)
{
	id3frame = id3v2.findFrame(F_UFID, Index);
	return (id3frame != NULL) ? cUFID(id3frame)->getData(arr, maxLen) : -1;	
}


/**
* Methode: ID3V2AddUserFrameW
* @link ID3V2
* @frame USER
* @since 2.0.1.0
* Beschreibung deutsch: "USER" setzt oder ersetzt einen benutzerdefinierten Eintrag<br />Es kann mehr als einen 'USER' Eintrag geben, aber keine zwei mit der gleichen Sprache.
* Beschreibung english: "USER" set a userdefined frame<br />There may be more than one 'USER' frame in a tag, but only one with the same Language.
* @paramD LPCWSTR language das Sprachkürzel des Eintrages ( genau 3 Bytes ) z.B. ENG
* @paramD LPCWSTR text der eigentliche benutzerdefinierte Eintrag
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE LPCWSTR Language ID of the entry, e.g. ENG for english ( must be 3 bytes long )
* @paramE LPCWSTR text the user frame
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddUserFrameW(LPCWSTR Language, LPCWSTR Text)
{
	return b2s(id3v2.replaceFrame(new CID3F_USER(getValidPointer(Text), getValidPointer(Language))));	
}


/**
* Methode: ID3V2GetUserFrameLanguageW
* @link ID3V2
* @frame USER
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Sprachkürzel eines benutzerdefinierten Frames
* Beschreibung english: get the language from a userdefined frame
* @paramD Int16 index Index von 1 bis Anzahl User Frames
* @returnD BSTR das Sprachkürzel des Frames, z.B. DEU für Deutsch
* @paramE Int16 index Index from 1 to user frame count
* @returnE BSTR the language id of the user frame, e.g. ENG for english
*/
extern "C" BSTR __stdcall ID3V2GetUserFrameLanguageW(short Index)
{
	id3frame = id3v2.findFrame(F_USER, Index);
	return (id3frame != NULL) ? cUSER(id3frame)->getLanguage().AllocSysString() : CTools::instance().GetEmptyBSTR();	
}


/**
* Methode: ID3V2GetUserFrameW
* @link ID3V2
* @frame USER
* @since 2.0.1.0
* Beschreibung deutsch: liefert einen benutzerdefinierten Text
* Beschreibung english: get a userdefined Frame
* @paramD Int16 index ein Index von 1 bis Anzahl User Frames
* @returnD BSTR der benutzerdefinierte Text
* @paramE Int16 index index from 1 to user frame count
* @returnE BSTR the userdefined text
*/
extern "C" BSTR __stdcall ID3V2GetUserFrameW(short Index)
{
	id3frame = id3v2.findFrame(F_USER, Index);
	return (id3frame != NULL) ? cUSER(id3frame)->getText().AllocSysString() : CTools::instance().GetEmptyBSTR();
}

/**
* Methode: ID3V2AddUserTextW
* @link ID3V2
* @frame TXXX
* @since 2.0.1.0
* Beschreibung deutsch: "TXXX" setzt oder ersetzt einen benutzerdefinierten Text-Eintrag<br />Es kann mehr als einen 'TXXX' Eintrag geben, aber keine zwei mit der gleichen Beschreibung.
* Beschreibung english: "TXXX" set a userdefined Text<br />There may be more than one 'TXXX' frame in each tag, but only one with the same description.
* @paramD LPCWSTR description die Beschreibung des Eintrages
* @paramD LPCWSTR text der eigentliche Text-Eintrag
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE LPCWSTR description the description of the entry
* @paramE LPCWSTR text the new text
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddUserTextW(LPCWSTR Description, LPCWSTR Text)
{
	return b2s(id3v2.replaceFrame(new CID3F_TXXX(getValidPointer(Description), getValidPointer(Text))));	
}



/**
* Methode: ID3V2GetUserTextDescriptionW
* @link ID3V2
* @frame TXXX
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Beschreibung eines benutzerdefinierten Textes
* Beschreibung english: get a Description from a userdefined Text
* @paramD Int16 index Index von 1 bis User Text frames
* @returnD BSTR die Beschreibung eines UserTextes
* @paramE Int16 index index from 1 to user text frame count
* @returnE BSTR the description of the user text
*/
extern "C" BSTR __stdcall ID3V2GetUserTextDescriptionW(short Index)
{
	id3frame = id3v2.findFrame(F_TXXX, Index);
	return (id3frame != NULL) ? cTXXX(id3frame)->getDescription().AllocSysString() : CTools::instance().GetEmptyBSTR();	
}


/**
* Methode: ID3V2GetUserTextW
* @link ID3V2
* @frame TXXX
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Text eines benutzerdefinierten Eintrages
* Beschreibung english: get the userdefined text
* @paramD Int16 index ein Index von 1 bis Anzahl User Text Frames
* @returnD BSTR Text des Eintrages
* @paramE Int16 index Index from 1 to user text frame count
* @returnE BSTR the userdefined text
*/
extern "C" BSTR __stdcall ID3V2GetUserTextW(short Index)
{
	id3frame = id3v2.findFrame(F_TXXX, Index);
	return (id3frame != NULL) ? cTXXX(id3frame)->getText().AllocSysString() : CTools::instance().GetEmptyBSTR();	
}


/**
* Methode: ID3V2AddUserURLW
* @link ID3V2
* @frame WXXX
* @since 2.0.1.0
* Beschreibung deutsch: "WXXX" setzt oder ersetzt einen benutzerdefinierten URL-Eintrag<br />Es kann mehr als einen 'WXXX' Eintrag geben, aber keine zwei mit der gleichen Beschreibung.
* Beschreibung english: "WXXX" set a userdefined url<br />There may be more than one 'WXXX' frame in each tag, but only one with the same description.
* @paramD LPCWSTR description die Beschreibung des Eintrages
* @paramD LPCWSTR text die eigentliche URL
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE LPCWSTR description the description of the entry
* @paramE LPCWSTR text the new url
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddUserURLW(LPCWSTR Description, LPCWSTR URL)
{
	return b2s(id3v2.replaceFrame(new CID3F_WXXX(getValidPointer(Description), getValidPointer(URL))));	
}


/**
* Methode: ID3V2GetUserURLDescriptionW
* @link ID3V2
* @frame WXXX
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Beschreibung eines URL-Eintrages
* Beschreibung english: get the description from a userdefined url
* @paramD Int16 index ein Index von 1 bis Anzahl User Url Frames
* @returnD BSTR Beschreibung der URL
* @paramE Int16 index index from 1 to user url frame count
* @returnE BSTR the description of the userdefined url
*/
extern "C" BSTR __stdcall ID3V2GetUserURLDescriptionW(short Index)
{
	id3frame = id3v2.findFrame(F_WXXX, Index);
	return (id3frame != NULL) ? cWXXX(id3frame)->getDescription().AllocSysString() : CTools::instance().GetEmptyBSTR();	
}


/**
* Methode: ID3V2GetUserURLW
* @link ID3V2
* @frame WXXX
* @since 2.0.1.0
* Beschreibung deutsch: liefert einen benutzerdefinierten URL-Eintrag
* Beschreibung english: get a userdefined url
* @paramD Int16 index ein Index von 1 bis Anzahl User Url Frames
* @returnD BSTR benutzerdefinierter URL-Eintrag
* @paramE Int16 index index from 0 to User Url frame count
* @returnE BSTR the userdefined url
*/
extern "C" BSTR __stdcall ID3V2GetUserURLW(short Index)
{
	id3frame = id3v2.findFrame(F_WXXX, Index);
	return (id3frame != NULL) ? cWXXX(id3frame)->getURL().AllocSysString() : CTools::instance().GetEmptyBSTR();	

}


/**
* Methode: ID3V2RemoveTagFromFileW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: entfernt den ID3v2-Tag aus Datei. Achtung: Die Löschung wird sofort durchgeführt!
* Beschreibung english: remove the ID3v2 tag from a file. Attention: This function removes the tag immediately!
* @paramD LPCWSTR FileName Name der Datei
* @returnD Int16 -1 wenn erfolgreich entfernt, ansonsten 0
* @paramE LPCWSTR fileName name of the file
* @returnE Int16 -1 if removed, 0 on error
*/
extern "C" short __stdcall ID3V2RemoveTagFromFileW(LPCWSTR FileName)
{
	if (id3v2.RemoveFromFile(getValidPointer(FileName)))
	{
		AUDIOAnalyzeFileW(getValidPointer(FileName));
		return b2s(true);
	}
	return b2s(false);	
}
/**
* Methode: ID3V2RemoveTagW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: entfernt den ID3v2-Tag aus der zuletzt analysierten Datei. Achtung: Die Löschung wird sofort durchgeführt!
* Beschreibung english: remove the ID3v2 tag from the last analyzed file. Attention: This function removes the tag immediately!
* @returnD Int16 -1 wenn erfolgreich entfernt, ansonsten 0
* @returnE Int16 -1 if removed, 0 on error
*/
extern "C" short __stdcall ID3V2RemoveTagW()
{
	if (id3v2.RemoveFromFile(lastFile))
	{
		return b2s(true);	
	}
	return b2s(false);  
}


/**
* Methode: ID3V2SaveChangesToFileW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: speichert den ID3v2-Tag in eine Datei
* Beschreibung english: store the ID3v2 tag in a file
* @paramD LPCWSTR FileName Name der Datei
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR FileName name of the file
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall ID3V2SaveChangesToFileW(LPCWSTR FileName)
{
	FileName = getValidPointer(FileName);	
	if (GetFormat(FileName) != AUDIO_FORMAT_INVALID)
		return b2s(id3v2.SaveToFile(FileName));
	CTools::instance().setLastError(ERR_TAG_NOT_ALLOWED);
	return b2s(false);   
}

/**
* Methode: ID3V2SaveChangesW
* @link ID3V2
* @since 2.0.1.0
* Beschreibung deutsch: speichert den ID3v2-Tag in die zuletzt analysierte Datei
* Beschreibung english: store the ID3v2 tag in the last analyzed file
* @returnD Int16 normalerweise -1, bei Fehler 0
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall ID3V2SaveChangesW()
{
	return ID3V2SaveChangesToFileW(lastFile);
}

/**
* Methode: ID3V2GetSyncLyricW
* @link ID3V2
* @frame SYLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Text eines synchronen Liedtext-Eintrages
* Beschreibung english: get the text from a synchronized lyrics
* @paramD Int16 index Index von 1 bis Anzahl Sync Lyric Frames
* @returnD BSTR der Liedtext Eintrag
* @paramE Int16 index index from 1 to sync lyric frame count
* @returnE BSTR the text value
*/
extern "C" BSTR __stdcall ID3V2GetSyncLyricW(short Index) 
{
	id3frame = id3v2.findFrame(F_SYLT, Index);
	return (id3frame != NULL) ? cSYLT(id3frame)->getText().AllocSysString() : CTools::instance().GetEmptyBSTR();	
}

/**
* Methode: ID3V2GetSyncLyricDescriptionW
* @link ID3V2
* @frame SYLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Beschreibung eines synchronen Liedtext-Eintrages
* Beschreibung english: get the description from a synchronized lyric
* @paramD Int16 index Index von 1 bis Anzahl Sync Lyric Frames
* @returnD BSTR Beschreibung eines synchronen Liedtext-Eintrages
* @paramE Int16 index index from 1 to sync lyric frame count
* @returnE BSTR the description of the synchronized lyric
*/
extern "C" BSTR __stdcall ID3V2GetSyncLyricDescriptionW(short Index) 
{
	id3frame = id3v2.findFrame(F_SYLT, Index);
	return (id3frame != NULL) ? cSYLT(id3frame)->getDescription().AllocSysString() : CTools::instance().GetEmptyBSTR();	  
}

/**
* Methode: ID3V2GetSyncLyricLanguageW
* @link ID3V2
* @frame SYLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Sprachkürzel eines synchronen Liedtext-Eintrages
* Beschreibung english: get the language from a synchronized lyrics
* @paramD Int16 index Index von 1 bis Anzahl Sync Lyric Frames
* @returnD BSTR Sprachkürzel des synchronen Eintrages, z.B. DEU für Deutsch
* @paramE Int16 index index from 1 to sync lyric frame count
* @returnE BSTR the language id of the synchronized lyric, e.g. ENG for english
*/
extern "C" BSTR __stdcall ID3V2GetSyncLyricLanguageW(short Index) 
{
	id3frame = id3v2.findFrame(F_SYLT, Index);
	return (id3frame != NULL) ? cSYLT(id3frame)->getLanguage().AllocSysString() : CTools::instance().GetEmptyBSTR();	 
}

/**
* Methode: ID3V2GetSyncLyricTimeFormatW
* @link ID3V2
* @frame SYLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert das Format der Zeitangabe eines synchronen Liedtext-Eintrages
* Beschreibung english: get the time stamp format from a synchronized lyrics
* @paramD Int16 index Index von 1 bis Anzahl Sync Lyric Frames
* @returnD Int16 Rückgabewert der Funktion mit folgender Bedeutung:<br /><br /><table width="280" class="tablelayout"><tr><th width="22%">Wert</th><th width="78%">Beschreibung</th></tr><tr><td width="22%">-1</td><td width="78%">ungültiger Eintrag</td></tr><tr><td width="22%">1</td><td width="78%">Zeitangaben in mpeg frames</td></tr><tr><td width="22%">2</td><td width="78%">Zeitangaben in millisekunden</td></tr></table>
* @paramE Int16 index index from 1 to sync lyric frame count
* @returnE Int16 return value with the following meaning:<br /><br /><table width="280" class="tablelayout"><tr><th width="22%">value</th><th width="78%">description</th></tr><tr><td width="22%">-1</td><td width="78%">invalid entry</td></tr><tr><td width="22%">1</td><td width="78%">mpeg frames as unit</td></tr><tr><td width="22%">2</td><td width="78%">milliseconds as unit</td></tr></table>
*/
extern "C" short __stdcall ID3V2GetSyncLyricTimeFormatW(short Index) 
{
	id3frame = id3v2.findFrame(F_SYLT, Index);
	return (id3frame != NULL) ? cSYLT(id3frame)->getTimestampFormat() : 0;	
}

/**
* Methode: ID3V2GetSyncLyricContentTypeW
* @link ID3V2
* @frame SYLT
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Inhaltstyp eines synchronen Liedtext-Eintrages
* Beschreibung english: get the content type from a synchronized lyrics
* @paramD Int16 index Index von 1 bis Anzahl Sync Lyrics Frames
* @returnD Int16 Rückgabewert der Funktion mit folgender Bedeutung:<br /><br /><table width="280" class="tablelayout"><tr><th width="22%">Wert</th><th width="78%">Beschreibung</th></tr><tr><td width="22%">-1</td><td width="78%">ungültiger Eintrag</td></tr><tr><td width="22%">0</td><td width="78%">Sonstiges</td></tr><tr><td width="22%">1</td><td width="78%">Songtexte</td></tr><tr><td width="22%">2</td><td width="78%">Textumschreibungen</td></tr><tr><td width="22%">3</td><td width="78%">Bewegungen</td></tr><tr><td width="22%">4</td><td width="78%">Ereignisse</td></tr><tr><td width="22%">5</td><td width="78%">Akkorde</td></tr><tr><td width="22%">6</td><td width="78%">Belangloses</td></tr><tr><td width="22%">7</td><td width="78%">URLs zu Webseiten</td></tr><tr><td width="22%">8</td><td width="78%">URLs zu Bildern</td></tr></table>
* @paramE Int16 index index from 1 to sync lyrics frame count
* @returnE Int16 return value with the following meaning:<br /><br /><table width="280" class="tablelayout"><tr><th width="22%">value</th><th width="78%">description</th></tr><tr><td width="22%">-1</td><td width="78%">invalid entry</td></tr><tr><td width="22%">0</td><td width="78%">other</td></tr><tr><td width="22%">1</td><td width="78%">lyrics</td></tr><tr><td width="22%">2</td><td width="78%">text transcription</td></tr><tr><td width="22%">3</td><td width="78%">movement/part name</td></tr><tr><td width="22%">4</td><td width="78%">events</td></tr><tr><td width="22%">5</td><td width="78%">chord</td></tr><tr><td width="22%">6</td><td width="78%">trivia/'pop up' information</td></tr><tr><td width="22%">7</td><td width="78%">URLs to webpages</td></tr><tr><td width="22%">8</td><td width="78%">URL to images</td></tr></table>
*/
extern "C" short __stdcall ID3V2GetSyncLyricContentTypeW(short Index) 
{
	id3frame = id3v2.findFrame(F_SYLT, Index);
	return (id3frame != NULL) ? cSYLT(id3frame)->getContentType() : 0;		
}


/** 
* Methode: ID3V2AddSyncLyricW
* @link ID3V2
* @frame SYLT
* @since 2.0.1.0
* Beschreibung deutsch: "SYLT" setzt oder ersetzt einen synchronen Liedtext-Eintrag<br />Es kann mehr als einen 'SYLT' Eintrag geben, aber keine zwei mit der gleichen Sprache und Beschreibung.
* Beschreibung english: "SYLT" set a synchronized lyric<br />There may be more than one 'SYLT' frame in each tag, but only one with the same language and content descriptor.
* @paramD LPCWSTR language das Sprachkürzel des Eintrages ( genau 3 Bytes ) z.B. ENG
* @paramD LPCWSTR description die Beschreibung des Eintrages
* @paramD LPCWSTR text die eigentlichen Eintraege durch Zeilenumbruch (CRLF) getrennt. Folgendes Format muss verwendet werden:<br /><br />[xxxxxxxx]eintrag1<br />[xxxxxxxx]eintrag2 usw<br /><br />xxxxxxxx = absolute Zeitangabe 8 stellig <br />eintrag = beliebiger Texteintrag<br /><br />Beispiel:<br />[00000010]Strang<br />[00000020]ers<br />[00000080]in
* @paramD Int16 contenttype legt den Inhaltstyp fest. Folgende Werte sind möglich:<br /><br />0 = Sonstiges<br />1 = Songtexte<br />2 = Textumschreibungen<br />3 = Bewegungen<br />4 = Ereignisse<br />5 = Akkorde<br />6 = Belangloses<br />7 = URLs zu Webseiten<br />8 = URLs zu Bildern
* @paramD Int16 timestampformat gibt an, wie die Zeitangaben erfolgen<br /><br />1 = Zeitangaben in MPEG frames<br />2 = Zeitangaben in millisekunden
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE LPCWSTR LanguageID language of the entry, e.g. ENG for english ( must be 3 bytes long )
* @paramE LPCWSTR description the description of the entry
* @paramE LPCWSTR text multi entries separated with line break (crlf). use the following format:<br /><br />[xxxxxxxx]entry1<br />[xxxxxxxx]entry2 etc.<br /><br />xxxxxxxx = formatted absolute time stamp 8 digits<br />entry = any text entry<br /><br />Example:<br />[00000010]Strang<br />[00000020]ers<br />[00000080]in
* @paramE Int16 contenttype set the content type. Possible values are:<br /><br />0 = other<br />1 = lyrics<br />2 = text transcription<br />3 = movement/part name<br />4 = events<br />5 = chord<br />6 = trivia/'pop up' information<br />7 = URLs to webpages<br />8 = URL to images
* @paramE LPCWSTR timestampformat set the timestamp format units<br /><br />1 = using mpeg frames as unit<br />2 = using milliseconds as unit
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddSyncLyricW(LPCWSTR Language, LPCWSTR Description, LPCWSTR Text, short ContentType, short TimeStampFormat)
{
	return b2s(id3v2.replaceFrame(new CID3F_SYLT(Language, (BYTE)TimeStampFormat, (BYTE)ContentType,getValidPointer(Description), getValidPointer(Text))));
}


/** 
* Methode: ID3V2AddPopularimeterW
* @link ID3V2
* @frame POPM
* @since 2.0.1.0
* Beschreibung deutsch: "POPM" setzt oder ersetzt einen Popularimeter<br />Es kann mehr als einen 'POPM' Eintrag geben, aber keine zwei mit der gleichen Email Adresse.
* Beschreibung english: "POPM" set a popularimeter<br />There may be more than one 'POPM' frame in each tag, but only one with the same email address.
* @paramD LPCWSTR Email eine Email-Adresse. 
* @paramD Int16 Rating eine Bewertung von 1 ( schlechteste ) bis 255 (beste), 0=keine Bewertung
* @paramD Int32 Counter Zähler, der die Anzahl der Abspielungen darstellt
* @returnD Int16 -1 falls frame ersetzt wurde, 0 falls neuer Eintrag erfolgte
* @paramE LPCWSTR Email a personal email adress
* @paramE Int16 Rating rating from 1 ( worst ) to 255 ( best ), 0=no rating
* @paramE Int32 Counter play counter 
* @returnE Int16 -1 if frame was replaced, 0 if frame was added
*/
extern "C" short __stdcall ID3V2AddPopularimeterW(LPCWSTR Email, short Rating, long Counter) 
{
	return b2s(id3v2.replaceFrame(new CID3F_POPM(getValidPointer(Email), (BYTE)Rating, Counter)));
}

/**
* Methode: ID3V2GetPopularimeterEmailW
* @link ID3V2
* @frame POPM
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Email-Adresse eines Popularimeter-Eintrages
* Beschreibung english: get the email from a popularimeter tag
* @paramD Int16 index Index von 1 bis Anzahl Popularimeter frames
* @returnD BSTR Email-Adresse
* @paramE Int16 index index from 1 to popularimeter frame count
* @returnE BSTR email adress
*/
extern "C" BSTR __stdcall ID3V2GetPopularimeterEmailW(short Index) 
{
	id3frame = id3v2.findFrame(F_POPM, Index);
	return (id3frame != NULL) ? cPOPM(id3frame)->getEmail().AllocSysString() : CTools::instance().GetEmptyBSTR();	
}

/**
* Methode: ID3V2GetPopularimeterRatingW
* @link ID3V2
* @frame POPM
* @since 2.0.1.0
* Beschreibung deutsch: liefert die Bewertung eines Popularimeter-Eintrages
* Beschreibung english: get the rating from a popularimeter tag
* @paramD Int16 index Index von 1 bis Anzahl Popularimeter frames
* @returnD Int16 Bewertung von 1 (schlecht) bis 255 (beste) oder 0 falls Eintrag nicht existiert
* @paramE Int16 index from 1 to popularimeter frame count
* @returnE Int16 rating from 1 (worst) to 255 (best) or 0 if not present
*/
extern "C" short __stdcall ID3V2GetPopularimeterRatingW(short Index) 
{
	id3frame = id3v2.findFrame(F_POPM, Index);
	return (id3frame != NULL) ? cPOPM(id3frame)->getRating() : 0;	
}

/**
* Methode: ID3V2GetPopularimeterCounterW
* @link ID3V2
* @frame POPM
* @since 2.0.1.0
* Beschreibung deutsch: liefert den Zähler eines Popularimeter-Eintrages
* Beschreibung english: get the counter from a popularimeter tag
* @paramD Int16 index Index von 1 bis Anzahl Popularimeter frames
* @returnD Int32 counter or -1 falls Eintrag nicht existiert
* @paramE Int16 index from 1 to popularimeter frame count
* @returnE Int32 counter or -1 if not present
*/
extern "C" long __stdcall ID3V2GetPopularimeterCounterW(short Index) 
{
	id3frame = id3v2.findFrame(F_POPM, Index);
	return (id3frame != NULL) ? cPOPM(id3frame)->getCounter() : -1;	

}

/**
* Methode: SetLogFileW
* @link UNIVERSAL
* @since 2.0.1.0
* Beschreibung deutsch: setzt den Dateiname, in den AudioGenie loggt
* Beschreibung english: set a file name for audiogenie logging
* @paramD LPCWSTR FileName Name der Logging-Datei
* @paramE LPCWSTR Filename name of the logging file
*/
extern "C" void __stdcall SetLogFileW(LPCWSTR fileName)
{
	CTools::instance().setLogFile(getValidPointer(fileName));
}

/**
* Methode: WAVGetTextFrameW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: liefert einen wav Text Frame (info chunk)
* Beschreibung english: get a wav text frame (info chunk)
* @paramD UInt32 FrameID die ID des Frames
* @returnD BSTR der Textstring
* @paramE UInt32 FrameID the ID of the frame
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall WAVGetTextFrameW(u32 FrameID)
{
	return wav.getTextFrame(FrameID).AllocSysString();
}

/**
* Methode: WAVSetTextFrameW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: setzt oder ersetzt einen Wav Text frame (info chunk)
* Beschreibung english: set a wav text frame (info chunk)
* @paramD UInt32 FrameID die ID des Frames
* @paramD LPCWSTR textString der neue Text
* @paramE UInt32 FrameID the ID of the frame
* @paramE LPCWSTR textString the new text
*/
extern "C" void __stdcall WAVSetTextFrameW(u32 FrameID, LPCWSTR textString)
{
	wav.setTextFrame(FrameID, getValidPointer(textString));	
}

/**
* Methode: WAVGetDisplayTextW
* @link WAV
* @since 2.0.3.0
* Beschreibung deutsch: liefert den Text eines Display chunks
* Beschreibung english: get the text of a display chunk
* @returnD BSTR der Textstring oder leer falls nicht gefunden
* @returnE BSTR the text string or empty if not present
*/
extern "C" BSTR __stdcall WAVGetDisplayTextW()
{
	return wav.getDisplayText().AllocSysString();
}

/**
* Methode: WAVSetDisplayTextW
* @link WAV
* @since 2.0.3.0
* Beschreibung deutsch: setzt den Text eines DISPLAY chunks
* Beschreibung english: set the etxt of a DISPLAY chunk
* @paramD LPCWSTR textString der neue Text
* @paramE LPCWSTR textString the new text
*/
extern "C" void __stdcall WAVSetDisplayTextW(LPCWSTR textString)
{
	wav.setDisplayText(getValidPointer(textString));	
}

/**
* Methode: WAVSaveChangesToFileW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: speichert den WAV Tag in eine Datei
* Beschreibung english: store the wav tag in a file
* @paramD LPCWSTR FileName Name der Datei
* @returnD Int16 normalerweise -1, bei Fehler 0
* @paramE LPCWSTR FileName name of the file
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall WAVSaveChangesToFileW(LPCWSTR FileName)
{
	FileName = getValidPointer(FileName);	
	if (GetFormat(FileName) != AUDIO_FORMAT_INVALID)
	{
		bool result = wav.SaveToFile(FileName);
		if (result && !lastFile.IsEmpty() && lastFile.CompareNoCase(FileName) == 0)
			AUDIOAnalyzeFileW(FileName);
		return b2s(result);
	}
	CTools::instance().setLastError(ERR_TAG_NOT_ALLOWED);
	return b2s(false);   
}

/**
* Methode: WAVSaveChangesW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: speichert den WAV-Tag in die zuletzt analysierte Datei
* Beschreibung english: stores the WAV tag into the last analyzed file
* @returnD Int16 normalerweise -1, bei Fehler 0
* @returnE Int16 normally -1, 0 on error
*/
extern "C" short __stdcall WAVSaveChangesW()
{
	return WAVSaveChangesToFileW(lastFile);
}


/**
* Methode: WAVGetCartChunkEntryW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: liefert einen wav cartchunk Eintrag
* Beschreibung english: get a wav cart chunk entry
* @paramD Int16 Index der Index des Eintrages von 0 bis 26
* @returnD BSTR der Textstring
* @paramE Int16 Index the Index of the entry from 0 to 26
* @returnE BSTR the text string
*/
extern "C" BSTR __stdcall WAVGetCartChunkEntryW(short Index)
{
	return wav.getCartText((BYTE)Index).AllocSysString();
}

/**
* Methode: WAVSetCartChunkEntryW
* @link WAV
* @since 2.0.1.0
* Beschreibung deutsch: setzt oder ersetzt einen Wav CartChunk Eintrag 
* Beschreibung english: set or replace a cartchunk entry
* @paramD Int16 Index der Index des Eintrages von 0 bis 26
* @paramD LPCWSTR textString der neue Text
* @paramE Int16 Index the Index of the entry from 0 to 26
* @paramE LPCWSTR textString the new text
*/
extern "C" void __stdcall WAVSetCartChunkEntryW(short Index, LPCWSTR textString)
{
	wav.setCartText((BYTE)Index, getValidPointer(textString));
}

/* ----------------------------------------------------------------------------------------- */

/**
* Methode: AUDIOGetMD5ValueW
* @link AUDIO
* @since 2.0.1.0
* Beschreibung deutsch: liefert einen MD5 Hash über die reinen Audiodaten ohne Taginformationen der zuletzt analysierten Datei<br />Dient zum Vergleichen von Audio-Dateien.
* Beschreibung english: get a MP5 hash only from the audio datas and without tag informations from the last analyzed file<br />You can use this for comparing audio files.
* @returnD BSTR MD5 Schlüssel ( 32 Zeichen lang )
* @returnE BSTR MD5 Key ( 32 bytes long )
*/
extern "C" BSTR __stdcall AUDIOGetMD5ValueW() 
{	
	if (Format != AUDIO_FORMAT_UNKNOWN)
	{
		md5Tool.calcHashFromFile(lastFile, audio->GetFirstAudioPosition(), audio->GetLastAudioPosition());
		return md5Tool.getHashValue().AllocSysString();	
	}
	return CTools::instance().GetEmptyBSTR();	
}

/**
* Methode: GetMD5ValueFromFileW
* @link UNIVERSAL
* @since 2.0.1.0
* Beschreibung deutsch: liefert einen MD5 Hash über die ganze Datei<br />Dient zum Vergleichen von Dateien.
* Beschreibung english: get a MP5 hash from the complete file<br />You can use this for comparing files.
* @paramD LPCWSTR Filename Name der Datei
* @paramE LPCWSTR Filename name of the file
* @returnD BSTR MD5 Schlüssel ( 32 Zeichen lang )
* @returnE BSTR MD5 Key ( 32 bytes long )
*/
extern "C" BSTR __stdcall GetMD5ValueFromFileW(LPCWSTR FileName) 
{	
	md5Tool.calcHashFromFile(getValidPointer(FileName), 0, 0);
	return md5Tool.getHashValue().AllocSysString();	
}
