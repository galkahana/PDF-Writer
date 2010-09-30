#pragma once

#include "ETokenSeparator.h"
#include <string>

using namespace std;

class IByteWriter;

class PrimitiveObjectsWriter
{
public:
	PrimitiveObjectsWriter(IByteWriter* inStreamForWriting = NULL);
	~PrimitiveObjectsWriter(void);

	void SetStreamForWriting(IByteWriter* inStreamForWriting);

	// Token Writing
	void WriteTokenSeparator(ETokenSeparator inSeparate);
	void EndLine();

	void WriteKeyword(const string& inKeyword);
	void WriteName(const string& inName,ETokenSeparator inSeparate = eTokenSeparatorSpace);
	void WriteInteger(long long inIntegerToken,ETokenSeparator inSeparate = eTokenSeparatorSpace);
	void WriteLiteralString(const string& inString,ETokenSeparator inSeparate = eTokenSeparatorSpace);
	void WriteHexString(const string& inString,ETokenSeparator inSeparate = eTokenSeparatorSpace);
	void WriteDouble(double inDoubleToken,ETokenSeparator inSeparate = eTokenSeparatorSpace);
	void WriteBoolean(bool inBoolean,ETokenSeparator inSeparate = eTokenSeparatorSpace);

	void StartArray();
	void EndArray(ETokenSeparator inSeparate = eTokenSepratorNone);

private:
	IByteWriter* mStreamForWriting;

	size_t DetermineDoubleTrimmedLength(const char* inBufferWithDouble);
};
