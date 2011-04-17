#pragma once

#include "IByteReader.h"

class InputPredictorTIFFSubStream : public IByteReader
{
public:
	InputPredictorTIFFSubStream(void);
	virtual ~InputPredictorTIFFSubStream(void);

	// Takes ownership (use Assign(NULL,0,0,0) to unassign)
	InputPredictorTIFFSubStream(IByteReader* inSourceStream,
								IOBasicTypes::LongBufferSizeType inColors,
								IOBasicTypes::Byte inBitsPerComponent,
								IOBasicTypes::LongBufferSizeType inColumns);

	virtual IOBasicTypes::LongBufferSizeType Read(IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inBufferSize);

	virtual bool NotEnded();

	// Takes ownership (use Assign(NULL,0,0,0) to unassign)
	void Assign(IByteReader* inSourceStream,
				IOBasicTypes::LongBufferSizeType inColors,
				IOBasicTypes::Byte inBitsPerComponent,
				IOBasicTypes::LongBufferSizeType inColumns);

private:
	IByteReader* mSourceStream;
	IOBasicTypes::LongBufferSizeType mColors;
	IOBasicTypes::Byte mBitsPerComponent;
	IOBasicTypes::LongBufferSizeType mColumns;
	
	IOBasicTypes::Byte* mRowBuffer;
	IOBasicTypes::LongBufferSizeType mReadColorsCount;
	unsigned short* mReadColors;
	unsigned short* mReadColorsIndex;
	IOBasicTypes::Byte mIndexInColor;
	unsigned short mBitMask;

	void ReadByteFromColorsArray(IOBasicTypes::Byte& outBuffer);
	void DecodeBufferToColors();


};
