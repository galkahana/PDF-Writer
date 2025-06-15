#pragma once

#include "IOBasicTypes.h"

#include <string>

typedef std::basic_string<IOBasicTypes::Byte> ByteList;


ByteList stringToByteList(const std::string& inString);
ByteList substr(const ByteList& inList, IOBasicTypes::LongBufferSizeType inStart, IOBasicTypes::LongBufferSizeType inLength);
void append(ByteList& ioTargetList, const ByteList& inSource);
ByteList concat(const ByteList& inA, const ByteList& inB);
std::string ByteListToString(const ByteList& inByteList);

