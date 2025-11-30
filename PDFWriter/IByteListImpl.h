#pragma once

#include "IOBasicTypes.h"

// Interface for ByteList storage implementations
class IByteListImpl {
public:
	typedef IOBasicTypes::Byte* iterator;
	typedef const IOBasicTypes::Byte* const_iterator;

	virtual ~IByteListImpl() {}

	// Element access
	virtual IOBasicTypes::Byte& operator[](size_t inIndex) = 0;
	virtual const IOBasicTypes::Byte& operator[](size_t inIndex) const = 0;

	// Iterators
	virtual iterator begin() = 0;
	virtual const_iterator begin() const = 0;
	virtual iterator end() = 0;
	virtual const_iterator end() const = 0;

	// Capacity
	virtual size_t size() const = 0;
	virtual bool empty() const = 0;

	// Modifiers
	virtual void push_back(IOBasicTypes::Byte inByte) = 0;
	virtual void clear() = 0;
	virtual void append(const_iterator first, const_iterator last) = 0;

	// Data access
	virtual const IOBasicTypes::Byte* data() const = 0;
	virtual IOBasicTypes::Byte* data() = 0;

	// Clone for copy operations
	virtual IByteListImpl* clone() const = 0;

	// Check if this implementation can handle the given size
	virtual bool canHandle(size_t inSize) const = 0;
};
