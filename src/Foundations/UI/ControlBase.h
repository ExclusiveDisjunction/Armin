#pragma once

#include <Windows.h>
#include "..\Color.h"
#include "..\Str.h"

#define WM_SPECIALCOMMAND 0x9F50
#define SpecialCommand_RightClick 0x1
#define SpecialCommand_DoubleClick 0x2

enum TextAlignment
{
    TA_CenterAlignment,
    TA_LeftAlignment,
    TA_RightAlignment
};

struct StyleSheet
{
    StyleSheet()
    {
        Background = 0xFF000000;
        BorderBrush = 0xFF000000;
        BorderThickness = 0;
        Radius = 0;
    }

    AaColor Background;
    AaColor BorderBrush;
    AaColor BaseBackground;
    int BorderThickness;
    int Radius;
};
struct TextStyleSheet
{
    AaColor Foreground = 0xFFFFFFFF;
    String FontFamily;
    int FontSize = 12, CharSpacing = 0;
    bool Bold = false;
    bool Italic = false;
    bool Underline = false;

    TextAlignment Alignment = TextAlignment::TA_LeftAlignment;
};

class ControlBase
{
protected:
    HWND _Base = nullptr;
    bool IsEnabled = true;

   static LRESULT __stdcall CommonWndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);

   virtual bool FocusOnClick() const { return true; }
public:
    ControlBase()
    {
        _Base = nullptr;
    }
    virtual ~ControlBase()
    {
        SetWindowLongPtrW(_Base, GWLP_USERDATA, 0);
        DestroyWindow(_Base);
    }

    virtual LRESULT KeyDown(WPARAM key);
    virtual LRESULT Create() { return 0; }
    virtual LRESULT Paint() { return 0; }
    virtual LRESULT OnDestroy() { return 0; }
    virtual LRESULT Click() { return 0; }
    virtual LRESULT DoubleClick() { return SendMessageW(GetParent(_Base), WM_SPECIALCOMMAND, MAKEWPARAM(SpecialCommand_DoubleClick, GetMenu(_Base)), reinterpret_cast<LPARAM>(this)); }
    virtual LRESULT MouseUp() { return 0; }
    virtual LRESULT TimerTick(LONG ID) { return 0; }
    virtual LRESULT Char(WPARAM ID) { return 0; }
    virtual LRESULT Command(WPARAM ID, LPARAM lp) { return SendMessageW(GetParent(_Base), WM_COMMAND, ID, lp); }
    virtual LRESULT Size() { return 0; }
    virtual LRESULT GotFocus() { return 0; }
    virtual LRESULT LostFocus() { return 0; }
    virtual LRESULT RightClick() { return SendMessageW(GetParent(_Base), WM_SPECIALCOMMAND, MAKEWPARAM(SpecialCommand_RightClick, GetMenu(_Base)), reinterpret_cast<LPARAM>(this)); }
    virtual LRESULT SpecialCommand(HMENU ID, uint Command, LPARAM Sender) { return 0; }

    bool Enable()
    {
        return IsEnabled;
    }
    void Enable(bool New)
    {
        IsEnabled = New;
        EnableWindow(_Base, New);
        Redraw();
    }

    virtual void Redraw()
    {
        RedrawWindow(_Base, NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW);
    }
    virtual void Move(int X, int Y, int Width, int Height)
    {
        MoveWindow(_Base, X, Y, Width, Height, TRUE);
    }

    void Placement(RECT& WndRect) const
    {
        GetWindowRect(_Base, &WndRect);
    }
    int XCoord() const
    {
        RECT Place;
        Placement(Place);

        MapWindowPoints(HWND_DESKTOP, GetParent(_Base), reinterpret_cast<LPPOINT>(&Place), 2);

        return Place.left;
    }
    int YCoord() const
    {
        RECT Place;
        Placement(Place);

        MapWindowPoints(HWND_DESKTOP, GetParent(_Base), reinterpret_cast<LPPOINT>(&Place), 2);

        return Place.top;
    }

    operator HWND() 
    {
        return _Base;
    }
};

void GetClientRect(ControlBase* Source, LPRECT Dest);
void GetWindowRect(ControlBase* Source, LPRECT Dest);

#ifdef Aaron_Container
/**
* For every item in the Vector, it deletes the items provided. After the process is complete, the Items will be empty. 
* 
* @param Items The ControlBases to destroy. T Must be a ControlBase derived class, or a ControlBase.
*/
template<typename T>
void CloseControls(Vector<T*>& Items)
{
    for (uint i = 0; i < Items.Size; i++)
    {
        T*& item = Items[i];

        if (!item)
            return;

        SetWindowLongPtrW(*item, GWLP_USERDATA, 0);
        item->OnDestroy();
        delete item;
        item = nullptr;
    }
    Items.Clear();
}
template<typename T>
void EnableControls(Vector<T*> Items, bool IsEnabled)
{
    for (uint i = 0; i < Items.Size; i++)
        EnableWindow(*Items[i], IsEnabled);
}
#endif