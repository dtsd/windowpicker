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
#if !defined(WINDOWPICKER_UNIQUE_RUNNABLE_INL_H)
#define WINDOWPICKER_UNIQUE_RUNNABLE_INL_H

#include "windowpicker/unique_runnable.h"

#include <QMutex>

namespace WindowPicker {

template <class T>
UniqueRunnable<T>::UniqueRunnable(int handle) {
	m_handle = 0;
	{
		QMutexLocker locker(&T::s_lock);
		if(T::s_handleSet.count(handle)) {
			return;
		}

		m_handle = handle;
		T::s_handleSet.insert(m_handle);
	}
}

template <class T>
UniqueRunnable<T>::~UniqueRunnable() {
	QMutexLocker locker(&T::s_lock);
	T::s_handleSet.erase(m_handle);
}	

}


#endif
