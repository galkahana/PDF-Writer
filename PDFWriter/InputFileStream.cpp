/*
   Source File : InputFileStream.cpp


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
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

EPDFStatusCode InputFileStream::Open(const wstring& inFilePath)
{
	SAFE_WFOPEN(mStream,inFilePath.c_str(),L"rb");
	return NULL == mStream ? ePDFFailure:ePDFSuccess;
}

EPDFStatusCode InputFileStream::Close()
{
	EPDFStatusCode result = fclose(mStream) == 0 ? ePDFSuccess:ePDFFailure;

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

void InputFileStream::SetPositionFromEnd(LongFilePositionType inOffsetFromEnd)
{
	if(mStream)
		_fseeki64(mStream,-inOffsetFromEnd,SEEK_END);

}
