#pragma once

#include "IByteReader.h"

class InputPredictorPNGPaethStream : public IByteReader
{
public:
	InputPredictorPNGPaethStream(void);
	// Takes ownership (use Assign(NULL,0) to unassign)
	InputPredictorPNGPaethStream(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns);
	virtual ~InputPredictorPNGPaethStream(void);

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

	char PaethPredictor(char inLeft,char inUp,char inUpLeft);
};
