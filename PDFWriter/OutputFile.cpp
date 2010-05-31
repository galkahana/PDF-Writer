#include "OutputFile.h"
#include "OutputBufferedStream.h"
#include "OutputFileStream.h"
#include "Trace.h"

OutputFile::OutputFile(void)
{
	mOutputStream = NULL;
	mFileStream = NULL;
}

OutputFile::~OutputFile(void)
{
	CloseFile();
}

EStatusCode OutputFile::OpenFile(const wstring& inFilePath,bool inAppend)
{
	EStatusCode status;
	do
	{
		status = CloseFile();
		if(status != eSuccess)
		{
			TRACE_LOG1("OutputFile::OpenFile, Unexpected Failure. Couldn't close previously open file - %s",mFilePath.c_str());
			break;
		}
	
		OutputFileStream* outputFileStream = new OutputFileStream();
		status = outputFileStream->Open(inFilePath,inAppend); // explicitly open, so status may be retrieved
		if(status != eSuccess)
		{
			TRACE_LOG1("OutputFile::OpenFile, Unexpected Failure. Cannot open file for writing - %s",inFilePath.c_str());
			delete outputFileStream;
			break;
		}

		mOutputStream = new OutputBufferedStream(outputFileStream);
		mFileStream = outputFileStream;
		mFilePath = inFilePath;
	} while(false);
	return status;
}

EStatusCode OutputFile::CloseFile()
{
	if(NULL == mOutputStream)
	{
		return eSuccess;
	}
	else
	{
		mOutputStream->Flush();
		EStatusCode status = mFileStream->Close(); // explicitly close, so status may be retrieved

		delete mOutputStream; // will delete the referenced file stream as well
		mOutputStream = NULL;
		mFileStream = NULL;
		return status;
	}
}

IByteWriterWithPosition* OutputFile::GetOutputStream()
{
	return mOutputStream;
}

const wstring& OutputFile::GetFilePath()
{
	return mFilePath;
}

