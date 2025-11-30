#pragma once

#include "IByteListImpl.h"
#include "IOBasicTypes.h"
#include <cstddef>

// Small String Optimization implementation for ByteList
// Encapsulates stack-allocated buffer storage for small byte lists
class ByteListSSOImpl : public IByteListImpl {
public:
	ByteListSSOImpl();
	ByteListSSOImpl(const IOBasicTypes::Byte* inData, size_t inSize);
	template<typename InputIterator>
	ByteListSSOImpl(InputIterator inFirst, InputIterator inLast);

	// IByteListImpl interface
	virtual void push_back(IOBasicTypes::Byte inByte);
	virtual size_t size() const;
	virtual bool empty() const;
	virtual void clear();
	virtual void append(const_iterator first, const_iterator last);
	virtual const IOBasicTypes::Byte* data() const;
	virtual IOBasicTypes::Byte* data();
	virtual IOBasicTypes::Byte& operator[](size_t inIndex);
	virtual const IOBasicTypes::Byte& operator[](size_t inIndex) const;

	virtual iterator begin();
	virtual const_iterator begin() const;
	virtual iterator end();
	virtual const_iterator end() const;

	virtual IByteListImpl* clone() const;
	virtual bool canHandle(size_t inSize) const;

private:
	static const size_t SSO_CAPACITY = 22;
	IOBasicTypes::Byte mBuffer[SSO_CAPACITY];
	size_t mSize;

};
