#pragma once

#include "EStatusCode.h"
#include "IByteReader.h"

class TrueTypePrimitiveReader
{
public:
	TrueTypePrimitiveReader(IByteReader* inTrueTypeFile = NULL);
	~TrueTypePrimitiveReader(void);

	void SetTrueTypeStream(IByteReader* inTrueTypeFile);

	void SetOffset(LongFilePositionType inNewOffset);
	void Skip(LongBufferSizeType inToSkip);

	EStatusCode ReadBYTE(unsigned char& outValue);
	EStatusCode ReadCHAR(char& outValue);
	EStatusCode ReadUSHORT(unsigned short& outValue);
	EStatusCode ReadSHORT(short& outValue);
	EStatusCode ReadULONG(unsigned long& outValue);
	EStatusCode ReadLONG(long& outValue);
	EStatusCode ReadLongDateTime(long long& outValue);
	EStatusCode ReadFixed(double& outValue);
	EStatusCode Read(Byte* inBuffer,LongBufferSizeType inBufferSize);


	LongFilePositionType GetCurrentPosition();

	EStatusCode GetInternalState();
private:

	IByteReader* mTrueTypeFile;
	LongFilePositionType mInitialPosition;
	EStatusCode mInternalState;

};
