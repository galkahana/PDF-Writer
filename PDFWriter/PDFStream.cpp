#include "PDFStream.h"
#include "InputStringBufferStream.h"
#include "OutputStringBufferStream.h"
#include "OutputFlateEncodeStream.h"
#include "IObjectsContextExtender.h"

PDFStream::PDFStream(bool inCompressStream,
					 IByteWriterWithPosition* inOutputStream,
					 ObjectIDType inExtentObjectID,
					 IObjectsContextExtender* inObjectsContextExtender)
{
	mExtender = inObjectsContextExtender;
	mCompressStream = inCompressStream;
	mExtendObjectID = inExtentObjectID;	
	mStreamStartPosition = inOutputStream->GetCurrentPosition();
	mOutputStream = inOutputStream;
	mStreamLength = 0;


	if(mCompressStream)
	{
		if(mExtender && mExtender->OverridesStreamCompression())
		{
			mWriteStream = mExtender->GetCompressionWriteStream(inOutputStream);
		}
		else
		{
			mFlateEncodingStream.Assign(inOutputStream);
			mWriteStream = &mFlateEncodingStream;
		}
	}
	else
		mWriteStream = inOutputStream;

}

PDFStream::~PDFStream(void)
{
}

IByteWriter* PDFStream::GetWriteStream()
{
	return mWriteStream;
}

void PDFStream::FinalizeStreamWrite()
{
	if(mExtender && mExtender->OverridesStreamCompression() && mCompressStream)
		mExtender->FinalizeCompressedStreamWrite(mWriteStream);
	mWriteStream = NULL;
	if(mCompressStream)
		mFlateEncodingStream.Assign(NULL);  // this both finished encoding any left buffers and releases ownership from mFlateEncodingStream
	mStreamLength = mOutputStream->GetCurrentPosition()-mStreamStartPosition;
	mOutputStream = NULL;
}

LongFilePositionType PDFStream::GetLength()
{
	return mStreamLength;
}

bool PDFStream::IsStreamCompressed()
{
	return mCompressStream;
}

ObjectIDType PDFStream::GetExtentObjectID()
{
	return mExtendObjectID;
}
