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
#include "EPDFStatusCode.h"
#include "IByteWriter.h"
#include "DictOperand.h"

using namespace IOBasicTypes;

class CFFPrimitiveWriter
{
public:
	CFFPrimitiveWriter(IByteWriter* inCFFOutput = NULL);
	~CFFPrimitiveWriter(void);

	void SetStream(IByteWriter* inCFFOutput);

	EPDFStatusCode GetInternalState();

	EPDFStatusCode WriteByte(Byte inValue);
	EPDFStatusCode Write(const Byte* inBuffer,LongBufferSizeType inBufferSize);

	// basic CFF values
	EPDFStatusCode WriteCard8(Byte inValue);
	EPDFStatusCode WriteCard16(unsigned short inValue);
	// set offsize to write offsets
	void SetOffSize(Byte inOffSize);
	EPDFStatusCode WriteOffset(unsigned long inValue);
	EPDFStatusCode WriteOffSize(Byte inValue);
	EPDFStatusCode WriteSID(unsigned short inValue);

	// dict data
	EPDFStatusCode WriteDictOperator(unsigned short inOperator);
	EPDFStatusCode WriteDictOperand(const DictOperand& inOperand);
	EPDFStatusCode WriteDictItems(unsigned short inOperator,const DictOperandList& inOperands);
	EPDFStatusCode WriteIntegerOperand(long inValue);
	EPDFStatusCode Write5ByteDictInteger(long inValue);
	EPDFStatusCode WriteRealOperand(double inValue,long inFractalLength=10);

	EPDFStatusCode Pad5Bytes();
	EPDFStatusCode PadNBytes(unsigned short inBytesToPad);

private:
	IByteWriter* mCFFOutput;
	EPDFStatusCode mInternalState;
	Byte mCurrentOffsize;

	EPDFStatusCode Write3ByteUnsigned(unsigned long inValue);
	EPDFStatusCode Write4ByteUnsigned(unsigned long inValue);
	EPDFStatusCode WriteIntegerOfReal(double inIntegerValue,Byte& ioBuffer,bool& ioUsedFirst);
	EPDFStatusCode SetOrWriteNibble(Byte inValue,Byte& ioBuffer,bool& ioUsedFirst);

};
