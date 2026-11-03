/*
   Source File : InputCappedStream.cpp


   Copyright 2026 Gal Kahana PDFWriter

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
#include "InputCappedStream.h"
#include "IReadInterrupt.h"
#include "Trace.h"

#include <algorithm>

using namespace IOBasicTypes;

static const LongBufferSizeType scInterruptPollBytes = 1 << 20;

InputCappedStream::InputCappedStream(IByteReader* inSourceStream,
										LongBufferSizeType inReadLimit,
										IReadInterrupt* inInterrupt,
										unsigned long* ioTruncatedCount,
										unsigned long* ioInterruptedCount)
{
	mStream = inSourceStream;
	mHasLimit = inReadLimit > 0;
	mMoreToRead = inReadLimit;
	mInterrupt = inInterrupt;
	mReadSincePoll = 0;
	mInterrupted = false;
	mTruncatedCount = ioTruncatedCount;
	mInterruptedCount = ioInterruptedCount;
	mCheckedTruncation = false;
}

InputCappedStream::~InputCappedStream(void)
{
	delete mStream;
}

LongBufferSizeType InputCappedStream::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	if(mInterrupted)
		return 0;
	if(IsAtLimit())
	{
		CheckTruncation();
		return 0;
	}
	if(mInterrupt && mReadSincePoll >= scInterruptPollBytes)
	{
		mReadSincePoll = 0;
		if(mInterrupt->ShouldStop())
		{
			TRACE_LOG("InputCappedStream::Read, the read interrupt stopped a stream");
			mInterrupted = true;
			++(*mInterruptedCount);
			return 0;
		}
	}
	LongBufferSizeType toRead = mHasLimit ? std::min<LongBufferSizeType>(inBufferSize,mMoreToRead) : inBufferSize;
	LongBufferSizeType readBytes = mStream->Read(inBuffer,toRead);
	if(mHasLimit)
		mMoreToRead -= readBytes;
	mReadSincePoll += readBytes;
	return readBytes;
}

bool InputCappedStream::NotEnded()
{
	if(mInterrupted)
		return false;
	if(IsAtLimit())
	{
		CheckTruncation();
		return false;
	}
	return mStream->NotEnded();
}

bool InputCappedStream::IsAtLimit()
{
	return mHasLimit && mMoreToRead == 0;
}

void InputCappedStream::CheckTruncation()
{
	if(mCheckedTruncation)
		return;
	mCheckedTruncation = true;
	Byte probe;
	if(mStream->NotEnded() && mStream->Read(&probe,1) == 1)
	{
		TRACE_LOG("InputCappedStream::CheckTruncation, a stream reached its read limit and was truncated");
		++(*mTruncatedCount);
	}
}
