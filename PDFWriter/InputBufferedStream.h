#pragma once

#include "EStatusCode.h"
#include "IByteReaderWithPosition.h"

#define DEFAULT_BUFFER_SIZE 256*1024

class InputBufferedStream : public IByteReaderWithPosition
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
	InputBufferedStream(IByteReaderWithPosition* inSourceReader,IOBasicTypes::LongBufferSizeType inBufferSize = DEFAULT_BUFFER_SIZE);

	/*
		Assigns a reader stream for buffered reading. from the moment of assigning the
		buffer assumes control of the stream.
		Assign a NULL or a different reader to release ownership.
	*/
	void Assign(IByteReaderWithPosition* inReader);

	// IByteReaderWithPosition implementation
	virtual LongBufferSizeType Read(Byte* inBuffer,LongBufferSizeType inBufferSize);
	virtual bool NotEnded();
	virtual void Skip(LongBufferSizeType inSkipSize);
	virtual void SetPosition(LongFilePositionType inOffsetFromStart);
	virtual LongFilePositionType GetCurrentPosition();

	IByteReaderWithPosition* GetSourceStream();

private:
	IOBasicTypes::Byte* mBuffer;
	IOBasicTypes::LongBufferSizeType mBufferSize;
	IOBasicTypes::Byte* mCurrentBufferIndex;
	IOBasicTypes::Byte* mLastAvailableIndex;
	IByteReaderWithPosition* mSourceStream;

	void Initiate(IByteReaderWithPosition* inSourceReader,IOBasicTypes::LongBufferSizeType inBufferSize);

};
