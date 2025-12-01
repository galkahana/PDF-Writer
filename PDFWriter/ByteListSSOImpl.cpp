#include "ByteListSSOImpl.h"
#include <cstring>

using namespace IOBasicTypes;

ByteListSSOImpl::ByteListSSOImpl() : mSize(0) {
}

ByteListSSOImpl::ByteListSSOImpl(const Byte* inData, size_t inSize) : mSize(inSize) {
	if (inSize > 0) {
		std::memcpy(mBuffer, inData, inSize);
	}
}

template<typename InputIterator>
ByteListSSOImpl::ByteListSSOImpl(InputIterator inFirst, InputIterator inLast) : mSize(0) {
	for (InputIterator it = inFirst; it != inLast; ++it) {
		push_back(*it);
	}
}

// Explicit instantiation for pointer types
template ByteListSSOImpl::ByteListSSOImpl(const Byte*, const Byte*);
template ByteListSSOImpl::ByteListSSOImpl(Byte*, Byte*);

void ByteListSSOImpl::push_back(Byte inByte) {
	mBuffer[mSize++] = inByte;
}

size_t ByteListSSOImpl::size() const {
	return mSize;
}

bool ByteListSSOImpl::empty() const {
	return mSize == 0;
}

void ByteListSSOImpl::clear() {
	mSize = 0;
}

const Byte* ByteListSSOImpl::data() const {
	return mBuffer;
}

Byte* ByteListSSOImpl::data() {
	return mBuffer;
}

Byte& ByteListSSOImpl::operator[](size_t inIndex) {
	return mBuffer[inIndex];
}

const Byte& ByteListSSOImpl::operator[](size_t inIndex) const {
	return mBuffer[inIndex];
}

ByteListSSOImpl::iterator ByteListSSOImpl::begin() {
	return mBuffer;
}

ByteListSSOImpl::const_iterator ByteListSSOImpl::begin() const {
	return mBuffer;
}

ByteListSSOImpl::iterator ByteListSSOImpl::end() {
	return mBuffer + mSize;
}

ByteListSSOImpl::const_iterator ByteListSSOImpl::end() const {
	return mBuffer + mSize;
}

IByteListImpl* ByteListSSOImpl::clone() const {
	return new ByteListSSOImpl(*this);
}

bool ByteListSSOImpl::canHandle(size_t inSize) const {
	return inSize <= SSO_CAPACITY;
}

void ByteListSSOImpl::append(const_iterator first, const_iterator last) {
	for (const_iterator it = first; it != last; ++it) {
		push_back(*it);
	}
}
