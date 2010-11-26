#pragma once

#include "EStatusCode.h"
#include "IOBasicTypes.h"

using namespace IOBasicTypes;

class OutputStringBufferStream;

class TrueTypePrimitiveWriter
{
public:
	TrueTypePrimitiveWriter(OutputStringBufferStream* inTrueTypeFile = NULL);
	~TrueTypePrimitiveWriter(void);

	void SetOpenTypeStream(OutputStringBufferStream* inTrueTypeFile);
	
	EStatusCode GetInternalState();

	EStatusCode WriteBYTE(Byte inValue);
	EStatusCode WriteULONG(unsigned long inValue);
	EStatusCode WriteUSHORT(unsigned short inValue);
	EStatusCode WriteSHORT(short inValue);

	EStatusCode Pad(int inCount);
	EStatusCode PadTo4();

private:
	OutputStringBufferStream* mTrueTypeFile;
	EStatusCode mInternalState;

};
