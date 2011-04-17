#pragma once

#include "IByteReader.h"

class InputPredictorPNGAverageStream : public IByteReader
{
public:
	InputPredictorPNGAverageStream(void);
	// Takes ownership (use Assign(NULL,0) to unassign)
	InputPredictorPNGAverageStream(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns);
	virtual ~InputPredictorPNGAverageStream(void);

	virtual IOBasicTypes::LongBufferSizeType Read(IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inBufferSize);

	virtual bool NotEnded();

	// Takes ownership (use Assign(NULL,0) to unassign)
	void Assign(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns);

private:
	IByteReader* mSourceStream;
	IOBasicTypes::Byte* mBuffer;
	IOBasicTypes::LongBufferSizeType mBufferSize;
	IOBasicTypes::Byte* mIndex;

	IOBasicTypes::Byte* mUpValues;

	void DecodeNextByte(IOBasicTypes::Byte& outDecodedByte);
};
