#pragma once

#include "IOBasicTypes.h"

#include <string>
#include <vector>
#include <cstring>

// ByteList implementation with Small String Optimization (SSO)
// Uses stack-allocated array for sizes up to 22 bytes, vector for larger sizes
class ByteList {
private:
	static const size_t SSO_CAPACITY = 22;

	// SSO implementation class - encapsulates small buffer storage
	class SSOImpl {
	private:
		IOBasicTypes::Byte mBuffer[SSO_CAPACITY];
		size_t mSize;

	public:
		SSOImpl();
		SSOImpl(const IOBasicTypes::Byte* inData, size_t inSize);

		void push_back(IOBasicTypes::Byte inByte);
		size_t size() const;
		bool empty() const;
		void clear();
		const IOBasicTypes::Byte* data() const;
		IOBasicTypes::Byte* data();
		IOBasicTypes::Byte& operator[](size_t inIndex);
		const IOBasicTypes::Byte& operator[](size_t inIndex) const;

		typedef IOBasicTypes::Byte* iterator;
		typedef const IOBasicTypes::Byte* const_iterator;

		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;
	};

	SSOImpl mSSOBuffer;
	std::vector<IOBasicTypes::Byte> mVectorBuffer;
	bool mUsingSSO;

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
	ByteList operator+(const ByteList& inOther) const;
	bool operator==(const ByteList& inOther) const;
	bool operator!=(const ByteList& inOther) const;

private:
	void switchToVector();
};

ByteList stringToByteList(const std::string& inString);
ByteList substr(const ByteList& inList, IOBasicTypes::LongBufferSizeType inStart, IOBasicTypes::LongBufferSizeType inLength);
void append(ByteList& ioTargetList, const ByteList& inSource);
ByteList concat(const ByteList& inA, const ByteList& inB);
std::string ByteListToString(const ByteList& inByteList);

