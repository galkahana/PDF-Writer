#include "OutputStreamTraits.h"
#include "IByteWriter.h"
#include "IByteReader.h"

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
