#pragma once

#include <chrono>
#include <thread>
#include <iostream>

namespace Armin::UI
{
	class WindowState
	{
	private:
		bool _Running;
		bool _Locked = false;

	public:
		WindowState(bool Inital = true) : _Running(true) {}

		operator bool()
		{
			while (_Locked)
				std::this_thread::sleep_for(std::chrono::milliseconds(100));

			_Locked = true;
			bool Return = _Running;
			_Locked = false;
			return Return;
		}
		WindowState& operator=(const bool& New)
		{
			while (_Locked)
				std::this_thread::sleep_for(std::chrono::milliseconds(100));

			_Locked = true;
			_Running = New;
			_Locked = false;

			return *this;
		}
	};
}