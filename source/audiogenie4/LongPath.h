/* AudioGenie is a Library for analyzing and tagging audio files.
   This file is part of the AudioGenie Library (LGPL 2.1, see LICENSE.txt).
*/
/* -------------------------------------------------------------------------- */
/* LongPath.h: file names of any length (4.0.4.9)                             */
/* -------------------------------------------------------------------------- */
// A path of MAX_PATH (260) characters or more couldn't be opened: AUDIOAnalyzeFileW returned 0 and every save
// failed (4.0.4.9 before this). The file functions the library uses are redirected here, and a long path is passed
// to Windows in its extended form (\\?\C:\... or \\?\UNC\server\share\...), which has no length limit. A short
// path is passed as it is, so nothing changes for it. Included at the end of StdAfx.h, after the headers that
// declare these functions, so every source file uses the redirection.
#pragma once
#include <stdio.h>
#include <io.h>
#include <share.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <windows.h>
#include <atlstr.h>

// The longest path passed unchanged: a directory can't be longer than MAX_PATH - 12, and a save adds "~" to the
// file name for its temporary copy
#define AG_LONGPATH_LIMIT (MAX_PATH - 14)

inline ATL::CAtlStringW AGLongPath(const wchar_t* path)
{
	if (path == NULL)
		return ATL::CAtlStringW();
	size_t len = wcslen(path);
	if (len < AG_LONGPATH_LIMIT || wcsncmp(path, L"\\\\?\\", 4) == 0 || wcsncmp(path, L"\\\\.\\", 4) == 0)
		return ATL::CAtlStringW(path);
	// the extended form must be absolute and use backslashes only (Windows no longer normalizes it)
	ATL::CAtlStringW full;
	DWORD n = GetFullPathNameW(path, 0, NULL, NULL);
	if (n > 0)
	{
		DWORD got = GetFullPathNameW(path, n, full.GetBuffer(n), NULL);
		full.ReleaseBuffer((got > 0 && got < n) ? (int)got : 0);
	}
	if (full.IsEmpty())
		full = path;
	full.Replace(L'/', L'\\');
	if (full.GetLength() >= 2 && full[0] == L'\\' && full[1] == L'\\')
		return ATL::CAtlStringW(L"\\\\?\\UNC\\") + full.Mid(2);
	return ATL::CAtlStringW(L"\\\\?\\") + full;
}

inline FILE* AG_wfsopen(const wchar_t* fileName, const wchar_t* mode, int shflag)
{
	return _wfsopen(AGLongPath(fileName), mode, shflag);
}

inline errno_t AG_wsopen_s(int* pfh, const wchar_t* fileName, int oflag, int shflag, int pmode)
{
	return _wsopen_s(pfh, AGLongPath(fileName), oflag, shflag, pmode);
}

inline int AG_wremove(const wchar_t* fileName)
{
	return _wremove(AGLongPath(fileName));
}

inline int AG_wrename(const wchar_t* oldName, const wchar_t* newName)
{
	return _wrename(AGLongPath(oldName), AGLongPath(newName));
}

inline BOOL AG_MoveFileExW(LPCWSTR existingName, LPCWSTR newName, DWORD flags)
{
	return MoveFileExW(AGLongPath(existingName), AGLongPath(newName), flags);
}

#define _wfsopen AG_wfsopen
#define _wsopen_s AG_wsopen_s
#define _wremove AG_wremove
#define _wrename AG_wrename
#define MoveFileExW AG_MoveFileExW
