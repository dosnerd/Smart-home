/*
 * Memoryleak.h
 *
 *  Created on: 21 apr. 2017
 *      Author: David de Prez
 *
 *
 * Copyright (C) 2017  David de Prez
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TEST_INC_MEMORYLEAK_H_
#define TEST_INC_MEMORYLEAK_H_

#include <cstdio>
#include <mutex>

class Memory_leak {
public:
								Memory_leak();
								Memory_leak(Memory_leak &item);
								Memory_leak(Memory_leak &&item);
	virtual 					~Memory_leak();

public:
	virtual Memory_leak			&operator=(const Memory_leak &item);

public:
	virtual void				*reserve(std::size_t size);
	virtual void				free(void *ptr);
	virtual bool				check();
	virtual bool				checkAndFree();

public:
	static Memory_leak			current;

private:
	int							m_nPointers;
	int							m_nPtrSizes;

	void						**m_pointers;
	std::size_t					*m_ptrSize;

	std::mutex					m_mtx;
};

extern void *operator new(std::size_t size);
extern void operator delete(void *ptr) noexcept;

#endif /* TEST_INC_MEMORYLEAK_H_ */
