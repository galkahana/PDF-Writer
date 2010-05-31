#pragma once

#include "IByteWriterWithPosition.h"

#define DEFAULT_BUFFER_SIZE 256*1024

class OutputBufferedStream : public IByteWriterWithPosition
{
public:
	/*
		default constructor with default buffer size
	*/
	OutputBufferedStream(void);

	/*
		Destroys an owned buffer, flushes the buffer before releasing
	*/
	virtual ~OutputBufferedStream(void);

	/*
		consturctor with buffer size setup
	*/
	OutputBufferedStream(IOBasicTypes::LongBufferSizeType inBufferSize);

	/*
		Constructor with assigning. see Assign for unassign instructions
	*/
	OutputBufferedStream(IByteWriterWithPosition* inTargetWriter,IOBasicTypes::LongBufferSizeType inBufferSize = DEFAULT_BUFFER_SIZE);

	/*
		Assigns a writer for buffered writing. from the moment of assigning the
		buffer assumes control of the stream.
		Assign a NULL or a different writer to release ownership.
		replacing a current stream automatically Flushes the buffer.
	*/
	void Assign(IByteWriterWithPosition* inWriter);

	// IByteWriter implementation
	virtual IOBasicTypes::LongBufferSizeType Write(const IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inSize);

	// IByteWriterWithPosition implementation
	virtual IOBasicTypes::LongFilePositionType GetCurrentPosition();

	// force buffer flush to underlying stream
	void Flush();

private:
	IOBasicTypes::Byte* mBuffer;
	IOBasicTypes::LongBufferSizeType mBufferSize;
	IOBasicTypes::Byte* mCurrentBufferIndex;
	IByteWriterWithPosition* mTargetStream;

	void Initiate(IByteWriterWithPosition* inTargetWriter,IOBasicTypes::LongBufferSizeType inBufferSize);
};
