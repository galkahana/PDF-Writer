#pragma once

#include "IByteWriterWithPosition.h"

#define DEFAULT_BUFFER_SIZE 10000

class BufferedOutputStream : public IByteWriterWithPosition
{
public:
	/*
		default constructor with default buffer size
	*/
	BufferedOutputStream(void);

	/*
		Destroys an owned buffer, flushes the buffer before releasing
	*/
	virtual ~BufferedOutputStream(void);

	/*
		consturctor with buffer size setup
	*/
	BufferedOutputStream(LongBufferSizeType inBufferSize);

	/*
		Constructor with assigning. see Assign for unassign instructions
	*/
	BufferedOutputStream(IByteWriterWithPosition* inTargetWriter,LongBufferSizeType inBufferSize = DEFAULT_BUFFER_SIZE);

	/*
		Assigns a writer for buffered writing. from the moment of assigning the
		buffer assumes control of the stream.
		Assign a NULL or a different writer to release ownership.
		replacing a current stream automatically Flushes the buffer.
	*/
	void Assign(IByteWriterWithPosition* inWriter);

	// IByteWriter implementation
	virtual LongBufferSizeType Write(const Byte* inBuffer,LongBufferSizeType inSize);

	// IByteWriterWithPosition implementation
	virtual LongFilePositionType GetCurrentPosition();

	// force buffer flush to underlying stream
	void Flush();

private:
	Byte* mBuffer;
	LongBufferSizeType mBufferSize;
	Byte* mCurrentBufferIndex;
	IByteWriterWithPosition* mTargetStream;

	void Initiate(IByteWriterWithPosition* inTargetWriter,LongBufferSizeType inBufferSize);
};
