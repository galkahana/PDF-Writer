#include "InputStreamSkipperStream.h"

InputStreamSkipperStream::InputStreamSkipperStream(void)
{
	mStream = NULL;
}

InputStreamSkipperStream::~InputStreamSkipperStream(void)
{
	if(mStream != NULL)
		delete mStream;
}

InputStreamSkipperStream::InputStreamSkipperStream(IByteReader* inSourceStream)
{

}


void InputStreamSkipperStream::Assign(IByteReader* inSourceStream)
{
	mStream = inSourceStream;
	mAmountRead = 0;
}

IOBasicTypes::LongBufferSizeType InputStreamSkipperStream::Read(IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inBufferSize)
{
	IOBasicTypes::LongBufferSizeType readThisTime = mStream->Read(inBuffer,inBufferSize);
	mAmountRead+=readThisTime;

	return readThisTime;
}

bool InputStreamSkipperStream::NotEnded()
{
	return mStream->NotEnded();
}


bool InputStreamSkipperStream::CanSkipTo(IOBasicTypes::LongFilePositionType inPositionInStream)
{
	return mAmountRead <= inPositionInStream;
}

void InputStreamSkipperStream::SkipTo(IOBasicTypes::LongFilePositionType inPositionInStream)
{
	if(!CanSkipTo(inPositionInStream))
		return;

	SkipBy(inPositionInStream-mAmountRead);
}

// will skip by, or hit EOF
void InputStreamSkipperStream::SkipBy(IOBasicTypes::LongFilePositionType inAmountToSkipBy)
{
	IOBasicTypes::Byte buffer;

	while(NotEnded() && inAmountToSkipBy>0)
	{
		Read(&buffer,1);
		--inAmountToSkipBy;
	}

}

void InputStreamSkipperStream::Reset()
{
	mAmountRead = 0;
}

IOBasicTypes::LongFilePositionType InputStreamSkipperStream::GetCurrentPosition()
{
	return mAmountRead;
}