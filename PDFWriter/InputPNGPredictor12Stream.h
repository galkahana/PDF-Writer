#pragma once

#include "IByteReader.h"

class InputPNGPredictor12Stream : public IByteReader
{
public:

	InputPNGPredictor12Stream();
	// Does NOT Take ownership
	InputPNGPredictor12Stream(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns);
	virtual ~InputPNGPredictor12Stream(void);

	virtual IOBasicTypes::LongBufferSizeType Read(IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inBufferSize);

	virtual bool NotEnded();

	// Does NOT Take ownership
	void Assign(IByteReader* inSourceStream,IOBasicTypes::LongBufferSizeType inColumns);


private:
	IByteReader* mSourceStream;
	IOBasicTypes::Byte* mBuffer;
	IOBasicTypes::Byte* mPreviousBuffer;
	IOBasicTypes::LongBufferSizeType mBufferSize;
	IOBasicTypes::Byte* mIndex;

	void DecodeNextByte(IOBasicTypes::Byte& outDecodedByte);

};
