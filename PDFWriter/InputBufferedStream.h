#pragma once

#include "EStatusCode.h"
#include "IByteReader.h"

#define DEFAULT_BUFFER_SIZE 256*1024

class InputBufferedStream : public IByteReader
{
public:
	/*
		default constructor with default buffer size
	*/
	InputBufferedStream(void);

	/*
		Destroys an owned buffer
	*/
	virtual ~InputBufferedStream(void);

	/*
		consturctor with buffer size setup
	*/
	InputBufferedStream(IOBasicTypes::LongBufferSizeType inBufferSize);

	/*
		Constructor with assigning. see Assign for unassign instructions
	*/
	InputBufferedStream(IByteReader* inSourceReader,IOBasicTypes::LongBufferSizeType inBufferSize = DEFAULT_BUFFER_SIZE);

	/*
		Assigns a reader stream for buffered reading. from the moment of assigning the
		buffer assumes control of the stream.
		Assign a NULL or a different reader to release ownership.
	*/
	void Assign(IByteReader* inReader);

	// IByteReader implementation
	virtual LongBufferSizeType Read(Byte* inBuffer,LongBufferSizeType inBufferSize);
	virtual bool NotEnded();
	virtual void Skip(LongBufferSizeType inSkipSize);

private:
	IOBasicTypes::Byte* mBuffer;
	IOBasicTypes::LongBufferSizeType mBufferSize;
	IOBasicTypes::Byte* mCurrentBufferIndex;
	IOBasicTypes::Byte* mLastAvailableIndex;
	IByteReader* mSourceStream;

	void Initiate(IByteReader* inSourceReader,IOBasicTypes::LongBufferSizeType inBufferSize);

};
