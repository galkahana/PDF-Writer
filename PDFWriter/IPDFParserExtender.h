#pragma once

class IByteReader;
class PDFName;
class PDFDictionary;

class IPDFParserExtender
{
public:

	virtual ~IPDFParserExtender(){}

	virtual IByteReader* CreateFilterForStream(IByteReader* inStream,PDFName* inFilterName,PDFDictionary* inDecodeParams) = 0;
};