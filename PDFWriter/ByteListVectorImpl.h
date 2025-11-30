#pragma once

#include "IByteListImpl.h"
#include "IOBasicTypes.h"
#include <vector>

// Vector-based implementation for ByteList
// Used for larger byte lists that exceed SSO capacity
class ByteListVectorImpl : public IByteListImpl {
public:
	ByteListVectorImpl();
	ByteListVectorImpl(const IOBasicTypes::Byte* inData, size_t inSize);
	template<typename InputIterator>
	ByteListVectorImpl(InputIterator inFirst, InputIterator inLast);

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
	std::vector<IOBasicTypes::Byte> mBuffer;    
};
