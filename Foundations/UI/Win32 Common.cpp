#include "Win32 Common.h"

String GetText(HWND Window)
{
	int TextLen = static_cast<int>(GetWindowTextLengthW(Window)) + 1;
	wchar_t* Chars = new wchar_t[TextLen];

	GetWindowTextW(Window, Chars, TextLen);
	String Return(Chars);

	delete[] Chars;
	return Return;
}
void SetText(HWND Window, String New)
{
	wchar_t* Chars = const_cast<wchar_t*>(static_cast<const wchar_t*>(New));
	SendMessageW(Window, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(Chars));
}