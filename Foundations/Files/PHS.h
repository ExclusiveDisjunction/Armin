#pragma once

/*
 * Copyright (C) Hollan Connor Sellars 2021
 *
 * PHS.h - A standard header for reading/writing files in Phosphor Script file.
 * 
 * To read/write to a file, first create a new PHSDoc instance by calling LoadPHSDoc or CreatePHSDoc. Never attempt to call the internal create/load functions of a PHSDoc.
 * ALL memory in PHS is dynamic (uses pointers). NEVER delete a PHSNodeList, PHSArgList, PHSNode, or PHSArg. To free the memory by a PHS Document, delete the PHSDoc instance and the rest of the memory WILL be freed afterwards.
 * 
 * DO NOT DISTRIBUTE
 * DO NOT MODIFY
 * 
 * Version: 2.1
*/

#include "..\Str.h"

class _PHSDoc;
class _PHSNode;
class _PHSNodeList;
class _PHSArg;
class _PHSArgCollection;

using std::wofstream;
using std::wostream;

template<typename T>
class PHSListBase //Represents the base class of a list in PHS.
{
public:
	virtual uint Count() const = 0;
	virtual T* operator[](uint Index) const = 0;
	virtual T* operator[](String Name) const = 0;
	virtual T* Item(uint Index) const
	{
		return operator[](Index);
	}
	virtual T* Item(String Name)
	{
		return operator[](Name);
	}

	virtual bool Contains(T* Obj) const = 0;
	virtual bool Contains(String Name) const = 0;
	virtual uint IndexOf(T* Obj) const = 0;
};

typedef class _PHSNodeList : public PHSListBase<_PHSNode> //A list of PHSNodes
{
private:
	_PHSNode* First;
	_PHSNode* Last;
	uint _Size;

	_PHSNode* GetAtIndex(uint Index) const;
	_PHSNode* Add(_PHSNode* Parent, _PHSDoc* Doc);
	bool Remove(uint Index);
	bool Remove(_PHSNode* Node);

	void Clear();

	void OutputAll(wostream& Out, uint TabIndex);

public:
	friend _PHSNode;
	friend _PHSDoc;

	_PHSNodeList();
	~_PHSNodeList();

	uint Count() const override
	{
		return _Size;
	}
	_PHSNode* operator[](uint Index) const override;
	_PHSNode* operator[](String Name) const override;
	_PHSNode* Item(uint Index) const
	{
		return operator[](Index);
	}
	_PHSNode* Item(String Name) const
	{
		return operator[](Name);
	}

	bool Contains(_PHSNode* Node) const override;
	bool Contains(String Name) const override;
	uint IndexOf(_PHSNode* Obj) const override;
} *PHSNodeList;
typedef class _PHSArgCollection : public PHSListBase<_PHSArg> //A list of PHSArgs
{
private:
	_PHSArg* First;
	_PHSArg* Last;
	uint _Size;

	_PHSArg* GetAtIndex(uint Index) const;
	_PHSArg* Add(String Raw);
	bool Remove(uint Index);
	bool Remove(_PHSArg* Node);

	void Clear();

	void OutputAll(wostream& Out);
public:
	friend _PHSArg;
	friend _PHSNode;

	_PHSArgCollection();
	~_PHSArgCollection();

	uint Count() const override;
	_PHSArg* operator[](uint Index) const override;
	_PHSArg* operator[](String Name) const override;

	bool Contains(_PHSArg* Node) const override;
	bool Contains(String Name) const override;
	uint IndexOf(_PHSArg* Obj) const override;
} *PHSArgCollection;

typedef class _PHSDoc //This is a document, which loads the original file, and gives access to the root node.
{
private:
	String _Path; //The path of the current document, used for saving

	_PHSNode* Element; //The root node, or the element. This node is the utmost parent.
	_PHSNodeList* _ClonedNodes; //When nodes are cloned, a copy of their address is saved here so that they can be deleted properly.

	_PHSDoc(); //Default Constructor, not used
	_PHSDoc(String Path); //Made private because if a path does not exist, the document should not be loaded. Use LoadPHSDoc or CreatePHSDoc.

public: 
	//All the friends
	friend _PHSArgCollection;
	friend _PHSNodeList;
	friend _PHSNode;

	friend _PHSDoc* CreatePHSDoc(String, String);
	friend _PHSDoc* LoadPHSDoc(String);

	~_PHSDoc();

	/*Saves the current document [CONST]*/
	void Save() const;
	/*Returns the path of the current document [CONST]*/
	String Path() const
	{
		return _Path;
	}
	/*Changes the current path to a new String.*/
	void ChangePath(String New)
	{
		_Path = New;
	}

	/*Returns the root node of the PHSDocument [CONST]*/
	_PHSNode* Root() const
	{
		return Element;
	}
} *PHSDoc;

