#include "PHS.h"

#include <iostream>
#include <fstream>

#include "Stream.h"

using namespace std;

_PHSDoc::_PHSDoc()
{
	Element = new _PHSNode(nullptr, this);
}
_PHSDoc::_PHSDoc(String Path)
{
	if (Element != nullptr)
	{
		delete Element;
		Element = nullptr;
	}

	if (!FileExists(Path))
		return;

	_Path = Path;
	Element = new _PHSNode(nullptr, this);

	wifstream Stream(Path);
	if (!Stream)
		return;

	Element->Fill(Stream);
}
_PHSDoc::~_PHSDoc()
{
	delete Element;
	Element = nullptr;
	delete _ClonedNodes;
	_ClonedNodes = nullptr;
}
void _PHSDoc::Save() const
{
	if (!FileExists(_Path))
		return;

	wofstream File(_Path, ios::trunc);
	wchar_t Chars[1024];
	if (File.fail())
		return;
	File.rdbuf()->pubsetbuf(Chars, 1024);
	Element->Push(File);
}

PHSDoc LoadPHSDoc(String Path)
{
	if (!FileExists(Path))
		return nullptr;

	_PHSDoc* Return = new _PHSDoc(Path);
	return Return;
}
PHSDoc CreatePHSDoc(String Path, String Name)
{
	String Header = L"begin~" + Name + L'\n' + L"end~" + Name;
	wofstream Stream;
	Stream.open((wstring)Path, ios::trunc);
	if (Stream.fail())
		return nullptr;

	Stream << Header;

	Stream.close();

	_PHSDoc* Return = new _PHSDoc();
	Return->_Path = Path;
	PHSNode Root = Return->Element;
	Root->Name(Name);

	return Return;
}