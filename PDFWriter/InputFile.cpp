#include "InputFile.h"
#include "InputBufferedStream.h"
#include "InputFileStream.h"
#include "Trace.h"

InputFile::InputFile(void)
{
	mInputStream = NULL;
	mFileStream = NULL;
}

InputFile::~InputFile(void)
{
	CloseFile();
}

EStatusCode InputFile::OpenFile(const wstring& inFilePath)
{
	EStatusCode status;
	do
	{
		status = CloseFile();
		if(status != eSuccess)
		{
			TRACE_LOG1("InputFile::OpenFile, Unexpected Failure. Couldn't close previously open file - %s",mFilePath.c_str());
			break;
		}
	
		InputFileStream* inputFileStream = new InputFileStream();
		status = inputFileStream->Open(inFilePath); // explicitly open, so status may be retrieved
		if(status != eSuccess)
		{
			TRACE_LOG1("InputFile::OpenFile, Unexpected Failure. Cannot open file for reading - %s",inFilePath.c_str());
			delete inputFileStream;
			break;
		}

		mInputStream = new InputBufferedStream(inputFileStream);
		mFileStream = inputFileStream;
		mFilePath = inFilePath;
	} while(false);
	return status;
}

EStatusCode InputFile::CloseFile()
{
	if(NULL == mInputStream)
	{
		return eSuccess;
	}
	else
	{
		EStatusCode status = mFileStream->Close(); // explicitly close, so status may be retrieved

		delete mInputStream; // will delete the referenced file stream as well
		mInputStream = NULL;
		mFileStream = NULL;
		return status;
	}
}


IByteReaderWithPosition* InputFile::GetInputStream()
{
	return mInputStream;
}

const wstring& InputFile::GetFilePath()
{
	return mFilePath;
}

LongFilePositionType InputFile::GetFileSize()
{
	if(mInputStream)
	{
		InputFileStream* inputFileStream = (InputFileStream*)mInputStream->GetSourceStream();

		return inputFileStream->GetFileSize();
	}
	else
		return 0;
}