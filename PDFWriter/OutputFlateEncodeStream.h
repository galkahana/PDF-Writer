#pragma once
#include "IByteWriterWithPosition.h"

struct z_stream_s;
typedef z_stream_s z_stream;

class OutputFlateEncodeStream : public IByteWriterWithPosition
{
public:
	OutputFlateEncodeStream(void);
	virtual ~OutputFlateEncodeStream(void);

	/* passing the target writer to OutputFlateEncodeStream makes it its owner. so if you don't want it to try and delete it on destructor - use Assign(NULL) */
	OutputFlateEncodeStream(IByteWriterWithPosition* inTargetWriter, bool inInitiallyOn = true);

	// Assing makes OutputFlateEncodeStream the owner of inWriter, so if you don't want the class to delete it upon destructions - use Assign(NULL)
	void Assign(IByteWriterWithPosition* inWriter,bool inInitiallyOn = true);

	virtual IOBasicTypes::LongBufferSizeType Write(const IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inSize);
	virtual IOBasicTypes::LongFilePositionType GetCurrentPosition();

	void TurnOnEncoding();
	void TurnOffEncoding();

private:
	IOBasicTypes::Byte* mBuffer;
	IByteWriterWithPosition* mTargetStream;
	bool mCurrentlyEncoding;
	z_stream* mZLibState;

	void FinalizeEncoding();
	void StartEncoding();
	IOBasicTypes::LongBufferSizeType EncodeBufferAndWrite(const IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inSize);
};
