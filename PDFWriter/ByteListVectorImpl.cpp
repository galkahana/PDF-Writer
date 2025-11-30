#include "ByteListVectorImpl.h"
#include <algorithm>
#include <iterator>

using namespace IOBasicTypes;

ByteListVectorImpl::ByteListVectorImpl() {
}

ByteListVectorImpl::ByteListVectorImpl(const Byte* inData, size_t inSize) 
	: mBuffer(inData, inData + inSize) {
}

template<typename InputIterator>
ByteListVectorImpl::ByteListVectorImpl(InputIterator inFirst, InputIterator inLast)
	: mBuffer(inFirst, inLast) {
}

// Explicit instantiation for pointer types (most common case)
template ByteListVectorImpl::ByteListVectorImpl(const Byte*, const Byte*);
template ByteListVectorImpl::ByteListVectorImpl(Byte*, Byte*);

void ByteListVectorImpl::push_back(Byte inByte) {
	mBuffer.push_back(inByte);
}

size_t ByteListVectorImpl::size() const {
	return mBuffer.size();
}

bool ByteListVectorImpl::empty() const {
	return mBuffer.empty();
}

void ByteListVectorImpl::clear() {
	mBuffer.clear();
}

const Byte* ByteListVectorImpl::data() const {
	return mBuffer.empty() ? nullptr : &mBuffer[0];
}

Byte* ByteListVectorImpl::data() {
	return mBuffer.empty() ? nullptr : &mBuffer[0];
}

Byte& ByteListVectorImpl::operator[](size_t inIndex) {
	return mBuffer[inIndex];
}

const Byte& ByteListVectorImpl::operator[](size_t inIndex) const {
	return mBuffer[inIndex];
}

ByteListVectorImpl::iterator ByteListVectorImpl::begin() {
	return mBuffer.empty() ? nullptr : &mBuffer[0];
}

ByteListVectorImpl::const_iterator ByteListVectorImpl::begin() const {
	return mBuffer.empty() ? nullptr : &mBuffer[0];
}

ByteListVectorImpl::iterator ByteListVectorImpl::end() {
	return mBuffer.empty() ? nullptr : &mBuffer[0] + mBuffer.size();
}

ByteListVectorImpl::const_iterator ByteListVectorImpl::end() const {
	return mBuffer.empty() ? nullptr : &mBuffer[0] + mBuffer.size();
}

IByteListImpl* ByteListVectorImpl::clone() const {
	return new ByteListVectorImpl(*this);
}

bool ByteListVectorImpl::canHandle(size_t inSize) const {
	return true; // Vector can handle any size
}

void ByteListVectorImpl::append(const_iterator first, const_iterator last) {
	mBuffer.insert(mBuffer.end(), first, last);
}
