#pragma once

#ifndef Aaron_Misc
#define Aaron_Misc

#include <Windows.h>
#include "String.h"
#include "Container.h"

void LoadExe(PCWSTR Path);
void LoadExe(PCWSTR Path, LPWSTR CommandLine);

#endif