/*
 * Memoryleak.cpp
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

#include <Memoryleak.h>
#include <iostream>

Memory_leak Memory_leak::current;

Memory_leak::Memory_leak()
	: m_nPointers(0)
	, m_nPtrSizes(0)
{
	m_pointers = (void**)std::malloc(0);
	m_ptrSize = (std::size_t *)std::malloc(0);
}

Memory_leak::Memory_leak(Memory_leak &item)
	: m_nPointers(item.m_nPointers)
	, m_nPtrSizes(item.m_nPtrSizes)
{
	int i;

	//allocate memory
	m_pointers = (void **)std::malloc(item.m_nPointers * sizeof(void *));
	m_ptrSize = (std::size_t *)std::malloc(item.m_nPtrSizes * sizeof(std::size_t));

	//copy items
	for (i = 0; i < m_nPointers; ++i) {
		m_pointers[i] = item.m_pointers[i];
		m_ptrSize[i] = item.m_ptrSize[i];
	}
}

Memory_leak::Memory_leak(Memory_leak &&item)
	: m_nPointers(item.m_nPointers)
	, m_nPtrSizes(item.m_nPtrSizes)
	, m_pointers(item.m_pointers)
	, m_ptrSize(item.m_ptrSize)
{
	item.m_nPointers = 0;
	item.m_nPtrSizes = 0;
	item.m_pointers = nullptr;
	item.m_ptrSize = nullptr;
}

Memory_leak::~Memory_leak() {
	std::free(m_pointers);
	std::free(m_ptrSize);
}

Memory_leak &Memory_leak::operator =(const Memory_leak &item){
	int i;

	if ( this != &item )
	{
		m_nPointers = item.m_nPointers;
		m_nPtrSizes = item.m_nPtrSizes;

		//allocate memory
		m_pointers = (void **)std::malloc(item.m_nPointers * sizeof(void *));
		m_ptrSize = (std::size_t *)std::malloc(item.m_nPtrSizes * sizeof(std::size_t));

		//copy items
		for (i = 0; i < m_nPointers; ++i) {
			m_pointers[i] = item.m_pointers[i];
			m_ptrSize[i] = item.m_ptrSize[i];
		}
	}

	return *this;
}

void *Memory_leak::reserve(std::size_t size){
	void *ptr = std::malloc(size);

	//getchar();

	m_mtx.lock();
	//add pointer to array
	m_pointers = (void**)std::realloc(m_pointers, (m_nPointers + 1) * sizeof(void *));
	m_pointers[m_nPointers++] = ptr;

	//add pointer size to array
	m_ptrSize = (std::size_t *)std::realloc(m_ptrSize, (m_nPtrSizes + 1) * sizeof(std::size_t));
	m_ptrSize[m_nPtrSizes++] = size;

	m_mtx.unlock();
	return ptr;
}

void Memory_leak::free(void *ptr){
	int i;
	bool found = false;

	m_mtx.lock();
	//find ptr and move all items behind it to the front
	m_nPointers--;
	m_nPtrSizes--;
	for (i = 0; i < m_nPointers; ++i) {
		if (m_pointers[i] == ptr)
			found = true;

		if (found){
			m_pointers[i] = m_pointers[i + 1];
			m_ptrSize[i] = m_ptrSize[i + 1];
		}
	}

	//resize array
	m_pointers = (void **)std::realloc(m_pointers, m_nPointers * sizeof(void *));
	m_ptrSize = (std::size_t *)std::realloc(m_ptrSize, m_nPtrSizes * sizeof(std::size_t));
	m_mtx.unlock();

	std::free(ptr);
}


/**
 * Check if there are memory leaks until that point.
 *
 * return value: true if there are leaks, false if not
 */
bool Memory_leak::check() {
	int i;

	if (m_nPointers > 0) {
		std::cout << "Follow memory adresses are reserved: " << std::endl;
		for (i = 0; i < m_nPointers; ++i) {
			std::cout << m_pointers[i] << ": " << m_ptrSize[i] << "byte(s)"
					<< std::endl;
		}
		return true;
	} else {
		std::cout << "No memory reserved" << std::endl;
		return false;
	}
}

/**
 * Check if there are memory leaks until that point. All leaks
 * that are found, will be removed
 *
 * return value: true if there where leaks, false if not
 */
bool Memory_leak::checkAndFree() {
	int i;

	if (m_nPointers > 0) {
		std::cout << "Leak found!" << std::endl;
		for (i = 0; i < m_nPointers; ++i) {
			std::cout << m_pointers[i] << ": " << m_ptrSize[i] << "byte(s)"
					<< std::endl;
			std::free(m_pointers[i]);
		}

		std::cout << "Leak are clean up" << std::endl;
		return true;
	} else {
		std::cout << "No leaks found\n";
		return false;
	}
}

void *operator new(std::size_t size){
	return Memory_leak::current.reserve(size);
}

void operator delete(void *ptr) noexcept
{
	Memory_leak::current.free(ptr);
}
