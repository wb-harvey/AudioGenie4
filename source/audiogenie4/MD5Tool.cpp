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
#include "MD5Tool.h"
#include "io.h"
#include "Tools.h"
#include <share.h>

CMD5Tool::CMD5Tool(void)
{
	buffer = new BYTE[MAX_MD5_BUFFER];
	md5 = new MD5();
}

CMD5Tool::~CMD5Tool(void)
{
	delete [] buffer;
	delete md5;
}

bool CMD5Tool::calcHashFromFile(LPCWSTR FileName, long startPos, long endPos)
{
	md5->MD5Init(&ctx);
	FILE *Stream;
	__int64 end, start;
	hash.Empty();
	if ( (Stream = _wfsopen(FileName, READ_ONLY, _SH_DENYWR)) != NULL)
	{
		// 4.0.4.7: 64-bit length and positions (_filelength is -1 for a file of 2 GB or more) (4.0.4.6 and earlier)
		int len;
		__int64 maxLoad;
		end = (endPos == 0) ? _filelengthi64(_fileno(Stream)) - 1 : endPos;
		start = min((__int64)startPos, end);
		maxLoad = end - start + 1;
		ATLTRACE(_T("MD5calc, start=%I64d ende=%I64d maxLoad=%I64d\n"), start, end, maxLoad);
		if (start >= 0 && _fseeki64(Stream, start, SEEK_SET) == 0)
		{
			while (maxLoad > 0 && (len = (int)fread(buffer, 1, (size_t)min((__int64)MAX_MD5_BUFFER, maxLoad), Stream)) )
			{
				//ATLTRACE(_T("Old StartPos: %d "), ftell(Stream) - len);
				//ATLTRACE(_T("Next StartPos: %d "), ftell(Stream));
				//ATLTRACE(_T("maxLoad: %d len:%d \n"), maxLoad, len);
				md5->MD5Update(&ctx, buffer, len);
				CTools::instance().doEvents();
				maxLoad-=len;				
			}		
			hashIt();
			fflush(Stream);
			fclose(Stream);
			ATLTRACE(_T("MD5: %s\n"), hash);
			return true;
		}
		fclose(Stream);		
	}
	CTools::instance().setLastError(errno);
	return false;	
}

void CMD5Tool::hashIt()
{
	unsigned char buff[16] = "";
	hash.Empty();
	md5->MD5Final((unsigned char*)buff,&ctx);
	for(int i = 0; i < 16; ++i)
		hash.AppendFormat(_T("%02x"), buff[i]);	
}