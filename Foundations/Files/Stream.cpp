#include "Stream.h"

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

using namespace std;

bool FileExists(String _Path)
{
	return std::filesystem::exists((wstring)_Path);
}
String FileName(String _Path)
{
	Vector<String> Parts = _Path.Split(L'\\');
	uint Len = Parts.Size;

	Vector<String> SubParts = Parts[Len - 1].Split(L'.');
	String Return = SubParts[0];
	
	return Return;
}
String FileExt(String _Path)
{
	Vector<String> Parts = _Path.Split(L'\\');
	uint Len = Parts.Size;

	Vector<String> SubParts = Parts[Len - 1].Split(L'.');
	String Return = SubParts[SubParts.Size - 1];

	return Return;
}
String FilePath(String _Path)
{
	Vector<String> Parts = _Path.Split(L'\\');
	uint Len = Parts.Size;

	String Return = (Len == 0 ? L"" : Parts[0]);
	for (uint i = 1; i < Len - 1; i++)
		Return += L'\\' + Parts[i];

	return Return;
}
String FileFullName(String Path)
{
	Vector<String> Parts = Path.Split(L'\\');

	return Parts.Size == 0 ? L"" : Parts[Parts.Size - 1];
}

String OpenFileEx(HWND Window)
{
	return OpenFileEx(Window, L"All Files (*.*)\0*.*", L"*.*");
}
String OpenFileEx(HWND Window, LPCWSTR FileTypeFilter, LPCWSTR DefExt)
{
	return OpenFileEx(Window, FileTypeFilter, DefExt, L"Please select a file.");
}
String OpenFileEx(HWND Window, LPCWSTR FileTypeFilter, LPCWSTR DefExt, LPCWSTR Title)
{
	wchar_t filename[OpenFileEx_StringMax] = L"";

	OPENFILENAME open;

	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&open, sizeof(open));

	open.lStructSize = sizeof(open);
	open.hwndOwner = Window;
	open.lpstrFilter = FileTypeFilter;
	open.lpstrFile = (LPWSTR)filename;
	open.nMaxFile = OpenFileEx_StringMax;
	open.lpstrDefExt = DefExt;
	open.lpstrTitle = Title;
	open.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_EXPLORER;

	String Return;

	if (GetOpenFileName(&open))
		Return = filename;

	return Return;
}
String SaveFileEx(HWND Window)
{
	return SaveFileEx(Window, L"All Files (*.*)\0*.*", L"*.*");
}
String SaveFileEx(HWND Window, LPCWSTR FileTypeFilter, LPCWSTR DefExt)
{
	wchar_t filename[OpenFileEx_StringMax] = L"";

	OPENFILENAME open;

	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&open, sizeof(open));

	open.lStructSize = sizeof(open);
	open.hwndOwner = Window;
	open.lpstrFilter = FileTypeFilter;
	open.lpstrFile = (LPWSTR)filename;
	open.nMaxFile = OpenFileEx_StringMax;
	open.lpstrDefExt = DefExt;
	open.lpstrTitle = L"Please select a file.";
	open.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_EXPLORER;

	String Return;

	if (GetSaveFileName(&open))
		Return = filename;

	return Return;
}