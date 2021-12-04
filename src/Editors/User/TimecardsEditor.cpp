#include "..\EditorFrame.h"

#include "Sort.h"
#include "Mapping.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\ComponentViewer.h"

namespace Armin::Editors::Users
{
	using namespace Files;
	using namespace UI;

	TimecardsEditor::TimecardsEditor(User* Query)
	{
		_TargetUser = Query;
		DateTime Now = DateTime::Now();
		_ThisMonth = Now.Month();
		_ThisYear = Now.Year();
	}

	LRESULT __stdcall TimecardsEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		TimecardsEditor* This = reinterpret_cast<TimecardsEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void TimecardsEditor::LoadControls()
	{
		if (_Loaded)
			return;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		LoadUpperButtons(WndRect, ins);

		int BaseYCoord = 110;
		AaColor BaseBk = EditorGrey;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.Alignment = TA_RightAlignment;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 4;
			Style.Radius = 20;

			Width = (WndRect.right - 10 - XCoord) / 2;
			XCoord += Width / 2;

			LastMonth = new Button(XCoord, YCoord, Height, Height, L"<", _Base, (HMENU)5, ins, Style, TextStyle);
			XCoord += Height + 5;
			Width = Width - (Height + 5) * 2;

			TextStyle.Alignment = TA_CenterAlignment;
			CurrentMonth = new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Month", Grey3, TextStyle, false);
			TextStyle.Alignment = TA_LeftAlignment;

			XCoord += Width + 5;
			NextMonth = new Button(XCoord, YCoord, Height, Height, L">", _Base, (HMENU)6, ins, Style, TextStyle);
			
			XCoord = 10;
			YCoord += 10 + Height;
			Width = (WndRect.right - 10 - XCoord - 30) / 7;
			TextStyle.Alignment = TA_CenterAlignment;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Sunday", BaseBk, TextStyle, false));
			XCoord += 5 + Width;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Monday", BaseBk, TextStyle, false));
			XCoord += 5 + Width;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Tuesday", BaseBk, TextStyle, false));
			XCoord += 5 + Width;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Wednesday", BaseBk, TextStyle, false));
			XCoord += 5 + Width;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Thursday", BaseBk, TextStyle, false));
			XCoord += 5 + Width;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Friday", BaseBk, TextStyle, false));
			XCoord += 5 + Width;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Saturday", BaseBk, TextStyle, false));

			XCoord = 10;
			BaseYCoord = (YCoord += 10 + Height);
			Height = (WndRect.bottom - 10 - YCoord - 20) / 6;
			TextStyle.Alignment = TA_LeftAlignment;

			for (uint i = 0; i < 6; i++, YCoord += Height + 5)
			{
				for (uint j = 0; j < 7; j++, XCoord += Width + 5)
					Grids.Add(new CalendarGrid(XCoord, YCoord, Width, Height, _Base, ins, (HMENU)5, 1, L"", Style, TextStyle));
				XCoord = 10;
			}
		}

		SetupMonth();
	}

	void TimecardsEditor::SetupMonth()
	{
		int q = 1;
		int m = _ThisMonth <= 2 ? _ThisMonth + 12 : _ThisMonth;
		int k = _ThisYear % 100;
		int j = _ThisYear / 100;

		int h = (q + ((13 * (m - 1)) / 5) + k + (k / 4) + (j / 4) - (2 * j));
		int d = ((h + 5) % 7);

		d--;
		for (uint i = 0; i < d; i++)
		{
			CalendarGrid* Obj = Grids[i];
			Obj->SetParams(0, L"Last Month");
			EnableWindow(*Obj, false);
		}

		int MaxCount = Grids.Size;
		String Month;
		switch (_ThisMonth)
		{
		case 1:
			MaxCount = 31;
			Month = L"January";
			break;
		case 2:
			MaxCount = 28;
			Month = L"February";
			break;
		case 3:
			MaxCount = 31;
			Month = L"March";
			break;
		case 4:
			MaxCount = 30;
			Month = L"April";
			break;
		case 5:
			MaxCount = 31;
			Month = L"May";
			break;
		case 6:
			MaxCount = 30;
			Month = L"June";
			break;
		case 7:
			MaxCount = 31;
			Month = L"July";
			break;
		case 8:
			MaxCount = 31;
			Month = L"August";
			break;
		case 9:
			MaxCount = 30;
			Month = L"September";
			break;
		case 10:
			MaxCount = 31;
			Month = L"October";
			break;
		case 11:
			MaxCount = 30;
			Month = L"November";
			break;
		case 12:
			MaxCount = 31;
			Month = L"December";
			break;
		}

		Month += L", " + String(_ThisYear);
		this->CurrentMonth->SetText(Month);

		for (uint i = 0, j = 1, k = d; i <= MaxCount, k < Grids.Size; i++, j++, k++)
		{
			CalendarGrid* Obj = Grids[k];
			Obj->SetDate(j);
			EnableWindow(*Obj, true);
			Obj->SetMemo(String());
		}

		for (uint i = MaxCount + d; i < Grids.Size; i++)
		{
			CalendarGrid* Obj = Grids[i];
			Obj->SetParams(0, L"Next Month");
			EnableWindow(*Obj, false);
		}

		FillRecords();
	}
	void TimecardsEditor::FillRecords()
	{
	}

	LRESULT TimecardsEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);

		int BaseYCoord = 110;

		{

		}

		return 0;
	}
	LRESULT TimecardsEditor::KeyDown(WPARAM wp)
	{
		return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
	}
	LRESULT TimecardsEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //Unused
		case 5: //Previous Month
		case 6: //Next Month
		{
			_ThisMonth++;
			_ThisYear += _ThisMonth / 13;
			_ThisMonth %= 12 + 1;

			SetupMonth();
			break;
		}
		case 7: //Click on Grid:
		{
			break;
		}
		}
		return 0;
	}

	Vector<void*> TimecardsEditor::CondenseArgs() const
	{
		return { _TargetUser };
	}
	bool TimecardsEditor::TestOnCondition(Vector<void*> Args) const
	{
		return CondenseArgs() == Args;
	}
	bool TimecardsEditor::EquatableTo(EditorFrame* Other) const
	{
		TimecardsEditor* This = dynamic_cast<TimecardsEditor*>(Other);
		return This && This->_TargetUser == _TargetUser;
	}
	void TimecardsEditor::Reset()
	{

	}
}