#include "CSV.h"

#include <iostream>
#include <fstream>
#include "Stream.h"

_CSVDoc* LoadCSVDoc(String Path)
{
	if (!FileExists(Path))
		return nullptr;

	return new _CSVDoc(Path);
}
_CSVDoc* CreateCSVDoc(String Path)
{
	wofstream Stream(Path, std::ios::trunc);
	if (!Stream)
		return nullptr;

	Stream.close();
	return new _CSVDoc(Path);
}

_CSVDoc::_CSVDoc(String Path)
{
	Loaded = FileExists(Path);
	_Lines = nullptr;
	if (Loaded)
	{
		StringList Lines;
		std::wifstream Stream(Path);
		String Current;
		while (getline(Stream, Current))
			Lines.Add(Current);
		_Lines = new _CSVLineList(Lines);
	}
}
_CSVDoc::~_CSVDoc()
{
	if (Loaded)
		delete _Lines;

	_Lines = nullptr;
	_Path.Clear();
}

void _CSVDoc::Save()
{
	if (!Loaded)
		return;

	wofstream File(Path());

	for (uint i = 0; i < _Lines->Count() - 1; i++)
		File << _Lines->Item(i)->Raw() << std::endl;
	File << _Lines->Item(_Lines->Count() - 1);
}