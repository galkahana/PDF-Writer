/*
   Source File : CFFPrimitiveWriter.h


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
#include "EStatusCode.h"
#include "IByteWriter.h"
#include "DictOperand.h"

using namespace IOBasicTypes;

class CFFPrimitiveWriter
{
public:
	CFFPrimitiveWriter(IByteWriter* inCFFOutput = NULL);
	~CFFPrimitiveWriter(void);

	void SetStream(IByteWriter* inCFFOutput);

	EStatusCode GetInternalState();

	EStatusCode WriteByte(Byte inValue);
	EStatusCode Write(const Byte* inBuffer,LongBufferSizeType inBufferSize);

	// basic CFF values
	EStatusCode WriteCard8(Byte inValue);
	EStatusCode WriteCard16(unsigned short inValue);
	// set offsize to write offsets
	void SetOffSize(Byte inOffSize);
	EStatusCode WriteOffset(unsigned long inValue);
	EStatusCode WriteOffSize(Byte inValue);
	EStatusCode WriteSID(unsigned short inValue);

	// dict data
	EStatusCode WriteDictOperator(unsigned short inOperator);
	EStatusCode WriteDictOperand(const DictOperand& inOperand);
	EStatusCode WriteDictItems(unsigned short inOperator,const DictOperandList& inOperands);
	EStatusCode WriteIntegerOperand(long inValue);
	EStatusCode Write5ByteDictInteger(long inValue);
	EStatusCode WriteRealOperand(double inValue,long inFractalLength=10);

	EStatusCode Pad5Bytes();
	EStatusCode PadNBytes(unsigned short inBytesToPad);

private:
	IByteWriter* mCFFOutput;
	EStatusCode mInternalState;
	Byte mCurrentOffsize;

	EStatusCode Write3ByteUnsigned(unsigned long inValue);
	EStatusCode Write4ByteUnsigned(unsigned long inValue);
	EStatusCode WriteIntegerOfReal(double inIntegerValue,Byte& ioBuffer,bool& ioUsedFirst);
	EStatusCode SetOrWriteNibble(Byte inValue,Byte& ioBuffer,bool& ioUsedFirst);

};
