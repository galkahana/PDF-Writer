/*
   Source File : InputCappedStream.h


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
#pragma once

#include "IByteReader.h"

class IReadInterrupt;

/*
	InputCappedStream ends a source stream once it gave inReadLimit bytes (0: no limit), or when
	inInterrupt (may be NULL), asked every scInterruptPollBytes bytes, says to stop. A stream cut
	at the limit with more to give increments *ioTruncatedCount, one stopped by the interrupt
	*ioInterruptedCount, each once. It owns the source; the counters and the interrupt must
	outlive it.
*/
class InputCappedStream : public IByteReader
{
public:
	InputCappedStream(IByteReader* inSourceStream,
						IOBasicTypes::LongBufferSizeType inReadLimit,
						IReadInterrupt* inInterrupt,
						unsigned long* ioTruncatedCount,
						unsigned long* ioInterruptedCount);
	virtual ~InputCappedStream(void);

	virtual IOBasicTypes::LongBufferSizeType Read(IOBasicTypes::Byte* inBuffer,IOBasicTypes::LongBufferSizeType inBufferSize);
	virtual bool NotEnded();

private:
	IByteReader* mStream;
	bool mHasLimit;
	IOBasicTypes::LongBufferSizeType mMoreToRead;
	IReadInterrupt* mInterrupt;
	IOBasicTypes::LongBufferSizeType mReadSincePoll;
	bool mInterrupted;
	unsigned long* mTruncatedCount;
	unsigned long* mInterruptedCount;
	bool mCheckedTruncation;

	bool IsAtLimit();
	void CheckTruncation();
};
