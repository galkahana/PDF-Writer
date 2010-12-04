#pragma once

#include "EStatusCode.h"
#include "IByteReaderWithPosition.h"

class OpenTypePrimitiveReader
{
public:
	OpenTypePrimitiveReader(IByteReaderWithPosition* inTrueTypeFile = NULL);
	~OpenTypePrimitiveReader(void);

	void SetOpenTypeStream(IByteReaderWithPosition* inOpenTypeFile);
	void SetOffset(LongFilePositionType inNewOffset);
	void Skip(LongBufferSizeType inToSkip);
	LongFilePositionType GetCurrentPosition();
	EStatusCode GetInternalState();

	EStatusCode ReadBYTE(unsigned char& outValue);
	EStatusCode ReadCHAR(char& outValue);
	EStatusCode ReadUSHORT(unsigned short& outValue);
	EStatusCode ReadSHORT(short& outValue);
	EStatusCode ReadULONG(unsigned long& outValue);
	EStatusCode ReadLONG(long& outValue);
	EStatusCode ReadLongDateTime(long long& outValue);
	EStatusCode ReadFixed(double& outValue);
	EStatusCode Read(Byte* inBuffer,LongBufferSizeType inBufferSize);

	IByteReaderWithPosition* GetReadStream();
private:

	IByteReaderWithPosition* mOpenTypeFile;
	LongFilePositionType mInitialPosition;
	EStatusCode mInternalState;

};
