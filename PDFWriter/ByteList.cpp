#include "ByteList.h"
#include <algorithm>
#include <iterator>

using namespace IOBasicTypes;

// ===== SSOImpl Implementation =====

ByteList::SSOImpl::SSOImpl() : mSize(0) {
}

ByteList::SSOImpl::SSOImpl(const Byte* inData, size_t inSize) : mSize(inSize) {
	if (inSize > 0) {
		std::memcpy(mBuffer, inData, inSize);
	}
}

void ByteList::SSOImpl::push_back(Byte inByte) {
	mBuffer[mSize++] = inByte;
}

size_t ByteList::SSOImpl::size() const {
	return mSize;
}

bool ByteList::SSOImpl::empty() const {
	return mSize == 0;
}

void ByteList::SSOImpl::clear() {
	mSize = 0;
}

const Byte* ByteList::SSOImpl::data() const {
	return mBuffer;
}

Byte* ByteList::SSOImpl::data() {
	return mBuffer;
}

Byte& ByteList::SSOImpl::operator[](size_t inIndex) {
	return mBuffer[inIndex];
}

const Byte& ByteList::SSOImpl::operator[](size_t inIndex) const {
	return mBuffer[inIndex];
}

ByteList::SSOImpl::iterator ByteList::SSOImpl::begin() {
	return mBuffer;
}

ByteList::SSOImpl::const_iterator ByteList::SSOImpl::begin() const {
	return mBuffer;
}

ByteList::SSOImpl::iterator ByteList::SSOImpl::end() {
	return mBuffer + mSize;
}

ByteList::SSOImpl::const_iterator ByteList::SSOImpl::end() const {
	return mBuffer + mSize;
}

// ===== ByteList Implementation =====

ByteList::ByteList() : mUsingSSO(true) {
}

ByteList::ByteList(const Byte* inData, size_t inSize) {
	if (inSize <= SSO_CAPACITY) {
		mUsingSSO = true;
		mSSOBuffer = SSOImpl(inData, inSize);
	} else {
		mUsingSSO = false;
		mVectorBuffer.assign(inData, inData + inSize);
	}
}

template<typename InputIterator>
ByteList::ByteList(InputIterator inFirst, InputIterator inLast) {
	size_t distance = std::distance(inFirst, inLast);
	if (distance <= SSO_CAPACITY) {
		mUsingSSO = true;
		for (InputIterator it = inFirst; it != inLast; ++it) {
			mSSOBuffer.push_back(*it);
		}
	} else {
		mUsingSSO = false;
		mVectorBuffer.assign(inFirst, inLast);
	}
}

// Explicit instantiation for pointer types (most common case)
template ByteList::ByteList(const Byte*, const Byte*);
template ByteList::ByteList(Byte*, Byte*);

ByteList::ByteList(const ByteList& inOther) : mUsingSSO(inOther.mUsingSSO) {
	if (mUsingSSO) {
		mSSOBuffer = inOther.mSSOBuffer;
	} else {
		mVectorBuffer = inOther.mVectorBuffer;
	}
}

ByteList& ByteList::operator=(const ByteList& inOther) {
	if (this != &inOther) {
		mUsingSSO = inOther.mUsingSSO;
		if (mUsingSSO) {
			mSSOBuffer = inOther.mSSOBuffer;
			mVectorBuffer.clear();
		} else {
			mVectorBuffer = inOther.mVectorBuffer;
			mSSOBuffer.clear();
		}
	}
	return *this;
}

Byte& ByteList::operator[](size_t inIndex) {
	return mUsingSSO ? mSSOBuffer[inIndex] : mVectorBuffer[inIndex];
}

const Byte& ByteList::operator[](size_t inIndex) const {
	return mUsingSSO ? mSSOBuffer[inIndex] : mVectorBuffer[inIndex];
}

Byte& ByteList::back() {
	return mUsingSSO ? mSSOBuffer[mSSOBuffer.size() - 1] : mVectorBuffer.back();
}

const Byte& ByteList::back() const {
	return mUsingSSO ? mSSOBuffer[mSSOBuffer.size() - 1] : mVectorBuffer.back();
}

ByteList::iterator ByteList::begin() {
	return mUsingSSO ? mSSOBuffer.begin() : (mVectorBuffer.empty() ? nullptr : &mVectorBuffer[0]);
}

ByteList::const_iterator ByteList::begin() const {
	return mUsingSSO ? mSSOBuffer.begin() : (mVectorBuffer.empty() ? nullptr : &mVectorBuffer[0]);
}

ByteList::iterator ByteList::end() {
	return mUsingSSO ? mSSOBuffer.end() : (mVectorBuffer.empty() ? nullptr : &mVectorBuffer[0] + mVectorBuffer.size());
}

ByteList::const_iterator ByteList::end() const {
	return mUsingSSO ? mSSOBuffer.end() : (mVectorBuffer.empty() ? nullptr : &mVectorBuffer[0] + mVectorBuffer.size());
}

size_t ByteList::size() const {
	return mUsingSSO ? mSSOBuffer.size() : mVectorBuffer.size();
}

bool ByteList::empty() const {
	return mUsingSSO ? mSSOBuffer.empty() : mVectorBuffer.empty();
}

void ByteList::push_back(Byte inByte) {
	if (mUsingSSO) {
		if (mSSOBuffer.size() < SSO_CAPACITY) {
			mSSOBuffer.push_back(inByte);
		} else {
			switchToVector();
			mVectorBuffer.push_back(inByte);
		}
	} else {
		mVectorBuffer.push_back(inByte);
	}
}

void ByteList::clear() {
	if (mUsingSSO) {
		mSSOBuffer.clear();
	} else {
		mVectorBuffer.clear();
	}
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
	
	if (mUsingSSO && newSize > SSO_CAPACITY) {
		switchToVector();
	}

	if (mUsingSSO) {
		for (size_t i = 0; i < inSource.size(); ++i) {
			mSSOBuffer.push_back(inSource[i]);
		}
	} else {
		mVectorBuffer.insert(mVectorBuffer.end(), inSource.begin(), inSource.end());
	}
}

const Byte* ByteList::data() const {
	return mUsingSSO ? mSSOBuffer.data() : (mVectorBuffer.empty() ? nullptr : &mVectorBuffer[0]);
}

Byte* ByteList::data() {
	return mUsingSSO ? mSSOBuffer.data() : (mVectorBuffer.empty() ? nullptr : &mVectorBuffer[0]);
}

ByteList ByteList::operator+(const ByteList& inOther) const {
	ByteList result(*this);
	result.append(inOther);
	return result;
}

bool ByteList::operator==(const ByteList& inOther) const {
	if (size() != inOther.size()) {
		return false;
	}
	return std::memcmp(data(), inOther.data(), size()) == 0;
}

bool ByteList::operator!=(const ByteList& inOther) const {
	return !(*this == inOther);
}

void ByteList::switchToVector() {
	if (!mUsingSSO) {
		return;
	}
	
	mVectorBuffer.assign(mSSOBuffer.begin(), mSSOBuffer.end());
	mSSOBuffer.clear();
	mUsingSSO = false;
}

// ===== Helper Functions =====

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
	return inA + inB;
}

std::string ByteListToString(const ByteList& inByteList) {
	return std::string(reinterpret_cast<const char *>(inByteList.data()), inByteList.size());
}
