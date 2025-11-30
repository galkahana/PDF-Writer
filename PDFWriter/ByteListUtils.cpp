#include "ByteListUtils.h"

using namespace IOBasicTypes;

namespace {
	// IsVector helps understanding if we're in vector<byte> or basic_string<byte>
	template <typename T>
	struct IsVector {
		enum { value = 0 };
	};

	template <typename T>
	struct IsVector<std::vector<T> > {
		enum { value = 1 };
	};

	// Yes, isVector, custom implementations of basic_string stuff
	template <bool UseVector>
	struct ByteListOps {
		static ByteList stringToByteList(const std::string& inString) {
			const Byte* data = reinterpret_cast<const Byte *>(inString.data());
			return ByteList(data, data + inString.size());
		}
		
		static ByteList substr(const ByteList& inList, LongBufferSizeType inStart, LongBufferSizeType inLength) {
			if (inStart >= inList.size())
				return ByteList();
			
			LongBufferSizeType actualLength = (inStart + inLength > inList.size()) 
				? (inList.size() - inStart) 
				: inLength;
			
			return ByteList(inList.begin() + inStart, inList.begin() + inStart + actualLength);
		}
		
		static void append(ByteList& ioTargetList, const ByteList& inSource) {
			ioTargetList.insert(ioTargetList.end(), inSource.begin(), inSource.end());
		}
		
		static ByteList concat(const ByteList& inA, const ByteList& inB) {
			ByteList result;
			result.reserve(inA.size() + inB.size());
			result.insert(result.end(), inA.begin(), inA.end());
			result.insert(result.end(), inB.begin(), inB.end());
			return result;
		}
	};

	// Nope - so can use basic_string stuff
	template <>
	struct ByteListOps<false> {
		static ByteList stringToByteList(const std::string& inString) {
			return ByteList(reinterpret_cast<const Byte *>(inString.data()), inString.size());
		}
		
		static ByteList substr(const ByteList& inList, LongBufferSizeType inStart, LongBufferSizeType inLength) {
			return inList.substr(inStart, inLength);
		}
		
		static void append(ByteList& ioTargetList, const ByteList& inSource) {
			ioTargetList.append(inSource);
		}
		
		static ByteList concat(const ByteList& inA, const ByteList& inB) {
			return inA + inB;
		}
	};

	typedef ByteListOps<IsVector<ByteList>::value> Ops;
}

ByteList stringToByteList(const std::string& inString) {
	return Ops::stringToByteList(inString);
}

ByteList substr(const ByteList& inList, IOBasicTypes::LongBufferSizeType inStart, IOBasicTypes::LongBufferSizeType inLength) {
	return Ops::substr(inList, inStart, inLength);
}

void append(ByteList& ioTargetList, const ByteList& inSource) {
	Ops::append(ioTargetList, inSource);
}

ByteList concat(const ByteList& inA, const ByteList& inB) {
	return Ops::concat(inA, inB);
}


// Common implementation - both vector and basic_string support this
std::string ByteListToString(const ByteList& inByteList) {
	return std::string(reinterpret_cast<const char *>(inByteList.data()), inByteList.size());
}
