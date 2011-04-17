#pragma once

#include "IByteReader.h"

class InputPredictorPNGSubStream : public IByteReader
{
public:
	InputPredictorPNGSubStream(void);
	// Takes ownership (use Assign(NULL,0) to unassign)
	InputPredictorPNGSubStream(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns);
	virtual ~InputPredictorPNGSubStream(void);

	virtual IOBasicTypes::LongBufferSizeType Read(IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inBufferSize);

	virtual bool NotEnded();

	// Takes ownership (use Assign(NULL,0) to unassign)
	void Assign(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns);

private:
	IByteReader* mSourceStream;
	IOBasicTypes::Byte* mBuffer;
	IOBasicTypes::LongBufferSizeType mBufferSize;
	IOBasicTypes::Byte* mIndex;

	void DecodeNextByte(IOBasicTypes::Byte& outDecodedByte);
};
