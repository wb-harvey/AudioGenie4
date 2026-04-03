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
#include "Tools.h"
#include "share.h"

int CTools::lastError;
long CTools::FileSize;
int CTools::ID3v1Size;
long CTools::ID3v2Size;
int CTools::LyricsSize;
int CTools::APESize;
long CTools::firstMpegAudioPos;
long CTools::configValues[MAX_CONFIG_VALUES];
wchar_t *CTools::wcTextPuffer = 0;
char *CTools::cTextPuffer = 0;
CAtlString CTools::lastErrorText;
MSG CTools::msg;
CAtlString CTools::logFile;
FILE* CTools::log;
SYSTEMTIME CTools::stTime; //To contain the date/time
CAtlString CTools::logOutput;
CBlob CTools::output;
DWORD CTools::oldTickCount;
BYTE CTools::ID3V2oldTagVersion = 0;
BYTE CTools::ID3V2newTagVersion = 3; // TAG_VERSION_2_3
BYTE CTools::ID3V2defaultEncodingID = TEXT_ENCODED_ANSI;
BYTE CTools::ID3V2Flags = 0;

CTools::CTools(void)
{
	// Folgende Initialisierung findet nur einmal statt!
	oldTickCount = 0;
	logFile.Empty();
	log = NULL;
	configValues[CONFIG_MPEGEXACTREAD] = 0;
	configValues[CONFIG_ID3V2PADDINGSIZE] = 4096;
	configValues[CONFIG_ID3V2WRITEBLOCKSIZE] = 524288l; // default 0.5MB
	configValues[CONFIG_WMAPADDINGSIZE] = 4096;
	configValues[CONFIG_DOEVENTSMILLIS] = 250;
	configValues[CONFIG_MP4PADDINGSIZE] = 4096;
	setConfigValue(CONFIG_MAXTEXTBUFFER, 0x40000l); // default 256 KB
}

CTools::~CTools(void)
{
	delete [] wcTextPuffer;
	delete [] cTextPuffer;
}

CTools &CTools::instance()
{
	static CTools factory;
	return factory;
}

void CTools::reset()
{
	FileSize = 0;
	ID3v1Size = 0;
	ID3v2Size = 0;
	LyricsSize = 0;
	APESize = 0;
	lastError = 0;
	lastErrorText.Empty();
	firstMpegAudioPos = 0;
}

void CTools::doEvents()
{
	DWORD ticks = GetTickCount();
	if (ticks < oldTickCount + CTools::configValues[CONFIG_DOEVENTSMILLIS]) // nur alle eventMillis Millisekunden
		return;
	doEventsNow();
}
void CTools::doEventsNow()
{
	if (PeekMessageW(&msg, (HWND) NULL, 0, 0, PM_REMOVE)) 
	{ 
		if (msg.message == WM_QUIT)
			return;
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);	
	}  
}

void CTools::setLastError(int error, ...)
{
	lastError = error;
	if (error == 0)
		return;
	if (error > 200 && error < 250) // Benutzerdefinierter Fehlertext
	{
		va_list vlist;
		va_start(vlist, error);
		_vsnwprintf_s(wcTextPuffer, 16000, 15000, ERR_TEXT[error - 201], vlist);	
		lastErrorText = wcTextPuffer;
		va_end(vlist);
	}
	else // Systemfehler
	{
		_wcserror_s(wcTextPuffer, 16000, lastError);  
	}
	lastErrorText = wcTextPuffer;
	writeError(lastErrorText);
}

void CTools::setLogFile(LPCWSTR file)
{
	logFile = file;
}

void CTools::writeInfo(LPCWSTR entry, ...)
{
	va_list vlist;
	va_start(vlist, entry);
	_vsnwprintf_s(wcTextPuffer, 16000, 15000, entry, vlist);	
	write( _T("INFO"), wcTextPuffer);
	va_end(vlist);
}

void CTools::writeError(LPCWSTR entry, ...)
{
	va_list vlist;
	va_start(vlist, entry);
	_vsnwprintf_s(wcTextPuffer, 16000, 15000, entry, vlist);	
	write( _T("ERROR"), wcTextPuffer);
	va_end(vlist);
}

void CTools::writeWarning(LPCWSTR entry, ...)
{
	va_list vlist;
	va_start(vlist, entry);
	_vsnwprintf_s(wcTextPuffer, 16000, 15000, entry, vlist);
	write( _T("WARNING"), wcTextPuffer);
	va_end(vlist);
}

void CTools::writeDebug(LPCWSTR entry, ...)
{
	entry;
#ifdef _DEBUG
	va_list vlist;
	va_start(vlist, entry);
	_vsnwprintf_s(wcTextPuffer, 16000, 15000, entry, vlist);
	write( _T("DEBUG"), wcTextPuffer);
	va_end(vlist);	
#endif
}


void CTools::write(LPCWSTR art, LPCWSTR entry)
{
	if (!logFile.IsEmpty())
	{
		log = _wfsopen(logFile, _T("ab"), _SH_DENYNO);
		if (log != NULL)
		{
			GetLocalTime(&stTime);
			output.Clear();		
			logOutput.Format(_T("%4i-%02i-%02i %02i:%02i:%02i.%04i %s: %s\r\n"), stTime.wYear, stTime.wMonth,
				stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds, art, entry);
			output.AddEncodedString(TEXT_ENCODED_ANSI, logOutput, TEXT_WITHOUT_ENCODING, TEXT_WITHOUT_NULLBYTES);
			output.FileWrite(output.GetLength(), log);
			fflush(log);
			fclose(log);
		}	
	}
}

CAtlString CTools::ExtractMimeFromPicture(const BYTE *buf)
{
	return IMAGE_LONG[CalcMimeFromPicture(buf)];
}

CAtlString CTools::ExtractSmallMimeFromPicture(const BYTE *buf)
{
	return IMAGE_SHORT[CalcMimeFromPicture(buf)];	
}

int CTools::CalcMimeFromPicture(const BYTE *buf)
{
	if (buf == NULL)
		return IMAGE_UNKNOWN; 
	if (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF)
		return IMAGE_JPG; 
	if (buf[0] == 'G' && buf[1] == 'I' && buf[2] == 'F')
		return IMAGE_GIF; 
	if (buf[0] == 0x89 && buf[1] == 0x50 && buf[2] == 0x4E && buf[3] == 0x47)
		return IMAGE_PNG; 
	if (buf[0] == 'B' && buf[1] == 'M')
		return IMAGE_BMP; 
	if (buf[0] == 0x49 && buf[1] == 0x49 && buf[2] == 0x2A && buf[3] == 0x00)
		return IMAGE_TIFF; 
	if (buf[0] == '-' && buf[1] == '-' && buf[2] == '>')
		return IMAGE_LINK; 
	return IMAGE_UNKNOWN;
}

void CTools::setConfigValue(long key, long value)
{
	if (key == CONFIG_MAXTEXTBUFFER)
	{
		int newValue = ((int)value / 2) * 2;
		if (newValue < 32768)
			newValue = 32768;
		configValues[key] = newValue;
		delete [] wcTextPuffer;
		delete [] cTextPuffer;
		wcTextPuffer = new wchar_t[newValue + 2];
		cTextPuffer = new char[newValue + 2];
	}
	else
	{
		if (key >= 0 && key < MAX_CONFIG_VALUES)
		configValues[key] = value;
	}
}
long CTools::getConfigValue(long key)
{
	return (key >= 0 && key < MAX_CONFIG_VALUES) ? configValues[key] : 0;
}