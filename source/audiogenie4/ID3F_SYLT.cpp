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
#include "ID3F_SYLT.h"
#include "id3_framefactory.h"
#include "Tools.h"

// SYLT
/*
This is another way of incorporating the words, said or sung lyrics,
in the audio file as text, this time, however, in sync with the
audio. It might also be used to describing events e.g. occurring on a
stage or on the screen in sync with the audio. The header includes a
content descriptor, represented with as terminated text string. If no
descriptor is entered, 'Content descriptor' is $00 (00) only.

<Header for 'Synchronised lyrics/text', ID: "SYLT">
Text encoding        $xx
Language             $xx xx xx
Time stamp format    $xx
Content type         $xx
Content descriptor   <text string according to encoding> $00 (00)

Content type:   $00 is other
$01 is lyrics
$02 is text transcription
$03 is movement/part name (e.g. "Adagio")
$04 is events (e.g. "Don Quijote enters the stage")
$05 is chord (e.g. "Bb F Fsus")
$06 is trivia/'pop up' information
$07 is URLs to webpages
$08 is URLs to images

Time stamp format:

$01  Absolute time, 32 bit sized, using MPEG [MPEG] frames as unit
$02  Absolute time, 32 bit sized, using milliseconds as unit

Absolute time means that every stamp contains the time from the
beginning of the file.

The text that follows the frame header differs from that of the
unsynchronised lyrics/text transcription in one major way. Each
syllable (or whatever size of text is considered to be convenient by
the encoder) is a null terminated string followed by a time stamp
denoting where in the sound file it belongs. Each sync thus has the
following structure:

Terminated text to be synced (typically a syllable)
Sync identifier (terminator to above string)   $00 (00)
Time stamp                                     $xx (xx ...)

The 'time stamp' is set to zero or the whole sync is omitted if
located directly at the beginning of the sound. All time stamps
should be sorted in chronological order. The sync can be considered
as a validator of the subsequent string.

Newline characters are allowed in all "SYLT" frames and MUST be used
after every entry (name, event etc.) in a frame with the content type
$03 - $04.

A few considerations regarding whitespace characters: Whitespace
separating words should mark the beginning of a new word, thus
occurring in front of the first syllable of a new word. This is also
valid for new line characters. A syllable followed by a comma should
not be broken apart with a sync (both the syllable and the comma
should be before the sync).

An example: The "USLT" passage

"Strangers in the night" $0A "Exchanging glances"

would be "SYLT" encoded as:

"Strang" $00 xx xx "ers" $00 xx xx " in" $00 xx xx " the" $00 xx xx
" night" $00 xx xx 0A "Ex" $00 xx xx "chang" $00 xx xx "ing" $00 xx
xx "glan" $00 xx xx "ces" $00 xx xx

There may be more than one "SYLT" frame in each tag, but only one
with the same language and content descriptor.
*/

CID3F_SYLT::CID3F_SYLT() 	
{
	init(EMPTY, 0, 0, EMPTY, EMPTY);	
}

CID3F_SYLT::CID3F_SYLT(LPCWSTR newLanguage, BYTE newTimestampFormat, BYTE newContentType, LPCWSTR newDescription, LPCWSTR newText)
{
	init(newLanguage, newTimestampFormat, newContentType, newDescription, newText);
}
void CID3F_SYLT::init(LPCWSTR newLanguage, BYTE newTimestampFormat, BYTE newContentType, LPCWSTR newDescription, LPCWSTR newText)
{
	_frameID = F_SYLT;
	_description = (newDescription != NULL) ? newDescription : EMPTY;
	_language = (newLanguage != NULL) ? newLanguage : EMPTY;
	_text = (newText != NULL) ? newText : EMPTY;
	_timestampformat = newTimestampFormat;
	_contenttype = newContentType;
	useTextEncoding = true;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_SYLT::~CID3F_SYLT(void)
{

}

bool CID3F_SYLT::equals(CID3_Frame *frame)
{
	if (!CID3_Frame::equals(frame))
		return false;
	decode();
	frame->decode();
	CID3F_SYLT *target = cSYLT(frame);
	return (this->_description.CompareNoCase(target->_description) == 0 && this->_language.CompareNoCase(target->_language) == 0);
}

void CID3F_SYLT::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		if (_blob.GetLength() < 7)
		{
			_language.Empty();
			_description.Empty();
			_text.Empty();
			_timestampformat = 0;
			_contenttype = 0;
		}
		else
		{
			encodingID = _blob.GetAt(0);
			_language = _blob.GetStringAt(1, 3);
			_timestampformat = _blob.GetAt(4);
			_contenttype = _blob.GetAt(5);
			int start = 6;
			_description = _blob.getNextString(encodingID, start);
			// Extrahiere Text ( Ansi oder Unicode oder UTF8 )
			// WICHTIG: evtl BOM wird hier nicht mehr aufgeführt!!!
			// Deshalb umsetzen
			BYTE code = (encodingID == TEXT_ENCODED_UTF16BOM) ? TEXT_ENCODED_UTF16 : encodingID;
			int nr = 0;
			_data.AddBlob(_blob, start);
			start = 0;
			CAtlString tmp;
			_text.Empty();
			while (start < (int)_data.GetLength())
			{
				tmp = _data.getNextString(code, start);
				nr = _data.Get4B(start);
				start+=4;
				_text.AppendFormat(_T("[%08d]%s\n"), nr, tmp);				
			}
		}
		isDecoded = true;
	}	
}
void CID3F_SYLT::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.AddValue(encodingID);
		_blob.AddFixedAnsiString(_language, 3);
		_blob.AddValue(_timestampformat);
		_blob.AddValue(_contenttype);
		_blob.AddEncodedString(encodingID, _description, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);		
		// text zerlegen und in id3 format umwandeln
		// Textschema: "[00000000]string1\n[00000000]string2\n" usw
		CAtlString tmp = _text;
		CAtlString entry;
		tmp.Append(_T("\n"));
		int pos = tmp.Find(_T("\n"));
		int start = 0;
		BYTE code = (encodingID == TEXT_ENCODED_UTF16BOM) ? TEXT_ENCODED_UTF16 : encodingID;
		while (pos >= 0)
		{
			entry = tmp.Mid(start, pos - start);
			if (entry.GetLength() <= 10)
			{
				CTools::instance().setLastError(ERR_SYNCPARSE_ERROR);
				return;
			}
			int nr = _wtoi(entry.Mid(1,8));
			CAtlString txt = entry.Mid(10);
			if (txt.GetLength() > 0)
			{
				_blob.AddEncodedString(code, txt, TEXT_WITHOUT_ENCODING, TEXT_WITH_NULLBYTES);
				_blob.Add4B(nr);
			}
			start = pos + 1;
			pos = tmp.Find(_T("\n"), start);
		}
		mustRebuild = false;
	}
}
void CID3F_SYLT::setValues(LPCWSTR newLanguage, BYTE newTimestampFormat, BYTE newContentType, LPCWSTR newDescription, LPCWSTR newText)
{
	init(newLanguage, newTimestampFormat, newContentType, newDescription, newText);
	isDecoded = true;
	mustRebuild = true;
}

CAtlString CID3F_SYLT::getDescription()
{
	decode();
	return _description;
}

CAtlString CID3F_SYLT::getLanguage()
{
	decode();
	return _language;
}

CAtlString CID3F_SYLT::getText()
{
	decode();
	return _text;
}

BYTE CID3F_SYLT::getTimestampFormat()
{
	decode();
	return _timestampformat;
}
BYTE CID3F_SYLT::getContentType()
{
	decode();
	return _contenttype;
}

