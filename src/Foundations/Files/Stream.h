#pragma once

#define OpenFileEx_StringMax 500


#include <Windows.h>

#include "../Str.h"
#include "../Container.h"

bool FileExists(String _Path);
String FileName(String _Path);
String FileExt(String _Path);
String FilePath(String _Path);
String FileFullName(String Path);

String OpenFileEx(HWND Window);
String OpenFileEx(HWND Window, LPCWSTR FileTypeFilter, LPCWSTR DefExt);
String OpenFileEx(HWND Window, LPCWSTR FileTypeFilter, LPCWSTR DefExt, LPCWSTR Title);
String SaveFileEx(HWND Window);
String SaveFileEx(HWND Window, LPCWSTR FileTypeFilter, LPCWSTR DefExt);