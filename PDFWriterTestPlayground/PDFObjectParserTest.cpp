/*
Source File : PDFObjectParserTest.cpp


Copyright 2011 Gal Kahana PDFWriter

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


*/
#include "PDFObjectParserTest.h"
#include "PDFParser.h"
#include "PDFObjectParser.h"
#include "OutputFile.h"
#include "IByteWriterWithPosition.h"
#include "PDFObjectCast.h"
#include "PDFObject.h"
#include "PDFLiteralString.h"
#include "PDFHexString.h"
#include "ParsedPrimitiveHelper.h"
#include "PDFTextString.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace PDFHummus;


class InputInterfaceToStream : public IByteReader, public IReadPositionProvider{
private:
	string mInput;
	string::size_type mLen, mPos;
public:
	virtual IOBasicTypes::LongBufferSizeType Read(IOBasicTypes::Byte* inBuffer, IOBasicTypes::LongBufferSizeType inBufferSize){
		const char* data = mInput.data();
		if (!NotEnded())  return 0;
		size_t size = mLen - mPos;
		if (size > inBufferSize) size = inBufferSize;
		memcpy(inBuffer, &data[mPos], size);
		mPos += size;
		return size;
	}

	virtual bool NotEnded() { return mPos < mLen; };

	virtual IOBasicTypes::LongFilePositionType GetCurrentPosition(){
		return static_cast<IOBasicTypes::LongFilePositionType>(mPos);
	};
	
	void setInput(const string& input){
		mInput = input;
		mLen = input.size();
		mPos = 0;
	}
	
	const string& getInput() const{
		return mInput;
	}

	InputInterfaceToStream()
		: mInput("")
		, mLen(0)
		, mPos(0)
	{}

	virtual ~InputInterfaceToStream(){}

};

class PDFObjectParserTestLogHelper{
private:
	OutputFile mOutput;
public:
	PDFObjectParserTestLogHelper(){}

	~PDFObjectParserTestLogHelper(){}

	EStatusCode openLog(const string& path){
		return mOutput.OpenFile(path);

	}

	template <class T>
	PDFObjectParserTestLogHelper& operator<<(T out){
		ostringstream ss;
		ss << out;
		mOutput.GetOutputStream()->Write((const unsigned char*)ss.str().c_str(), ss.str().size());
		return *this;
	}


};

template<class _resultObjectType>
class ExpectedResult{
private:
	RefCountPtr<PDFObject> mObject;
	PDFObjectCastPtr<_resultObjectType> mTypedObject;
public:
	ExpectedResult(PDFObject* object)
	: mObject(object) // ParseNewObject constructs with refcount 1
	{
		mTypedObject = object; // addrefs if valid object
	}
	~ExpectedResult(){}

	int setResult(const string& input, const string& expected, PDFObjectParserTestLogHelper& log){
		if (!mObject){
			log << "Failed parse: no object parsed from input: '"<< input << "'";
			return 1;
		}
		if (!!mObject && !mTypedObject){
			int object_type = mObject->GetType();
			string result_type = "invalid";
			if (object_type >= 0 && object_type <= PDFObject::ePDFObjectSymbol){
				result_type = PDFObject::scPDFObjectTypeLabel(mObject->GetType());
			}

			log << "Failed parse. Expected '" << PDFObject::scPDFObjectTypeLabel(_resultObjectType::eType) << "' got '"
				<< result_type << "' from input: '" << input << "'\n";
			return 1;
		}
		string result = PDFTextString(ParsedPrimitiveHelper(mTypedObject.GetPtr()).ToString()).ToUTF8String();
		if (result != expected){
			log << "Failed parse. Expected '" << expected << "' got '"
				<< result << "' from input: '" << input << "'\n";
			return 1;
		}
		log << "Parse passed: '" << input << "'\n";
		return 0;
	}

};



PDFObjectParserTest::PDFObjectParserTest()
{

}


PDFObjectParserTest::~PDFObjectParserTest()
{
}



