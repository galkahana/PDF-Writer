/*
   Source File : OpenTypePrimitiveReader.h


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
#pragma once

#include "EPDFStatusCode.h"
#include "IByteReaderWithPosition.h"

class OpenTypePrimitiveReader
{
public:
	OpenTypePrimitiveReader(IByteReaderWithPosition* inTrueTypeFile = NULL);
	~OpenTypePrimitiveReader(void);

	void SetOpenTypeStream(IByteReaderWithPosition* inOpenTypeFile);
	void SetOffset(LongFilePositionType inNewOffset);
	void Skip(LongBufferSizeType inToSkip);
	LongFilePositionType GetCurrentPosition();
	EPDFStatusCode GetInternalState();

	EPDFStatusCode ReadBYTE(unsigned char& outValue);
	EPDFStatusCode ReadCHAR(char& outValue);
	EPDFStatusCode ReadUSHORT(unsigned short& outValue);
	EPDFStatusCode ReadSHORT(short& outValue);
	EPDFStatusCode ReadULONG(unsigned long& outValue);
	EPDFStatusCode ReadLONG(long& outValue);
	EPDFStatusCode ReadLongDateTime(long long& outValue);
	EPDFStatusCode ReadFixed(double& outValue);
	EPDFStatusCode Read(Byte* inBuffer,LongBufferSizeType inBufferSize);

	IByteReaderWithPosition* GetReadStream();
private:

	IByteReaderWithPosition* mOpenTypeFile;
	LongFilePositionType mInitialPosition;
	EPDFStatusCode mInternalState;

};
