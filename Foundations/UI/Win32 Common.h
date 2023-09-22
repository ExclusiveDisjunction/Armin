#pragma once

#if (!defined(_WIN32) and !defined(_WIN64)) or !defined(__cplusplus)
#error Win32 or Win64 is required, and the code must be compled in C++
#endif

#include <Windows.h>
#include "ControlBase.h"
#include "..\Container.h"
#include "..\Str.h"

String GetText(HWND Window);
void SetText(HWND Window, String New);