EStatusCode PDFObjectParserTest::Run(const TestConfiguration& inTestConfiguration)
{
	int failures = 0;
	PDFObjectParserTestLogHelper log;
	if (log.openLog(RelativeURLToLocalPath(inTestConfiguration.mSampleFileBase, "PDFObjectParserTest.txt")) != eSuccess){
		cout << "failed to initialize log output\n";
		return eFailure;
	}


	PDFParser parser;
	PDFObjectParser*  pObjectParser = &parser.GetObjectParser();

	if (ParseCommentedTokens(pObjectParser,&log) == eFailure){
		++failures;
	}

	if (ParseHexStringTokens(pObjectParser,&log) == eFailure){
		++failures;
	}

	if (ParseLiteralStringTokens(pObjectParser,&log) == eFailure){
		++failures;
	}



	if (failures > 0){
		cout << "Failed tests in PDFObjectParserTest: " << failures << endl;
		return eFailure;
	}
	return eSuccess;
}


EStatusCode PDFObjectParserTest::ParseCommentedTokens(PDFObjectParser* objectParser, PDFObjectParserTestLogHelper* log){
	int failures = 0;
	InputInterfaceToStream input;
	RefCountPtr<PDFObject> object;


	{
		input.setInput("(val%ami)");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), "val%ami", *log);
	};

	/*
	// I don't know what this should return. Currently it returns 'x41 x54 x20' which is wrong (parse error?)
	{
		input.setInput("<41%\n42>");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), "AB", *log);
	};*/




	if (failures > 0){
			cout << "Failed tests in PDFObjectParserTest::ParseCommentedTokens: " << failures << endl;
			return eFailure;
		}
	return eSuccess;
}
EStatusCode PDFObjectParserTest::ParseHexStringTokens(PDFObjectParser* objectParser, PDFObjectParserTestLogHelper* log){
	int failures = 0;
	InputInterfaceToStream input;
	RefCountPtr<PDFObject> object;

	{
		input.setInput("<>");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), "", *log);
	};

	{
		input.setInput("<   >");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), "", *log);
	};

	{
		input.setInput("<\n>");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), "", *log);
	};


	{
		input.setInput("< 41   4 2 >");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), "AB", *log);
	};

	{
		input.setInput("<04000201>");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), string("\x04\x00\x02\x01",4), *log);
	};

	{
		input.setInput("<feff00410042>");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), "\x41\x42", *log);
	};

	{
		input.setInput("<f e f f 0 04 10042>");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), "\x41\x42", *log);
	};

	{
		input.setInput("<5>");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFHexString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), "\x50", *log);
	};

	if (failures > 0){
		cout << "Failed tests in PDFObjectParserTest::ParseHexStringTokens: " << failures << endl;
		return eFailure;
	}
	return eSuccess;
}
EStatusCode PDFObjectParserTest::ParseLiteralStringTokens(PDFObjectParser* objectParser, PDFObjectParserTestLogHelper* log){
	int failures = 0;
	InputInterfaceToStream input;
	RefCountPtr<PDFObject> object;

	{
		input.setInput("()");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), "", *log);
	};

	{
		input.setInput("( )");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), " ", *log);
	};

	{
		input.setInput("( \n )");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), " \n ", *log);
	};
	{
		input.setInput("( \\\n )");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), "  ", *log);
	};

	{
		input.setInput("( ( ) )");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), " ( ) ", *log);
	};

	{
		input.setInput("( \\( \\) )");
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), " ( ) ", *log);
	};

	{
		input.setInput("(\\101\\102)"); // in hex: 41 42
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), "AB", *log);
	};

	{
		input.setInput(string("(\\376\\377\\0\\101)",16)); // in hex this is: fe ff 00 41
		objectParser->SetReadStream(&input, &input);
		ExpectedResult<PDFLiteralString> result(objectParser->ParseNewObject());

		failures += result.setResult(input.getInput(), "A", *log);
	};


	if (failures > 0){
		cout << "Failed tests in PDFObjectParserTest::ParseLiteralStringTokens: " << failures << endl;
		return eFailure;
	}
	return eSuccess;
}

ADD_CATEGORIZED_TEST(PDFObjectParserTest, "Parsing")

