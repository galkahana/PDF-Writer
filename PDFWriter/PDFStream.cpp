#include "PDFStream.h"
#include "InputStringBufferStream.h"
#include "OutputStringBufferStream.h"
#include "OutputFlateEncodeStream.h"

PDFStream::PDFStream(bool inCompressStream)
{
	mCompressStream = inCompressStream;
	if(mCompressStream)
		mWriteStream = new OutputFlateEncodeStream(new OutputStringBufferStream(&mIOString));
	else
		mWriteStream = new OutputStringBufferStream(&mIOString);
	mStreamLength = 0;
	mReadStream = NULL;
}

PDFStream::~PDFStream(void)
{
	delete mReadStream;
}

IByteWriter* PDFStream::GetWriteStream()
{
	return mWriteStream;
}

EStatusCode PDFStream::FinalizeWriteAndStratRead()
{
	mReadStream = new InputStringBufferStream(&mIOString);
	delete mWriteStream;
	mWriteStream = NULL;
	mStreamLength = mIOString.str().length(); 
		// Gal 28/5/10: that's the only way to know the string size. in_avail is not good for this, see http://social.msdn.microsoft.com/Forums/en/vclanguage/thread/13009a88-933f-4be7-bf3d-150e425e66a6
		// Might want to consider a different implementation, then, maybe of the higher level basic_stringstream (although this will still not help)
	return eSuccess;
}

LongFilePositionType PDFStream::GetLength()
{
	return mStreamLength;
}

IByteReader* PDFStream::GetReadStream()
{
	return mReadStream;
}

bool PDFStream::IsStreamCompressed()
{
	return mCompressStream;
}
