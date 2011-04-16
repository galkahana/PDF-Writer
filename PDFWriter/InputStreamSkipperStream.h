#pragma once

#include "IByteReader.h"
#include "IReadPositionProvider.h"

/*
	This class is to help with decoder/encoder class, that don't really have "SetPosition" kind of capabilities,
	but sometimes you just want to skip. It does skip by recording the amount already read. then, if possible, reads to the skipped to position
*/

class InputStreamSkipperStream : public IByteReader, public IReadPositionProvider
{
public:
	InputStreamSkipperStream();

	// gets ownership, so if you wanna untie it, make sure to call Assign(NULL), before finishing
	InputStreamSkipperStream(IByteReader* inSourceStream);
	virtual ~InputStreamSkipperStream(void);

	void Assign(IByteReader* inSourceStream);

	// IByteReader implementation
	virtual IOBasicTypes::LongBufferSizeType Read(IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inBufferSize);
	virtual bool NotEnded();

	// IReadPositionProvider implementation
	virtual IOBasicTypes::LongFilePositionType GetCurrentPosition();


	// here's the interesting part.
	bool CanSkipTo(IOBasicTypes::LongFilePositionType inPositionInStream);

	// will skip if can, which is either if not passed position (check with CanSkipTo), or if while skipping will hit EOF
	void SkipTo(IOBasicTypes::LongFilePositionType inPositionInStream);
	
	// will skip by, or hit EOF
	void SkipBy(IOBasicTypes::LongFilePositionType inAmountToSkipBy);

	// use this after resetting the input stream to an initial position, to try skipping to the position once more
	// this will reset the read count
	void Reset();


private:
	IByteReader* mStream;
	IOBasicTypes::LongFilePositionType mAmountRead;
};
