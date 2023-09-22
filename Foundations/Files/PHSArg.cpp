#include "PHS.h"

#include "..\Container.h"

String _PHSArg::Value() const
{
	StringList Parts = _Text.Split(L':');
	String Return = (Parts.Size < 2 ? L"" : Parts[1]);
	for (uint i = 2; i < Parts.Size; i++)
		Return += L':' + Parts[i];

	return Return.ReplaceChar(L'`', L'\n');
}
void _PHSArg::Value(String New)
{
	_Text = (String)Name() + L':' + New;
}
String _PHSArg::Name() const
{
	StringList Parts = _Text.Split(L':');
	return (Parts.Size == 0 ? L"" : Parts[0]);
}
void _PHSArg::Name(String NewName)
{
	if (NewName == L"")
		throw std::exception("The name cannot be blank string.");
	_Text = (String)NewName + Value();
}
