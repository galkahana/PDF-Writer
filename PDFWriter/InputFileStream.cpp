#include "InputFileStream.h"
#include "SafeBufferMacrosDefs.h"

InputFileStream::InputFileStream(void)
{
	mStream = NULL;
}

InputFileStream::~InputFileStream(void)
{
	if(mStream)
		Close();
}


InputFileStream::InputFileStream(const wstring& inFilePath)
{
	mStream = NULL;
	Open(inFilePath);
}

EStatusCode InputFileStream::Open(const wstring& inFilePath)
{
	SAFE_WFOPEN(mStream,inFilePath.c_str(),L"rb");
	return NULL == mStream ? eFailure:eSuccess;
}

EStatusCode InputFileStream::Close()
{
	EStatusCode result = fclose(mStream) == 0 ? eSuccess:eFailure;

	mStream = NULL;
	return result;
}

LongBufferSizeType InputFileStream::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	LongBufferSizeType readItems = mStream ? fread(static_cast<void*>(inBuffer),1,inBufferSize,mStream):0;
	return readItems;
}

bool InputFileStream::NotEnded()
{
	if(mStream)
		return !feof(mStream);
	else
		return false;
}

void InputFileStream::Skip(LongBufferSizeType inSkipSize)
{
	if(mStream)
		_fseeki64(mStream,inSkipSize,SEEK_CUR);
}

void InputFileStream::SetPosition(LongFilePositionType inOffsetFromStart)
{
	if(mStream)
		_fseeki64(mStream,inOffsetFromStart,SEEK_SET);
}

LongFilePositionType InputFileStream::GetCurrentPosition()
{
	if(mStream)
		return _ftelli64(mStream);
	else
		return 0;
}

LongFilePositionType InputFileStream::GetFileSize()
{
	if(mStream)
	{
		// very messy...prefer a different means sometime
		LongFilePositionType currentPosition = _ftelli64(mStream);
		LongFilePositionType result;

		_fseeki64(mStream,0,SEEK_END);
		result = _ftelli64(mStream);
		_fseeki64(mStream,currentPosition,SEEK_SET);
		return result;
	}
	else
		return 0;
}