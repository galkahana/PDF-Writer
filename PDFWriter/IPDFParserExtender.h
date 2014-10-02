#pragma once

#include <string>

class IByteReader;
class IByteWriter;
class PDFName;
class PDFDictionary;

class IPDFParserExtender
{
public:

	virtual ~IPDFParserExtender(){}

	// for extending fliter support in stream read
	virtual IByteReader* CreateFilterForStream(IByteReader* inStream,PDFName* inFilterName,PDFDictionary* inDecodeParams) = 0;

	// for decryption extension

	// DoesSupportEncryption should return true if encryption is supported
	virtual bool DoesSupportEncryption() = 0;
	// return input stream if did nothing, or a filter stream owning the input stream if encryption is required
	virtual IByteReader* CreateDecryptionFilterForStream(IByteReader* inStream) = 0;
	// decrypt input string. this serves both literal and hexadecimal strings
	virtual std::string DecryptString(std::string inStringToDecrypt) = 0;
	// event for indirect object parsing start
	virtual void OnObjectStart(long long inObjectID, long long inGenerationNumber) = 0;
	// event for indirect object parsing end
	virtual void OnObjectEnd(PDFObject* inObject) = 0;

	// for custom content transfer

	// should return true if page content transfer is supported
	virtual bool DoesSupportContentTransfer() { return false; }
	// transfers page content from given byte reader to given page content context
	virtual bool TransferContent( IByteWriter* inStreamWriter, IByteReader* inStreamReader ) { return false; }
};
