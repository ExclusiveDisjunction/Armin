#include "PHS.h"

#include <iostream>
#include <fstream>
#include <ostream>

#include "..\Container.h"

using namespace std;

_PHSNode::_PHSNode()
{
	Doc = nullptr;
	_Parent = nullptr;
	_Args = nullptr;
	_Children = nullptr;
	NextObj = nullptr;
	LastObj = nullptr;
}
_PHSNode::_PHSNode(_PHSNode* Parent, PHSDoc Doc) : _PHSNode()
{
	this->_Parent = Parent;
	this->Doc = Doc;

	if (_Args)
		delete _Args;
	if (_Children)
		delete _Children;

	_Args = new _PHSArgCollection();
	_Children = new _PHSNodeList();
}
_PHSNode::_PHSNode(String Name) : _PHSNode()
{
	this->_Name = Name;
}
_PHSNode::~_PHSNode()
{
	this->_Parent = nullptr;

	if (_Args)
		delete _Args;
	if (_Children)
		delete _Children;
}

String _PHSNode::Name() const
{
	return _Name;
}
void _PHSNode::Name(String New)
{
	if (New == L"")
		throw std::exception("The name cannot be empty string.");
	_Name = New;
}

void _PHSNode::Push(wostream& out, uint TabIndex) const
{
	String TabIndexValue;
	for (uint i = 0; i < TabIndex; i++)
		TabIndexValue += L'\t';

	String Header = TabIndexValue + L"begin~" + _Name;
	out << Header;
	_Args->OutputAll(out);	

	if (HasChildren())
	{
		out << endl;
		_Children->OutputAll(out, TabIndex + 1);
		out << TabIndexValue + L"end~" + _Name << endl;
	}
	else
		out << L"~end" << endl;
}
void _PHSNode::Fill(wifstream& in)
{
	String Header;
	getline(in, Header);

	StringList Parts = Header.Split(L'~');
	bool Multiline = Parts[Parts.Size - 1] != L"end";
	_Name = Parts[1];
	int TabIndex = Header.TabIndex();

	if (Multiline)
	{
		while (!in.eof())
		{
			String ThisLine;
			streampos PrePos = in.tellg();
			getline(in, ThisLine);

			String Temp = ThisLine;
			Temp.RemoveAllOf(L'\t');
			if (Temp == L"")
				continue;

			StringList ThisParts = Temp.Split(L'~');
			if (ThisLine.TabIndex() == TabIndex && ThisParts[1] == _Name && ThisParts[0] == L"end")
				break;

			else if (ThisLine.TabIndex() - 1 == TabIndex)
			{
				in.seekg(PrePos);
				PHSNode NewNode = _Children->Add(this, Doc);
				NewNode->Fill(in);
			}
		}

		for (uint i = 2; i < Parts.Size; i++)
			AddArg(Parts[i]);
	}
	else
	{
		for (uint i = 2; i < Parts.Size - 1; i++)
			AddArg(Parts[i]);
	}
}

_PHSNode* _PHSNode::Clone() const
{
	_PHSNode* Return = Doc->_ClonedNodes->Add(nullptr, Doc);
	for (uint i = 0; i < _Args->Count(); i++)
		Return->AddArg((*_Args)[i]);
	for (uint i = 0; i < _Children->Count(); i++)
		Return->AddChild((*_Children)[i]);

	return Return;
}

PHSArg _PHSNode::AddArg(PHSArg Obj)
{
	PHSArg Return = _Args->Add(Obj->_Text);
	return Return;
}
PHSArg _PHSNode::AddArg(String Name, String Value)
{
	if (Name == L"")
		throw std::exception("The name cannot be empty string.");
	return AddArg(Name + L':' + Value);
}
PHSArg _PHSNode::AddArg(String Raw)
{
	PHSArg Arg = _Args->Add(Raw);
	return Arg;
}
bool _PHSNode::RemoveArg(int Index)
{
	return _Args->Remove(Index);
}
bool _PHSNode::RemoveArg(String Name)
{
	for (uint i = 0; i < _Args->Count(); i++)
	{
		if ((*_Args)[i]->Name() == Name)
			return _Args->Remove(i);
	}

	return false;
}

_PHSNode* _PHSNode::AddChild(_PHSNode* Obj)
{
	_PHSNode* Return = _Children->Add(this, Doc);
	Return->Doc = this->Doc;
	for (uint i = 0; i < Obj->_Args->Count(); i++)
		Return->AddArg((*Obj->_Args)[i]);
	for (uint i = 0; i < Obj->_Children->Count(); i++)
		Return->AddChild((*Obj->_Children)[i]);

	return Return;
}
_PHSNode* _PHSNode::AddChild(String Name)
{
	if (Name == L"")
		throw std::exception("The name cannot be empty string.");
	return AddChild(Name, Vector<String>());
}
_PHSNode* _PHSNode::AddChild(String Name, Vector<String> Args)
{
	_PHSNode* Return = _Children->Add(this, Doc);
	Return->Doc = this->Doc;
	Return->_Name = Name;
	for (uint i = 0; i < Args.Size; i++)
		Return->AddArg(Args[i]);

	return Return;
}
bool _PHSNode::RemoveChild(int Index)
{
	return _Children->Remove(Index);
}
bool _PHSNode::RemoveChild(String Name)
{
	bool Return = false;
	for (uint i = 0; i < _Children->Count(); i++)
	{
		if ((*_Children)[i]->Name() == Name)
			Return = _Children->Remove(i);
	}

	return Return;
}