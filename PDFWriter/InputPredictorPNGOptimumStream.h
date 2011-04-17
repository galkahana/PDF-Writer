#pragma once

#include "IByteReader.h"

class InputPredictorPNGOptimumStream : public IByteReader
{
public:
	InputPredictorPNGOptimumStream(void);
	// Takes ownership (use Assign(NULL,0) to unassign)
	InputPredictorPNGOptimumStream(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns);
	virtual ~InputPredictorPNGOptimumStream(void);

	virtual IOBasicTypes::LongBufferSizeType Read(IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inBufferSize);

	virtual bool NotEnded();

	// Takes ownership (use Assign(NULL,0) to unassign)
	void Assign(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns);

private:
	IByteReader* mSourceStream;
	IOBasicTypes::Byte* mBuffer;
	IOBasicTypes::LongBufferSizeType mBufferSize;
	IOBasicTypes::Byte* mIndex;
	IOBasicTypes::Byte mFunctionType;

	IOBasicTypes::Byte* mUpValues;

	void DecodeNextByte(IOBasicTypes::Byte& outDecodedByte);
	char PaethPredictor(char inLeft,char inUp,char inUpLeft);
};
