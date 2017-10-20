/*
 * Memoryleak.h
 *
 *  Created on: 21 apr. 2017
 *      Author: Acer
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
