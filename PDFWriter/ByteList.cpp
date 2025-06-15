#include "ByteList.h"

using namespace IOBasicTypes;

ByteList stringToByteList(const std::string& inString) {
	return ByteList(reinterpret_cast<const Byte *>(inString.data()), inString.size());
}

// are substr, append and concat really needed?
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
