/*
    Application for picking right windows fast and easy way

    Copyright (C) 2010  Dmitry Teslenko

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "windowpicker/mouse_controller_impl.h"

#include "windowpicker/mouse_controller.h"

#include <QPoint>

#define _WIN32_WINNT 0x0501

#include "windows.h"

namespace WindowPicker {

HHOOK MouseControllerImpl_hook = 0;
MouseCallback MouseControllerImpl_callback = 0;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	LRESULT lRet = 0;
	if(nCode == HC_ACTION) {
		switch(wParam)
		{
			case WM_MOUSEMOVE:
			{
				MSLLHOOKSTRUCT  *pMouse = (MSLLHOOKSTRUCT *)lParam;
				QPoint pt;
				pt.setX(pMouse->pt.x); 
				pt.setY(pMouse->pt.y); 
				if(MouseControllerImpl_callback 
					&& MouseControllerImpl_callback(pt)) {
				}
				//lRet = 1;
				break;
			}
			default:
				break;
		}
	} 
	return lRet ? lRet : CallNextHookEx(0, nCode, wParam, lParam);
}

bool MouseControllerImpl_installHook() {
	HINSTANCE appInstance = GetModuleHandle(0);
	// Set a global Windows Hook to capture keystrokes.
	MouseControllerImpl_hook = SetWindowsHookEx(
		WH_MOUSE_LL, 
		LowLevelMouseProc, 
		appInstance, 
		0
	);
	return MouseControllerImpl_hook != 0;
}

bool MouseControllerImpl_removeHook() {
	return UnhookWindowsHookEx(MouseControllerImpl_hook) != 0;
}

MouseControllerImpl::MouseControllerImpl(MouseCallback callback) {
	MouseControllerImpl_callback = callback;
}

bool MouseControllerImpl::install() {
	if(!MouseControllerImpl_installHook()) {
		return false;
	}
	return true;
}

bool MouseControllerImpl::remove() {
	if(!MouseControllerImpl_removeHook()) {
		return false;
	}
	return true;
}

}
