#pragma once
#include "EStatusCode.h"
#include "IByteReaderWithPosition.h"
#include "DictOperand.h"

class CFFPrimitiveReader
{
public:
	CFFPrimitiveReader(IByteReaderWithPosition* inCFFFile = NULL);
	~CFFPrimitiveReader(void);


	void SetStream(IByteReaderWithPosition* inCFFFile);
	void SetOffset(LongFilePositionType inNewOffset);
	void Skip(LongBufferSizeType inToSkip);
	LongFilePositionType GetCurrentPosition();
	EStatusCode GetInternalState();

	EStatusCode ReadByte(Byte& outValue);
	EStatusCode Read(Byte* ioBuffer,LongBufferSizeType inBufferSize);

	// basic CFF values
	EStatusCode ReadCard8(Byte& outValue);
	EStatusCode ReadCard16(unsigned short& outValue);
	// set offsize to read offsets
	void SetOffSize(Byte inOffSize);
	EStatusCode ReadOffset(unsigned long& outValue);
	EStatusCode ReadOffSize(Byte& outValue);
	EStatusCode ReadSID(unsigned short& outValue);

	// dict data
	bool IsDictOperator(Byte inCandidate);
	EStatusCode ReadDictOperator(Byte inFirstByte,unsigned short& outOperator);
	EStatusCode ReadDictOperand(Byte inFirstByte,DictOperand& outOperand);

private:
	IByteReaderWithPosition* mCFFFile;
	LongFilePositionType mInitialPosition;
	EStatusCode mInternalState;
	Byte mCurrentOffsize;

	EStatusCode Read3ByteUnsigned(unsigned long& outValue);
	EStatusCode Read4ByteUnsigned(unsigned long& outValue);
	EStatusCode Read4ByteSigned(long& outValue);
	EStatusCode Read2ByteSigned(short& outValue);
	EStatusCode ReadIntegerOperand(Byte inFirstByte,long& outValue);
	// make sure you get here after discarding the initial byte, 30.
	EStatusCode ReadRealOperand(double& outValue,long& outRealValueFractalEnd);
};
