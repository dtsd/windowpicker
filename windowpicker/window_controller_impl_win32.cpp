/*
    Application for picking right windows fast and easy way

	Copyright © 2004-2006 by Alexander Avdonin. All rights reserved.
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

#include "windowpicker/window_controller_impl.h"

#include <QList>
#include <QCoreApplication>

#define _WIN32_WINNT 0x0501

#include "windows.h"
#include "WinUser.h"

namespace WindowPicker {

const int MAX_TASK_CAPTION_SIZE = 200;
const int MAX_TASK_CLASS_NAME_SIZE = 200;


WindowInfoList WindowControllerImpl_taskInfoList;

QString WindowControllerImpl_getWindowCaption(HWND hwnd) {
	WCHAR pText[MAX_TASK_CAPTION_SIZE] = L"";
	InternalGetWindowText(hwnd, pText, MAX_TASK_CAPTION_SIZE);
	return QString::fromUtf16((ushort *)pText);
}

QString WindowControllerImpl_getWindowClassName(HWND hwnd) {
	WCHAR pText[MAX_TASK_CLASS_NAME_SIZE] = L"";
	GetClassName(hwnd, pText, MAX_TASK_CLASS_NAME_SIZE);
	return QString::fromUtf16((ushort *)pText);
}

BOOL CALLBACK WindowControllerImpl_updateWindowInfoListCallback(HWND hwnd, LPARAM /*lParam*/) 
{
//IsIconic
	//DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
	if(IsWindow(hwnd)) {
		if(IsWindowVisible(hwnd)) {
			QString caption = WindowControllerImpl_getWindowCaption(hwnd);
			if(!caption.isEmpty()) {
				QString className = WindowControllerImpl_getWindowClassName(hwnd);

				if(caption.compare(QCoreApplication::applicationName()) && className.compare("progman", Qt::CaseInsensitive)
					&& className.compare("shell_traywnd", Qt::CaseInsensitive)) {

					WindowInfo ti;
					ti.handle = (int)hwnd;
					ti.className = className;
					WindowControllerImpl_taskInfoList.push_back(ti);

				}
			}
		}
	}
	return true;
}


/*int WindowControllerImpl::taskCount() const {
	return WindowControllerImpl_taskInfoList.size();
}*/

void WindowControllerImpl::updateWindowInfoList() {
	WindowControllerImpl_taskInfoList.clear();

	EnumDesktopWindows(
		NULL, 
		WindowControllerImpl_updateWindowInfoListCallback, 
		NULL
	);
}

WindowHandleList WindowControllerImpl::windowHandleList() const {
	WindowHandleList list;
	for(QList<WindowInfo>::const_iterator it = WindowControllerImpl_taskInfoList.begin();
		it != WindowControllerImpl_taskInfoList.end(); ++it) {
		list.push_back((int)(*it).handle);
	}
	return list;
}

QString WindowControllerImpl::windowCaption(int handle) const {
	return WindowControllerImpl_getWindowCaption((HWND)handle)
		//+ QString(": ") + WindowControllerImpl_getWindowClassName((HWND)handle)
		;
}

QString WindowControllerImpl::windowDescription(int handle) const {
	return WindowControllerImpl_getWindowCaption((HWND)handle)
		//+ QString(": ") + WindowControllerImpl_getWindowClassName((HWND)handle)
		;
}

QString WindowControllerImpl::windowClassName(int handle) const {
	return WindowControllerImpl_getWindowClassName((HWND)handle)
		//+ QString(": ") + WindowControllerImpl_getWindowClassName((HWND)handle)
		;
}

QPixmap WindowControllerImpl::windowPixmap(int handle) const {
	QPixmap pixmap;

	HWND hwnd = (HWND)handle;
	HDC hdc = GetWindowDC(hwnd);
	if(hdc)
	{
		HDC hdcMem = CreateCompatibleDC(hdc);
		if(hdcMem)
		{
			RECT rc;
			GetWindowRect(hwnd, &rc);

			HBITMAP hbitmap = CreateCompatibleBitmap(hdc, rc.right - rc.left/*RECTWIDTH(rc)*/, rc.bottom - rc.top /*RECTHEIGHT(rc)*/);
			if (hbitmap)
			{
				SelectObject(hdcMem, hbitmap);

				PrintWindow(hwnd, hdcMem, 0);

				pixmap = QPixmap::fromWinHBITMAP(hbitmap);

				DeleteObject(hbitmap);
			}
			DeleteObject(hdcMem);
		}
		ReleaseDC(hwnd, hdc);
	}

	return pixmap;
}

QPixmap WindowControllerImpl::windowIcon(int handle) const {
	QPixmap pixmap;

	HWND hwnd = (HWND)handle;

	HICON hIcon = NULL;
	if (!SendMessageTimeout(hwnd, WM_GETICON, ICON_BIG, 0, 
		SMTO_ABORTIFHUNG, 250, (PDWORD_PTR)&hIcon)) {
		/*DWORD dwErr = GetLastError();
		if (dwErr == 0 || dwErr == 1460) {
			fIsHungApp = TRUE;
			goto _HUNG_ICON;
		}*/
	}
	if (!hIcon) 
		hIcon = (HICON)(UINT_PTR)GetClassLongPtr(hwnd, GCLP_HICON);

	if (!hIcon) {
		hIcon = LoadIcon(NULL, IDI_APPLICATION);
	}

	pixmap = QPixmap::fromWinHICON(hIcon);

	return pixmap;
}


