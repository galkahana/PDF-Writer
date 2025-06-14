#include "ByteList.h"

using namespace IOBasicTypes;

ByteList stringToByteList(const std::string& inString) {
	ByteList buffer;
	std::string::const_iterator it = inString.begin();

	for (; it != inString.end(); ++it)
		buffer.push_back((Byte)*it);

	return buffer;
}

ByteList substr(const ByteList& inList, IOBasicTypes::LongBufferSizeType inStart, IOBasicTypes::LongBufferSizeType inLength) {
	ByteList buffer;
	ByteList::const_iterator it = inList.begin();

	for (IOBasicTypes::LongBufferSizeType i = 0; i < inStart && it != inList.end(); ++i, ++it);

	for (IOBasicTypes::LongBufferSizeType i = 0; i < inLength && it != inList.end(); ++i, ++it)
		buffer.push_back((Byte)*it);

	return buffer;
}

void append(ByteList& ioTargetList, const ByteList& inSource) {
	ByteList::const_iterator it = inSource.begin();

	for (; it != inSource.end(); ++it)
		ioTargetList.push_back(*it);
}

ByteList concat(const ByteList& inA, const ByteList& inB) {
	ByteList buffer;

	append(buffer, inA);
	append(buffer, inB);

	return buffer;
}


std::string ByteListToString(const ByteList& inByteList) {
	std::string buffer;
	ByteList::const_iterator it = inByteList.begin();

	for (; it != inByteList.end(); ++it)
		buffer.push_back((char)*it);

	return buffer;
}