/*Creates a new PHSDoc from scratch. When the document is created, the root node's name will be set to the 'Name' parameter. If the file cannot be created, nullptr is the return value.*/
PHSDoc CreatePHSDoc(String Path, String Name);
/*Loads a PHSDoc from a path. If the path does not exist, it returns nullptr.*/
PHSDoc LoadPHSDoc(String Path);

/*A PHSArg is the data storing structure of a PHSDocument. This stores a name and value.*/
typedef class _PHSArg
{
private:
	String _Text; //The text value of this argument. Stored in this format '[Name]:[Value]'.

	_PHSArg* NextObj; //The next argument in the memory sequence (Used by PHSArgCollection)
	_PHSArg* LastObj; //The last arguemnt in the memory sequence (Used by PHSArgCollection)

	_PHSArg() : NextObj(nullptr), LastObj(nullptr) {}
	_PHSArg(String Raw) : _PHSArg()
	{
		_Text = Raw;
	}
public:
	friend _PHSNode;
	friend _PHSArgCollection;

	/*Retrives the Value of the current PHSArg [CONST]*/
	String Value() const;
	/*Sets the Value of the current PHSArg.*/
	void Value(String New);
	/*Retrives the Name of the current PHSArg [CONST]*/
	String Name() const;
	/*Sets the Name of the current PHSArg.*/
	void Name(String NewName);
} *PHSArg;

typedef class _PHSNode //The heart of PHS. A node is a unit that has Arguments, and can have children. They store the data in the Document.
{
private:
	_PHSNode(_PHSNode* Parent, PHSDoc Doc); //Constructor used by PHSNodeList.
	String _Name; //The name of the current node.

	PHSDoc Doc; //The parent document (where the node comes from). This is needed for node cloning.
	_PHSNode* _Parent; //The idmediate parent of the current node.
	_PHSNode* NextObj; //The next node in memory (used by PHSNodeList)
	_PHSNode* LastObj; //The last node in memory (Used by PHSNodeList)

	_PHSArgCollection* _Args; //The arguments of this node.
	_PHSNodeList* _Children; //The children of this node.

	void Push(wostream& Out, uint TabIndex = 0u) const; //Outputs this node and its children (and so on) into the stream. This will throw an error if the name is empty string ("")
	void Fill(std::wifstream& infile); //Fills the data of this node from a string list, and enumerates its children (and so on).
public:
	friend _PHSDoc;
	friend _PHSNodeList;

	_PHSNode(); //Sets all values do defaults (nullptr, String())
	_PHSNode(String Name); //Sets all the values to defaults, and then sets a name.
	~_PHSNode(); //Deletes the children and arguments, and sets the Doc, Parent, Next, and Last values to nullptr.

	String Name() const; //Returns the name of the current node.
	void Name(String New); //Sets the name of the current node. If the new value is empty string (""), it throws an error.

	bool HasChildren() const //Determines if the current node has children.
	{
		return Degree() != 0;
	}
	bool IsLeafNode() const //Determines if the current node is a leaf node (lacks children).
	{
		return !HasChildren();
	}
	bool IsRootNode() const //Determines if the current node is the root node (the utmost node in the document).
	{
		return _Parent == nullptr;
	}
	int Degree() const //Returns the degree of the node (the # of children that it posesses)
	{
		return _Children->Count();
	}

	_PHSNode* Clone() const; //Clones the current node.

	_PHSArgCollection* Args() const { return _Args; } //Returns the arguemtns of this node.
	PHSArg AddArg(String Name, String Value); //Adds a new argument to the node with a value and name. If the name is empty string, it throws an error.
	PHSArg AddArg(String Raw); //Adds a new argument to the node based on a raw string value.
	PHSArg AddArg(PHSArg Obj); //Clones the other arguments values and adds the clone to this node.
	bool RemoveArg(int Index); //Removes an argument at an index. If the index is out of range, it returns false.
	bool RemoveArg(String Name); //Removes an arguemnt based on a name. If the name was not found, it returns false.

	_PHSNodeList* Children() const { return _Children; } //Returns the children of this node.
	_PHSNode* AddChild(_PHSNode* Other); //Adds a new node to this node, cloneing the other node.
	_PHSNode* AddChild(String Name); //Adds a new node to this node, using the name provided. if the name is empty string (""), an error occurs.
	_PHSNode* AddChild(String Name, Vector<String> Args); //Adds a new child using a name and a list of strings as the arguments. The arguments must be in thi format: '[Name]:[Value]'. 
	bool RemoveChild(int Index); //Removes a child at an index. If the index is out of range, it returns false.
	bool RemoveChild(String Name); //Removes a child based on a name. If the name was not found, it returns false.
} *PHSNode;