#include "OutputStreamTraits.h"
#include "IByteWriter.h"
#include "IByteReaderWithPosition.h"

OutputStreamTraits::OutputStreamTraits(IByteWriter* inOutputStream)
{
	mOutputStream = inOutputStream;
}

OutputStreamTraits::~OutputStreamTraits(void)
{
}

#define TENMEGS 10*1024*1024
EStatusCode OutputStreamTraits::CopyToOutputStream(IByteReader* inInputStream)
{
	Byte* buffer = new Byte[TENMEGS];
	LongBufferSizeType readBytes,writeBytes;
	EStatusCode status = eSuccess;

	while(inInputStream->NotEnded() && eSuccess == status)
	{
		readBytes = inInputStream->Read(buffer,TENMEGS);
		writeBytes = mOutputStream->Write(buffer,readBytes);
		status = (readBytes == writeBytes) ? eSuccess:eFailure;
	}
	delete[] buffer;
	return status;
}

EStatusCode OutputStreamTraits::CopyToOutputStream(IByteReader* inInputStream,LongBufferSizeType inLength)
{
	Byte* buffer = new Byte[inLength];
	LongBufferSizeType readBytes,writeBytes;

	readBytes = inInputStream->Read(buffer,inLength);
	writeBytes = mOutputStream->Write(buffer,readBytes);
	EStatusCode status = (readBytes == writeBytes) ? eSuccess:eFailure;
	delete[] buffer;
	return status;
}