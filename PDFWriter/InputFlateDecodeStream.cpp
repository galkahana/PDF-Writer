#include "InputFlateDecodeStream.h"

#include "Trace.h"
#include "zlib.h"


InputFlateDecodeStream::InputFlateDecodeStream(void)
{
	mZLibState = new z_stream;
	mSourceStream = NULL;
	mCurrentlyEncoding = false;
}

InputFlateDecodeStream::~InputFlateDecodeStream(void)
{
	if(mCurrentlyEncoding)
		FinalizeEncoding();
	if(mSourceStream)
		delete mSourceStream;
	delete mZLibState;
}

void InputFlateDecodeStream::FinalizeEncoding()
{
	// no need for flushing here, there's no notion of Z_FINISH. so just end the library work
	inflateEnd(mZLibState);
	mCurrentlyEncoding = false;
}

InputFlateDecodeStream::InputFlateDecodeStream(IByteReader* inSourceReader)
{
	mZLibState = new z_stream;
	mSourceStream = NULL;
	mCurrentlyEncoding = false;

	Assign(mSourceStream);
}

void InputFlateDecodeStream::Assign(IByteReader* inSourceReader)
{
	mSourceStream = inSourceReader;
	if(mSourceStream)
		StartEncoding();
}

void InputFlateDecodeStream::StartEncoding()
{
	mZLibState->zalloc = Z_NULL;
    mZLibState->zfree = Z_NULL;
    mZLibState->opaque = Z_NULL;
	mZLibState->avail_in = 0;
	mZLibState->next_in = Z_NULL;

    int inflateStatus = inflateInit(mZLibState);
    if (inflateStatus != Z_OK)
		TRACE_LOG1("InputFlateDecodeStream::StartEncoding, Unexpected failure in initializating flate library. status code = %d",inflateStatus);
	else
		mCurrentlyEncoding = true;
}

IOBasicTypes::LongBufferSizeType InputFlateDecodeStream::Read(IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inBufferSize)
{
	if(mCurrentlyEncoding)
		return DecodeBufferAndRead(inBuffer,inBufferSize);
	else if(mSourceStream)
		return mSourceStream->Read(inBuffer,inBufferSize);
	else
		return 0;
}

IOBasicTypes::LongBufferSizeType InputFlateDecodeStream::DecodeBufferAndRead(const IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inSize)
{
	if(0 == inSize)
		return 0; // inflate kinda touchy about getting 0 lengths

	int inflateResult = Z_OK;

	do
	{
		mZLibState->avail_out = inSize;
		mZLibState->next_out = (Bytef*)inBuffer;

		// first, flush whatever is already available
		while(mZLibState->avail_in != 0 && mZLibState->avail_out != 0)
		{
			inflateResult = inflate(mZLibState,Z_NO_FLUSH);
			if(Z_STREAM_ERROR == inflateResult ||
			   Z_NEED_DICT == inflateResult ||
			   Z_DATA_ERROR == inflateResult ||
			   Z_MEM_ERROR == inflateResult)
			{
				TRACE_LOG1("InputFlateDecodeStream::DecodeBufferAndWrite, failed to write zlib information. returned error code = %d",inflateResult);
				inflateEnd(mZLibState);
				break;
			}
		}
		if(Z_OK != inflateResult && Z_STREAM_END != inflateResult)
			break;

		// if not finished read buffer, repeatedly read from input stream, and inflate till done
		while((0 < mZLibState->avail_out) && mSourceStream->NotEnded())
		{
			if(mSourceStream->Read(&mBuffer,1) != 1)
			{
				TRACE_LOG("InputFlateDecodeStream::DecodeBufferAndWrite, failed to read from source stream");
				inflateEnd(mZLibState);
				inflateResult = Z_STREAM_ERROR;
				mCurrentlyEncoding = false;
				break;
			}

			mZLibState->avail_in = 1; 
			mZLibState->next_in = (Bytef*)&mBuffer;

			while(mZLibState->avail_in != 0 && mZLibState->avail_out != 0)
			{
				inflateResult = inflate(mZLibState,Z_NO_FLUSH);
				if(Z_STREAM_ERROR == inflateResult ||
				   Z_NEED_DICT == inflateResult ||
				   Z_DATA_ERROR == inflateResult ||
				   Z_MEM_ERROR == inflateResult)
				{
					TRACE_LOG1("InputFlateDecodeStream::DecodeBufferAndWrite, failed to write zlib information. returned error code = %d",inflateResult);
					inflateEnd(mZLibState);
					break;
				}
			}
			if(Z_OK != inflateResult && Z_STREAM_END != inflateResult)
				break;
		}
	} while(false);

	// should be that at the last buffer we'll get here a nice Z_STREAM_END
	if(Z_OK == inflateResult || Z_STREAM_END == inflateResult)
		return inSize - mZLibState->avail_out;
	else
		return 0;
}

bool InputFlateDecodeStream::NotEnded()
{
	if(mSourceStream)
		return mSourceStream->NotEnded() || mZLibState->avail_in != 0;
	else
		return mZLibState->avail_in != 0;
}