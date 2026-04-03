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
#include "ID3F_RVRB.h"
#include "id3_framefactory.h"

/*
Reverb

Yet another subjective frame, with which you can adjust echoes of
different kinds. Reverb left/right is the delay between every bounce
in ms. Reverb bounces left/right is the number of bounces that should
be made. $FF equals an infinite number of bounces. Feedback is the
amount of volume that should be returned to the next echo bounce. $00
is 0%, $FF is 100%. If this value were $7F, there would be 50% volume
reduction on the first bounce, 50% of that on the second and so on.
Left to left means the sound from the left bounce to be played in the
left speaker, while left to right means sound from the left bounce to
be played in the right speaker.

'Premix left to right' is the amount of left sound to be mixed in the
right before any reverb is applied, where $00 id 0% and $FF is 100%.
'Premix right to left' does the same thing, but right to left.
Setting both premix to $FF would result in a mono output (if the
reverb is applied symmetric). There may only be one "RVRB" frame in
each tag.

<Header for 'Reverb', ID: "RVRB">
Reverb left (ms)                 $xx xx
Reverb right (ms)                $xx xx
Reverb bounces, left             $xx
Reverb bounces, right            $xx
Reverb feedback, left to left    $xx
Reverb feedback, left to right   $xx
Reverb feedback, right to right  $xx
Reverb feedback, right to left   $xx
Premix left to right             $xx
Premix right to left             $xx
*/

CID3F_RVRB::CID3F_RVRB() 	
{
	init(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);	
}

CID3F_RVRB::CID3F_RVRB(short reverbLeft, short reverbRight, BYTE bouncesLeft, BYTE bouncesRight, BYTE feedbackLeftToLeft, BYTE feedbackLeftToRight, BYTE feedbackRightToRight, BYTE feedbackRightToLeft, BYTE premixLeftToRight, BYTE premixRightToLeft)
{
	init(reverbLeft, reverbRight, bouncesLeft, bouncesRight, feedbackLeftToLeft, feedbackLeftToRight, feedbackRightToRight, feedbackRightToLeft, premixLeftToRight, premixRightToLeft);
}
void CID3F_RVRB::init(short reverbLeft, short reverbRight, BYTE bouncesLeft, BYTE bouncesRight, BYTE feedbackLeftToLeft, BYTE feedbackLeftToRight, BYTE feedbackRightToRight, BYTE feedbackRightToLeft, BYTE premixLeftToRight, BYTE premixRightToLeft)
{
	_frameID = F_RVRB;
	_datas[0] = reverbLeft;
	_datas[1] = reverbRight;
	_datas[2] = bouncesLeft;
	_datas[3] = bouncesRight;
	_datas[4] = feedbackLeftToLeft;
	_datas[5] = feedbackLeftToRight;
	_datas[6] = feedbackRightToRight;
	_datas[7] = feedbackRightToLeft;
	_datas[8] = premixLeftToRight;
	_datas[9] = premixRightToLeft;
	mustRebuild = true;
	isDecoded = true;
}

CID3F_RVRB::~CID3F_RVRB(void)
{

}

void CID3F_RVRB::decode()
{
	if (!isDecoded)
	{	
		if (isUnsynchronized())
			resync();
		if (_blob.GetLength() < 12)
		{
			for (int i = 0; i < 10; i++)
				_datas[i] = 0;			
		}
		else
		{
			_datas[0] = _blob.Get2B(0);
			_datas[1] = _blob.Get2B(2);
			_datas[2] = _blob.GetAt(4);
			_datas[3] = _blob.GetAt(5);
			_datas[4] = _blob.GetAt(6);
			_datas[5] = _blob.GetAt(7);
			_datas[6] = _blob.GetAt(8);
			_datas[7] = _blob.GetAt(9);
			_datas[8] = _blob.GetAt(10);
			_datas[9] = _blob.GetAt(11);			
		}
		isDecoded = true;
	}	
}
void CID3F_RVRB::encode()
{
	if (mustRebuild)
	{
		decode();
		_blob.Clear();
		_blob.Add2B(_datas[0]);
		_blob.Add2B(_datas[1]);
		_blob.AddValue((BYTE)_datas[2]);
		_blob.AddValue((BYTE)_datas[3]);
		_blob.AddValue((BYTE)_datas[4]);
		_blob.AddValue((BYTE)_datas[5]);
		_blob.AddValue((BYTE)_datas[6]);
		_blob.AddValue((BYTE)_datas[7]);
		_blob.AddValue((BYTE)_datas[8]);
		_blob.AddValue((BYTE)_datas[9]);
		mustRebuild = false;
	}
}

short CID3F_RVRB::getDatas(BYTE index)
{
	ATLASSERT(index >= 0 && index <= 9);
	decode();	
	return _datas[index];
}
