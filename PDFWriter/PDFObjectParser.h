#pragma once

#include "PDFParserTokenizer.h"
#include "EStatusCode.h"
#include "IOBasicTypes.h"

#include <string>
#include <list>
#include <utility>

class PDFObject;
class IByteReaderWithPosition;

using namespace std;
using namespace IOBasicTypes;

typedef pair<bool,Byte> BoolAndByte;
typedef list<string> StringList;

class PDFObjectParser
{
public:
	PDFObjectParser(void);
	~PDFObjectParser(void);

	
	// Assign the stream to read from (does not take ownership of the stream)
	void SetReadStream(IByteReaderWithPosition* inSourceStream);

	PDFObject* ParseNewObject();

	// calls this when changing underlying stream position
	void ResetReadState();

private:
	PDFParserTokenizer mTokenizer;
	StringList mTokenBuffer;
	IByteReaderWithPosition* mStream;

	bool GetNextToken(string& outToken);
	void SaveTokenToBuffer(string& inToken);

	bool IsBoolean(const string& inToken);
	PDFObject* ParseBoolean(const string& inToken);

	bool IsLiteralString(const string& inToken);
	PDFObject* ParseLiteralString(const string& inToken);

	bool IsHexadecimalString(const string& inToken);
	PDFObject* ParseHexadecimalString(const string& inToken);
	// return "false" if not hex value
	BoolAndByte GetHexValue(Byte inValue);

	bool IsNull(const string& inToken);

	bool IsName(const string& inToken);
	PDFObject* ParseName(const string& inToken);

	bool IsNumber(const string& inToken);
	PDFObject* ParseNumber(const string& inToken);

	bool IsArray(const string& inToken);
	PDFObject* ParseArray();

	bool IsDictionary(const string& inToken);
	PDFObject* ParseDictionary();

	bool IsComment(const string& inToken);

};
