/*
   Source File : FreeTypeWrapper.cpp


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
#include "FreeTypeWrapper.h"
#include "Trace.h"
#include "InputFile.h"
#include "IByteReaderWithPosition.h"
#include "InputOwnedByteArrayStream.h"
#include "InputBufferedStream.h"
#include "InputFileStream.h"

#include <memory.h>


using namespace PDFHummus;

static void InputStreamClose(FT_Stream  inStream)
{
	if(!inStream || !inStream->descriptor.pointer)
		return;

	inStream->descriptor.pointer = NULL; // just remove the reference to the underlying reader stream. underlying stream lifecycle is managed with mOpenReaders list
}

static void CleanFtStreamsList(FTStreamList& inStreams)
{
	FTStreamList::iterator itStreams = inStreams.begin();
	for (; itStreams != inStreams.end(); ++itStreams)
	{
		InputStreamClose(*itStreams);
		delete *itStreams;
	}
	inStreams.clear();
}

static void CleanReaderStreamsList(StreamWithOwnershipList& inReaders)
{
	StreamWithOwnershipList::iterator itReader = inReaders.begin();
	for (; itReader != inReaders.end(); ++itReader)
	{
		if(itReader->ownsStream && itReader->stream)
			delete itReader->stream;
	}
	inReaders.clear();
}

FreeTypeWrapper::FreeTypeWrapper(void)
{
	if(FT_Init_FreeType(&mFreeType))
	{
		TRACE_LOG("FreeTypeWrapper::FreeTypeWrapper, unexpected failure. failed to initialize Free Type");
		mFreeType = NULL;
	}
}

FreeTypeWrapper::~FreeTypeWrapper(void)
{
	// clean open ft streams for all faces
	FTFaceToFTStreamListMap::iterator it = mOpenFTStreams.begin();
	for (; it != mOpenFTStreams.end(); ++it)
		CleanFtStreamsList(it->second);
	mOpenFTStreams.clear();

	// clean open reader streams for all faces
	FTFaceToStreamWithOwnershipListMap::iterator itReaders = mOpenReaders.begin();
	for (; itReaders != mOpenReaders.end(); ++itReaders)
		CleanReaderStreamsList(itReaders->second);
	mOpenReaders.clear();

	if (mFreeType)
		FT_Done_FreeType(mFreeType);

}


FT_Face FreeTypeWrapper::NewFace(IByteReaderWithPosition* inStream, bool inOwnsStream, IOBasicTypes::LongFilePositionType inSize, FT_Long inFontIndex) {
	FT_Face face = NULL;
	FT_Open_Args openFaceArguments = {0};

	if(!inStream) {
		TRACE_LOG("FreeTypeWrapper::NewFace, exception, input stream is NULL");
		return NULL;
	}
	if(inSize == 0) {
		TRACE_LOG("FreeTypeWrapper::NewFace, exception, input stream size is 0");
		return NULL;
	}
	if(inSize > (IOBasicTypes::LongFilePositionType)0x7fffffff) {
		TRACE_LOG("FreeTypeWrapper::NewFace, exception, input stream size is too large for FreeType FT_Long");
		return NULL;
	}


	FT_Stream ftStream = CreateFTStreamForIByteWriterWithPosition(inStream, (unsigned long)inSize);
	FillOpenFaceArgumentsForFTStream(ftStream, openFaceArguments);

	FT_Error ftStatus =  FT_Open_Face(mFreeType,&openFaceArguments,inFontIndex,&face);

	if(ftStatus)
	{
		const char* errMsg = FT_Error_String(ftStatus);
		TRACE_LOG1("FreeTypeWrapper::NewFace, unable to load font from stream with index %ld", inFontIndex);
		TRACE_LOG2("FreeTypeWrapper::NewFace, Free Type Error, Code = %d, Message = %s",ftStatus,errMsg ? errMsg : "");
		face = NULL;
	}

	if(!face)
		ReleaseStreamForFace(ftStream, inStream, inOwnsStream);
	else
		RegisterStreamForFace(face,ftStream, inStream, inOwnsStream);
	return face;
}

void FreeTypeWrapper::ReleaseStreamForFace(FT_Stream inFtStream, IByteReaderWithPosition* inReader, bool inOwnsStream) {
	// ft stream release
	if(inFtStream)
	{
		InputStreamClose(inFtStream);
		delete inFtStream;
	}

	// reader stream release, if owned
	if(inOwnsStream && inReader)
		delete inReader;
}

void FreeTypeWrapper::RegisterStreamForFace(FT_Face inFace,FT_Stream inFtStream, IByteReaderWithPosition* inReader, bool inOwnsStream) {

	FTFaceToFTStreamListMap::iterator it = mOpenFTStreams.find(inFace);
	if(it == mOpenFTStreams.end())
		it = mOpenFTStreams.insert(FTFaceToFTStreamListMap::value_type(inFace,FTStreamList())).first;
	it->second.push_back(inFtStream);

	FTFaceToStreamWithOwnershipListMap::iterator itReaders = mOpenReaders.find(inFace);
	if(itReaders == mOpenReaders.end())
		itReaders = mOpenReaders.insert(FTFaceToStreamWithOwnershipListMap::value_type(inFace,StreamWithOwnershipList())).first;
	StreamWithOwnership swo;
	swo.stream = inReader;
	swo.ownsStream = inOwnsStream;
	itReaders->second.push_back(swo);
}


FT_Face FreeTypeWrapper::NewFace(const std::string& inFilePath,FT_Long inFontIndex)
{
	InputBufferedStream* inputBufferedFileReadSteam = InputFile::CreateBufferedReadStream(inFilePath);
	if(!inputBufferedFileReadSteam) {
		TRACE_LOG1("FreeTypeWrapper::NewFace,  Cannot Open file for reading %s", inFilePath.c_str());
		return NULL;
	}

	return NewFace(inputBufferedFileReadSteam,
					true, 
					((InputFileStream*)inputBufferedFileReadSteam->GetSourceStream())->GetFileSize(), 
					inFontIndex
	);
}


FT_Face FreeTypeWrapper::NewFace(const std::vector<IOBasicTypes::Byte>& inFontBuffer,FT_Long inFontIndex)
{
	InputOwnedByteArrayStream* inputStream = new InputOwnedByteArrayStream(inFontBuffer);

	return NewFace(inputStream,
		true,
		inFontBuffer.size(),
		inFontIndex
	);
}

FT_Face FreeTypeWrapper::NewFace(IByteReaderWithPosition* inFontStream,FT_Long inFontIndex)
{
	// get total size by seeking to the end and then returning to the original position (we do kinda assume that position 0 is where the font starts in the later embedding readers...)
	LongFilePositionType originalPosition = inFontStream->GetCurrentPosition();
	inFontStream->SetPositionFromEnd(0);
	LongFilePositionType endPosition = inFontStream->GetCurrentPosition();

	LongFilePositionType totalSize = endPosition - originalPosition;

	// stream is NOT owned by FreeTypeWrapper when using this variant of NewFace
	return NewFace(inFontStream, false, totalSize,inFontIndex);
}

void FreeTypeWrapper::FillOpenFaceArgumentsForFTStream(FT_Stream inStream, FT_Open_Args& ioArgs) {
	ioArgs.flags = FT_OPEN_STREAM;
	ioArgs.memory_base = NULL;
	ioArgs.memory_size = 0;
	ioArgs.pathname = NULL;
	ioArgs.driver = NULL;
	ioArgs.num_params = 0;
	ioArgs.params = NULL;
	ioArgs.stream = inStream;
}

IByteReaderWithPosition* FreeTypeWrapper::GetFontReader(FT_Face inFace) {
	// get main readers stream for a face. if it  exits it's always the first one (the second in the list, if exists, is additional metrics file)
	if(!inFace)
		return NULL;

	FTFaceToStreamWithOwnershipListMap::iterator it = mOpenReaders.find(inFace);
	if(it == mOpenReaders.end())
		return NULL;

	if(it->second.empty())
		return NULL;

	StreamWithOwnership& streamWithOwnership = it->second.front();
	if(!streamWithOwnership.stream)
		return NULL;

	return streamWithOwnership.stream;
}


FT_Face FreeTypeWrapper::NewFace(const std::string& inFilePath,const std::string& inSecondaryFilePath,FT_Long inFontIndex)
{
	FT_Open_Args attachStreamArguments = {0};
	FT_Face face = NewFace(inFilePath,inFontIndex);
	if(!face)
		return NULL;

	EStatusCode status = eSuccess;
	InputBufferedStream* inputBufferedFileReadSteam = NULL;
	FT_Stream attachFTStream = NULL;
	do
	{
		// secondary file attachment
		inputBufferedFileReadSteam = InputFile::CreateBufferedReadStream(inSecondaryFilePath);
		if(!inputBufferedFileReadSteam) {
			TRACE_LOG1("FreeTypeWrapper::NewFace,  Cannot Open secondary file path for reading %s", inSecondaryFilePath.c_str());
			status = eFailure;
			break;
		}
		LongFilePositionType totalSize = ((InputFileStream*)inputBufferedFileReadSteam->GetSourceStream())->GetFileSize();

		if(totalSize == 0) {
			TRACE_LOG("FreeTypeWrapper::NewFace, exception, input secondary stream size is 0");
			status = eFailure;
			break;
		}
		if(totalSize > (IOBasicTypes::LongFilePositionType)0x7fffffff) {
			TRACE_LOG("FreeTypeWrapper::NewFace, exception, input secondary stream size is too large for FreeType FT_Long");
			status = eFailure;
			break;
		}		

		attachFTStream = CreateFTStreamForIByteWriterWithPosition(inputBufferedFileReadSteam, (unsigned long)totalSize);
		FillOpenFaceArgumentsForFTStream(attachFTStream, attachStreamArguments);		

		FT_Error ftStatus = FT_Attach_Stream(face,&attachStreamArguments);
		if(ftStatus != 0)
		{
			const char* errMsg = FT_Error_String(ftStatus);
			TRACE_LOG1("FreeTypeWrapper::NewFace, unable to attache secondary file %s",inSecondaryFilePath.c_str());
			TRACE_LOG2("FreeTypeWrapper::NewFace, Free Type Error, Code = %d, Message = %s",ftStatus,errMsg ? errMsg : "");
			status = eFailure;
			break;
		}

		RegisterStreamForFace(face, attachFTStream, inputBufferedFileReadSteam, true);
	}while(false);

	if(status != eSuccess)
	{
		// release both stream and underlying input stream
		ReleaseStreamForFace(attachFTStream, inputBufferedFileReadSteam, true);
		// this should clean up also the original attached streams for the face, and close it
		DoneFace(face);
		face = NULL;
	}


	return face;	
}

void FreeTypeWrapper::CleanStreamsForFace(FT_Face inFace) {
	// clean readers streams for the face
	FTFaceToStreamWithOwnershipListMap::iterator it = mOpenReaders.find(inFace);
	if(it != mOpenReaders.end()) {
		CleanReaderStreamsList(it->second);
		mOpenReaders.erase(it);
	}

	// clean FT streams for the face
	FTFaceToFTStreamListMap::iterator itFtStream = mOpenFTStreams.find(inFace);
	if(itFtStream != mOpenFTStreams.end()) {
		CleanFtStreamsList(itFtStream->second);
		mOpenFTStreams.erase(itFtStream);
	}
}

FT_Error FreeTypeWrapper::DoneFace(FT_Face ioFace)
{
	FT_Error status = FT_Done_Face(ioFace);
	CleanStreamsForFace(ioFace);
	return status;
}

FT_Library FreeTypeWrapper::operator->()
{
	return mFreeType;
}

static IByteReaderWithPosition* GetFTStreamReader(FT_Stream inStream) {
	if(!inStream)
		return NULL;

	if(!inStream->descriptor.pointer)
		return NULL;

	return (IByteReaderWithPosition*)(inStream->descriptor.pointer);
}

static unsigned long InputStreamReadSeek(FT_Stream	   stream,
										 unsigned long   offset,
										 unsigned char*  buffer,
										 unsigned long   count)
{
	IByteReaderWithPosition* inputFileStream = GetFTStreamReader(stream);
	if(!inputFileStream)
		return 0;
	unsigned long readBytes = 0;
	
	inputFileStream->SetPosition(offset);
	if(count > 0)
		readBytes = (unsigned long)inputFileStream->Read(buffer,count);
	return readBytes;
}

FT_Stream FreeTypeWrapper::CreateFTStreamForIByteWriterWithPosition(IByteReaderWithPosition* inInputStream, unsigned long inSize) {
	FT_Stream aStream = new FT_StreamRec();
	aStream->base = NULL;
	aStream->size = inSize;
	aStream->pos = 0;
	aStream->descriptor.pointer =  inInputStream;
	aStream->pathname.pointer = NULL;
	aStream->read = InputStreamReadSeek;
	aStream->close = InputStreamClose;
	aStream->memory = NULL;
	aStream->cursor = NULL;
	aStream->limit = NULL;

	return aStream;
}


FreeTypeWrapper::operator FT_Library() const
{
	return mFreeType;
}
