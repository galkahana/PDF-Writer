#include "OutputFileStream.h"
#include "SafeBufferMacrosDefs.h"

OutputFileStream::OutputFileStream(void)
{
	mStream = NULL;
}

OutputFileStream::~OutputFileStream(void)
{
	if(mStream)
		Close();
}


OutputFileStream::OutputFileStream(const wstring& inFilePath,bool inAppend)
{
	mStream = NULL;
	Open(inFilePath,inAppend);
}

EStatusCode OutputFileStream::Open(const wstring& inFilePath,bool inAppend)
{
	SAFE_WFOPEN(mStream,inFilePath.c_str(),inAppend ? L"ab":L"wb")
	return NULL == mStream ? eFailure:eSuccess;
};

EStatusCode OutputFileStream::Close()
{
	EStatusCode result = fclose(mStream) == 0 ? eSuccess:eFailure;

	mStream = NULL;
	return result;
}

LongBufferSizeType OutputFileStream::Write(const Byte* inBuffer,LongBufferSizeType inSize)
{

	LongBufferSizeType writtenItems = mStream ? fwrite(static_cast<const void*>(inBuffer),1,inSize,mStream):0;
	return writtenItems;
}

LongFilePositionType OutputFileStream::GetCurrentPosition()
{
	return mStream ? _ftelli64(mStream):0;
}