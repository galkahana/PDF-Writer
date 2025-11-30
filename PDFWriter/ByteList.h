#pragma once

#include "IOBasicTypes.h"
#include "IByteListImpl.h"

#include <string>

#include "ByteListSSOImpl.h"
#include "ByteListVectorImpl.h"

// ByteList implementation using vector with fallback on Small String Optimization (SSO) for small sizes
class ByteList {
public:
	typedef IOBasicTypes::Byte* iterator;
	typedef const IOBasicTypes::Byte* const_iterator;

	// Constructors
	ByteList();
	ByteList(const IOBasicTypes::Byte* inData, size_t inSize);
	template<typename InputIterator>
	ByteList(InputIterator inFirst, InputIterator inLast);
	ByteList(const ByteList& inOther);
	ByteList& operator=(const ByteList& inOther);
	~ByteList();

	// Element access
	IOBasicTypes::Byte& operator[](size_t inIndex);
	const IOBasicTypes::Byte& operator[](size_t inIndex) const;
	IOBasicTypes::Byte& back();
	const IOBasicTypes::Byte& back() const;

	// Iterators
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;

	// Capacity
	size_t size() const;
	bool empty() const;

	// Modifiers
	void push_back(IOBasicTypes::Byte inByte);
	void clear();
	ByteList substr(size_t inStart, size_t inLength) const;
	void append(const ByteList& inSource);

	// Data access
	const IOBasicTypes::Byte* data() const;
	IOBasicTypes::Byte* data();

	// Operators
	bool operator==(const ByteList& inOther) const;

private:
	ByteListSSOImpl mSSOImpl;
	ByteListVectorImpl mVectorImpl;
	IByteListImpl* mImpl;  // Points to either mSSOImpl or mVectorImpl

	void switchToVector();
};

// helper functions
ByteList stringToByteList(const std::string& inString);
ByteList substr(const ByteList& inList, IOBasicTypes::LongBufferSizeType inStart, IOBasicTypes::LongBufferSizeType inLength);
void append(ByteList& ioTargetList, const ByteList& inSource);
ByteList concat(const ByteList& inA, const ByteList& inB);
std::string ByteListToString(const ByteList& inByteList);

