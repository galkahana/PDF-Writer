#pragma once

#include "EStatusCode.h"
#include "IByteReader.h"

struct z_stream_s;
typedef z_stream_s z_stream;

class InputFlateDecodeStream : public IByteReader
{
public:
	InputFlateDecodeStream();

	// Note that assigning passes ownership on the stream, use Assign(NULL) to remove ownership
	InputFlateDecodeStream(IByteReader* inSourceReader);
	virtual ~InputFlateDecodeStream(void);

	// Assigning passes ownership of the input stream to the decoder stream. 
	// if you don't care for that, then after finishing with the decode, Assign(NULL).
	void Assign(IByteReader* inSourceReader);

	// IByteReader implementation. note that "inBufferSize" determines how many
	// bytes will be placed in the Buffer...not how many are actually read from the underlying
	// encoded stream. got it?!
	virtual IOBasicTypes::LongBufferSizeType Read(IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inBufferSize);

	virtual bool NotEnded();

private:
	IOBasicTypes::Byte mBuffer;
	IByteReader* mSourceStream;
	z_stream* mZLibState;
	bool mCurrentlyEncoding;

	void FinalizeEncoding();
	IOBasicTypes::LongBufferSizeType DecodeBufferAndRead(const IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inSize);
	void StartEncoding();

};
