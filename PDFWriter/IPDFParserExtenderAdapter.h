#pragma once

#include "IPDFParserExtender.h"


class IPDFParserExtenderAdapter
	: public IPDFParserExtender
{
public:

	// for extending fliter support in stream read
	IByteReader* CreateFilterForStream(IByteReader* inStream,PDFName* inFilterName,PDFDictionary* inDecodeParams) 
	{ 
		return inStream; 
	}

	// for decryption extension

	bool DoesSupportEncryption() { return false; }

	IByteReader* CreateDecryptionFilterForStream(IByteReader* inStream) { return inStream; }

	std::string DecryptString(std::string inStringToDecrypt) { return inStringToDecrypt; }

	void OnObjectStart(long long inObjectID, long long inGenerationNumber) {}

	void OnObjectEnd(PDFObject* inObject) {}

	// for custom content transfer

	bool DoesSupportContentTransfer() { return false; }

	bool TransferContent( IByteWriter* inStreamWriter, IByteReader* inStreamReader ) { return false; }
};
