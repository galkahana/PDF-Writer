#pragma once

#include "IByteReader.h"

class InputPredictorPNGNoneStream : public IByteReader
{
public:
	InputPredictorPNGNoneStream(void);
	// Takes ownership (use Assign(NULL,0) to unassign)
	InputPredictorPNGNoneStream(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns);
	virtual ~InputPredictorPNGNoneStream(void);

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
