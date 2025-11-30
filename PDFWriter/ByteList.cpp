#include "ByteList.h"
#include "ByteListSSOImpl.h"
#include "ByteListVectorImpl.h"
#include <algorithm>
#include <iterator>
#include <cstring>

using namespace IOBasicTypes;

ByteList::ByteList() : mImpl(new ByteListSSOImpl()) {
}

ByteList::~ByteList() {
	delete mImpl;
}

ByteList::ByteList(const Byte* inData, size_t inSize) {
	ByteListSSOImpl ssoImpl;
	if (ssoImpl.canHandle(inSize)) {
		mImpl = new ByteListSSOImpl(inData, inSize);
	} else {
		mImpl = new ByteListVectorImpl(inData, inSize);
	}
}

template<typename InputIterator>
ByteList::ByteList(InputIterator inFirst, InputIterator inLast) {
	size_t distance = std::distance(inFirst, inLast);
	ByteListSSOImpl tempImpl;
	if (tempImpl.canHandle(distance)) {
		mImpl = new ByteListSSOImpl(inFirst, inLast);
	} else {
		mImpl = new ByteListVectorImpl(inFirst, inLast);
	}
}

// Explicit instantiation for pointer types (most common case)
template ByteList::ByteList(const Byte*, const Byte*);
template ByteList::ByteList(Byte*, Byte*);

ByteList::ByteList(const ByteList& inOther) : mImpl(inOther.mImpl->clone()) {
}

ByteList& ByteList::operator=(const ByteList& inOther) {
	if (this != &inOther) {
		delete mImpl;
		mImpl = inOther.mImpl->clone();
	}
	return *this;
}

Byte& ByteList::operator[](size_t inIndex) {
	return (*mImpl)[inIndex];
}

const Byte& ByteList::operator[](size_t inIndex) const {
	return (*mImpl)[inIndex];
}

Byte& ByteList::back() {
	return (*mImpl)[mImpl->size() - 1];
}

const Byte& ByteList::back() const {
	return (*mImpl)[mImpl->size() - 1];
}

ByteList::iterator ByteList::begin() {
	return mImpl->begin();
}

ByteList::const_iterator ByteList::begin() const {
	return mImpl->begin();
}

ByteList::iterator ByteList::end() {
	return mImpl->end();
}

ByteList::const_iterator ByteList::end() const {
	return mImpl->end();
}

size_t ByteList::size() const {
	return mImpl->size();
}

bool ByteList::empty() const {
	return mImpl->empty();
}

void ByteList::push_back(Byte inByte) {
	if (!mImpl->canHandle(mImpl->size() + 1)) {
		switchToVector();
	}
	mImpl->push_back(inByte);
}

void ByteList::clear() {
	mImpl->clear();
}

ByteList ByteList::substr(size_t inStart, size_t inLength) const {
	size_t currentSize = size();
	if (inStart >= currentSize) {
		return ByteList();
	}
	size_t actualLength = std::min(inLength, currentSize - inStart);
	return ByteList(data() + inStart, actualLength);
}

void ByteList::append(const ByteList& inSource) {
	if (inSource.empty()) {
		return;
	}

	size_t newSize = size() + inSource.size();
	
	if (!mImpl->canHandle(newSize)) {
		switchToVector();
	}

	mImpl->append(inSource.begin(), inSource.end());
}

const Byte* ByteList::data() const {
	return mImpl->data();
}

Byte* ByteList::data() {
	return mImpl->data();
}

bool ByteList::operator==(const ByteList& inOther) const {
	if (size() != inOther.size()) {
		return false;
	}
	return std::memcmp(data(), inOther.data(), size()) == 0;
}

void ByteList::switchToVector() {
	// Assume we need to switch - caller already determined this
	ByteListVectorImpl* newImpl = new ByteListVectorImpl(mImpl->begin(), mImpl->end());
	delete mImpl;
	mImpl = newImpl;
}


ByteList stringToByteList(const std::string& inString) {
	return ByteList(reinterpret_cast<const Byte *>(inString.data()), inString.size());
}

ByteList substr(const ByteList& inList, IOBasicTypes::LongBufferSizeType inStart, IOBasicTypes::LongBufferSizeType inLength) {
	return inList.substr(inStart, inLength);
}

void append(ByteList& ioTargetList, const ByteList& inSource) {
	ioTargetList.append(inSource);
}

ByteList concat(const ByteList& inA, const ByteList& inB) {
	ByteList result(inA);
	result.append(inB);
	return result;
}

std::string ByteListToString(const ByteList& inByteList) {
	return std::string(reinterpret_cast<const char *>(inByteList.data()), inByteList.size());
}
