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
#if !defined(WINDOWPICKER_UNIQUE_RUNNABLE_H)
#define WINDOWPICKER_UNIQUE_RUNNABLE_H

#include <QRunnable>
#include <set>

namespace WindowPicker {

typedef std::set<int> HandleSet;

template <class T>
class UniqueRunnable : public QRunnable {
public:
	UniqueRunnable(int handle);
	virtual ~UniqueRunnable();

	virtual void run() {
		if(m_handle) {
			this->uniqueRun(m_handle);
		}
	};

	virtual void uniqueRun(int handle) = 0;

	//int handle() const { return m_handle; };
private:
	int m_handle;
};

}

#endif