BOOL MyIsHungAppWindow(HWND hwnd) {
    /*if (g_pfnIsHungAppWindow && g_pfnIsHungAppWindow(hwnd))
		return(TRUE);*/

	DWORD dw;
	LRESULT lResult = SendMessageTimeout(hwnd, WM_NULL, 0, 0, 
		SMTO_ABORTIFHUNG, 250 /*HUNG_TIMEOUT*/, &dw);
	if (lResult)
		return(FALSE);

	DWORD dwErr = GetLastError();
	return(dwErr == 0 || dwErr == 1460);
}

void WindowControllerImpl::selectWindow(int handle) {
	HWND hwnd = (HWND)handle;
	
	/*
	if(IsIconic(hwnd)) {
		SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
	}

	BringWindowToTop(hwnd);
	SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	SetForegroundWindow(hwnd);
	*/

	if(!IsWindow(hwnd) /*|| !IsWindowVisible(hwnd)*/) {
		return;
	}

	if(MyIsHungAppWindow(hwnd)) {
		return;
	}

	/*
	if (MyIsHungAppWindow(hwnd))
		return(FALSE);
		*/

	HWND hwndFrgnd = GetForegroundWindow();
	if (hwnd != hwndFrgnd) {

		BringWindowToTop(hwnd);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		if (!SetForegroundWindow(hwnd)) {

			if (!hwndFrgnd)
				hwndFrgnd = FindWindow(L"Shell_TrayWnd", NULL);

			DWORD idFrgnd = GetWindowThreadProcessId(hwndFrgnd, NULL);
			DWORD idSwitch = GetWindowThreadProcessId(hwnd, NULL);

			/*_RPT2(_CRT_WARN, "SetForegroundWindow1: idFrgnd = %d, idSwitch = %d\n", idFrgnd, idSwitch);*/

			AttachThreadInput(idFrgnd, idSwitch, TRUE);

			if (!SetForegroundWindow(hwnd)) {
				INPUT inp[4];
				ZeroMemory(&inp, sizeof(inp));
				inp[0].type = inp[1].type = inp[2].type = inp[3].type = INPUT_KEYBOARD;
				inp[0].ki.wVk = inp[1].ki.wVk = inp[2].ki.wVk = inp[3].ki.wVk = VK_MENU;
				inp[0].ki.dwFlags = inp[2].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
				inp[1].ki.dwFlags = inp[3].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
				SendInput(4, inp, sizeof(INPUT));

				SetForegroundWindow(hwnd);
			}

			AttachThreadInput(idFrgnd, idSwitch, FALSE);
		}
	}
	if (IsIconic(hwnd)) {
		PostMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
	}

	return;
}

bool WindowControllerImpl::isMinimized(int handle) const {
	HWND hwnd = (HWND)handle;
	return IsIconic(hwnd);
}

void WindowControllerImpl::cascadeWindows(const WindowHandleList &list) {
	HWND *pHwnd = (HWND*)HeapAlloc(GetProcessHeap(), 0, list.size() * sizeof(HWND));
	if (!pHwnd) return;
	int nHwnd = 0;
	for(WindowHandleList::const_iterator it = list.begin(); it != list.end(); ++it) {
		pHwnd[nHwnd++] = (HWND)(*it);
	}
	CascadeWindows(NULL, MDITILE_SKIPDISABLED, NULL, nHwnd, pHwnd);
	HeapFree(GetProcessHeap(), 0, pHwnd);
}

void WindowControllerImpl::tileWindows(const WindowHandleList &list, WindowController::ETile tile) {
	HWND *pHwnd = (HWND*)HeapAlloc(GetProcessHeap(), 0, list.size() * sizeof(HWND));
	if (!pHwnd) return;
	int nHwnd = 0;
	for(WindowHandleList::const_iterator it = list.begin(); it != list.end(); ++it) {
		pHwnd[nHwnd++] = (HWND)(*it);
	}
	TileWindows(
		NULL, 
		(tile == WindowController::ETile_Horizontally) ? MDITILE_HORIZONTAL : MDITILE_VERTICAL, 
		NULL, 
		list.size(), 
		pHwnd);
	HeapFree(GetProcessHeap(), 0, pHwnd);
}

QPixmap WindowControllerImpl::desktopPixmap() const {
	return windowPixmap((int)GetDesktopWindow());
}

void WindowControllerImpl::minimizeWindow(int handle) {
	HWND hwnd = (HWND)handle;

	DWORD dw;
	SendMessageTimeout(hwnd, WM_SYSCOMMAND, 
		SC_MINIMIZE, 0, SMTO_ABORTIFHUNG, 500, &dw);
}

void WindowControllerImpl::maximizeWindow(int handle) {
	HWND hwnd = (HWND)handle;

	DWORD dw;
	SendMessageTimeout(hwnd, WM_SYSCOMMAND, 
		SC_MAXIMIZE, 0, SMTO_ABORTIFHUNG, 500, &dw);
}

void WindowControllerImpl::closeWindow(int handle) {
	HWND hwnd = (HWND)handle;

	DWORD dw;
	SendMessageTimeout(hwnd, WM_SYSCOMMAND, 
		SC_CLOSE, 0, SMTO_ABORTIFHUNG, 500, &dw);
}

}
