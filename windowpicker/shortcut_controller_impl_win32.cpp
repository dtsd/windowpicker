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
#include "windowpicker/shortcut_controller_impl.h"

#include "windowpicker/shortcut_controller.h"

#include <QKeySequence>

#define _WIN32_WINNT 0x0501

#include "windows.h"
#include "Winuser.h"

namespace WindowPicker {

int ShortcutControllerImpl_installedShortcuts = 0;
HHOOK ShortcutControllerImpl_hook = 0;
ShortcutCallback ShortcutControllerImpl_callback = 0;

QKeySequence ShortcutControllerImpl_vkToKeySequence(int vkCode, bool isCtlDown, bool isShiftDown, bool isAltDown, bool isMetaDown) {
	int qtKeyCode = 0;
	if(vkCode >= 0x30 && vkCode <= 0x39) /*digits*/  {
		qtKeyCode = vkCode;
	} else if (vkCode >= 0x41 && vkCode <= 0x5A) /*letters*/ {
		qtKeyCode = vkCode;
	} else if (vkCode == VK_TAB) {
		qtKeyCode = Qt::Key_Tab;
	} else if (vkCode == VK_ESCAPE) {
		qtKeyCode = Qt::Key_Escape;
	/*} else if(vkCode == VK_RMENU || vkCode == VK_LMENU) {
		qtKeyCode = Qt::Key_Alt;
		isAltDown = false;
	} else if(vkCode == VK_RCONTROL || vkCode == VK_LCONTROL) {
		qtKeyCode = Qt::Key_Control;
		isCtlDown = false;
	} else if(vkCode == VK_RSHIFT || vkCode == VK_LSHIFT) {
		qtKeyCode = Qt::Key_Shift;
		isShiftDown = false;*/
	} else {
		Q_ASSERT(false);
	}
	if(isCtlDown) {
		qtKeyCode += Qt::CTRL;
	}
	if(isShiftDown) {
		qtKeyCode += Qt::SHIFT;
	}
	if(isAltDown) {
		qtKeyCode += Qt::ALT;
	}
	if(isMetaDown) {
		qtKeyCode += Qt::META;
	}
	return QKeySequence(qtKeyCode);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	LRESULT lRet = 0;
	if(nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;
		switch(wParam)
		{
			case WM_KEYUP: 
			case WM_SYSKEYUP:
			case WM_KEYDOWN: 
			case WM_SYSKEYDOWN:
			{
				bool isDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);

				bool isCtlDown = (GetAsyncKeyState(VK_RCONTROL) & 0x8000)
					|| GetAsyncKeyState(VK_LCONTROL) & 0x8000;
				bool isShiftDown = (GetAsyncKeyState(VK_RSHIFT) & 0x8000)
					|| (GetAsyncKeyState(VK_LSHIFT) & 0x8000);
				bool isAltDown = (GetAsyncKeyState(VK_RMENU) & 0x8000)
					|| (GetAsyncKeyState(VK_LMENU) & 0x8000);
				bool isMetaDown = (GetAsyncKeyState(VK_RWIN) & 0x8000)
					|| (GetAsyncKeyState(VK_LWIN) & 0x8000);

				bool isAllowedKey = false;
				if((pKeyBoard->vkCode >= 0x30 && pKeyBoard->vkCode <= 0x39) /*digits*/ 
					|| (pKeyBoard->vkCode >= 0x41 && pKeyBoard->vkCode <= 0x5A) /*letters*/
					|| (pKeyBoard->vkCode == VK_TAB)
					|| (pKeyBoard->vkCode == VK_ESCAPE)
					)
				{
					isAllowedKey = true;
				}



				QKeySequence ks;
				if(isAllowedKey && (isCtlDown || isShiftDown || isAltDown || isMetaDown)) {
					ks = ShortcutControllerImpl_vkToKeySequence(pKeyBoard->vkCode, isCtlDown, isShiftDown, isAltDown, isMetaDown);
					if(ShortcutControllerImpl_callback 
						&& ShortcutControllerImpl_callback(ks, isDown)) {
						lRet = 1;
					}
					break;
				}

				switch(pKeyBoard->vkCode) {
					case VK_RMENU:
					case VK_LMENU:
						ks = QKeySequence(Qt::Key_Alt);
						break;
					case VK_RCONTROL:
					case VK_LCONTROL:
						ks = QKeySequence(Qt::Key_Control);
						break;
					case VK_RSHIFT:
					case VK_LSHIFT:
						ks = QKeySequence(Qt::Key_Shift);
						break;
					default:
						ks = QKeySequence();
				}

				if(!ks.isEmpty()) {
					if(ShortcutControllerImpl_callback 
						&& ShortcutControllerImpl_callback(ks, isDown)) {
						/*dont trap it*/
					}
				}

				break;
			}
			default:
				break;
		}
	} 
	return lRet ? lRet : CallNextHookEx(0, nCode, wParam, lParam);
}

bool ShortcutControllerImpl_installHook() {
	HINSTANCE appInstance = GetModuleHandle(0);
	// Set a global Windows Hook to capture keystrokes.
	ShortcutControllerImpl_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, appInstance, 0);
	return ShortcutControllerImpl_hook != 0;
}

bool ShortcutControllerImpl_removeHook() {
	return UnhookWindowsHookEx(ShortcutControllerImpl_hook) != 0;
}

ShortcutControllerImpl::ShortcutControllerImpl(ShortcutCallback callback) {
	ShortcutControllerImpl_callback = callback;
}

bool ShortcutControllerImpl::installShortcut(const QKeySequence &/*ks*/) {
	Q_ASSERT(ShortcutControllerImpl_installedShortcuts >= 0);

	if(ShortcutControllerImpl_installedShortcuts == 0) {
		if(!ShortcutControllerImpl_installHook()) {
			return false;
		}
	}
	ShortcutControllerImpl_installedShortcuts += 1;
	return true;
}

bool ShortcutControllerImpl::removeShortcut(const QKeySequence &/*ks*/) {
	if(ShortcutControllerImpl_installedShortcuts <= 0 ) {
		return false;
	}


	if(ShortcutControllerImpl_installedShortcuts == 1) {
		if(!ShortcutControllerImpl_removeHook()) {
			return false;
		}
	}

	ShortcutControllerImpl_installedShortcuts -= 1;
	return true;
}

}
